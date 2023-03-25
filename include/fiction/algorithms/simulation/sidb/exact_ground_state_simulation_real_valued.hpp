//
// Created by marcel on 07.03.23.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_SIM_REAL_HPP
#define FICTION_EXACT_SIM_REAL_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>
#include <z3++.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace fiction
{

struct exact_ground_state_simulation_real_params
{
    /**
     * General parameters for the simulation of the physical SiDB system.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * The number of valid layouts to enumerate. If set to `-1`, all valid layouts are enumerated. If set to `0`, no
     * layouts are being returned. If set to `1`, only the layout with minimum system energy is determined, and so on.
     */
    uint16_t number_of_valid_layouts_to_enumerate{1u};
    /**
     * Sets a timeout in ms for the solving process. Standard is 4294967 seconds as defined by Z3.
     */
    unsigned timeout{4294967u};
};

template <typename Lyt>
struct exact_ground_state_simulation_real_stats
{
    /**
     * Total simulation runtime.
     */
    mockturtle::stopwatch<>::duration time_total{};
    /**
     * Number of invalid charge configurations that were enumerated before finding a metastable ground state.
     */
    std::size_t number_of_invalid_configurations{0u};
    /**
     * Number of charges that were pre-assigned before the solving process.
     */
    std::size_t number_of_pre_assigned_charges{0u};
    /**
     * Number of negative charges in the final configuration.
     */
    std::size_t number_of_negative_charges{0u};
    /**
     * Number of neutral charges in the final configuration.
     */
    std::size_t number_of_neutral_charges{0u};
    /**
     * Number of positive charges in the final configuration.
     */
    std::size_t number_of_positive_charges{0u};
    /**
     * All physically valid charge layouts.
     */
    std::vector<charge_distribution_surface<Lyt>> valid_lyts{};
};

namespace detail
{

template <typename Lyt>
class exact_ground_state_simulation_real_impl
{
  public:
    exact_ground_state_simulation_real_impl(const Lyt& lyt, const exact_ground_state_simulation_real_params& p,
                                            exact_ground_state_simulation_real_stats<Lyt>& st) noexcept :
            charge_lyt{lyt, p.phys_params, sidb_charge_state::NEGATIVE},
            params{p},
            stats{st}
    {
        st.valid_lyts.reserve(p.number_of_valid_layouts_to_enumerate);
    }

    void run()
    {
        // measure runtime
        mockturtle::stopwatch stop{stats.time_total};

        // no need to run the simulation if the layout is empty
        if (charge_lyt.is_empty())
        {
            return;
        }

        // set the timeout
        set_timeout(params.timeout);

        // set up the SMT instance
        generate_smt_instance();
        // run the solver and extract the valid layouts
        gather_valid_charge_configurations();
    }

  private:
    /**
     * The charge distribution surface of the input layout.
     */
    charge_distribution_surface<Lyt> charge_lyt;
    /**
     * The simulation parameters.
     */
    const exact_ground_state_simulation_real_params& params;
    /**
     * The simulation statistics.
     */
    exact_ground_state_simulation_real_stats<Lyt>& stats;

    /**
     * The Z3 context.
     */
    z3::context ctx{};
    /**
     * The Z3 optimizer.
     */
    z3::optimize optimizer{ctx};

    /**
     * Sets the given timeout for the current solver.
     *
     * @param t Timeout in ms.
     */
    void set_timeout(const unsigned t)
    {
        z3::params p{ctx};
        p.set("timeout", t);
        optimizer.set(p);
    }
    /**
     * Alias for an SiDB.
     */
    using sidb = typename charge_distribution_surface<Lyt>::cell;
    /**
     * Returns a Z3 expression representing the charge state of the given SiDB.
     *
     * The SiDB charge is represented as a real number in the interval [-1, 0] for two-state simulation and [-1, 1] for
     * three-state simulation.
     *
     * @param s The SiDB.
     * @return A Z3 expression representing the charge of the given SiDB.
     */
    [[nodiscard]] z3::expr get_sidb_charge(const sidb& s)
    {
        return ctx.real_const(fmt::format("n_({},{},{})", s.x, s.y, s.z).c_str());
    }
    /**
     * Returns a Z3 expression representing the negative charge state of the given SiDB.
     *
     * @param s The SiDB.
     * @return A Z3 expression representing the negative charge state of the given SiDB.
     */
    [[nodiscard]] z3::expr negative_sidb_charge_state(const sidb& s)
    {
        const auto charge = get_sidb_charge(s);

        //        return charge >= ctx.real_val("-1") && charge <= ctx.real_val("-0.5");
        return charge == ctx.real_val("-1");
    }
    /**
     * Returns a Z3 expression representing the neutral charge state of the given SiDB.
     *
     * @param s The SiDB.
     * @return A Z3 expression representing the neutral charge state of the given SiDB.
     */
    [[nodiscard]] z3::expr neutral_sidb_charge_state(const sidb& s)
    {
        const auto charge = get_sidb_charge(s);

        //        return charge > ctx.real_val("-0.5") && charge < ctx.real_val("0.5");
        return charge == ctx.real_val("0");
    }
    /**
     * Returns a Z3 expression representing the positive charge state of the given SiDB.
     *
     * @param s The SiDB.
     * @return A Z3 expression representing the positive charge state of the given SiDB.
     */
    [[nodiscard]] z3::expr positive_sidb_charge_state(const sidb& s)
    {
        const auto charge = get_sidb_charge(s);

        //        return charge >= ctx.real_val("0.5") && charge <= ctx.real_val("1");
        return charge == ctx.real_val("1");
    }
    /**
     * Returns a Z3 expression representing the electrostatic potential between the given SiDBs.
     *
     * The electrostatic potential is represented by a real variable `V_{s1, s2}`.
     *
     * @param s1 The first SiDB.
     * @param s2 The second SiDB.
     * @return A Z3 expression representing the electrostatic potential between the given SiDBs.
     */
    [[nodiscard]] z3::expr get_electrostatic_potential(const sidb& s1, const sidb& s2)
    {
        return ctx.real_const(fmt::format("V_({},{},{}),({},{},{})", s1.x, s1.y, s1.z, s2.x, s2.y, s2.z).c_str());
    }
    /**
     * Returns a Z3 expression representing the local potential at the given SiDB.
     *
     * The local potential is represented by a real variable `V_local_{s}`.
     *
     * @param s The SiDB.
     * @return A Z3 expression representing the local potential at the given SiDB.
     */
    [[nodiscard]] z3::expr get_local_potential(const sidb& s)
    {
        return ctx.real_const(fmt::format("V_local,({},{},{})", s.x, s.y, s.z).c_str());
    }
    /**
     * Returns a Z3 expression representing the total system energy.
     *
     * @return A Z3 expression representing the total system energy.
     */
    [[nodiscard]] z3::expr get_system_energy()
    {
        return ctx.real_const("E");
    }
    /**
     * Adds the constraints that restrict the values of the charges to the interval [-1, 0] for two-state simulation or
     * [-1, 1] for three-state simulation.
     */
    void restrict_sidb_charge_state_values()
    {
        charge_lyt.foreach_cell(
            [this](const sidb& s)
            {
                //                // two-state simulation
                //                if (params.phys_params.base == 2)
                //                {
                //                    optimizer.add(get_sidb_charge(s) == ctx.real_val("-1") || get_sidb_charge(s) ==
                //                    ctx.real_val("0"));
                //                }
                //                // three-state simulation
                //                else
                //                {
                //                    optimizer.add(get_sidb_charge(s) == ctx.real_val("-1") || get_sidb_charge(s) ==
                //                    ctx.real_val("0") ||
                //                                  get_sidb_charge(s) == ctx.real_val("1"));
                //                }

                optimizer.add(get_sidb_charge(s) >= -1);

                // two-state simulation
                if (params.phys_params.base == 2)
                {
                    optimizer.add(get_sidb_charge(s) <= 0);
                }
                // three-state simulation
                else
                {
                    optimizer.add(get_sidb_charge(s) <= 1);
                }
            });
    }
    /**
     * Adds the constraints that define the electrostatic potential between all mutually exclusive pairs of SiDBs.
     */
    void define_electrostatic_potentials()
    {
        charge_lyt.foreach_cell(
            [this](const sidb& s1)
            {
                charge_lyt.foreach_cell(
                    [this, &s1](const sidb& s2)
                    {
                        if (s1 == s2)
                        {
                            return;
                        }

                        // define the electrostatic potential between two SiDBs

                        const auto potential_val = ctx.real_val(
                            std::to_string(charge_lyt.get_chargeless_potential_between_sidbs(s1, s2)).c_str());

                        // this encoding is more performant than using `z3::ite` expressions with 0-valued sub-terms
                        optimizer.add(get_electrostatic_potential(s1, s2) == potential_val * get_sidb_charge(s2));
                    });
            });
    }
    /**
     * Adds the constraints that define the local potential at all SiDBs.
     */
    void define_local_potentials()
    {
        charge_lyt.foreach_cell(
            [this](const sidb& s1)
            {
                z3::expr_vector local_potential_terms{ctx};

                // gather electrostatic potentials
                charge_lyt.foreach_cell(
                    [this, &local_potential_terms, &s1](const sidb& s2)
                    {
                        if (s1 != s2)
                        {
                            // this is way more performant than using `z3::ite` expressions with 0-valued sub-terms
                            local_potential_terms.push_back(get_electrostatic_potential(s1, s2));
                        }
                    });

                // the local potential is the sum of the potentials between the SiDB and all other SiDBs
                if (!local_potential_terms.empty())
                {
                    optimizer.add(get_local_potential(s1) == z3::sum(local_potential_terms));
                }
            });
    }
    /**
     * Adds the constraints that define the population stability.
     */
    void define_population_stability()
    {
        const auto mu_minus = ctx.real_val(std::to_string(params.phys_params.mu).c_str());
        const auto mu_plus  = ctx.real_val(std::to_string(params.phys_params.mu_p).c_str());

        charge_lyt.foreach_cell(
            [this, &mu_minus, &mu_plus](const sidb& s1)
            {
                // two-state simulation
                if (params.phys_params.base == 2)
                {
                    optimizer.add(z3::ite(mu_minus + -get_local_potential(s1) < 0,  // if mu_minus + V < 0
                                          negative_sidb_charge_state(s1),           // then n == -1
                                          neutral_sidb_charge_state(s1)));          // else n == 0
                }
                // three-state simulation
                else if (params.phys_params.base == 3)
                {
                    optimizer.add(z3::ite(mu_minus + -get_local_potential(s1) < 0,         // if mu_minus + V < 0
                                          negative_sidb_charge_state(s1),                  // then n == -1
                                          z3::ite(mu_plus + -get_local_potential(s1) > 0,  // else if mu_plus + V > 0
                                                  positive_sidb_charge_state(s1),          // then n == 1
                                                  neutral_sidb_charge_state(s1))));        // else n == 0
                }

                //                // two-state simulation
                //                if (params.phys_params.base == 2)
                //                {
                //                    optimizer.add(z3::ite(mu_minus + -get_local_potential(s1) < 0,     // if mu_minus
                //                    + V < 0
                //                                          get_sidb_charge(s1) == ctx.real_val("-1"),   // then n == -1
                //                                          get_sidb_charge(s1) == ctx.real_val("0")));  // else n == 0
                //                }
                //                // three-state simulation
                //                else if (params.phys_params.base == 3)
                //                {
                //                    optimizer.add(z3::ite(mu_minus + -get_local_potential(s1) < 0,           // if
                //                    mu_minus + V < 0
                //                                          get_sidb_charge(s1) == ctx.real_val("-1"),         // then n
                //                                          == -1 z3::ite(mu_plus + -get_local_potential(s1) > 0,    //
                //                                          else if mu_plus + V > 0
                //                                                  get_sidb_charge(s1) == ctx.real_val("1"),  // then n
                //                                                  == 1 get_sidb_charge(s1) == ctx.real_val("0")))); //
                //                                                  else n == 0
                //                }
            });
    }
    /**
     * Adds constraints that pre-assigns negative charges to SiDBs based on physical implications. This should not
     * eliminate valid solutions, but reduce the search space for the SMT solver.
     */
    void pre_assign_known_negative_charges()
    {
        const auto negative_sidb_indices = charge_lyt.negative_sidb_detection();

        std::for_each(negative_sidb_indices.cbegin(), negative_sidb_indices.cend(),
                      [this](const auto& sidb_index)
                      { optimizer.add(get_sidb_charge(charge_lyt.index_to_cell(sidb_index)) == ctx.real_val("-1")); });

        stats.number_of_pre_assigned_charges = negative_sidb_indices.size();
    }
    /**
     * Adds the constraints that define and minimize the system energy.
     */
    void minimize_system_energy()
    {
        const auto system_energy = get_system_energy();

        z3::expr_vector energy_terms{ctx};

        charge_lyt.foreach_cell(
            [this, &energy_terms](const sidb& s)
            {
                // two-state simulation
                if (params.phys_params.base == 2)
                {
                    // testing determined that this encoding is more performant than simply using
                    // `get_local_potential(s) * get_sidb_charge(s)`
                    energy_terms.push_back(z3::ite(neutral_sidb_charge_state(s), ctx.real_val(0),
                                                   get_local_potential(s) * get_sidb_charge(s)));
                }
                // three-state simulation
                else
                {
                    // however, for three-state simulation, the following encoding works best
                    energy_terms.push_back(z3::ite(
                        neutral_sidb_charge_state(s), ctx.real_val(0),
                        z3::ite(negative_sidb_charge_state(s), -get_local_potential(s), get_local_potential(s))));
                }
            });

        if (!energy_terms.empty())
        {
            // the system energy is in fact half of the sum of all local potential terms
            // we do not multiply by 0.5 here, because this is irrelevant for the optimization (equisatisfiable)
            optimizer.add(system_energy == z3::sum(energy_terms));
        }

        // minimize the system energy
        optimizer.minimize(system_energy);
    }
    /**
     * Generates the SMT instance.
     */
    void generate_smt_instance()
    {
        // restrict the values of the charge state variables
        restrict_sidb_charge_state_values();
        // define the electrostatic potential V_{i,j} between SiDBs
        define_electrostatic_potentials();
        // define the local potential V_i at SiDBs
        define_local_potentials();

        // population stability conditions
        define_population_stability();

        // backdoor constraints
        pre_assign_known_negative_charges();

        // minimize the system energy
        minimize_system_energy();
    }

    sidb_charge_state get_charge_state_from_model(const z3::model& m, const sidb& s)
    {
        // extract charge state from model with model completion turned on
        const auto charge_model_val = m.eval(get_sidb_charge(s), true);
        // since the model value is a real, we need to extract the numerator and denominator separately
        const auto charge_numerator   = charge_model_val.numerator().as_double();
        const auto charge_denominator = charge_model_val.denominator().as_double();
        // compute the charge value
        const auto charge = charge_numerator / charge_denominator;
        // map the charge value to a charge state
        const auto charge_state = charge <= -0.5 ? sidb_charge_state::NEGATIVE :
                                  charge >= 0.5  ? sidb_charge_state::POSITIVE :
                                                   sidb_charge_state::NEUTRAL;

        switch (charge_state)
        {
            case (sidb_charge_state::NEGATIVE):
            {
                ++stats.number_of_negative_charges;
                break;
            }
            case (sidb_charge_state::NEUTRAL):
            {
                ++stats.number_of_neutral_charges;
                break;
            }
            case (sidb_charge_state::POSITIVE):
            {
                ++stats.number_of_positive_charges;
                break;
            }
            default:
            {
                break;
            }
        }

        return charge_state;
    }
    /**
     * Extracts the charge configuration from the given Z3 model and returns a `charge_distribution_surface`
     * representation.
     *
     * @param m The model.
     * @return The charge configuration in a `charge_distribution_surface` object.
     */
    charge_distribution_surface<Lyt> extract_charge_configuration_from_model(const z3::model& m)
    {
        // make a copy of the charge layout
        charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};

        charge_lyt_copy.foreach_cell(
            [this, &m, &charge_lyt_copy](const sidb& s)
            {
                // extract the SiDB's  charge state and assign it to the charge layout
                charge_lyt_copy.assign_charge_state(s, get_charge_state_from_model(m, s));
            });

        charge_lyt_copy.update_after_charge_change();

        return charge_lyt_copy;
    }
    /**
     * Exclude the given model from the search space to prevent it from reoccurring in subsequent solver calls.
     *
     * This function is used to prevent the same charge configuration from being enumerated multiple times. Thus, it
     * particularly only excludes the exact charge configuration present in the model.
     *
     * @param m The model to exclude.
     */
    void exclude_model_from_search_space(const z3::model& m)
    {
        // make sure that the same model is not considered again
        z3::expr_vector model_constraints{ctx};

        charge_lyt.foreach_cell(
            [this, &m, &model_constraints](const sidb& s)
            {
                const auto charge_state = get_charge_state_from_model(m, s);

                model_constraints.push_back(
                    charge_state == sidb_charge_state::NEGATIVE ? negative_sidb_charge_state(s) :
                    charge_state == sidb_charge_state::POSITIVE ? positive_sidb_charge_state(s) :
                                                                  neutral_sidb_charge_state(s));

                std::cout << (get_sidb_charge(s) == m.eval(get_sidb_charge(s), true)) << std::endl;
            });

        optimizer.add(!z3::mk_and(model_constraints));

        // reset charge statistics
        stats.number_of_negative_charges = 0ul;
        stats.number_of_neutral_charges  = 0ul;
        stats.number_of_positive_charges = 0ul;
    }
    /**
     * Enumerates all valid charge configurations up to the bound specified in the parameters.
     */
    void gather_valid_charge_configurations()
    {
        while (stats.valid_lyts.size() < params.number_of_valid_layouts_to_enumerate)
        {
            z3::check_result z3_result{z3::unknown};

            //            std::cout << optimizer.assertions() << std::endl;

            try
            {
                z3_result = optimizer.check();
            }
            catch (z3::exception&)  // timeout
            {
                break;
            }

            if (z3_result == z3::sat)
            {
                z3::model m = optimizer.get_model();

                const auto lyt = extract_charge_configuration_from_model(m);

                if (lyt.is_physically_valid())
                {
                    stats.valid_lyts.push_back(lyt);  // TODO move semantics

                    std::cout << "layout is valid!" << std::endl;
                }
                else
                {
                    std::cout << "layout is not valid!" << std::endl;
                    ++stats.number_of_invalid_configurations;
                }

                // if there is a next model to be considered, exclude the current one from the search space
                if (stats.valid_lyts.size() != params.number_of_valid_layouts_to_enumerate)
                {
                    exclude_model_from_search_space(m);
                }
            }
            else
            {
                break;
            }
        }
    }
};

}  // namespace detail

/**
 * This function performs an exact ground state simulation of the given SiDB layout. It is based on the Z3 SMT solver.
 * The simulation is performed by translating the SiDB layout into a set of logical constraints that are then passed to
 * the Z3 solver. The solver then enumerates all valid charge configurations up to the bound specified in the
 * parameters. The valid charge configurations are then used to construct a set of valid layouts.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The SiDB layout to simulate.
 * @param ps Parameters for the simulation.
 * @param pst Statistics for the simulation.
 */
template <typename Lyt>
void exact_ground_state_simulation_real(const Lyt& lyt, const exact_ground_state_simulation_real_params& ps = {},
                                        exact_ground_state_simulation_real_stats<Lyt>* pst = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    exact_ground_state_simulation_real_stats<Lyt> st{};

    detail::exact_ground_state_simulation_real_impl<Lyt>{lyt, ps, st}.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EXACT_SIM_HPP

#endif  // FICTION_Z3_SOLVER