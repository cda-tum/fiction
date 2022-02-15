//
// Created by marcel on 14.02.21.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>      // routing based on graph coloring
#include <fiction/algorithms/physical_design/exact.hpp>              // SMT-based physical design of FCN layouts
#include <fiction/algorithms/verification/equivalence_checking.hpp>  // equivalence checking of FCN layouts
#include <fiction/types.hpp>                                         // pre-defined types suitable for the FCN domain
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
                     "runtime exact (sec)",
                     "runtime color routing (sec)",
                     "runtime EPG (sec)",
                     "runtime graph coloring (sec)",
                     "equivalent"};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = false;
    exact_params.border_io     = false;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;
    fiction::color_routing_stats routing_stats{};

    constexpr const uint64_t bench_select =
        fiction_experiments::all & ~fiction_experiments::par_check & ~fiction_experiments::parity &
        ~fiction_experiments::xor5_maj & ~fiction_experiments::two_bit_add_maj & ~fiction_experiments::cm82a_5 &
        ~fiction_experiments::xor5_r1 & ~fiction_experiments::b1_r2 & ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        fiction::tec_nt network{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(network));
        assert(read_verilog_result == lorina::return_code::success);

        fiction::equivalence_checking_stats equiv_stats{};

        // perform layout generation with an SMT-based exact algorithm
        auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

        if (gate_level_layout.has_value())
        {
            // extract routing objectives
            const auto objectives = fiction::extract_routing_objectives(*gate_level_layout);

            // remove routing
            clear_routing(*gate_level_layout);

            // perform routing
            const auto success = fiction::color_routing(*gate_level_layout, objectives, routing_params, &routing_stats);

            if (success)
            {
                // check equivalence
                fiction::equivalence_checking(network, *gate_level_layout, &equiv_stats);
            }

            // log results
            bestagon_exp(
                benchmark, network.num_pis(), network.num_pos(), network.num_gates(), gate_level_layout->x() + 1,
                gate_level_layout->y() + 1, (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1),
                gate_level_layout->num_gates(), gate_level_layout->num_wires(), objectives.size(),
                routing_stats.epg_stats.number_of_unsatisfiable_objectives, routing_stats.epg_stats.num_vertices,
                routing_stats.epg_stats.num_edges, mockturtle::to_seconds(exact_stats.time_total),
                mockturtle::to_seconds(routing_stats.time_total),
                mockturtle::to_seconds(routing_stats.epg_stats.time_total),
                mockturtle::to_seconds(routing_stats.color_stats.time_total), equiv_stats.eq != fiction::eq_type::NO);
        }

        bestagon_exp.save();
        bestagon_exp.table();
    }

    return 0;
}
