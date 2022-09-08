//
// Created by marcel on 14.02.21.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>      // routing based on graph coloring
#include <fiction/algorithms/physical_design/orthogonal.hpp>         // OGD-based physical design of FCN layouts
#include <fiction/algorithms/verification/equivalence_checking.hpp>  // equivalence checking of FCN layouts
#include <fiction/io/print_layout.hpp>                               // layout printer
#include <fiction/types.hpp>                                         // pre-defined types suitable for the FCN domain
#include <fiction/utils/debug/network_writer.hpp>                    // DOT writer
#include <fiction/utils/routing_utils.hpp>                           // routing utility functions

#include <fmt/format.h>                      // output formatting
#include <lorina/lorina.hpp>                 // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/io/verilog_reader.hpp>  // call-backs to read Verilog files into networks

#include <cstdint>
#include <string>

int main()
{
    using gate_lyt = fiction::cart_gate_clk_lyt;

    const std::string layouts_folder = fmt::format("{}/color_routing/layouts", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint64_t, double, double, double, double, bool>

        bestagon_exp{"color_routing",
                     "benchmark",
                     "inputs",
                     "outputs",
                     "initial nodes",
                     "layout width (tiles)",
                     "layout height (tiles)",
                     "layout area (tiles)",
                     "gates",
                     "wires",
                     "routing objectives",
                     "unsat. objectives",
                     "EPG vertices",
                     "EPG edges",
                     "runtime ortho (sec)",
                     "runtime color routing (sec)",
                     "runtime EPG (sec)",
                     "runtime graph coloring (sec)",
                     "equivalent"};

    fiction::orthogonal_physical_design_stats ortho_stats{};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 12;
    routing_params.engine                  = fiction::graph_coloring_engine::MCS;
    fiction::color_routing_stats routing_stats{};

    constexpr const uint64_t bench_select = fiction_experiments::all;

    //    constexpr const uint64_t bench_select =
    //        fiction_experiments::all & ~fiction_experiments::par_check & ~fiction_experiments::one_bit_add_aoig &
    //        ~fiction_experiments::c17 &
    //        ~fiction_experiments::majority  //& ~fiction_experiments::t  // t works but takes long
    //        & ~fiction_experiments::majority_5_r1 & ~fiction_experiments::newtag & ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        fiction::tec_nt network{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(network));
        assert(read_verilog_result == lorina::return_code::success);

        fiction::equivalence_checking_stats equiv_stats{};

        std::cout << "Read network" << std::endl;

        // perform layout generation with an OGD-based scalable algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);
        fiction::print_gate_level_layout(std::cout, gate_level_layout);

        std::cout << "Created Layout" << std::endl;

        fiction::debug::write_dot_layout(gate_level_layout, gate_level_layout.get_layout_name() + "_ortho");

        // extract routing objectives
        const auto objectives = fiction::extract_routing_objectives(gate_level_layout);

        std::cout << "Extracted the following objectives:" << std::endl;

        for (const auto& o : objectives)
        {
            std::cout << fmt::format("\t{}-->{}", o.source, o.target) << std::endl;
        }

        std::cout << "Extracted objectives" << std::endl;

        // remove routing
        clear_routing(gate_level_layout);

        std::cout << "Cleared routing" << std::endl;

        fiction::print_gate_level_layout(std::cout, gate_level_layout);

        // perform routing
        const auto success = fiction::color_routing(gate_level_layout, objectives, routing_params, &routing_stats);

        std::cout << "Color routing done" << std::endl;
        fiction::print_gate_level_layout(std::cout, gate_level_layout);

        if (success)
        {
            // check equivalence
            fiction::equivalence_checking(network, gate_level_layout, &equiv_stats);

            std::cout << "Checked equivalence" << std::endl;
        }

        // log results
        bestagon_exp(benchmark, network.num_pis(), network.num_pos(), network.num_gates(), gate_level_layout.x() + 1,
                     gate_level_layout.y() + 1, (gate_level_layout.x() + 1) * (gate_level_layout.y() + 1),
                     gate_level_layout.num_gates(), gate_level_layout.num_wires(), objectives.size(),
                     routing_stats.number_of_unsatisfied_objectives, routing_stats.epg_stats.num_vertices,
                     routing_stats.epg_stats.num_edges, mockturtle::to_seconds(ortho_stats.time_total),
                     mockturtle::to_seconds(routing_stats.time_total),
                     mockturtle::to_seconds(routing_stats.epg_stats.time_total),
                     mockturtle::to_seconds(routing_stats.color_stats.time_total),
                     equiv_stats.eq != fiction::eq_type::NO);

        std::cout << "Logged results" << std::endl;

        bestagon_exp.save();
        bestagon_exp.table();
    }

    return 0;
}
