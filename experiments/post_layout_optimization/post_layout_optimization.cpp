#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking

#include <fmt/format.h>                      // output formatting
#include <lorina/lorina.hpp>                 // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/io/verilog_reader.hpp>  // call-backs to read Verilog files into networks

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <string>

int main()  // NOLINT
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, double, float, std::string>
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
    fiction::post_layout_optimization_stats   post_layout_optimization_stats{};

    static constexpr const uint64_t bench_select =
        fiction_experiments::all & ~fiction_experiments::epfl & ~fiction_experiments::iscas85;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);

        fiction::technology_network network{};

        const auto read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(network));
        assert(read_verilog_result == lorina::return_code::success);

        // perform layout generation with an OGD-based heuristic algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &orthogonal_stats);

        //  compute critical path and throughput
        const auto cp_tp = fiction::critical_path_length_and_throughput(gate_level_layout);

        // calculate bounding box
        const auto bounding_box_before_optimization = fiction::bounding_box_2d(gate_level_layout);

        const auto width_before_optimization  = bounding_box_before_optimization.get_x_size() + 1;
        const auto height_before_optimization = bounding_box_before_optimization.get_y_size() + 1;
        const auto area_before_optimization   = width_before_optimization * height_before_optimization;

        // perform post-layout optimization
        fiction::post_layout_optimization<gate_lyt>(gate_level_layout, &post_layout_optimization_stats);

        // check equivalence
        fiction::equivalence_checking_stats eq_stats{};
        fiction::equivalence_checking<fiction::technology_network, gate_lyt>(network, gate_level_layout, &eq_stats);

        const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                "NO";

        // calculate bounding box
        const auto bounding_box_after_optimization = fiction::bounding_box_2d(gate_level_layout);

        const auto width_after_optimization  = bounding_box_after_optimization.get_x_size() + 1;
        const auto height_after_optimization = bounding_box_after_optimization.get_y_size() + 1;
        const auto area_after_optimization   = width_after_optimization * height_after_optimization;

        const float improv = 100 * static_cast<float>((area_before_optimization - area_after_optimization)) /
                             static_cast<float>(area_before_optimization);
        // log results
        optimization_exp(benchmark, network.num_pis(), network.num_pos(), network.num_gates(),
                         width_before_optimization, height_before_optimization, area_before_optimization,
                         width_after_optimization, height_after_optimization, area_after_optimization,
                         gate_level_layout.num_gates(), gate_level_layout.num_wires(), cp_tp.critical_path_length,
                         cp_tp.throughput, mockturtle::to_seconds(orthogonal_stats.time_total),
                         mockturtle::to_seconds(post_layout_optimization_stats.time_total), improv, eq_result);

        optimization_exp.save();
        optimization_exp.table();
    }

    return EXIT_SUCCESS;
}
