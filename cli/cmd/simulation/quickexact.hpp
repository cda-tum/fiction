//
// Created by marcel on 06.12.23.
//

#ifndef FICTION_CMD_QUICKEXACT_HPP
#define FICTION_CMD_QUICKEXACT_HPP

#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{
/**
 *
 */
class quickexact_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit quickexact_command(const environment::ptr& e) :
            command(e, "QuickExact is a quick and exact electrostatic ground state simulation algorithm designed "
                       "specifically for SiDB layouts. It provides a significant performance advantage of more than "
                       "three orders of magnitude over ExGS from SiQAD.")
    {
        add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
        add_option("--global_potential,-g", params.global_potential,
                   "Global potential applied to the entire layout (unit: V)", true);
    }

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override
    {
        // reset sim result
        sim_result = {};

        if (physical_params.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive" << std::endl;
            reset_params();
            return;
        }
        if (physical_params.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive" << std::endl;
            reset_params();
            return;
        }

        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            reset_params();
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto quickexact = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (fiction::has_sidb_technology_v<Lyt>)
            {
                if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
                {
                    env->out() << fmt::format(
                                      "[w] {} already possesses a charge distribution; no simulation is conducted",
                                      get_name(lyt_ptr))
                               << std::endl;
                }
                else
                {
                    params.physical_parameters = physical_params;
                    params.base_number_detection =
                        fiction::quickexact_params<fiction::sidb_cell_clk_lyt>::automatic_base_number_detection::OFF;

                    sim_result = fiction::quickexact(*lyt_ptr, params);

                    if (sim_result.charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] ground state of {} could not be determined", get_name(lyt_ptr))
                                   << std::endl;
                    }
                    else
                    {
                        const auto min_energy_distr = fiction::minimum_energy_distribution(
                            sim_result.charge_distributions.cbegin(), sim_result.charge_distributions.cend());

                        store<fiction::cell_layout_t>().extend() =
                            std::make_shared<fiction::cds_sidb_cell_clk_lyt>(*min_energy_distr);
                    }
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout", get_name(lyt_ptr)) << std::endl;
            }
        };

        std::visit(quickexact, s.current());

        reset_params();
    }

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{2, -0.32, 5.6, 5.0};
    /**
     * QuickExact parameters.
     */
    fiction::quickexact_params<fiction::sidb_cell_clk_lyt> params{};
    /**
     * Simulation result.
     */
    fiction::sidb_simulation_result<fiction::sidb_cell_clk_lyt> sim_result{};

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    nlohmann::json log() const override
    {
        return nlohmann::json{{"Algorithm name", sim_result.algorithm_name},
                              {"Simulation runtime", sim_result.simulation_runtime.count()},
                              {"Physical parameters",
                               {"base", sim_result.physical_parameters.base},
                               {"epsilon_r", sim_result.physical_parameters.epsilon_r},
                               {"lambda_tf", sim_result.physical_parameters.lambda_tf},
                               {"mu_minus", sim_result.physical_parameters.mu_minus}},
                              {"Ground state energy (eV)", sim_result.charge_distributions.front().get_system_energy()},
                              {"Number of stable states", sim_result.charge_distributions.size()}};
    }
    /**
     * Resets the parameters to their default values.
     */
    void reset_params()
    {
        physical_params = fiction::sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
        params          = {};
        params.base_number_detection =
            fiction::quickexact_params<fiction::sidb_cell_clk_lyt>::automatic_base_number_detection::OFF;
    }
};

ALICE_ADD_COMMAND(quickexact, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_QUICKEXACT_HPP
