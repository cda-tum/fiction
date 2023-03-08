//
// Created by marcel on 07.03.23.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_SIM_HPP
#define FICTION_EXACT_SIM_HPP

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

struct exact_ground_state_simulation_params
{
    /**
     * General parameters for the simulation of the physical SiDB system.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Representation of the number of simulation states.
     */
    enum class simulation_states : uint8_t
    {
        /**
         * Two-state simulation.
         */
        TWO = 2,
        /**
         * Three-state simulation.
         */
        THREE = 3
    };
    /**
     * Number of simulation states to use.
     */
    simulation_states simulation_states{simulation_states::TWO};
    /**
     * The number of valid layouts to enumerate. If set to `-1`, all valid layouts are enumerated. If set to `0`, no
     * layouts are being returned. If set to `1`, only the layout with minimum system energy is determined, and so on.
     */
    uint16_t number_of_valid_layouts_to_enumerate{1u};
};

template <typename Lyt>
struct exact_ground_state_simulation_stats
{
    /**
     * Total simulation runtime.
     */
    mockturtle::stopwatch<>::duration time_total{};
    /**
     * All physically valid charge layouts.
     */
    std::vector<charge_distribution_surface<Lyt>> valid_lyts{};
};

namespace detail
{

template <typename Lyt>
class exact_ground_state_simulation_impl
{
  public:
    exact_ground_state_simulation_impl(const Lyt& lyt, const exact_ground_state_simulation_params& p,
                                       exact_ground_state_simulation_stats<Lyt>& st) noexcept :
            charge_lyt{lyt, p.phys_params},
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

        generate_smt_instance();
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
    const exact_ground_state_simulation_params& params;
    /**
     * The simulation statistics.
     */
    exact_ground_state_simulation_stats<Lyt>& stats;

    /**
     * The Z3 context.
     */
    z3::context ctx{};
    /**
     * The Z3 optimizer.
     */
    z3::optimize optimizer{ctx};

    using sidb = typename charge_distribution_surface<Lyt>::cell;

    [[nodiscard]] z3::expr get_sidb_charge_state(const sidb& s)
    {
        // two-state simulation: n == false -> n == -1 (negative charge), n == true -> n == 0 (neutral)
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            return ctx.bool_const(fmt::format("n_({},{},{})", s.x, s.y, s.z).c_str());
        }

        // three-state simulation: n == -1 (negative charge), n == 0 (neutral), n == 1 (positive charge)
        return ctx.int_const(fmt::format("n_({},{},{})", s.x, s.y, s.z).c_str());
    }

    [[nodiscard]] z3::expr negative_sidb_charge_state(const sidb& s)
    {
        const auto sidb_charge_state = get_sidb_charge_state(s);

        // two-state simulation: n == false -> n == -1 (negative charge)
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            return !sidb_charge_state;
        }

