/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Graph-oriented layout design across the benchmark set.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "fiction_experiments.hpp"

#include <fiction/layouts/bounding_box.hpp>                              // calculate area of generated layouts
#include <fiction/networks/io/network_reader.hpp>                        // read networks from files
#include <fiction/physical_design/graph_oriented_layout_design.hpp>      // graph-oriented layout design algorithm
#include <fiction/verification/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/verification/equivalence_checking.hpp>                 // SAT-based equivalence checking

#include <fmt/format.h>  // output formatting

#include <cstdlib>
#include <ostream>
#include <string>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::networks::io;
using namespace fiction::physical_design;
using namespace fiction::verification;

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream      os{};
    network_reader<tec_ptr> reader{fiction_experiments::benchmark_path(name), os};
    const auto              nets    = reader.get_networks();
    const auto              network = *nets.front();

    return network;
}

int main()  // NOLINT
{
    using gate_lyt = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

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

    graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
    graph_oriented_layout_design_params graph_oriented_layout_design_params{};
    graph_oriented_layout_design_params.mode         = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
    graph_oriented_layout_design_params.verbose      = true;
    graph_oriented_layout_design_params.return_first = false;

    static constexpr const uint64_t bench_select = fiction_experiments::trindade16 | fiction_experiments::fontes18;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        auto network = read_ntk<tec_nt>(benchmark);

        auto gate_level_layout = graph_oriented_layout_design<gate_lyt, tec_nt>(
            network, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

        if (gate_level_layout.has_value())
        {
            //  compute critical path and throughput
            const auto cp_tp = critical_path_length_and_throughput(*gate_level_layout);

            // check equivalence
            const auto eq_stats = equivalence_checking<technology_network, gate_lyt>(network, *gate_level_layout);

            const std::string eq_result = eq_stats == eq_type::STRONG ? "STRONG" :
                                          eq_stats == eq_type::WEAK   ? "WEAK" :
                                                                        "NO";

            // calculate bounding box
            const auto bounding_box = bounding_box_2d(*gate_level_layout);

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
