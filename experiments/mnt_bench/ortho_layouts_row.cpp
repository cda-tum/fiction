//
// Created by simon on 17.10.23.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/algorithms/physical_design/hexagonalization.hpp>  // layout conversion to hexagonal gird
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
// #include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>
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
    using hex_lyt = fiction::hex_even_row_gate_clk_lyt;

    const std::string layouts_folder = fmt::format("{}/mnt_bench/layouts/Bestagon", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, std::string>
        mnt_bench_exp{"mnt_bench",
                      "benchmark",
                      "inputs",
                      "outputs",
                      "initial nodes",
                      "initial depth",
                      "nodes after optimization",
                      "depth after optimization",
                      "nodes after mapping",
                      "depth after mapping",
                      "layout width (in tiles)",
                      "layout height (in tiles)",
                      "layout area (in tiles)",
                      "gates",
                      "wires",
                      "critical path",
                      "throughput",
                      "runtime (in sec)",
                      "equivalent"};

    // instantiate a complete XAG NPN database for node re-synthesis
    const mockturtle::xag_npn_resynthesis<fiction::tec_nt,                    // the input network type
                                          mockturtle::xag_network,                    // the database network type
                                          mockturtle::xag_npn_db_kind::xag_complete>  // the kind of database to use
        resynthesis_function{};
    // parameters for cut rewriting
    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // instantiate a technology mapping library
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV << fiction::GATE_AND2 << fiction::GATE_NAND2 << fiction::GATE_OR2 << fiction::GATE_NOR2 << fiction::GATE_XOR2 << fiction::GATE_XNOR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    fiction::orthogonal_physical_design_stats orthogonal_stats{};
    fiction::hexagonalization_stats           hexagonalization_stats{};
    fiction::post_layout_optimization_stats optimization_stats{};


    static constexpr const uint64_t bench_select = fiction_experiments::full_adder; //fiction_experiments::epfl & ~fiction_experiments::log2 & ~fiction_experiments::sqrt & ~fiction_experiments::multiplier & ~fiction_experiments::voter & ~fiction_experiments::square;


    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        std::ostringstream                        os{};
        fiction::network_reader<fiction::tec_ptr > reader{fiction_experiments::benchmark_path(benchmark), os};
        const auto                                nets = reader.get_networks();
        const auto net = fiction::convert_network<fiction::tec_nt, fiction::tec_nt>(*nets.front());

        // compute depth
        const mockturtle::depth_view depth_net{net};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_net = mockturtle::cut_rewriting(net, resynthesis_function, cut_params);
        // compute depth
        mockturtle::depth_view depth_cut_net{cut_net};

        // perform technology mapping
        const auto mapped_network_klut = mockturtle::map(cut_net, gate_lib, map_params);
        auto mapped_network = fiction::convert_network<fiction::tec_nt, mockturtle::klut_network>(mapped_network_klut);
        fiction::restore_names(net, mapped_network);

        // compute depth
        mockturtle::depth_view depth_mapped_network{mapped_network};

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_layout = fiction::orthogonal<gate_lyt>(mapped_network, {}, &orthogonal_stats);
        // const auto gate_layout = fiction::orthogonal_ordering_network<gate_lyt>(mapped_network, {}, &orthogonal_stats);

        // optimization
        // fiction::post_layout_optimization<gate_lyt>(gate_layout, &optimization_stats);
        const auto gate_level_layout = fiction::hexagonalization<hex_lyt, gate_lyt>(gate_layout, &hexagonalization_stats);

        // write an FGL gate-level layout file
        fiction::write_fgl_layout(gate_level_layout, fmt::format("{}/{}_Bestagon_ROW_ortho_UnOpt_UnOrd.fgl", layouts_folder, benchmark));

        // check equivalence
        fiction::equivalence_checking_stats eq_stats{};
        //fiction::equivalence_checking(mapped_network, gate_level_layout, &eq_stats);

        const std::string eq_result = "?";//eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      //eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                      //                                              "NO";
        // compute critical path and throughput
        fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
        fiction::critical_path_length_and_throughput(gate_level_layout, &cp_tp_stats);

        // log results
        mnt_bench_exp(benchmark, net.num_pis(), net.num_pos(), net.num_gates(), depth_net.depth(), cut_net.num_gates(),
                      depth_cut_net.depth(), mapped_network.num_gates(), depth_mapped_network.depth(),
                      gate_level_layout.x() + 1, gate_level_layout.y() + 1,
                      (gate_level_layout.x() + 1) * (gate_level_layout.y() + 1), gate_level_layout.num_gates(),
                      gate_level_layout.num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                      mockturtle::to_seconds(orthogonal_stats.time_total) + mockturtle::to_seconds(hexagonalization_stats.time_total),
                      eq_result);

        mnt_bench_exp.save();
        mnt_bench_exp.table();
    }

    return EXIT_SUCCESS;
}
