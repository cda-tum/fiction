#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>        // scalable heuristic for physical design
#include <fiction/algorithms/physical_design/wiring_reduction.hpp>  // wiring reduction algorithm
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files

#include <fmt/core.h>
#include <fmt/format.h>  // output formatting
#include <mockturtle/utils/stopwatch.hpp>

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
                            uint64_t, uint64_t, size_t, size_t, float, uint64_t, uint64_t, float, uint64_t, uint64_t,
                            float, double, double, float, std::string>
        wiring_reduction_exp{"wiring_reduction",
                             "benchmark",
                             "inputs",
                             "outputs",
                             "initial nodes",
                             "width ortho",
                             "height ortho",
                             "area ortho",
                             "width optimized",
                             "height optimized",
                             "area optimized",
                             "gates",
                             "wires ortho",
                             "wires optimized",
                             "improv wires (%)",
                             "critical path ortho",
                             "critical path optimized",
                             "improv cp (%)",
                             "throughput ortho",
                             "throughput optimized",
                             "improv tp (%)",
                             "runtime ortho (s)",
                             "runtime wiring_reduction (s)",
                             "improv area (%)",
                             "equivalent"};

    // stats for SMT-based physical design
    fiction::orthogonal_physical_design_stats orthogonal_stats{};
    fiction::wiring_reduction_stats           wiring_reduction_stats{};

    static constexpr const uint64_t bench_select =
        (fiction_experiments::trindade16 | fiction_experiments::fontes18) & ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based heuristic algorithm
        auto       gate_level_layout = fiction::orthogonal<gate_lyt>(benchmark_network, {}, &orthogonal_stats);
        const auto layout_copy       = gate_level_layout.clone();

        auto num_wires = gate_level_layout.num_wires() - gate_level_layout.num_pis() - gate_level_layout.num_pos();
        //  compute critical path and throughput
        const auto cp_tp_stats = fiction::critical_path_length_and_throughput(gate_level_layout);

        // calculate bounding box
        const auto bounding_box_before_wiring_reduction = fiction::bounding_box_2d(gate_level_layout);

        const auto width_before_wiring_reduction  = bounding_box_before_wiring_reduction.get_x_size() + 1;
        const auto height_before_wiring_reduction = bounding_box_before_wiring_reduction.get_y_size() + 1;
        const auto area_before_wiring_reduction   = width_before_wiring_reduction * height_before_wiring_reduction;

        // perform post-layout optimization
        fiction::wiring_reduction<gate_lyt>(gate_level_layout, &wiring_reduction_stats);

        //  compute critical path and throughput
        const auto cp_tp_stats_after = fiction::critical_path_length_and_throughput(gate_level_layout);

        auto num_wires_after =
            gate_level_layout.num_wires() - gate_level_layout.num_pis() - gate_level_layout.num_pos();

        // check equivalence
        const auto eq_stats = fiction::equivalence_checking<gate_lyt, gate_lyt>(layout_copy, gate_level_layout);

        const std::string eq_result = eq_stats == fiction::eq_type::STRONG ? "STRONG" :
                                      eq_stats == fiction::eq_type::WEAK   ? "WEAK" :
                                                                             "NO";

        // calculate bounding box
        const auto bounding_box_after_wiring_reduction = fiction::bounding_box_2d(gate_level_layout);

        const auto width_after_wiring_reduction  = bounding_box_after_wiring_reduction.get_x_size() + 1;
        const auto height_after_wiring_reduction = bounding_box_after_wiring_reduction.get_y_size() + 1;
        const auto area_after_wiring_reduction   = width_after_wiring_reduction * height_after_wiring_reduction;

        const float improv_wires =
            100 * static_cast<float>((num_wires - num_wires_after)) / static_cast<float>(num_wires);
        const float improv_critical_path =
            100 * static_cast<float>((cp_tp_stats.critical_path_length - cp_tp_stats_after.critical_path_length)) /
            static_cast<float>(cp_tp_stats.critical_path_length);
        const float improv_throughput = 100 *
                                        static_cast<float>((cp_tp_stats.throughput - cp_tp_stats_after.throughput)) /
                                        static_cast<float>(cp_tp_stats.throughput);
        const float improv_area = 100 *
                                  static_cast<float>((area_before_wiring_reduction - area_after_wiring_reduction)) /
                                  static_cast<float>(area_before_wiring_reduction);
        // log results
        wiring_reduction_exp(
            benchmark, benchmark_network.num_pis(), benchmark_network.num_pos(), benchmark_network.num_gates(),
            width_before_wiring_reduction, height_before_wiring_reduction, area_before_wiring_reduction,
            width_after_wiring_reduction, height_after_wiring_reduction, area_after_wiring_reduction,
            gate_level_layout.num_gates(), num_wires, num_wires_after, improv_wires, cp_tp_stats.critical_path_length,
            cp_tp_stats_after.critical_path_length, improv_critical_path, cp_tp_stats.throughput,
            cp_tp_stats_after.throughput, improv_throughput, mockturtle::to_seconds(orthogonal_stats.time_total),
            mockturtle::to_seconds(wiring_reduction_stats.time_total), improv_area, eq_result);

        wiring_reduction_exp.save();
        wiring_reduction_exp.table();
    }

    return EXIT_SUCCESS;
}
