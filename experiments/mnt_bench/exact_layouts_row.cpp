//
// Created by simon on 16.10.23.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/exact.hpp>               // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/io/print_layout.hpp>
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/io/write_fgl_layout.hpp>                    // writer for SiQAD files (physical simulation)
#include <fiction/networks/technology_network.hpp>            // technology-mapped network type
#include <fiction/technology/area.hpp>                        // area requirement calculations
#include <fiction/technology/technology_mapping_library.hpp>  // pre-defined gate types for technology mapping
#include <fiction/traits.hpp>                                 // traits for type-checking
#include <fiction/types.hpp>                                  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>                                        // output formatting
#include <lorina/genlib.hpp>                                   // Genlib file parsing
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/equivalence_checking.hpp>      // equivalence checking
#include <mockturtle/algorithms/mapper.hpp>                    // Technology mapping on the logic level
#include <mockturtle/algorithms/miter.hpp>                     // miter structure
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/genlib_reader.hpp>                     // call-backs to read Genlib files into gate libraries
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/io/write_blif.hpp>
#include <mockturtle/networks/aig.hpp>        // XOR-AND-inverter graphs
#include <mockturtle/networks/xag.hpp>        // XOR-AND-inverter graphs
#include <mockturtle/networks/klut.hpp>       // k-LUT network
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
    using gate_lyt = fiction::hex_even_row_gate_clk_lyt;

    const std::string layouts_folder = fmt::format("{}/mnt_bench/layouts/Bestagon", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, bool>
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
    const mockturtle::xag_npn_resynthesis<fiction::xag_nt ,                    // the input network type
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

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::row_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    static constexpr const uint64_t bench_select = fiction_experiments::c17;//(fiction_experiments::trindade16 | fiction_experiments::fontes18) & ~fiction_experiments::xor2_f & ~fiction_experiments::c17_f; // & ~fiction_experiments::parity;// fiction_experiments::fontes18 & ~fiction_experiments::cm82a_5 & ~fiction_experiments::parity & ~fiction_experiments::xor2_f & ~fiction_experiments::c17_f & ~fiction_experiments::xor5_maj & ~fiction_experiments::two_bit_add_maj & ~fiction_experiments::one_bit_add_maj;


    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        std::ostringstream                        os{};
        fiction::network_reader<fiction::xag_ptr> reader{fiction_experiments::benchmark_path(benchmark), os};
        const auto                                nets = reader.get_networks();
        const auto net = fiction::convert_network<fiction::xag_nt, fiction::xag_nt>(*nets.front());

        // compute depth
        mockturtle::depth_view depth_net{net};

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
        const auto gate_level_layout = fiction::exact<gate_lyt>(mapped_network, exact_params, &exact_stats);

        if (gate_level_layout.has_value())
        {
            // check equivalence
            const auto miter = mockturtle::miter<fiction::technology_network>(mapped_network, *gate_level_layout);
            const auto eq    = mockturtle::equivalence_checking(*miter);
            assert(eq.has_value());

            // compute critical path and throughput
            fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
            fiction::critical_path_length_and_throughput(*gate_level_layout, &cp_tp_stats);

            // write a FGL gate-level layout file
            fiction::write_fgl_layout(*gate_level_layout, fmt::format("{}/{}_Bestagon_ROW_exact_UnOpt_UnOrd.fgl", layouts_folder, benchmark));

            // log results
            mnt_bench_exp(benchmark, net.num_pis(), net.num_pos(), net.num_gates(), depth_net.depth(),
                          cut_net.num_gates(), depth_cut_net.depth(), mapped_network.num_gates(),
                          depth_mapped_network.depth(), gate_level_layout->x() + 1, gate_level_layout->y() + 1,
                          (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1), gate_level_layout->num_gates(),
                          gate_level_layout->num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                          mockturtle::to_seconds(exact_stats.time_total), *eq);
        }

        mnt_bench_exp.save();
        mnt_bench_exp.table();
    }

    return EXIT_SUCCESS;
}

#else  // FICTION_Z3_SOLVER

#include <cstdlib>
#include <iostream>

int main()  // NOLINT
{
    std::cerr << "[e] Z3 solver is not available, please install Z3 and recompile the code" << std::endl;

    return EXIT_FAILURE;
}

#endif  // FICTION_Z3_SOLVER
