//
// Created by marcel on 16.11.21.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/exact.hpp>               // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/io/write_sqd_layout.hpp>                    // writer for SiQAD files (physical simulation)
#include <fiction/networks/technology_network.hpp>            // technology-mapped network type
#include <fiction/technology/area.hpp>                        // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>           // cell implementations
#include <fiction/technology/sidb_bestagon_library.hpp>       // a pre-defined SiDB gate library
#include <fiction/technology/technology_mapping_library.hpp>  // pre-defined gate types for technology mapping
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
#include <mockturtle/networks/xag.hpp>                         // XOR-AND-inverter graphs
#include <mockturtle/utils/tech_library.hpp>                   // technology library utils
#include <mockturtle/views/depth_view.hpp>                     // to determine network levels

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

int main()  // NOLINT
{
    using gate_lyt = fiction::hex_even_row_gate_clk_lyt;
    using cell_lyt = fiction::sidb_cell_clk_lyt;

    const std::string layouts_folder = fmt::format("{}/bestagon/layouts", EXPERIMENTS_PATH);

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, bool,
                            uint64_t, double>
        bestagon_exp{"bestagon",
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
                     "equivalent",
                     "SiDB dots",
                     "layout area in nm²"};

    // instantiate a complete XAG NPN database for node re-synthesis
    mockturtle::xag_npn_resynthesis<mockturtle::xag_network,                    // the input network type
                                    mockturtle::xag_network,                    // the database network type
                                    mockturtle::xag_npn_db_kind::xag_complete>  // the kind of database to use
        resynthesis_function{};

    // parameters for cut rewriting
    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // instantiate a technology mapping library
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                   << fiction::GATE_AND2 << fiction::GATE_NAND2 << fiction::GATE_OR2 << fiction::GATE_NOR2
                   << fiction::GATE_XOR2 << fiction::GATE_XNOR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params exact_params{};
    exact_params.scheme        = "Row";
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    static constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::b1_r2 &
                                                   ~fiction_experiments::clpl & ~fiction_experiments::two_bit_add_maj &
                                                   ~fiction_experiments::parity & ~fiction_experiments::iscas85 &
                                                   ~fiction_experiments::epfl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        mockturtle::xag_network xag{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(xag));
        assert(read_verilog_result == lorina::return_code::success);

        // compute depth
        mockturtle::depth_view depth_xag{xag};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_xag = mockturtle::cut_rewriting(xag, resynthesis_function, cut_params);
        // compute depth
        mockturtle::depth_view depth_cut_xag{cut_xag};

        // perform technology mapping
        const auto mapped_network = mockturtle::map(cut_xag, gate_lib, map_params);
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
            const auto cp_tp = fiction::critical_path_length_and_throughput(*gate_level_layout);

            // apply gate library
            const auto cell_level_layout =
                fiction::apply_gate_library<cell_lyt, fiction::sidb_bestagon_library>(*gate_level_layout);

            // compute area
            fiction::area_stats                            area_stats{};
            fiction::area_params<fiction::sidb_technology> area_ps{};
            fiction::area(cell_level_layout, area_ps, &area_stats);

            // write a SiQAD simulation file
            fiction::write_sqd_layout(cell_level_layout, fmt::format("{}/{}.sqd", layouts_folder, benchmark));

            // log results
            bestagon_exp(benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth(),
                         cut_xag.num_gates(), depth_cut_xag.depth(), mapped_network.num_gates(),
                         depth_mapped_network.depth(), gate_level_layout->x() + 1, gate_level_layout->y() + 1,
                         (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1), gate_level_layout->num_gates(),
                         gate_level_layout->num_wires(), cp_tp.critical_path_length, cp_tp.throughput,
                         mockturtle::to_seconds(exact_stats.time_total), *eq, cell_level_layout.num_cells(),
                         area_stats.area);
        }
        else  // no layout was obtained
        {
            // log results
            bestagon_exp(benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth(),
                         cut_xag.num_gates(), depth_cut_xag.depth(), mapped_network.num_gates(),
                         depth_mapped_network.depth(), 0, 0, 0, 0, 0, 0, 0,
                         mockturtle::to_seconds(exact_stats.time_total), false, 0, 0);
        }

        bestagon_exp.save();
        bestagon_exp.table();
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
