#include "experiment_setup.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>  // layout conversion to cell-level
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/write_sqd_layout.hpp>  // writer for SiQAD files (physical simulation)
#include <fiction/layouts/bounding_box.hpp>  // computes a minimum-sized box around all non-empty coordinates in a given layout
#include <fiction/technology/area.hpp>                        // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>           // cell implementations
#include <fiction/technology/sidb_bestagon_library.hpp>       // a pre-defined SiDB gate library
#include <fiction/technology/technology_mapping_library.hpp>  // pre-defined gate types for technology mapping
#include <fiction/traits.hpp>                                 // traits for type-checking
#include <fiction/types.hpp>                                  // pre-defined types suitable for the FCN domain
#include <fiction/utils/name_utils.hpp>                       // restore names

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
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <vector>

template <typename Lyt>
fiction::coordinate<Lyt> to_hex(int cartesian_x, int cartesian_y, int64_t cartesian_layout_height, int cartesian_z)
{
    const auto y = cartesian_x + cartesian_y;
    const auto x =
        static_cast<int>(cartesian_x + std::ceil(std::floor(static_cast<double>(cartesian_layout_height) / 2) -
                                                 static_cast<double>(y) / 2));
    const auto z = cartesian_z;

    fiction::coordinate<Lyt> hex{x, y, z};

    return hex;
}

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

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    const mockturtle::tech_library<2> gate_lib{gates};

    // stats for SMT-based physical design
    fiction::orthogonal_physical_design_stats orthogonal_stats{};

    static constexpr const uint64_t bench_select =
        experiment_setup::all & ~experiment_setup::log2 & ~experiment_setup::sqrt & ~experiment_setup::multiplier;

    for (const auto& benchmark : experiment_setup::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        mockturtle::xag_network xag{};

        const auto read_verilog_result =
            lorina::read_verilog(experiment_setup::benchmark_path(benchmark), mockturtle::verilog_reader(xag));
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
        fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
        fiction::critical_path_length_and_throughput(gate_level_layout, &cp_tp_stats);

        const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        // get width & height of cartesian layout
        const int64_t layout_width  = static_cast<int64_t>(gate_level_layout.x()) + 1;
        const int64_t layout_height = static_cast<int64_t>(gate_level_layout.y()) + 1;

        // calculate max width & height of hexagonal layout
        const auto hex_height =
            to_hex<gate_lyt>(static_cast<int>(layout_width) - 1, static_cast<int>(layout_height) - 1,
                             static_cast<int>(layout_height), 0)
                .y;
        const auto hex_width = to_hex<gate_lyt>(static_cast<int>(layout_width) - 1, 0, layout_height, 0).x;

        // instantiate hexagonal layout
        hex_lyt hex_layout{{hex_width, hex_height, 1}, fiction::row_clocking<hex_lyt>()};

        // iterate through cartesian layout diagonally
        for (int k = 0; k < layout_width + layout_height - 1; k++)
        {
            for (int x = 0; x < k + 1; x++)
            {
                const auto y = k - x;
                if (y < layout_height && x < layout_width)
                {
                    for (int z = 0; z < 2; z++)
                    {
                        // old coordinate
                        const fiction::coordinate<gate_lyt> old_coord{x, y, z};
                        // new coordinate
                        const fiction::coordinate<gate_lyt> hex{to_hex<gate_lyt>(x, y, layout_height, z)};

                        if (gate_level_layout.is_empty_tile(old_coord))
                        {
                            continue;
                        }

                        if (const auto node = gate_level_layout.get_node(old_coord); gate_level_layout.is_pi(node))
                        {
                            hex_layout.create_pi(gate_level_layout.get_name(gate_level_layout.get_node({x, y, z})),
                                                 hex);
                        }
                        else if (gate_level_layout.is_po(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_po(hex_layout.make_signal(hex_layout.get_node(
                                                     to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                                 gate_level_layout.get_name(gate_level_layout.get_node(old_coord)),
                                                 hex);
                        }
                        else if (gate_level_layout.is_wire(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_buf(hex_layout.make_signal(hex_layout.get_node(
                                                      to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_inv(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_not(hex_layout.make_signal(hex_layout.get_node(
                                                      to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_and(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_and(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_nand(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_nand(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                   hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                   hex);
                        }
                        else if (gate_level_layout.is_or(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_or(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                     signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                 hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                     signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                 hex);
                        }
                        else if (gate_level_layout.is_nor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_nor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_xor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_xor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_xnor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_xnor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                   hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                   hex);
                        }
                        else if (gate_level_layout.is_function(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            const auto node_fun = gate_level_layout.node_function(node);

                            hex_layout.create_node({hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                        signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                    hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                        signal_b.x, signal_b.y, layout_height, signal_b.z)))},
                                                   node_fun, hex);
                        }
                    }
                }
            }
        }
        const auto end = std::chrono::steady_clock::now();

        fiction::restore_names(gate_level_layout, hex_layout);
        // check equivalence
        fiction::equivalence_checking_stats eq_stats{};
        fiction::equivalence_checking(gate_level_layout, hex_layout, &eq_stats);

        const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                "NO";

        // apply gate library
        const auto cell_level_layout =
            fiction::apply_gate_library<cell_lyt, fiction::sidb_bestagon_library>(hex_layout);

        // calculate bounding box
        const auto bounding_box      = fiction::bounding_box_2d(hex_layout);
        const auto hex_layout_width  = bounding_box.get_x_size() + 1;
        const auto hex_layout_height = bounding_box.get_y_size() + 1;

        // compute area
        fiction::area_stats                            area_stats{};
        fiction::area_params<fiction::sidb_technology> area_ps{};
        fiction::area(cell_level_layout, area_ps, &area_stats);

        // log results
        hexagonalization_exp(
            benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth(), cut_xag.num_gates(),
            depth_cut_xag.depth(), mapped_network.num_gates(), depth_mapped_network.depth(), gate_level_layout.x() + 1,
            gate_level_layout.y() + 1, (gate_level_layout.x() + 1) * (gate_level_layout.y() + 1), hex_layout_width,
            hex_layout_height, hex_layout_width * hex_layout_height, gate_level_layout.num_gates(),
            gate_level_layout.num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
            mockturtle::to_seconds(orthogonal_stats.time_total),
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0,
            eq_result, cell_level_layout.num_cells(), area_stats.area);

        hexagonalization_exp.save();
        hexagonalization_exp.table();
    }

    return EXIT_SUCCESS;
}
