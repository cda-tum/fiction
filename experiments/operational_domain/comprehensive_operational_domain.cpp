//
// Created by Jan Drewniok on 02.03.25.
//

#include "fiction_experiments.hpp"  // experiment class

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>      // operational domain computation algorithms
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>  // SiDB simulation engines
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>  // SiDB simulation parameters
#include <fiction/io/read_sqd_layout.hpp>                                     // reader for SiDB layouts
#include <fiction/types.hpp>                    // pre-defined types suitable for the FCN domain
#include <fiction/utils/truth_table_utils.hpp>  // truth tables helper functions

#include <fmt/format.h>  // output formatting

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

[[nodiscard]] static std::size_t
number_of_operational_parameter_points_above_ct(const critical_temperature_domain& ct_domain, const double& ct,
                                                const std::size_t& num_evaluated_parameters) noexcept
{
    std::size_t number_of_op_parameters_above_ct = 0;
    ct_domain.for_each(
        [&ct, &number_of_op_parameters_above_ct](const auto& coord [[maybe_unused]], const auto& op_value)
        {
            if (std::get<1>(op_value) >= ct)
            {
                number_of_op_parameters_above_ct++;
            }
        });

    return number_of_op_parameters_above_ct / num_evaluated_parameters;
}

[[nodiscard]] static double minimum_ct(const critical_temperature_domain& ct_domain) noexcept
{
    double min_ct = std::numeric_limits<double>::max();

    ct_domain.for_each(
        [&min_ct](const auto& coord [[maybe_unused]], const auto& op_value)
        {
            if (std::get<1>(op_value) <= min_ct)
            {
                min_ct = std::get<1>(op_value);
            }
        });
    return min_ct;
}

[[nodiscard]] static double maximum_ct(const critical_temperature_domain& ct_domain) noexcept
{
    double max_ct = 0.0;

    ct_domain.for_each(
        [&max_ct](const auto& coord [[maybe_unused]], const auto& op_value)
        {
            if (std::get<1>(op_value) >= max_ct)
            {
                max_ct = std::get<1>(op_value);
            }
        });
    return max_ct;
}

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double, double, double, double, double, double> opdomain_exp{
        "Critical Temperature Domain Bestagon",
        "Name",
        "#SiDBs",  // Benchmark
        "CT min",
        "CT max",
        "20% of CT max",
        "40% of CT max",
        "60% of CT max",
        "80% of CT max",
    };

    // simulation parameters
    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    // operational domain parameters
    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.operational_params.sim_engine            = sidb_simulation_engine::QUICKEXACT;

    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::TOLERATE_KINKS;

    op_domain_params.sweep_dimensions         = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.5;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.5;

    //    // write operational domain parameters
    //    write_operational_domain_params write_op_domain_params{};
    //    write_op_domain_params.non_operational_tag = "0";
    //    write_op_domain_params.operational_tag     = "1";
    //    write_op_domain_params.writing_mode        =
    //    write_operational_domain_params::sample_writing_mode::ALL_SAMPLES;

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 10> gates = {
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()}),
        std::make_pair("wire_diag", std::vector<tt>{create_id_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("inv_diag", std::vector<tt>{create_not_tt()})};

    for (const auto& [gate, truth_table] : gates)
    {
        auto lyt = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate), gate);

        // operational domain stats
        operational_domain_stats op_domain_stats_gs{};

        // compute the operational domains
        const auto ct_domain =
            critical_temperature_domain_grid_search(lyt, truth_table, op_domain_params, &op_domain_stats_gs);

        const auto min_ct = minimum_ct(ct_domain);
        const auto max_ct = maximum_ct(ct_domain);

        const auto ct_20 = min_ct + 0.2 * (max_ct - min_ct);
        const auto ct_40 = min_ct + 0.4 * (max_ct - min_ct);
        const auto ct_60 = min_ct + 0.6 * (max_ct - min_ct);
        const auto ct_80 = min_ct + 0.8 * (max_ct - min_ct);

        const auto op_ratio_ct_20 = number_of_operational_parameter_points_above_ct(
            ct_domain, ct_20, op_domain_stats_gs.num_evaluated_parameter_combinations);
        const auto op_ratio_ct_40 = number_of_operational_parameter_points_above_ct(
            ct_domain, ct_40, op_domain_stats_gs.num_evaluated_parameter_combinations);
        const auto op_ratio_ct_60 = number_of_operational_parameter_points_above_ct(
            ct_domain, ct_60, op_domain_stats_gs.num_evaluated_parameter_combinations);
        const auto op_ratio_ct_80 = number_of_operational_parameter_points_above_ct(
            ct_domain, ct_80, op_domain_stats_gs.num_evaluated_parameter_combinations);

        opdomain_exp(
            // Benchmark
            gate, lyt.num_cells(),

            min_ct, max_ct,

            op_ratio_ct_20, op_ratio_ct_40, op_ratio_ct_60, op_ratio_ct_80);
        opdomain_exp.save();
        opdomain_exp.table();
    }

    return EXIT_SUCCESS;
}
