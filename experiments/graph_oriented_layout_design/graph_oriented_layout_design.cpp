//
// Created by Simon Hofmann on 30.01.24.
//
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>  // graph enhanced layout search algorithm
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/layouts/bounding_box.hpp>                                       // calculate area of generated layouts
#include <fiction/layouts/cartesian_layout.hpp>                                   // cartesian layout
#include <fiction/layouts/clocked_layout.hpp>                                     // clocked layout
#include <fiction/layouts/gate_level_layout.hpp>                                  // gate-level layout
#include <fiction/layouts/tile_based_layout.hpp>                                  // tile-based layout
#include <fiction/networks/technology_network.hpp>                                // technology network

#include <fmt/format.h>  // output formatting

#include <cstdlib>
#include <ostream>
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

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, double, std::string>
        graph_oriented_layout_design_exp{"graph_oriented_layout_design_exp",
                                         "benchmark",
                                         "inputs",
                                         "outputs",
                                         "initial nodes",
                                         "layout width (in tiles)",
                                         "layout height (in tiles)",
                                         "layout area (in tiles)",
                                         "gates",
                                         "wires",
                                         "critical path",
                                         "throughput",
                                         "runtime graph_oriented_layout_design (in sec)",
                                         "equivalent"};

    fiction::graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
    fiction::graph_oriented_layout_design_params graph_oriented_layout_design_params{};
    graph_oriented_layout_design_params.high_effort_mode = false;
    graph_oriented_layout_design_params.verbose          = true;
    graph_oriented_layout_design_params.return_first     = false;

    static constexpr const uint64_t bench_select = fiction_experiments::parity;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        auto network = read_ntk<fiction::tec_nt>(benchmark);

        auto gate_level_layout = fiction::graph_oriented_layout_design<gate_lyt, fiction::tec_nt>(
            network, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

        if (gate_level_layout.has_value())
        {
            //  compute critical path and throughput
            const auto cp_tp = fiction::critical_path_length_and_throughput(*gate_level_layout);

            // check equivalence
            const auto eq_stats =
                fiction::equivalence_checking<fiction::technology_network, gate_lyt>(network, *gate_level_layout);

            const std::string eq_result = eq_stats == fiction::eq_type::STRONG ? "STRONG" :
                                          eq_stats == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                 "NO";

            // calculate bounding box
            const auto bounding_box = fiction::bounding_box_2d(*gate_level_layout);

            const auto width  = bounding_box.get_x_size() + 1;
            const auto height = bounding_box.get_y_size() + 1;
            const auto area   = width * height;

            // log results
            graph_oriented_layout_design_exp(
                benchmark, network.num_pis(), network.num_pos(), network.num_gates(), width, height, area,
                gate_level_layout->num_gates(), gate_level_layout->num_wires(), cp_tp.critical_path_length,
                cp_tp.throughput, mockturtle::to_seconds(graph_oriented_layout_design_stats.time_total), eq_result);
        }

        graph_oriented_layout_design_exp.save();
        graph_oriented_layout_design_exp.table();
    }

    return EXIT_SUCCESS;
}
