#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/hexagonalization.hpp>    // layout conversion to hexagonal gird
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/technology/area.hpp>                                            // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>                               // cell implementations
#include <fiction/technology/sidb_bestagon_library.hpp>                           // a pre-defined SiDB gate library
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
#include <mockturtle/networks/xag.hpp>                         // XOR-AND-inverter graphs
#include <mockturtle/utils/tech_library.hpp>                   // technology library utils

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

int main()  // NOLINT
{
    using gate_lyt = fiction::gate_level_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
    using hex_lyt  = fiction::hex_even_row_gate_clk_lyt;
    using cell_lyt = fiction::sidb_cell_clk_lyt;

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t,
                            uint64_t, double, double, std::string, uint64_t, double>
        hexagonalization_exp{"hexagonalization",
                             "benchmark",
                             "inputs",
                             "outputs",
                             "initial nodes",
                             "initial depth",
                             "nodes after optimization",
                             "depth after optimization",
                             "nodes after mapping",
                             "depth after mapping",
                             "ortho layout width (in tiles)",
                             "ortho layout height (in tiles)",
                             "ortho layout area (in tiles)",
                             "mapped layout width (in tiles)",
                             "mapped layout height (in tiles)",
                             "mapped layout area (in tiles)",
                             "gates",
                             "wires",
                             "critical path",
                             "throughput",
                             "runtime ortho (in sec)",
                             "runtime mapping (in sec)",
                             "equivalent",
                             "SiDB dots",
                             "layout area in nm²"};

    // instantiate a complete XAG NPN database for node re-synthesis
    const mockturtle::xag_npn_resynthesis<mockturtle::xag_network,                    // the input network type
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
    const mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});  // NOLINT
    assert(read_genlib_result == lorina::return_code::success);
    const mockturtle::tech_library<2> gate_lib{gates};

    // stats for ortho
    fiction::orthogonal_physical_design_stats orthogonal_stats{};
    // stats for hexagonalization
    fiction::hexagonalization_stats hexagonalization_stats{};

    static constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::log2 &
                                                   ~fiction_experiments::sqrt & ~fiction_experiments::multiplier;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        mockturtle::xag_network xag{};

        const auto read_verilog_result =                                          // NOLINT
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark),  // NOLINT
                                 mockturtle::verilog_reader(xag));                // NOLINT
        assert(read_verilog_result == lorina::return_code::success);

        // compute depth
        const mockturtle::depth_view depth_xag{xag};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_xag = mockturtle::cut_rewriting(xag, resynthesis_function, cut_params);
        // compute depth
        const mockturtle::depth_view depth_cut_xag{cut_xag};

        // perform technology mapping
        const auto mapped_network = mockturtle::map(cut_xag, gate_lib, map_params);
        // compute depth
        const mockturtle::depth_view depth_mapped_network{mapped_network};

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_level_layout = fiction::orthogonal<gate_lyt>(mapped_network, {}, &orthogonal_stats);

        // compute critical path and throughput
        const auto cp_tp = fiction::critical_path_length_and_throughput(gate_level_layout);

        const auto hex_layout =
            fiction::hexagonalization<hex_lyt, gate_lyt>(gate_level_layout, &hexagonalization_stats);

        // check equivalence
        fiction::equivalence_checking_stats eq_stats{};
        fiction::equivalence_checking(gate_level_layout, hex_layout, &eq_stats);

        const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                "NO";

        // apply gate library
        const auto cell_level_layout =
            fiction::apply_gate_library<cell_lyt, fiction::sidb_bestagon_library>(hex_layout);

        // compute area
        fiction::area_stats                            area_stats{};
        fiction::area_params<fiction::sidb_technology> area_ps{};
        fiction::area(cell_level_layout, area_ps, &area_stats);

        // log results
        hexagonalization_exp(benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth(),
                             cut_xag.num_gates(), depth_cut_xag.depth(), mapped_network.num_gates(),
                             depth_mapped_network.depth(), gate_level_layout.x() + 1, gate_level_layout.y() + 1,
                             (gate_level_layout.x() + 1) * (gate_level_layout.y() + 1), (hex_layout.x() + 1),
                             (hex_layout.y() + 1), (hex_layout.x() + 1) * (hex_layout.y() + 1),
                             gate_level_layout.num_gates(), gate_level_layout.num_wires(), cp_tp.critical_path_length,
                             cp_tp.throughput, mockturtle::to_seconds(orthogonal_stats.time_total),
                             mockturtle::to_seconds(hexagonalization_stats.time_total), eq_result,
                             cell_level_layout.num_cells(), area_stats.area);

        hexagonalization_exp.save();
        hexagonalization_exp.table();
    }

    return EXIT_SUCCESS;
}
