#include "fiction/layouts/coordinates.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/optimization.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/physical_design/orthogonal.hpp>    // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>                      // output formatting
#include <lorina/lorina.hpp>                 // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/io/verilog_reader.hpp>  // call-backs to read Verilog files into networks

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <string>

using coordinate = fiction::offset::ucoord_t;
using gate_lyt   = fiction::gate_level_layout<
    fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<coordinate>>>>;
using obs_gate_lyt = fiction::obstruction_layout<gate_lyt>;

int main()  // NOLINT
{
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, double, double, std::string>
        optimization_exp{"optimization",
                         "benchmark",
                         "inputs",
                         "outputs",
                         "initial nodes",
                         "ortho layout width (in tiles)",
                         "ortho layout height (in tiles)",
                         "ortho layout area (in tiles)",
                         "optimized layout width (in tiles)",
                         "optimized layout height (in tiles)",
                         "optimized layout area (in tiles)",
                         "gates",
                         "wires",
                         "critical path",
                         "throughput",
                         "runtime ortho (in sec)",
                         "runtime optimization (in sec)",
                         "improvement (%)",
                         "equivalent"};

    // stats for SMT-based physical design
    fiction::orthogonal_physical_design_stats orthogonal_stats{};

    static constexpr const uint64_t bench_select =
        fiction_experiments::all & ~fiction_experiments::epfl &
        ~fiction_experiments::iscas85;  // & fiction_experiments::fontes18; // &
                                        // ~fiction_experiments::log2 &
                                        //~fiction_experiments::sqrt & ~fiction_experiments::multiplier;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        // mockturtle::xag_network xag{};
        fiction::technology_network network;
        const auto                  read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(network));
        assert(read_verilog_result == lorina::return_code::success);

        // perform layout generation with an SMT-based exact algorithm
        const auto        gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &orthogonal_stats);
        std::stringstream print_stream1{};

        //  compute critical path and throughput
        fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
        fiction::critical_path_length_and_throughput(gate_level_layout, &cp_tp_stats);

        // calculate bounding box
        const auto bounding_box_before = fiction::bounding_box_2d(gate_level_layout);
        const auto width               = bounding_box_before.get_x_size();
        const auto height              = bounding_box_before.get_y_size();

        const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        // Optimization
        obs_gate_lyt layout = fiction::post_layout_optimization<gate_lyt>(gate_level_layout);

        print_gate_level_layout(print_stream1, layout, true, false);
        std::cout << print_stream1.str();
        const auto end = std::chrono::steady_clock::now();
        // check equivalence

        fiction::equivalence_checking_stats eq_stats{};
        fiction::equivalence_checking<fiction::technology_network, obs_gate_lyt>(network, layout, &eq_stats);

        const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                "NO";

        // calculate bounding box
        const auto bounding_box            = fiction::bounding_box_2d(layout);
        const auto optimized_layout_width  = bounding_box.get_x_size();
        const auto optimized_layout_height = bounding_box.get_y_size();
        layout.resize({optimized_layout_width, optimized_layout_height, 1});
        const double improv = 100 *
                              (static_cast<float>(((width + 1) * (height + 1)) -
                                                  ((optimized_layout_width + 1) * (optimized_layout_height + 1)))) /
                              static_cast<float>((width + 1) * (height + 1));
        // log results
        optimization_exp(benchmark, network.num_pis(), network.num_pos(), network.num_gates(), width + 1, height + 1,
                         (width + 1) * (height + 1), optimized_layout_width + 1, optimized_layout_height + 1,
                         (optimized_layout_width + 1) * (optimized_layout_height + 1), layout.num_gates(),
                         layout.num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                         mockturtle::to_seconds(orthogonal_stats.time_total),
                         static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()),
                         improv, eq_result);

        optimization_exp.save();
        optimization_exp.table();
    }

    return EXIT_SUCCESS;
}
