//
// Created by marcel on 16.11.21.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/exact.hpp>               // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/technology/area.hpp>                                            // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>                               // cell implementations
#include <fiction/technology/qca_one_library.hpp>                                 // a pre-defined QCA gate library
#include <fiction/technology/technology_mapping_library.hpp>  // pre-defined gate types for technology mapping
#include <fiction/types.hpp>                                  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>                                        // output formatting
#include <lorina/genlib.hpp>                                   // Genlib file parsing
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/equivalence_checking.hpp>      // equivalence checking
#include <mockturtle/algorithms/mapper.hpp>                    // Technology mapping on the logic level
#include <mockturtle/algorithms/miter.hpp>                     // miter structure
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of aigs
#include <mockturtle/io/genlib_reader.hpp>                     // call-backs to read Genlib files into gate libraries
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/networks/aig.hpp>                         // AND-inverter graphs
#include <mockturtle/networks/klut.hpp>                        // kLUT network
#include <mockturtle/utils/tech_library.hpp>                   // technology library utils
#include <mockturtle/views/depth_view.hpp>                     // to determine network levels

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    using gate_lyt = fiction::cart_gate_clk_lyt;
    using cell_lyt = fiction::qca_cell_clk_lyt;

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, bool,
                            uint64_t, double>
        bestagon_exp{"qca_one",
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
                     "QCA cells",
                     "layout area in nm²"};

    // instantiate a complete AIG NPN database for node re-synthesis
    mockturtle::xag_npn_resynthesis<mockturtle::aig_network,                    // the input network type
                                    mockturtle::aig_network,                    // the database network type
                                    mockturtle::xag_npn_db_kind::aig_complete>  // the kind of database to use
        resynthesis_function{};

    // parameters for cut rewriting
    mockturtle::cut_rewriting_params cut_params{};
    cut_params.cut_enumeration_ps.cut_size = 4;

    // instantiate a technology mapping library
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                   << fiction::GATE_AND2 << fiction::GATE_OR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms
    fiction::exact_physical_design_stats exact_stats{};

    constexpr const uint64_t bench_select =
        fiction_experiments::all & ~fiction_experiments::parity & ~fiction_experiments::xor5_maj &
        ~fiction_experiments::two_bit_add_maj & ~fiction_experiments::cm82a_5 & ~fiction_experiments::xor5_r1 &
        ~fiction_experiments::one_bit_add_maj & ~fiction_experiments::b1_r2 & ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        mockturtle::aig_network aig{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(aig));
        assert(read_verilog_result == lorina::return_code::success);

        // compute depth
        mockturtle::depth_view depth_aig{aig};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_aig = mockturtle::cut_rewriting(aig, resynthesis_function, cut_params);
        // compute depth
        mockturtle::depth_view depth_cut_aig{cut_aig};

        // perform technology mapping
        const auto mapped_network = mockturtle::map(cut_aig, gate_lib, map_params);
        // compute depth
        mockturtle::depth_view depth_mapped_network{mapped_network};

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_level_layout = fiction::exact<gate_lyt>(mapped_network, exact_params, &exact_stats);

        if (gate_level_layout.has_value())
        {
            // check equivalence
            const auto miter = mockturtle::miter<mockturtle::klut_network>(mapped_network, *gate_level_layout);
            const auto eq    = mockturtle::equivalence_checking(*miter);
            assert(eq.has_value());

            // compute critical path and throughput
            fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
            fiction::critical_path_length_and_throughput(*gate_level_layout, &cp_tp_stats);

            // apply gate library
            const auto cell_level_layout =
                fiction::apply_gate_library<cell_lyt, fiction::qca_one_library>(*gate_level_layout);

            // compute area
            fiction::area_stats                           area_stats{};
            fiction::area_params<fiction::qca_technology> area_ps{};
            fiction::area(cell_level_layout, area_ps, &area_stats);

            // log results
            bestagon_exp(benchmark, aig.num_pis(), aig.num_pos(), aig.num_gates(), depth_aig.depth(),
                         cut_aig.num_gates(), depth_cut_aig.depth(), mapped_network.num_gates(),
                         depth_mapped_network.depth(), gate_level_layout->x() + 1, gate_level_layout->y() + 1,
                         (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1), gate_level_layout->num_gates(),
                         gate_level_layout->num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                         mockturtle::to_seconds(exact_stats.time_total), *eq, cell_level_layout.num_cells(),
                         area_stats.area);
        }
        else  // no layout was obtained
        {
            // log results
            bestagon_exp(benchmark, aig.num_pis(), aig.num_pos(), aig.num_gates(), depth_aig.depth(),
                         cut_aig.num_gates(), depth_cut_aig.depth(), mapped_network.num_gates(),
                         depth_mapped_network.depth(), 0, 0, 0, 0, 0, 0, 0,
                         mockturtle::to_seconds(exact_stats.time_total), false, 0, 0);
        }

        bestagon_exp.save();
        bestagon_exp.table();
    }

    return 0;
}
