//
// Created by simon on 16.10.23.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/exact.hpp>               // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/io/print_layout.hpp>
#include <fiction/io/write_fgl_layout.hpp>                    // writer for SiQAD files (physical simulation)
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/utils/name_utils.hpp>
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
#include <mockturtle/views/names_view.hpp>

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
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, bool>
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
    // instantiate a complete XAG NPN database for node re-synthesis
    const mockturtle::xag_npn_resynthesis<mockturtle::aig_network,                    // the input network type
                                          mockturtle::aig_network,                    // the database network type
                                          mockturtle::xag_npn_db_kind::aig_complete>  // the kind of database to use
        resynthesis_function{};

    // parameters for cut rewriting
    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // instantiate a technology mapping library
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV << fiction::GATE_AND2 << fiction::GATE_OR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme = fiction::ptr<gate_lyt>(fiction::res_clocking<gate_lyt>());
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    static constexpr const uint64_t bench_select = fiction_experiments::c17; //& ~fiction_experiments::cm82a_5 & ~fiction_experiments::parity & ~fiction_experiments::xor2_f & ~fiction_experiments::c17_f;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        std::ostringstream                        os{};
        fiction::network_reader<fiction::aig_ptr> reader{fiction_experiments::benchmark_path(benchmark), os};
        const auto                                nets = reader.get_networks();
        const auto net = fiction::convert_network<fiction::aig_nt, fiction::aig_nt>(*nets.front());

        // compute depth
        const mockturtle::depth_view depth_xag{net};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_xag = mockturtle::cut_rewriting(net, resynthesis_function, cut_params);
        // compute depth
        mockturtle::depth_view depth_cut_xag{cut_xag};

        // perform technology mapping
        const auto mapped_network_klut = mockturtle::map(cut_xag, gate_lib, map_params);
        auto mapped_network = fiction::convert_network<fiction::tec_nt, mockturtle::klut_network>(mapped_network_klut);
        fiction::restore_names(net, mapped_network);

        // compute depth
        mockturtle::depth_view depth_net{mapped_network};

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_level_layout = fiction::exact<gate_lyt>(mapped_network, exact_params, &exact_stats);

        if (gate_level_layout.has_value())
        {
            std::stringstream print_stream{};
            print_gate_level_layout(print_stream, *gate_level_layout, false, false);
            std::cout << print_stream.str();
            // check equivalence
            const auto miter = mockturtle::miter<fiction::technology_network>(net, *gate_level_layout);
            const auto eq    = mockturtle::equivalence_checking(*miter);
            assert(eq.has_value());

            // compute critical path and throughput
            fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
            fiction::critical_path_length_and_throughput(*gate_level_layout, &cp_tp_stats);

            // write a FGL gate-level layout file
            fiction::write_fgl_layout(*gate_level_layout, fmt::format("{}/{}_ONE_BEST.fgl", layouts_folder, benchmark));

            // log results
            mnt_bench_exp(benchmark, net.num_pis(), net.num_pos(), net.num_gates(), depth_net.depth(),
                          gate_level_layout->x() + 1, gate_level_layout->y() + 1,
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
