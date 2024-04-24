//
// Created by Jan Drewniok on 20.10.23.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>  // technology mapping
#include <fiction/algorithms/physical_design/apply_gate_library.hpp>         // layout conversion to cell-level
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>          // design gate
#include <fiction/algorithms/physical_design/exact.hpp>                      // SMT-based physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>  // choose design engine (ExGS, QuickSim, QuickExact)
#include <fiction/io/read_sidb_surface_defects.hpp>                       // reader for simulated SiDB surfaces
#include <fiction/io/write_sqd_layout.hpp>                    // writer for SiQAD files (physical simulation)
#include <fiction/layouts/coordinates.hpp>                    // layout coordinates
#include <fiction/networks/technology_network.hpp>            // technology-mapped network type
#include <fiction/technology/area.hpp>                        // area requirement calculations
#include <fiction/technology/cell_technologies.hpp>           // cell implementations
#include <fiction/technology/parameterized_gate_library.hpp>  // a dynamic SiDB gate library
#include <fiction/technology/sidb_defect_surface.hpp>         // SiDB surface with support for atomic defects
#include <fiction/technology/sidb_defects.hpp>                // Atomic defects
#include <fiction/technology/sidb_skeleton_bestagon_library.hpp>  // a static skeleton SiDB gate library defining the input/output wires
#include <fiction/technology/sidb_surface_analysis.hpp>  // Analyzes a given defective SiDB surface and matches it against gate tiles provided by a library
#include <fiction/types.hpp>  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>                                        // output formatting
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/equivalence_checking.hpp>      // equivalence checking
#include <mockturtle/algorithms/miter.hpp>                     // miter structure
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/networks/xag.hpp>                         // XOR-AND-inverter graphs
#include <mockturtle/utils/stopwatch.hpp>                      // used to measure runtime
#include <mockturtle/views/depth_view.hpp>                     // to determine network levels

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <optional>
#include <string>
#include <utility>

// This script conducts defect-aware placement and routing with defect-aware on-the-fly SiDB gate design. Thereby, SiDB
// circuits can be designed in the presence of atomic defects.

