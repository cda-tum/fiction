//
// Created by simon on 17.10.23.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/io/write_fgl_layout.hpp>                    // writer for SiQAD files (physical simulation)
#include <fiction/networks/technology_network.hpp>            // technology-mapped network type
#include <fiction/technology/technology_mapping_library.hpp>  // pre-defined gate types for technology mapping
#include <fiction/traits.hpp>                                 // traits for type-checking
#include <fiction/types.hpp>                                  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>                                        // output formatting
#include <lorina/genlib.hpp>                                   // Genlib file parsing
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/mapper.hpp>                    // Technology mapping on the logic level
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/genlib_reader.hpp>                     // call-backs to read Genlib files into gate libraries
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/io/write_blif.hpp>
#include <mockturtle/networks/aig.hpp>        // AND-inverter graphs
#include <mockturtle/networks/klut.hpp>       // k-LUT network
#include <mockturtle/networks/xag.hpp>        // AND-inverter graphs
#include <mockturtle/utils/tech_library.hpp>  // technology library utils
#include <mockturtle/views/depth_view.hpp>    // to determine network levels

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

int main()  // NOLINT
{
    using gate_lyt = fiction::cart_gate_clk_lyt;

    const std::string layouts_folder = fmt::format("{}/mnt_bench/layouts/QCAOne", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, std::string>
        mnt_bench_exp{"mnt_bench",
                      "benchmark",
                      "inputs",
                      "outputs",
                      "initial nodes",
                      "initial depth",
                      "layout width (in tiles)",
                      "layout height (in tiles)",
                      "layout area (in tiles)",
                      "gates",
                      "wires",
                      "critical path",
                      "throughput",
                      "runtime (in sec)",
                      "equivalent"};

    fiction::orthogonal_physical_design_stats orthogonal_stats{};
    fiction::post_layout_optimization_stats optimization_stats{};

    static constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::c6288 & ~fiction_experiments::log2 & ~fiction_experiments::sqrt & ~fiction_experiments::multiplier & ~fiction_experiments::voter & ~fiction_experiments::square & ~fiction_experiments::arbiter;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        std::ostringstream                        os{};
        fiction::network_reader<fiction::tec_ptr> reader{fiction_experiments::benchmark_path(benchmark), os};
        const auto                                nets = reader.get_networks();
        const auto net = fiction::convert_network<fiction::tec_nt, fiction::tec_nt>(*nets.front());

        // compute depth
        const mockturtle::depth_view depth_net{net};

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_level_layout = fiction::orthogonal<gate_lyt>(net, {}, &orthogonal_stats);
        // const auto gate_layout = fiction::orthogonal_ordering_network<gate_lyt>(net, {}, &orthogonal_stats);
        // optimization
        // fiction::post_layout_optimization<gate_lyt>(gate_level_layout, &optimization_stats);

        // write an FGL gate-level layout file
        // fiction::write_fgl_layout(gate_level_layout, fmt::format("{}/{}_ONE_2DDWave_ortho_UnOpt_Ord.fgl", layouts_folder, benchmark));

        // check equivalence
        fiction::equivalence_checking_stats eq_stats{};
        fiction::equivalence_checking(net, gate_level_layout, &eq_stats);

        const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                "NO";

        // compute critical path and throughput
        fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
        fiction::critical_path_length_and_throughput(gate_level_layout, &cp_tp_stats);

        // log results
        mnt_bench_exp(benchmark, net.num_pis(), net.num_pos(), net.num_gates(), depth_net.depth(),
                      gate_level_layout.x() + 1, gate_level_layout.y() + 1,
                      (gate_level_layout.x() + 1) * (gate_level_layout.y() + 1), gate_level_layout.num_gates(),
                      gate_level_layout.num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                      mockturtle::to_seconds(orthogonal_stats.time_total),
                      eq_result);

        mnt_bench_exp.save();
        mnt_bench_exp.table();
    }

    return EXIT_SUCCESS;
}
