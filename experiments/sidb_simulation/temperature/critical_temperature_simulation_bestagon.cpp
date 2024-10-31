//
// Created by Jan Drewniok on 03.09.24.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, double, double> simulation_exp{
        "Benchmark",
        "Gate Name",
        "Critical Temperature [K]",
        "E_{g,err} [meV]",
    };

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 12> gates = {
        std::make_pair("hourglass", create_double_wire_tt()),
        std::make_pair("cx", create_crossing_wire_tt()),
        std::make_pair("ha", create_half_adder_tt()),
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("fo2", std::vector<tt>{create_fan_out_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()})};

    const sidb_simulation_parameters  sim_params{2, -0.32};
    const critical_temperature_params ct_params{sim_params};

    for (const auto& [gate, truth_table] : gates)
    {
        for (const auto& file : std::filesystem::directory_iterator(fmt::format("{}{}", folder, gate)))
        {
            const auto layout = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(file.path().string());

            critical_temperature_stats ct_stats{};

            const auto ct =
                critical_temperature_gate_based<sidb_100_cell_clk_lyt_siqad>(layout, truth_table, ct_params, &ct_stats);

            simulation_exp(gate, ct, ct_stats.energy_between_ground_state_and_first_erroneous);
            simulation_exp.save();
            simulation_exp.table();
        }
    }
    return EXIT_SUCCESS;
}