int main()  // NOLINT
{
    using gate_lyt = fiction::hex_even_row_gate_clk_lyt;
    using cell_lyt = fiction::sidb_cell_clk_lyt;

    fiction::design_sidb_gates_params<fiction::cube::coord_t> design_gate_params{};
    design_gate_params.phys_params     = fiction::sidb_simulation_parameters{2, -0.32};
    design_gate_params.canvas          = {{24, 17}, {34, 28}};
    design_gate_params.number_of_sidbs = 3;
    design_gate_params.sim_engine      = fiction::sidb_simulation_engine::QUICKEXACT;
    design_gate_params.termination_cond =
        fiction::design_sidb_gates_params<fiction::cube::coord_t>::termination_condition::AFTER_FIRST_SOLUTION;

    // save atomic defects which their respective phyiscal parameters as exerimentally determined by T. R. Huff, T.
    // Dienel, M. Rashidi, R. Achal, L. Livadaru, J. Croshaw, and R. A. Wolkow, "Electrostatic landscape of a
    // Hydrogen-terminated Silicon Surface Probed by a Moveable Quantum Dot."
    const auto stray_db   = fiction::sidb_defect{fiction::sidb_defect_type::DB, -1, 4.1, 1.8};
    const auto si_vacancy = fiction::sidb_defect{fiction::sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};

    static const std::string layouts_folder =
        fmt::format("{}/physical_design_with_on_the_fly_gate_design/layouts", EXPERIMENTS_PATH);

    auto surface_lattice_initial = fiction::read_sidb_surface_defects<cell_lyt>(
        "../../experiments/defect_aware_physical_design/1_percent_with_charged_surface.txt");

    fiction::sidb_defect_surface<cell_lyt> surface_lattice{
        {std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()}};

    surface_lattice_initial.foreach_sidb_defect(
        [&surface_lattice, &stray_db, &si_vacancy](const auto& cd)
        {
            if (cd.second.type == fiction::sidb_defect_type::DB)
            {
                surface_lattice.assign_sidb_defect(cd.first, stray_db);
            }
            else if (cd.second.type == fiction::sidb_defect_type::SI_VACANCY)
            {
                surface_lattice.assign_sidb_defect(cd.first, si_vacancy);
            }
            else
            {
                surface_lattice.assign_sidb_defect(cd.first, cd.second);
            }
        });

    const auto lattice_tiling = gate_lyt{{11, 30}};

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, bool, uint64_t, double>
        sidb_circuits_with_defects{"sidb_circuits_with_defects",
                                   "benchmark",
                                   "inputs",
                                   "outputs",
                                   "initial nodes",
                                   "initial depth",
                                   "optimized nodes",
                                   "optimized depth",
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

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::row_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = false;
    exact_params.border_io     = false;
    exact_params.desynchronize = true;
    exact_params.upper_bound_x = 11;         // 12 x 31 tiles
    exact_params.upper_bound_y = 30;         // 12 x 31 tiles
    exact_params.timeout       = 3'600'000;  // 1h in ms

    constexpr const uint64_t bench_select =
        fiction_experiments::all & ~fiction_experiments::parity & ~fiction_experiments::two_bit_add_maj &
        ~fiction_experiments::b1_r2 & ~fiction_experiments::clpl & ~fiction_experiments::iscas85 &
        ~fiction_experiments::epfl & ~fiction_experiments::half_adder & ~fiction_experiments::full_adder &
        ~fiction_experiments::one_bit_add_aoig & ~fiction_experiments::one_bit_add_maj;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[attempts] processing {}\n", benchmark);
        mockturtle::xag_network xag{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(xag));
        assert(read_verilog_result == lorina::return_code::success);

        // compute depth
        const mockturtle::depth_view depth_xag{xag};

        const fiction::technology_mapping_params tech_map_params = fiction::all_2_input_functions();

        // parameters for cut rewriting
        mockturtle::cut_rewriting_params cut_params{};
        cut_params.cut_enumeration_ps.cut_size = 4;

        const mockturtle::xag_npn_resynthesis<mockturtle::xag_network,                    // the input network type
                                              mockturtle::xag_network,                    // the database network type
                                              mockturtle::xag_npn_db_kind::xag_complete>  // the kind of database to use

            resynthesis_function{};

        // rewrite network cuts using the given re-synthesis function
        const auto cut_xag = mockturtle::cut_rewriting(xag, resynthesis_function, cut_params);

        // perform technology mapping
        const auto mapped_network = fiction::technology_mapping(cut_xag, tech_map_params);
        // compute depth
        const mockturtle::depth_view depth_mapped_network{mapped_network};

        std::optional<gate_lyt> gate_level_layout = std::nullopt;
        cell_lyt                cell_level_layout{{}, "fail"};

        auto black_list = fiction::sidb_surface_analysis<fiction::sidb_skeleton_bestagon_library>(
            lattice_tiling, surface_lattice, std::make_pair(0, 0));

        auto attempts = 0u;

        mockturtle::stopwatch<>::duration time_counter{};

        auto gate_design_failed = true;

        {
            const mockturtle::stopwatch stop{time_counter};

            while (!gate_level_layout.has_value() || gate_design_failed)
            {
                exact_params.black_list = black_list;
                fiction::exact_physical_design_stats exact_stats{};
                if (!gate_level_layout.has_value() && attempts > 0)
                {
                    break;
                }
                std::cout << black_list.size() << '\n';
                gate_level_layout = fiction::exact<gate_lyt>(mapped_network, exact_params, &exact_stats);
                if (gate_level_layout.has_value())
                {
                    try
                    {
                        auto parameter_gate_library =
                            fiction::parameterized_gate_library_params<cell_lyt>{surface_lattice, design_gate_params};

                        cell_level_layout = fiction::apply_parameterized_gate_library<
                            cell_lyt, fiction::parameterized_gate_library, gate_lyt,
                            fiction::parameterized_gate_library_params<cell_lyt>>(*gate_level_layout,
                                                                                  parameter_gate_library);
                        gate_design_failed = false;
                    }
                    catch (const fiction::gate_design_exception<fiction::tt, gate_lyt>& e)
                    {
                        gate_design_failed = true;
                        black_list[e.which_tile()][e.which_truth_table()].push_back(e.which_port_list());
                    }

                    catch (const std::exception& e)
                    {
                        std::cerr << "Caught std::exception: " << e.what() << '\n';
                    }
                }
                attempts++;
            }
        }

        if (!gate_level_layout.has_value())
        {
            continue;
        }

        // check equivalence
        const auto miter = mockturtle::miter<fiction::technology_network>(mapped_network, *gate_level_layout);
        const auto eq    = mockturtle::equivalence_checking(*miter);
        assert(eq.has_value());

        // compute critical path and throughput
        const auto cp_tp_stats = fiction::critical_path_length_and_throughput(*gate_level_layout);

        // apply dynamic gate library

        // compute area
        fiction::area_stats                            area_stats{};
        fiction::area_params<fiction::sidb_technology> area_ps{};
        fiction::area(cell_level_layout, area_ps, &area_stats);
        fiction::sidb_defect_surface<cell_lyt> defect_surface{cell_level_layout};

        // add defects to the file
        surface_lattice.foreach_sidb_defect([&defect_surface](const auto& defect)
                                            { defect_surface.assign_sidb_defect(defect.first, defect.second); });
        // write a SiQAD simulation file
        fiction::write_sqd_layout(defect_surface,
                                  fmt::format("{}/{}_percent_after_big_change.sqd", layouts_folder, benchmark));

        // log results
        sidb_circuits_with_defects(
            benchmark, xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth(), mapped_network.num_gates(),
            depth_mapped_network.depth(), gate_level_layout->x() + 1, gate_level_layout->y() + 1,
            (gate_level_layout->x() + 1) * (gate_level_layout->y() + 1), gate_level_layout->num_gates(),
            gate_level_layout->num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
            mockturtle::to_seconds(time_counter), *eq, cell_level_layout.num_cells(), area_stats.area);
        sidb_circuits_with_defects.save();
        sidb_circuits_with_defects.table();
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