        // three-state simulation: n == -1 (negative charge)
        return sidb_charge_state == -1;
    }

    [[nodiscard]] z3::expr neutral_sidb_charge_state(const sidb& s)
    {
        const auto sidb_charge_state = get_sidb_charge_state(s);

        // two-state simulation: n == true -> n == 0 (neutral)
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            return sidb_charge_state;
        }

        // three-state simulation: n == 0 (neutral)
        return sidb_charge_state == 0;
    }

    [[nodiscard]] z3::expr positive_sidb_charge_state(const sidb& s)
    {
        const auto sidb_charge_state = get_sidb_charge_state(s);

        // three-state simulation: n == 1 (positive charge)
        return sidb_charge_state == 1;
    }

    [[nodiscard]] z3::expr get_sidb_sign(const sidb& s)
    {
        // two-state simulation
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            const auto zero         = ctx.real_val(0);
            const auto negative_one = ctx.real_val(-1);

            return z3::ite(get_sidb_charge_state(s), zero, negative_one);
        }

        // three-state simulation
        return get_sidb_charge_state(s);
    }

    [[nodiscard]] z3::expr get_electrostatic_potential(const sidb& s1, const sidb& s2)
    {
        return ctx.real_const(fmt::format("V_({},{},{}),({},{},{})", s1.x, s1.y, s1.z, s2.x, s2.y, s2.z).c_str());
    }

    [[nodiscard]] z3::expr get_local_potential(const sidb& s)
    {
        return ctx.real_const(fmt::format("V_local,({},{},{})", s.x, s.y, s.z).c_str());
    }

    void restrict_sidb_charge_state_values()
    {
        // only applicable to three-state simulation
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::THREE)
        {
            // restrict the values of the charge state variables to {-1, 0, 1}
            charge_lyt.foreach_cell(
                [this](const sidb& s)
                {
                    optimizer.add(get_sidb_charge_state(s) >= -1);
                    optimizer.add(get_sidb_charge_state(s) <= 1);
                });
        }

        // for two-state simulation, the charge state variables are Boolean, thus, false == -1 and true == 0
    }

    void define_electrostatic_potential()
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
                        // we do not need a minus here, because we defined the electric charge with a positive sign
                        const auto potential_val =
                            ctx.real_val(std::to_string(-charge_lyt.potential_between_sidbs(s1, s2)).c_str());

                        std::cout << "potential_val:   " << -charge_lyt.potential_between_sidbs(s1, s2) << "\n"
                                  << "Z3's value repr: " << potential_val.as_double() << std::endl;

                        std::cout << (get_electrostatic_potential(s1, s2) == potential_val * get_sidb_sign(s2))
                                  << std::endl;

                        optimizer.add(get_electrostatic_potential(s1, s2) == potential_val * get_sidb_sign(s2));
                    });
            });
    }

    void define_population_stability()
    {
        const auto mu_minus = ctx.real_val(std::to_string(params.phys_params.mu).c_str());
        const auto mu_plus  = ctx.real_val(std::to_string(params.phys_params.mu_p).c_str());

        std::cout << "mu_minus: " << mu_minus.as_double() << std::endl;
        std::cout << "mu_plus:  " << mu_plus.as_double() << std::endl;

        charge_lyt.foreach_cell(
            [this, &mu_minus, &mu_plus](const sidb& s1)
            {
                z3::expr_vector local_potential_terms{ctx};

                // gather local potentials
                charge_lyt.foreach_cell(
                    [this, &local_potential_terms, &s1](const sidb& s2)
                    {
                        if (s1 != s2)
                        {
                            local_potential_terms.push_back(get_electrostatic_potential(s1, s2));
                        }
                    });

                // the local potential is the sum of the potentials between the SiDB and all other SiDBs
                if (!local_potential_terms.empty())
                {
                    optimizer.add(get_local_potential(s1) == z3::sum(local_potential_terms));
                }

                // the population stability conditions

                // two-state simulation
                if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
                {
                    optimizer.add(z3::ite(mu_minus + get_local_potential(s1) < 0,  // if mu_minus + V < 0
                                          negative_sidb_charge_state(s1),          // then n == -1
                                          neutral_sidb_charge_state(s1)));         // else n == 0
                }
                // three-state simulation
                else if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::THREE)
                {
                    optimizer.add(z3::ite(mu_minus + get_local_potential(s1) < 0,         // if mu_minus + V < 0
                                          negative_sidb_charge_state(s1),                 // then n == -1
                                          z3::ite(mu_plus + get_local_potential(s1) > 0,  // else if mu_plus + V > 0
                                                  positive_sidb_charge_state(s1),         // then n == 1
                                                  neutral_sidb_charge_state(s1))));       // else n == 0
                }
            });
    }

    void minimize_system_energy()
    {
        const auto system_energy = ctx.real_const("E");

        z3::expr_vector energy_terms{ctx};

        charge_lyt.foreach_cell(
            [this, &energy_terms](const sidb& s1)
            {
                charge_lyt.foreach_cell(
                    [this, &energy_terms, &s1](const sidb& s2)
                    {
                        if (s1 < s2)
                        {
                            // add the electrostatic potential energy term
                            energy_terms.push_back(get_electrostatic_potential(s1, s2) * get_sidb_sign(s1));
                        }
                    });
            });

        if (!energy_terms.empty())
        {
            // the system energy is the negative sum of all energy terms
            optimizer.add(system_energy == -z3::sum(energy_terms));
        }

        // minimize the system energy
        optimizer.minimize(system_energy);
    }

    void generate_smt_instance()
    {
        // restrict the values of the charge state variables if necessary (three-state simulation)
        restrict_sidb_charge_state_values();
        // define the electrostatic potential V_{i,j} between SiDBs
        define_electrostatic_potential();

        // population stability conditions
        define_population_stability();

        // minimize the system energy
        minimize_system_energy();
    }

    charge_distribution_surface<Lyt> extract_charge_configuration_from_model(const z3::model& m)
    {
        // make a copy of the charge layout
        charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};

        charge_lyt_copy.foreach_cell(
            [this, &m, &charge_lyt_copy](const sidb& s)
            {
                if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
                {
                    // extract charge state from model with model completion turned on
                    const auto sidb_charge_state = m.eval(get_sidb_charge_state(s), true).bool_value();

                    charge_lyt_copy.assign_charge_state(
                        s, sidb_charge_state == Z3_L_TRUE ? sidb_charge_state::NEUTRAL : sidb_charge_state::NEGATIVE);
                }
                else if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::THREE)
                {
                    // extract charge state from model with model completion turned on
                    const auto sidb_charge_state = m.eval(get_sidb_charge_state(s), true).get_numeral_int();

                    charge_lyt_copy.assign_charge_state(s, sidb_charge_state == -1 ? sidb_charge_state::NEGATIVE :
                                                           sidb_charge_state == 0  ? sidb_charge_state::NEUTRAL :
                                                                                     sidb_charge_state::POSITIVE);
                }
            });

        charge_lyt_copy.update_after_charge_change();

        return charge_lyt_copy;
    }

    void exclude_model_from_search_space(const z3::model& m)
    {
        // make sure that the same model is not considered again
        z3::expr_vector model_constraints{ctx};

        charge_lyt.foreach_cell(
            [this, &m, &model_constraints](const sidb& s)
            {
                model_constraints.push_back(get_sidb_charge_state(s) == m.eval(get_sidb_charge_state(s), true));

                std::cout << (get_sidb_charge_state(s) == m.eval(get_sidb_charge_state(s), true)) << std::endl;
            });

        optimizer.add(!z3::mk_and(model_constraints));
    }

    void gather_valid_charge_configurations()
    {
        while (stats.valid_lyts.size() < params.number_of_valid_layouts_to_enumerate)
        {
            if (optimizer.check() == z3::sat)
            {
                z3::model m = optimizer.get_model();

                std::cout << "MODEL:\n" << m << std::endl;

                const auto lyt = extract_charge_configuration_from_model(m);

                if (lyt.is_physically_valid())
                {
                    stats.valid_lyts.push_back(lyt);  // TODO move semantics

                    std::cout << "layout is valid!" << std::endl;
                }
                else
                {
                    std::cout << "layout is not valid!" << std::endl;
                }

                // print the system energy
                std::cout << "System Energy: " << lyt.get_system_energy() << std::endl;
                std::cout << "Z3           : " << m.eval(ctx.real_const("E"), true).as_double() << std::endl;

                // print the local potentials
                lyt.foreach_cell(
                    [this, &m, &lyt](const sidb& s1)
                    {
                        std::cout << fmt::format("V_local,{} = {}", s1, *lyt.get_local_potential(s1)) << std::endl;
                        std::cout << fmt::format("Z3              = {}",
                                                 m.eval(get_local_potential(s1), true).as_double())
                                  << std::endl;

                        // print the electrostatic potential
                        lyt.foreach_cell(
                            [this, &m, &lyt, &s1](const sidb& s2)
                            {
                                if (s1 == s2)
                                {
                                    return;
                                }

                                std::cout
                                    << fmt::format("V_{},{} = {}", s1, s2, lyt.get_electrostatic_potential(s1, s2))
                                    << std::endl;
                                std::cout << fmt::format("Z3                = {}",
                                                         m.eval(get_electrostatic_potential(s1, s2), true).as_double())
                                          << std::endl;
                            });
                    });

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

template <typename Lyt>
void exact_ground_state_simulation(const Lyt& lyt, const exact_ground_state_simulation_params& ps = {},
                                   exact_ground_state_simulation_stats<Lyt>* pst = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    exact_ground_state_simulation_stats<Lyt> st{};

    detail::exact_ground_state_simulation_impl<Lyt>{lyt, ps, st}.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EXACT_SIM_HPP

#endif  // FICTION_Z3_SOLVER