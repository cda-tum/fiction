//
// Created by Simon Hofmann on 30.08.24.
//
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/algorithms/verification/equivalence_checking.hpp>
#include <fiction/io/network_reader.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream                        os{};
    fiction::network_reader<fiction::tec_ptr> reader{fiction_experiments::benchmark_path(name), os};
    const auto                                nets    = reader.get_networks();
    const auto                                network = *nets.front();

    return network;
}

int main()  // NOLINT
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, std::string, uint64_t, uint64_t, uint64_t, uint64_t, double, std::string>
        gold_cost_objectives_exp{"graph_oriented_layout_design_exp",
                                 "benchmark",
                                 "cost objective",
                                 "A",
                                 "|C|",
                                 "|W|",
                                 "ACP",
                                 "runtime (in sec)",
                                 "equivalent"};

    fiction::graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
    fiction::graph_oriented_layout_design_params graph_oriented_layout_design_params{};
    graph_oriented_layout_design_params.mode =
        fiction::graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
    graph_oriented_layout_design_params.verbose      = true;
    graph_oriented_layout_design_params.return_first = false;
    graph_oriented_layout_design_params.timeout      = 60000;

    static constexpr const uint64_t bench_select = fiction_experiments::trindade16 | fiction_experiments::fontes18;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        for (const auto& cost_pair :
             std::vector<std::pair<fiction::graph_oriented_layout_design_params::cost_objective, std::string>>{
                 {fiction::graph_oriented_layout_design_params::cost_objective::AREA, "A"},
                 {fiction::graph_oriented_layout_design_params::cost_objective::WIRES, "|W|"},
                 {fiction::graph_oriented_layout_design_params::cost_objective::CROSSINGS, "|C|"},
                 {fiction::graph_oriented_layout_design_params::cost_objective::ACP, "ACP"}})
        {
            const auto& [cost, cost_name]            = cost_pair;
            graph_oriented_layout_design_params.cost = cost;

            auto network = read_ntk<fiction::tec_nt>(benchmark);

            auto gate_level_layout = fiction::graph_oriented_layout_design<gate_lyt, fiction::tec_nt>(
                network, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

            if (gate_level_layout.has_value())
            {
                // check equivalence
                const auto eq_stats =
                    fiction::equivalence_checking<fiction::technology_network, gate_lyt>(network, *gate_level_layout);

                std::string eq_result = "NO";
                if (eq_stats == fiction::eq_type::STRONG)
                {
                    eq_result = "STRONG";
                }
                else if (eq_stats == fiction::eq_type::WEAK)
                {
                    eq_result = "WEAK";
                };
                // calculate bounding box
                const auto bounding_box = fiction::bounding_box_2d(*gate_level_layout);

                const auto width  = bounding_box.get_x_size() + 1;
                const auto height = bounding_box.get_y_size() + 1;
                const auto area   = width * height;

                const auto num_wires =
                    gate_level_layout->num_wires() - gate_level_layout->num_pis() - gate_level_layout->num_pos();
                const auto num_crossings = gate_level_layout->num_crossings();

                // log results
                gold_cost_objectives_exp(
                    benchmark, cost_name, area, num_crossings, num_wires, area * (num_crossings + 1),
                    mockturtle::to_seconds(graph_oriented_layout_design_stats.time_total), eq_result);
            }

            gold_cost_objectives_exp.save();
            gold_cost_objectives_exp.table();
        }
    }

    return EXIT_SUCCESS;
}
