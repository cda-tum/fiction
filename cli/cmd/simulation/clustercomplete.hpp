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
#include <fiction/utils/layout_utils.hpp>
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
                "complexity with added SiDBs. The shorter alias command is 'ccsim'.")
    {
        add_option("--base,-b", physical_params.base, "The simulation base, can be 2 or 3", true);
        add_option("--epsilon_r,-e", physical_params.epsilon_r, "Electric permittivity of the substrate (unit-less)",
                   true);
        add_option("--lambda_tf,-l", physical_params.lambda_tf, "Thomas-Fermi screening distance (unit: nm)", true);
        add_option("--mu_minus,-m", physical_params.mu_minus, "Energy transition level (0/-) (unit: eV)", true);
        add_option("--global_potential,-g", params.global_potential,
                   "Global potential applied to the entire layout (unit: V)", true);
        add_option("--witness_partitioning_limit,-w", params.validity_witness_partitioning_max_cluster_size_gss,
                   "The limit on the cluster size before Ground State Space omits the check for which it solves the "
                   "validity witness partitioning NP-complete sub-problem; values above 15 severely impact the runtime",
                   true);
        add_option("--overlapping_witnesses_limit,-o", params.num_overlapping_witnesses_limit_gss,
                   "The limit on the number of overlapping witnesses (that determines the factorial scaling of the "
                   "sub-procedure) before Ground State Space skips validity witness partitioning",
                   true);
        add_option("--report_gss_stats,-r", params.report_gss_stats,
                   "When set to true, statistics like the number of pruned top level multisets (out of the maximum "
                   "number) and the Ground State Space runtime are shown",
                   true);
    }

  protected:
    /**
     * Function to perform the simulation call.
     */
    void execute() override
    {
        // reset sim result
        sim_result_100 = {};
        sim_result_111 = {};
        min_energy     = std::numeric_limits<double>::infinity();

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

            if constexpr (fiction::has_sidb_technology_v<Lyt>)
            {
                if constexpr (fiction::is_charge_distribution_surface_v<Lyt>)
                {
                    env->out() << fmt::format(
                        "[w] {} already possesses a charge distribution; no simulation is conducted\n",
                        get_name(lyt_ptr));
                }
                else
                {
                    params.physical_parameters = physical_params;

                    if constexpr (fiction::is_sidb_lattice_100_v<Lyt>)
                    {
                        sidb_111_used  = false;
                        sim_result_100 = fiction::clustercomplete(*lyt_ptr, params);
                    }
                    else if constexpr (fiction::is_sidb_lattice_111_v<Lyt>)
                    {
                        sidb_111_used  = true;
                        auto cps       = convert_params<Lyt>(params);
                        sim_result_111 = fiction::clustercomplete(*lyt_ptr, cps);
                    }
                    else
                    {
                        env->out() << "[e] no valid lattice orientation\n";
                        return;
                    }

                    if (sim_result_100.charge_distributions.empty() && sim_result_111.charge_distributions.empty())
                    {
                        env->out() << fmt::format("[e] ground state of {} could not be determined\n",
                                                  get_name(lyt_ptr));
                    }
                    else
                    {
                        if constexpr (fiction::is_sidb_lattice_100_v<Lyt>)
                        {
                            const auto min_energy_distr =
                                fiction::minimum_energy_distribution(sim_result_100.charge_distributions.cbegin(),
                                                                     sim_result_100.charge_distributions.cend());

                            min_energy = min_energy_distr->get_system_energy();
                            store<fiction::cell_layout_t>().extend() =
                                std::make_shared<fiction::cds_sidb_100_cell_clk_lyt>(*min_energy_distr);
                        }
                        else if constexpr (fiction::is_sidb_lattice_111_v<Lyt>)
                        {
                            const auto min_energy_distr =
                                fiction::minimum_energy_distribution(sim_result_111.charge_distributions.cbegin(),
                                                                     sim_result_111.charge_distributions.cend());

                            min_energy = min_energy_distr->get_system_energy();
                            store<fiction::cell_layout_t>().extend() =
                                std::make_shared<fiction::cds_sidb_111_cell_clk_lyt>(*min_energy_distr);
                        }
                    }
                }
            }
            else
            {
                env->out() << fmt::format("[e] {} is not an SiDB layout\n", get_name(lyt_ptr));
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
    fiction::clustercomplete_params<fiction::sidb_100_cell_clk_lyt> params{};
    /**
     * Simulation result for H-Si(100)-2x1 surface.
     */
    fiction::sidb_simulation_result<fiction::sidb_100_cell_clk_lyt> sim_result_100{};
    /**
     * Simulation result for H-Si(111)-1x1 surface.
     */
    fiction::sidb_simulation_result<fiction::sidb_111_cell_clk_lyt> sim_result_111{};
    /**
     * Minimum energy.
     */
    double min_energy{std::numeric_limits<double>::infinity()};
    /**
     * Flag to determine the SiDB lattice used for the simulation when logging
     */
    bool sidb_111_used = false;
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the simulation.
     */
    [[nodiscard]] nlohmann::json log() const override
    {
        try
        {
            if (sidb_111_used)
            {
                return nlohmann::json{
                    {"Algorithm name", sim_result_111.algorithm_name},
                    {"Simulation runtime", sim_result_111.simulation_runtime.count()},
                    {{"Simulation parameters",
                      {"Physical parameters",
                       {{"base", sim_result_111.physical_parameters.base},
                        {"epsilon_r", sim_result_111.physical_parameters.epsilon_r},
                        {"lambda_tf", sim_result_111.physical_parameters.lambda_tf},
                        {"mu_minus", sim_result_111.physical_parameters.mu_minus},
                        {"global_potential", std::any_cast<double>(sim_result_111.additional_simulation_parameters.at(
                                                 "global_potential"))}}},
                      {"validity witness partitioning limit",
                       std::any_cast<uint64_t>(
                           sim_result_111.additional_simulation_parameters.at("validity_witness_partitioning_limit"))},
                      {"number of overlapping witnesses limit",
                       std::any_cast<uint64_t>(
                           sim_result_111.additional_simulation_parameters.at("num_overlapping_witnesses_limit"))}}},
                    {"Ground state energy (eV)", min_energy},
                    {"Number of stable states", sim_result_111.charge_distributions.size()}};
            }

            return nlohmann::json{
                {"Algorithm name", sim_result_100.algorithm_name},
                {"Simulation runtime", sim_result_100.simulation_runtime.count()},
                {{"Simulation parameters",
                  {"Physical parameters",
                   {{"base", sim_result_100.physical_parameters.base},
                    {"epsilon_r", sim_result_100.physical_parameters.epsilon_r},
                    {"lambda_tf", sim_result_100.physical_parameters.lambda_tf},
                    {"mu_minus", sim_result_100.physical_parameters.mu_minus},
                    {"global_potential",
                     std::any_cast<double>(sim_result_100.additional_simulation_parameters.at("global_potential"))}}},
                  {"validity witness partitioning limit",
                   std::any_cast<uint64_t>(
                       sim_result_100.additional_simulation_parameters.at("validity_witness_partitioning_limit"))},
                  {"number of overlapping witnesses limit",
                   std::any_cast<uint64_t>(
                       sim_result_100.additional_simulation_parameters.at("num_overlapping_witnesses_limit"))}}},
                {"Ground state energy (eV)", min_energy},
                {"Number of stable states", sim_result_100.charge_distributions.size()}};
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
        params          = {};
        sim_result_100  = {};
        sim_result_111  = {};
    }

    template <typename LytDest, typename LytSrc>
    [[nodiscard]] fiction::clustercomplete_params<LytDest>
    convert_params(const fiction::clustercomplete_params<LytSrc>& ps_src) const noexcept
    {
        fiction::clustercomplete_params<LytDest> ps_dest{};

        ps_dest.physical_parameters = ps_src.physical_parameters;
        ps_dest.global_potential    = ps_src.global_potential;

        return ps_dest;
    }
};

using ccsim_command = clustercomplete_command;

ALICE_ADD_COMMAND(clustercomplete, "Simulation")
ALICE_ADD_COMMAND(ccsim, "Simulation")

}  // namespace alice

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CMD_CLUSTERCOMPLETE_HPP
