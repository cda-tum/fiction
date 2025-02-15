//
// Created by Willem Lambooy on 07.03.2024.
//

#ifndef FICTION_CMD_CLUSTERCOMPLETE_HPP
#define FICTION_CMD_CLUSTERCOMPLETE_HPP

#if (FICTION_ALGLIB_ENABLED)

#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
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
 * Executes *ClusterComplete* exact simulation for the current SiDB cell-level layout in store.
 */
class clustercomplete_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit clustercomplete_command(const environment::ptr& e) :
            command(
                e,
                "ClusterComplete is a proof of concept of the more general idea of state space pruning in a cluster "
                "hierarchy. In the application on SiDB layouts, it is able to simulate SiDB logic in base 3 for 50 DBs "
                "and more, depending on the layout that determines base of the exponential growth in simulation "
                "complexity with added SiDBs.")
    {
        add_option("--base,-b", physical_params.base, "The simulation base, can be 2 or 3", true);
        add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
        add_option("--global_potential,-g", cc_params.global_potential,
                   "Global potential applied to the entire layout (unit: V)", true);
        add_option("--witness_partitioning_limit,-w", cc_params.validity_witness_partitioning_max_cluster_size_gss,
                   "The limit on the cluster size before Ground State Space omits the check for which it solves the "
                   "validity witness partitioning NP-complete sub-problem",
                   true);
        add_option("--overlapping_witnesses_limit,-o", cc_params.num_overlapping_witnesses_limit_gss,
                   "The limit on the number of overlapping witnesses (that determines the factorial scaling of the "
                   "sub-procedure) before Ground State Space skips validity witness partitioning",
                   true);
        add_flag("--report_gss_stats,-r",
                 "When set, Ground State Space statistics are shown, which give an estimate for the ClusterComplete "
                 "runtimes, and thus allow the user to configure the validity witness partitioning parameters that "
                 "could benefit large simulation problems through more intensive pruning before starting a lengthy "
                 "unfolding process");
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

        const auto clustercomplete = [this, &get_name](auto&& lyt_ptr)
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if constexpr (!fiction::has_sidb_technology_v<Lyt>)
            {
                env->out() << fmt::format("[e] '{}' is not an SiDB layout\n", get_name(lyt_ptr));
                return;
            }

            if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
            {
                env->out() << fmt::format(
                    "[w] '{}' already possesses a charge distribution; no simulation is conducted\n",
                    get_name(lyt_ptr));
                return;
            }

            if (physical_params.base != 2 && physical_params.base != 3)
            {
                env->out() << "[e] The simulation base has to be 2 or 3\n";
                return;
            }

            cc_params.simulation_parameters = physical_params;

            // To aid the compiler
            if constexpr (fiction::has_sidb_technology_v<Lyt> && !fiction::is_charge_distribution_surface_v<Lyt>)
            {
                cc_params.report_gss_stats =
                    is_set("report_gss_stats") ?
                        fiction::clustercomplete_params<fiction::cell<Lyt>>::ground_state_space_reporting::ON :
                        fiction::clustercomplete_params<fiction::cell<Lyt>>::ground_state_space_reporting::OFF;

                sim_result = fiction::clustercomplete(*lyt_ptr, cc_params);

                if constexpr (fiction::is_sidb_lattice_100_v<Lyt>)
                {
                    if (std::get<sim_result_100>(sim_result).charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] ground state of '{}' could not be determined\n",
                                                  get_name(lyt_ptr));
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
                        env->out() << fmt::format("[e] ground state of '{}' could not be determined\n",
                                                  get_name(lyt_ptr));
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

        std::visit(clustercomplete, s.current());

        reset_params();
    }

  private:
    /**
     * Physical parameters for the simulation.
     */
    fiction::sidb_simulation_parameters physical_params{3, -0.32, 5.6, 5.0};
    /**
     * ClusterComplete parameters.
     */
    fiction::clustercomplete_params<> cc_params{};
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
                                      {"Number of stable states", sim_res.charge_distributions.size()},
                                      {"Validity witness partitioning limit",
                                       std::any_cast<uint64_t>(sim_res.additional_simulation_parameters.at(
                                           "validity_witness_partitioning_limit"))},
                                      {"Number of overlapping witnesses limit",
                                       std::any_cast<uint64_t>(sim_res.additional_simulation_parameters.at(
                                           "num_overlapping_witnesses_limit"))}};
            }

            const auto& sim_res = std::get<sim_result_111>(sim_result);

            return nlohmann::json{{"Algorithm name", sim_res.algorithm_name},
                                  {"Simulation runtime", sim_res.simulation_runtime.count()},
                                  {"Physical parameters",
                                   {{"epsilon_r", sim_res.simulation_parameters.epsilon_r},
                                    {"lambda_tf", sim_res.simulation_parameters.lambda_tf},
                                    {"mu_minus", sim_res.simulation_parameters.mu_minus}}},
                                  {"Lowest state energy (eV)", min_energy},
                                  {"Number of stable states", sim_res.charge_distributions.size()},
                                  {"Validity witness partitioning limit",
                                   std::any_cast<uint64_t>(sim_res.additional_simulation_parameters.at(
                                       "validity_witness_partitioning_limit"))},
                                  {"Number of overlapping witnesses limit",
                                   std::any_cast<uint64_t>(sim_res.additional_simulation_parameters.at(
                                       "num_overlapping_witnesses_limit"))}};
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
        physical_params = fiction::sidb_simulation_parameters{3, -0.32, 5.6, 5.0};
        cc_params       = {};
        sim_result      = {};
    }
};

ALICE_ADD_COMMAND(clustercomplete, "Simulation")

}  // namespace alice

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CMD_CLUSTERCOMPLETE_HPP
