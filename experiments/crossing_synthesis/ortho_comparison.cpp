//
// Created by marcel on 14.11.22.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>  // OGD-based physical design of FCN layouts
#include <fiction/types.hpp>                                  // pre-defined types

#include <fmt/format.h>                     // output formatting
#include <lorina/lorina.hpp>                // network file format parser
#include <mockturtle/io/aiger_reader.hpp>   // read AIGER files
#include <mockturtle/networks/aig.hpp>      // AIG network types
#include <mockturtle/networks/xag.hpp>      // XAG network types
#include <mockturtle/views/depth_view.hpp>  // to determine network levels
#include <mockturtle/views/names_view.hpp>  // to name networks and their signals

#include <cstdint>
#include <string>

template <typename Ntk, typename Experiment>
void run_ortho(Experiment& exp, const std::string& benchmark, const std::string& ntk_type)
{
    static constexpr auto bench_template =
        "../../libs/mockturtle/experiments/benchmarks/{}.aig";  // path to the benchmark files

    using gate_lyt = fiction::cart_gate_clk_lyt;

    const auto bench_path = fmt::format(bench_template, benchmark);

    // statistics for ortho
    fiction::orthogonal_physical_design_stats ortho_stats{};

    Ntk ntk{};

    // read the network
    [[maybe_unused]] const auto result = lorina::read_aiger(bench_path, mockturtle::aiger_reader(ntk));
    assert(result == lorina::return_code::success);

    // compute depth
    mockturtle::depth_view depth{ntk};

    // perform layout generation with an OGD-based heuristic algorithm
    const auto layout = fiction::orthogonal<gate_lyt>(ntk, {}, &ortho_stats);

    mockturtle::depth_view depth_lyt{layout};

    // log results
    exp(benchmark, ntk_type, ntk.num_pis(), ntk.num_pos(), ntk.num_gates(), depth.depth(), layout.x() + 1,
        layout.y() + 1, (layout.x() + 1) * (layout.y() + 1), layout.num_gates(), layout.num_wires(),
        layout.num_crossings(), depth_lyt.depth(), mockturtle::to_seconds(ortho_stats.time_total));
}

int main()
{
    using aig_nt = mockturtle::names_view<mockturtle::aig_network>;
    using xag_nt = mockturtle::names_view<mockturtle::xag_network>;

    experiments::experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint32_t, uint64_t, double>
        ortho_compare_exp{"ortho_comparison",
                          "benchmark",
                          "type",
                          "inputs",
                          "outputs",
                          "gates",
                          "depth",
                          "layout width (in tiles)",
                          "layout height (in tiles)",
                          "layout area (in tiles)",
                          "gates",
                          "wires",
                          "crossings",
                          "critical path",
                          "runtime (in sec)"};

    static constexpr auto bench_select = (experiments::epfl | experiments::iscas) & ~experiments::div &
                                         ~experiments::hyp & ~experiments::log2 & ~experiments::multiplier &
                                         ~experiments::sqrt & ~experiments::square & ~experiments::mem_ctrl;

    // for each EPFL and ISCAS benchmark
    for (const auto& benchmark : experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);

        run_ortho<aig_nt>(ortho_compare_exp, benchmark, "aig");
        run_ortho<xag_nt>(ortho_compare_exp, benchmark, "xag");

        ortho_compare_exp.save();
        ortho_compare_exp.table();
    }

    return 0;
}
