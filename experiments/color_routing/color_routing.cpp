//
// Created by marcel on 14.02.21.
//

#if (FICTION_Z3_SOLVER)

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>      // routing based on graph coloring
#include <fiction/algorithms/physical_design/exact.hpp>              // SMT-based physical design of FCN layouts
#include <fiction/algorithms/physical_design/orthogonal.hpp>         // OGD-based physical design of FCN layouts
#include <fiction/algorithms/verification/equivalence_checking.hpp>  // equivalence checking of FCN layouts
#include <fiction/types.hpp>                                         // pre-defined types suitable for the FCN domain
#include <fiction/utils/routing_utils.hpp>                           // routing utility functions

#include <fmt/format.h>                      // output formatting
#include <lorina/lorina.hpp>                 // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/io/verilog_reader.hpp>  // call-backs to read Verilog files into networks
#include <mockturtle/networks/aig.hpp>       // AND-inverter graphs

#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

using gate_lyt = fiction::gate_level_layout<
    fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

using color_routing_experiment =
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, std::string_view, uint64_t, uint64_t, uint64_t,
                            uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, double, double, double, double,
                            bool>;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static fiction::exact_physical_design_stats      exact_stats{};
static fiction::orthogonal_physical_design_stats ortho_stats{};
static fiction::color_routing_stats              routing_stats{};
static fiction::equivalence_checking_stats       equiv_stats{};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    Ntk network{};

    const auto read_verilog_result =
        lorina::read_verilog(fiction_experiments::benchmark_path(name), mockturtle::verilog_reader(network));
    assert(read_verilog_result == lorina::return_code::success);

    return network;
}

template <typename Ntk, typename GateLyt, typename Stats>
void re_route_and_log(const std::string& benchmark, const Ntk& ntk, GateLyt& lyt,
                      const fiction::color_routing_params& routing_params, const Stats& stats,
                      color_routing_experiment& exp)
{
    routing_stats = {};
    equiv_stats   = {};

    // extract routing objectives
    const auto objectives = fiction::extract_routing_objectives(lyt);

    // remove routing
    clear_routing(lyt);

    // perform routing
    const auto success = fiction::color_routing(lyt, objectives, routing_params, &routing_stats);

    if (success)
    {
        // check equivalence
        fiction::equivalence_checking(ntk, lyt, &equiv_stats);
    }

    // log results
    exp(benchmark, ntk.num_pis(), ntk.num_pos(), ntk.num_gates(), lyt.get_clocking_scheme().name, lyt.x() + 1,
        lyt.y() + 1, (lyt.x() + 1) * (lyt.y() + 1), lyt.num_gates(), lyt.num_wires(), objectives.size(),
        routing_stats.number_of_unsatisfied_objectives, routing_stats.epg_stats.num_vertices,
        routing_stats.epg_stats.num_edges, mockturtle::to_seconds(stats.time_total),
        mockturtle::to_seconds(routing_stats.time_total), mockturtle::to_seconds(routing_stats.epg_stats.time_total),
        mockturtle::to_seconds(routing_stats.color_stats.time_total), equiv_stats.eq != fiction::eq_type::NO);

    exp.save();
    exp.table();
}

void smt_sat_complete()
{
    exact_stats = {};

    const std::array<std::string, 3> clocking_schemes{"2DDWave", "USE", "RES"};

    static color_routing_experiment color_routing_exp{"color_routing_smt_sat_complete",
                                                      "benchmark",
                                                      "inputs",
                                                      "outputs",
                                                      "nodes",
                                                      "clocking scheme",
                                                      "layout width (tiles)",
                                                      "layout height (tiles)",
                                                      "layout area (tiles)",
                                                      "gates",
                                                      "wires",
                                                      "routing objectives",
                                                      "unsat. objectives",
                                                      "EPG vertices",
                                                      "EPG edges",
                                                      "runtime exact (sec)",
                                                      "runtime color routing (sec)",
                                                      "runtime EPG (sec)",
                                                      "runtime graph coloring (sec)",
                                                      "equivalent"};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        for (const auto& clock : clocking_schemes)
        {
            // parameters for SMT-based physical design
            fiction::exact_physical_design_params exact_params{};
            exact_params.scheme        = clock;
            exact_params.crossings     = true;
            exact_params.border_io     = true;
            exact_params.desynchronize = true;
            exact_params.timeout       = 3'600'000;  // 1h in ms

            // perform layout generation with an SMT-based exact algorithm
            auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

            if (gate_level_layout.has_value())
            {
                re_route_and_log(benchmark, network, *gate_level_layout, routing_params, exact_stats,
                                 color_routing_exp);
            }
        }
    }
}

void ortho_sat_complete()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 75;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;

    static color_routing_experiment color_routing_exp{"color_routing_ortho_sat_complete",
                                                      "benchmark",
                                                      "inputs",
                                                      "outputs",
                                                      "nodes",
                                                      "clocking scheme",
                                                      "layout width (tiles)",
                                                      "layout height (tiles)",
                                                      "layout area (tiles)",
                                                      "gates",
                                                      "wires",
                                                      "routing objectives",
                                                      "unsat. objectives",
                                                      "EPG vertices",
                                                      "EPG edges",
                                                      "runtime ortho (sec)",
                                                      "runtime color routing (sec)",
                                                      "runtime EPG (sec)",
                                                      "runtime graph coloring (sec)",
                                                      "equivalent"};

    constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::majority &
                                            ~fiction_experiments::newtag & ~fiction_experiments::iscas85 &
                                            ~fiction_experiments::epfl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

void ortho_mcs()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 75;
    routing_params.engine                  = fiction::graph_coloring_engine::MCS;

    static color_routing_experiment color_routing_exp{"color_routing_ortho_mcs",
                                                      "benchmark",
                                                      "inputs",
                                                      "outputs",
                                                      "nodes",
                                                      "clocking scheme",
                                                      "layout width (tiles)",
                                                      "layout height (tiles)",
                                                      "layout area (tiles)",
                                                      "gates",
                                                      "wires",
                                                      "routing objectives",
                                                      "unsat. objectives",
                                                      "EPG vertices",
                                                      "EPG edges",
                                                      "runtime ortho (sec)",
                                                      "runtime color routing (sec)",
                                                      "runtime EPG (sec)",
                                                      "runtime graph coloring (sec)",
                                                      "equivalent"};

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

int main()  // NOLINT
{
    smt_sat_complete();
    ortho_sat_complete();
    ortho_mcs();

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
