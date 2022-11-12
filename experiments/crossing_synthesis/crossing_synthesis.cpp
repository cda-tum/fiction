//
// Created by marcel on 16.11.21.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_conversion.hpp>  // conversion between network types
#include <fiction/algorithms/physical_design/exact.hpp>                      // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // equivalence checking
#include <fiction/io/dot_drawers.hpp>                                             // networks and layout DOT drawers
#include <fiction/io/network_reader.hpp>                                          // read folders of networks
#include <fiction/types.hpp>                                                      // pre-defined types

#include <fmt/format.h>                      // output formatting
#include <mockturtle/io/write_dot.hpp>       // write networks to DOT
#include <mockturtle/io/write_verilog.hpp>   // write networks to Verilog
#include <mockturtle/networks/buffered.hpp>  // buffered network types
#include <mockturtle/networks/crossed.hpp>   // crossed network types
#include <mockturtle/views/depth_view.hpp>   // to determine network levels
#include <mockturtle/views/rank_view.hpp>    // to sort network nodes within their levels

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    using gate_ntk_ptr = fiction::tec_ptr;
    using crossed_ntk  = mockturtle::buffered_crossed_klut_network;
    using rank_ntk     = mockturtle::rank_view<crossed_ntk>;
    using gate_lyt     = fiction::hex_even_row_gate_clk_lyt;

    const std::string npn_database = fmt::format("{}/crossing_synthesis/4-input-npn-exact/", EXPERIMENTS_PATH);
    const std::string outputs      = fmt::format("{}/crossing_synthesis/outputs/", EXPERIMENTS_PATH);

    constexpr const std::array databases{"aig", "xag"};

    experiments::experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, double>
        crossing_synth_exp{"crossing_synthesis",
                           "benchmark",
                           "permutation",
                           "inputs",
                           "outputs",
                           "nodes",
                           "depth",
                           "layout width (in tiles)",
                           "layout height (in tiles)",
                           "layout area (in tiles)",
                           "gates",
                           "wires",
                           "crossings",
                           "critical path",
                           "throughput",
                           "runtime (in sec)"};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::row_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    // statistics for equivalence checking
    fiction::equivalence_checking_stats eq_stats{};

    // statistics for critical path and throughput calculations
    fiction::critical_path_length_and_throughput_stats cp_tp_stats{};

    // primary input permutation
    static std::vector<uint32_t> permutation{0, 1, 2, 3};

    // for each database types
    for (const auto& db : databases)
    {
        // read a folder of networks
        fiction::network_reader<gate_ntk_ptr> ntk_reader{fmt::format("{}{}/", npn_database, db), std::cout};
        const auto&                           networks = ntk_reader.get_networks();

        // for each benchmark
        for (const auto& benchmark : networks)
        {
            do {
                const auto permutation_str = fmt::format("{}", fmt::join(permutation, ""));

                fmt::print("[i] processing {}: {} (p: {})\n", db, benchmark->get_network_name(), permutation_str);

                // compute depth
                mockturtle::depth_view depth_ntk{*benchmark};

                // define the PI order
                exact_params.pi_permutation = permutation;

                // perform layout generation with an SMT-based exact algorithm
                if (const auto gate_level_layout = fiction::exact<gate_lyt>(*benchmark, exact_params, &exact_stats);
                    gate_level_layout.has_value())
                {
                    // check equivalence
                    fiction::equivalence_checking(*benchmark, *gate_level_layout, &eq_stats);
                    // compute critical path length and throughput
                    fiction::critical_path_length_and_throughput(*gate_level_layout, &cp_tp_stats);

                    // convert layout to a network
                    const auto rank_cross_ntk = fiction::convert_layout_to_network<crossed_ntk>(*gate_level_layout);

                    // write resulting layout to DOT
                    fiction::write_dot_layout<gate_lyt, fiction::gate_layout_hexagonal_drawer<gate_lyt>>(
                        *gate_level_layout, fmt::format("{}{}/{}_{}_layout.dot", outputs, db,
                                                        benchmark->get_network_name(), permutation_str));

                    // // write resulting network to DOT
                    // mockturtle::write_dot<rank_ntk, fiction::technology_dot_drawer<rank_ntk>>(
                    //     rank_cross_ntk,
                    //     fmt::format("{}{}_{}.dot", outputs, benchmark->get_network_name(), permutation_str));

                    // write resulting network to Verilog
                    mockturtle::write_verilog(
                        rank_cross_ntk,
                        fmt::format("{}{}/{}_{}.v", outputs, db, benchmark->get_network_name(), permutation_str));

                    // log results
                    crossing_synth_exp(benchmark->get_network_name(), permutation_str, benchmark->num_pis(),
                                       benchmark->num_pos(), benchmark->num_gates(), depth_ntk.depth(),
                                       gate_level_layout->x() + 1, gate_level_layout->y() + 1,
                                       (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1),
                                       gate_level_layout->num_gates(), gate_level_layout->num_wires(),
                                       gate_level_layout->num_crossings(), cp_tp_stats.critical_path_length,
                                       cp_tp_stats.throughput, mockturtle::to_seconds(exact_stats.time_total));
                }
                else  // no layout was obtained
                {
                    // log results
                    crossing_synth_exp(benchmark->get_network_name(), permutation_str, benchmark->num_pis(),
                                       benchmark->num_pos(), benchmark->num_gates(), depth_ntk.depth(), 0, 0, 0, 0, 0,
                                       0, 0, 0, mockturtle::to_seconds(exact_stats.time_total));
                }

                crossing_synth_exp.save();
                crossing_synth_exp.table();

            } while (std::next_permutation(permutation.begin(), permutation.end()));  // generate next permutation
        }
    }

    return 0;
}

#endif  // FICTION_Z3_SOLVER
