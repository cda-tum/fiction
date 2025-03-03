//
// Created by Jan Drewniok on 02.03.25.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

/*
 * Compute the operational domain for a given CT in relation to the critical temperature domain at 0 K.
 *
 * @param ct_domain The critical temperature domain at 0 K.
 * @param ct The critical temperature for which the operational domain should be computed.
 * @param num_op_params The number of operational parameters.
 * @return The operational domain ratio in percent.
 */
[[nodiscard]] static double compute_op_domain_relative_to_0_k(const critical_temperature_domain& ct_domain,
                                                              const double ct, const std::size_t num_op_params)
{
    std::size_t count = 0;

    ct_domain.for_each(
        [&ct, &count](const auto&, const auto& op_value)
        {
            if (std::get<0>(op_value) == operational_status::OPERATIONAL && std::get<1>(op_value) >= ct)
            {
                count++;
            }
        });

    return static_cast<double>(count) / static_cast<double>(num_op_params) * 100;
}

[[nodiscard]] static std::array<double, 6>
compute_op_domains_relative_to_0_k(const critical_temperature_domain& ct_domain,
                                   const operational_domain_stats& op_domain_stats_gs, double min_ct, double max_ct)
{
    std::array<double, 6> op_ratios{};

    // Critical temperatures at 0%, 20%, 40%, 60%, 80%, and 100% of the temperature range. 20% means, for example, "CT
    // min + 0.2 * ΔCT", where ΔCT is "CT max - CT min".
    const std::array<double, 6> relative_temperature = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};

    for (uint64_t i = 0; i < relative_temperature.size(); i++)
    {
        const auto ct = min_ct + (relative_temperature[i] * (max_ct - min_ct));
        op_ratios[i] =
            compute_op_domain_relative_to_0_k(ct_domain, ct, op_domain_stats_gs.num_operational_parameter_combinations);
    }

    return op_ratios;
}

// This script analyzes the critical temperature within the operational domain of the Bestagon gates. It calculates
// the operational domain at various temperatures relative to the total operational domain at 0 K.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double, double, double, double, double, double, double, double>
        opdomain_exp{
            "Critical Temperature Domain Bestagon",
            "Name",
            "#SiDBs",  // Benchmark
            "CT min",
            "CT max",
            "Rel. OpD (CT min)",
            "Rel. OpD (CT min + 0.2 * ΔCT) [%]",  //
            "Rel. OpD (CT min + 0.4 * ΔCT) [%]",
            "Rel. OpD (CT min + 0.6 * ΔCT) [%]",
            "Rel. OpD (CT min + 0.8 * ΔCT) [%]",
            "Rel. OpD (CT max)",
        };

    // simulation parameters
    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    // operational domain parameters
    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.operational_params.sim_engine            = sidb_simulation_engine::QUICKEXACT;

    op_domain_params.sweep_dimensions         = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.01;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.01;

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::vector<tt>, std::string>, 14> gates = {
        {{std::vector<tt>{create_id_tt()}, "wire"},
         {std::vector<tt>{create_id_tt()}, "wire_diag"},
         {std::vector<tt>{create_not_tt()}, "inv"},
         {std::vector<tt>{create_not_tt()}, "inv_diag"},
         {std::vector<tt>{create_and_tt()}, "and"},
         {std::vector<tt>{create_nand_tt()}, "nand"},
         {std::vector<tt>{create_or_tt()}, "or"},
         {std::vector<tt>{create_nor_tt()}, "nor"},
         {std::vector<tt>{create_xor_tt()}, "xor"},
         {std::vector<tt>{create_xnor_tt()}, "xnor"},
         {create_fan_out_tt(), "fo2"},
         {create_crossing_wire_tt(), "cx"},
         {create_half_adder_tt(), "ha"},
         {create_double_wire_tt(), "hourglass"}}};

    for (const auto& [truth_table, gate] : gates)
    {
        auto lyt = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate), gate);

        // Loop over operational conditions
        for (uint64_t i = 0; i < 2; i++)
        {
            operational_domain_stats op_domain_stats_gs{};
            std::string              gate_name = gate;

            // tolerate kinks first
            if (i == 0)
            {
                op_domain_params.operational_params.op_condition =
                    is_operational_params::operational_condition::TOLERATE_KINKS;
            }

            // reject kinks afterwards
            else
            {
                op_domain_params.operational_params.op_condition =
                    is_operational_params::operational_condition::REJECT_KINKS;
            }

            if (i == 1)
            {
                gate_name += " (reject kinks)";  // Update gate name for kink rejection case
            }

            const auto ct_domain =
                critical_temperature_domain_grid_search(lyt, truth_table, op_domain_params, &op_domain_stats_gs);

            const auto min_ct = ct_domain.minimum_ct();
            const auto max_ct = ct_domain.maximum_ct();

            // Calculate operational ratios at various critical temperatures
            const auto op_ratios = compute_op_domains_relative_to_0_k(ct_domain, op_domain_stats_gs, min_ct, max_ct);

            // Unpack operational ratios (from 0% to 100%)
            const auto [op_ratio_ct_0, op_ratio_ct_20, op_ratio_ct_40, op_ratio_ct_60, op_ratio_ct_80,
                        op_ratio_ct_100] = op_ratios;

            // Benchmark and save
            opdomain_exp(gate_name, lyt.num_cells(), min_ct, max_ct, op_ratio_ct_0, op_ratio_ct_20, op_ratio_ct_40,
                         op_ratio_ct_60, op_ratio_ct_80, op_ratio_ct_100);
            opdomain_exp.save();
            opdomain_exp.table();
        }
    }

    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

    return EXIT_SUCCESS;
}
