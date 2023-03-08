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
     * The number of valid layouts to enumerate. If set to `0`, no layouts are being returned. If set to `1`, only the
     * layout with minimum system energy is determined.
     */
    uint16_t number_of_valid_layouts_to_enumerate{1u};
    /**
     * Flag to indicate whether the population stability should be computed outside the solver. If set to `true`, the
     * check is performed combinatorically on results yielded by the solver. Should a layout be found to be unstable,
     * this configuration is excluded from the search space and the solver is called again.
     */
    bool compute_population_stability_outside_the_solver{false};
    /**
     * Flag to indicate whether the configuration stability should be computed outside the solver. If set to `true`, the
     * check is performed combinatorically on results yielded by the solver. Should a layout be found to be unstable,
     * this configuration is excluded from the search space and the solver is called again.
     */
    bool compute_configuration_stability_outside_the_solver{false};
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
    {}

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

    [[nodiscard]] z3::expr get_sidb_var(const sidb& s)
    {
        // two-state simulation
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            return ctx.bool_const(fmt::format("n_({},{},{})", s.x, s.y, s.z).c_str());
        }

        // three-state simulation
        return ctx.int_const(fmt::format("n_({},{},{})", s.x, s.y, s.z).c_str());
    }

    [[nodiscard]] z3::expr get_sidb_value(const sidb& s)
    {
        // two-state simulation
        if (params.simulation_states == exact_ground_state_simulation_params::simulation_states::TWO)
        {
            const auto zero         = ctx.real_val(0);
            const auto negative_one = ctx.real_val(-1);

            return ctx.real_val(z3::ite(get_sidb_var(s), zero, negative_one));
        }

        // three-state simulation
        return get_sidb_var(s);
    }

    [[nodiscard]] z3::expr get_potential_var(const sidb& s1, const sidb& s2)
    {
        return ctx.real_const(fmt::format("V_({},{},{}),({},{},{})", s1.x, s1.y, s1.z, s2.x, s2.y, s2.z).c_str());
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
                    optimizer.add(get_sidb_var(s) >= -1);
                    optimizer.add(get_sidb_var(s) <= 1);
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
                        if (s1 != s2)
                        {
                            // define the electrostatic potential between two SiDBs
                            // we do not need a minus here, because we defined the electric charge with a positive sign
                            const auto potential_val =
                                ctx.real_val(std::to_string(charge_lyt.potential_between_sidbs(s1, s2)).c_str());

                            std::cout << "potential_val: " << charge_lyt.potential_between_sidbs(s1, s2)
                                      << ", Z3's value repr: " << potential_val << std::endl;

                            optimizer.add(get_potential_var(s1, s2) == potential_val * get_sidb_value(s2));
                        }
                    });
            });
    }

    void minimize_system_energy()
    {
        const auto energy_var = ctx.real_const("E");

        z3::expr_vector energy_terms{ctx};

        charge_lyt.foreach_cell(
            [this, &energy_var, &energy_terms](const sidb& s1)
            {
                charge_lyt.foreach_cell(
                    [this, &energy_var, &energy_terms, &s1](const sidb& s2)
                    {
                        if (s1 < s2)
                        {
                            // the electric charge of an electron
                            const auto charge_val =
                                ctx.real_val(std::to_string(physical_constants::ELECTRIC_CHARGE).c_str());
                            // TODO this value is too small to be represented correctly by Z3

                            std::cout << "charge_val: " << physical_constants::ELECTRIC_CHARGE
                                      << ", Z3's value repr: " << charge_val << std::endl;

                            // add the electrostatic potential energy term
                            energy_terms.push_back(get_potential_var(s1, s2) * get_sidb_value(s1) * charge_val);
                        }
                    });
            });

        if (!energy_terms.empty())
        {
            // the system energy is the negative sum of all energy terms
            optimizer.add(energy_var == -z3::sum(energy_terms));
        }

        // minimize the system energy
        optimizer.minimize(energy_var);
    }

    void generate_smt_instance()
    {
        // restrict the values of the charge state variables if necessary (three-state simulation)
        restrict_sidb_charge_state_values();
        // define the electrostatic potential V_{i,j} between SiDBs
        define_electrostatic_potential();
        // minimize the system energy
        minimize_system_energy();
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