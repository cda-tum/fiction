#include "fiction/layouts/bounding_box.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>                // scalable heuristic for physical design
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>  // post-layout optimization
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/types.hpp>                                                      // types suitable for the FCN domain

#include <fmt/format.h>                    // output formatting
#include <mockturtle/utils/stopwatch.hpp>  // runtime measurements

#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream os{};

    fiction::network_reader<fiction::tec_ptr> reader{fiction_experiments::benchmark_path(name), os};

    const auto nets = reader.get_networks();

    return *nets.front();
}

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

    // stats
    fiction::orthogonal_physical_design_stats orthogonal_stats{};
    fiction::post_layout_optimization_stats   post_layout_optimization_stats{};
    fiction::post_layout_optimization_params  post_layout_optimization_params{};
    // post_layout_optimization_params.max_gate_relocations = 1;

    static constexpr const uint64_t bench_select = fiction_experiments::trindade16 | fiction_experiments::fontes18;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based heuristic algorithm
        auto       gate_level_layout = fiction::orthogonal<gate_lyt>(benchmark_network, {}, &orthogonal_stats);
        const auto layout_copy       = gate_level_layout.clone();

        //  compute critical path and throughput
        const auto cp_tp = fiction::critical_path_length_and_throughput(gate_level_layout);

        // calculate bounding box
        const auto bounding_box_before_optimization = fiction::bounding_box_2d(gate_level_layout);

        const auto width_before_optimization  = bounding_box_before_optimization.get_x_size() + 1;
        const auto height_before_optimization = bounding_box_before_optimization.get_y_size() + 1;
        const auto area_before_optimization   = width_before_optimization * height_before_optimization;

        // perform post-layout optimization
        fiction::post_layout_optimization<gate_lyt>(gate_level_layout, post_layout_optimization_params,
                                                    &post_layout_optimization_stats);

        // check equivalence
        const auto eq_stats = fiction::equivalence_checking<gate_lyt, gate_lyt>(layout_copy, gate_level_layout);

        const std::string eq_result = eq_stats == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats == fiction::eq_type::WEAK   ? "WEAK" :
                                                                             "NO";

        // calculate bounding box
        const auto bounding_box_after_optimization = fiction::bounding_box_2d(gate_level_layout);

        const auto width_after_optimization  = bounding_box_after_optimization.get_x_size() + 1;
        const auto height_after_optimization = bounding_box_after_optimization.get_y_size() + 1;
        const auto area_after_optimization   = width_after_optimization * height_after_optimization;

        const float improv = 100 * static_cast<float>((area_before_optimization - area_after_optimization)) /
                             static_cast<float>(area_before_optimization);
        // log results
        optimization_exp(benchmark, benchmark_network.num_pis(), benchmark_network.num_pos(),
                         benchmark_network.num_gates(), width_before_optimization, height_before_optimization,
                         area_before_optimization, width_after_optimization, height_after_optimization,
                         area_after_optimization, gate_level_layout.num_gates(), gate_level_layout.num_wires(),
                         cp_tp.critical_path_length, cp_tp.throughput,
                         mockturtle::to_seconds(orthogonal_stats.time_total),
                         mockturtle::to_seconds(post_layout_optimization_stats.time_total), improv, eq_result);

        optimization_exp.save();
        optimization_exp.table();
    }

    return EXIT_SUCCESS;
}
