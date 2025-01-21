//
// Created by marcel on 06.12.23.
//

#ifndef FICTION_CMD_QUICKEXACT_HPP
#define FICTION_CMD_QUICKEXACT_HPP

#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <any>
#include <cstdint>
#include <iostream>
#include <limits>
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
        add_option("--global_potential,-g", qe_params.global_potential,
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
        min_energy = std::numeric_limits<double>::infinity();

        if (physical_params.epsilon_r <= 0)
        {
            env->out() << "[e] epsilon_r must be positive\n";
            reset_params();
            return;
        }
        if (physical_params.lambda_tf <= 0)
        {
            env->out() << "[e] lambda_tf must be positive\n";
            reset_params();
            return;
        }

        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store\n";
            reset_params();
            return;
        }

        const auto get_name = [](auto&& lyt_ptr) -> std::string { return fiction::get_name(*lyt_ptr); };

        const auto quickexact = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (!fiction::has_sidb_technology_v<Lyt>)
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout\n", get_name(lyt_ptr));
                return;
            }

            if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
            {
                env->out() << fmt::format(
                    "[w] {} already possesses a charge distribution; no simulation is conducted\n", get_name(lyt_ptr));
                return;
            }

            qe_params.simulation_parameters = physical_params;

            // To aid the compiler
            if constexpr (fiction::has_sidb_technology_v<Lyt> && !fiction::is_charge_distribution_surface_v<Lyt>)
            {
                sim_result = fiction::quickexact(*lyt_ptr, qe_params);

                if constexpr (fiction::is_sidb_lattice_100_v<Lyt>)
                {

                    if (std::get<sim_result_100>(sim_result).charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] ground state of {} could not be determined\n",
                                                  get_name(lyt_ptr));
                        return;
                    }

                    const auto min_energy_distr =
                        minimum_energy_distribution(std::get<sim_result_100>(sim_result).charge_distributions.cbegin(),
                                                    std::get<sim_result_100>(sim_result).charge_distributions.cend());

                    min_energy = min_energy_distr->get_system_energy();
                    store<fiction::cell_layout_t>().extend() =
                        std::make_shared<fiction::cds_sidb_100_cell_clk_lyt>(*min_energy_distr);
                }
                else if constexpr (fiction::is_sidb_lattice_111_v<Lyt>)
                {
                    if (std::get<sim_result_111>(sim_result).charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] ground state of {} could not be determined\n",
                                                  get_name(lyt_ptr));
                        return;
                    }

                    const auto min_energy_distr =
                        minimum_energy_distribution(std::get<sim_result_111>(sim_result).charge_distributions.cbegin(),
                                                    std::get<sim_result_111>(sim_result).charge_distributions.cend());

                    min_energy = min_energy_distr->get_system_energy();
                    store<fiction::cell_layout_t>().extend() =
                        std::make_shared<fiction::cds_sidb_111_cell_clk_lyt>(*min_energy_distr);
                }
                else
                {
                    env->out() << "[e] no valid lattice orientation\n";
                }
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
    fiction::quickexact_params<> qe_params{};
    /**
     * Type alias for H-Si(100)-2x1 simulation result.
     */
    using sim_result_100 = fiction::sidb_simulation_result<fiction::sidb_100_cell_clk_lyt>;
    /**
     * Type alias for H-Si(111)-1x1 simulation result.
     */
    using sim_result_111 = fiction::sidb_simulation_result<fiction::sidb_111_cell_clk_lyt>;
    /**
     * Simulation result for either the H-Si(100)-2x1 or the H-Si(111)-1x1 surface.
     */
    std::variant<sim_result_100, sim_result_111> sim_result;
    /**
     * Minimum energy.
     */
    double min_energy{std::numeric_limits<double>::infinity()};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    [[nodiscard]] nlohmann::json log() const override
    {
        try
        {
            if (std::holds_alternative<sim_result_100>(sim_result))
            {
                const auto& sim_res = std::get<sim_result_100>(sim_result);

                return nlohmann::json{{"Algorithm name", sim_res.algorithm_name},
                                      {"Simulation runtime", sim_res.simulation_runtime.count()},
                                      {"Physical parameters",
                                       {{"epsilon_r", sim_res.simulation_parameters.epsilon_r},
                                        {"lambda_tf", sim_res.simulation_parameters.lambda_tf},
                                        {"mu_minus", sim_res.simulation_parameters.mu_minus}}},
                                      {"Lowest state energy (eV)", min_energy},
                                      {"Number of stable states", sim_res.charge_distributions.size()}};
            }

            const auto& sim_res = std::get<sim_result_111>(sim_result);

            return nlohmann::json{{"Algorithm name", sim_res.algorithm_name},
                                  {"Simulation runtime", sim_res.simulation_runtime.count()},
                                  {"Physical parameters",
                                   {{"epsilon_r", sim_res.simulation_parameters.epsilon_r},
                                    {"lambda_tf", sim_res.simulation_parameters.lambda_tf},
                                    {"mu_minus", sim_res.simulation_parameters.mu_minus}}},
                                  {"Lowest state energy (eV)", min_energy},
                                  {"Number of stable states", sim_res.charge_distributions.size()}};
        }
        catch (...)
        {
            return nlohmann::json{};
        }
    }

    /**
     * Resets the parameters to their default values.
     */
    void reset_params()
    {
        physical_params = fiction::sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
        qe_params       = {};
        sim_result      = {};
    }
};

ALICE_ADD_COMMAND(quickexact, "Simulation")

}  // namespace alice

#endif  // FICTION_CMD_QUICKEXACT_HPP
