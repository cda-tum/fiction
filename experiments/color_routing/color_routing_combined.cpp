//
// Created by marcel on 14.02.21.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>      // routing based on graph coloring
#include <fiction/algorithms/physical_design/exact.hpp>              // SMT-based physical design of FCN layouts
#include <fiction/algorithms/physical_design/orthogonal.hpp>         // OGD-based physical design of FCN layouts
#include <fiction/algorithms/verification/equivalence_checking.hpp>  // equivalence checking of FCN layouts
#include <fiction/technology/technology_mapping_library.hpp>         // pre-defined gate types for technology mapping
#include <fiction/types.hpp>                                         // pre-defined types suitable for the FCN domain
#include <fiction/utils/routing_utils.hpp>                           // routing utility functions

#include <fmt/format.h>                                        // output formatting
#include <lorina/lorina.hpp>                                   // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/algorithms/cut_rewriting.hpp>             // logic optimization with cut rewriting
#include <mockturtle/algorithms/mapper.hpp>                    // Technology mapping on the logic level
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>  // NPN databases for cut rewriting of XAGs and AIGs
#include <mockturtle/io/verilog_reader.hpp>                    // call-backs to read Verilog files into networks
#include <mockturtle/networks/aig.hpp>                         // AND-inverter graphs
#include <mockturtle/utils/tech_library.hpp>                   // technology library utils

#include <cstdint>
#include <string>

// using gate_lyt = fiction::cart_gate_clk_lyt;

using gate_lyt = fiction::gate_level_layout<
    fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

using color_routing_experiment =
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint32_t, uint32_t,
                            uint64_t, uint64_t, uint64_t, uint64_t, double, double, double, double, bool>;

static fiction::exact_physical_design_stats      exact_stats{};
static fiction::orthogonal_physical_design_stats ortho_stats{};
static fiction::color_routing_stats              routing_stats{};
static fiction::equivalence_checking_stats       equiv_stats{};

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
    exp(benchmark, ntk.num_pis(), ntk.num_pos(), ntk.num_gates(), lyt.x() + 1, lyt.y() + 1,
        (lyt.x() + 1) * (lyt.y() + 1), lyt.num_gates(), lyt.num_wires(), objectives.size(),
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

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an SMT-based exact algorithm
        auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

        static color_routing_experiment color_routing_exp{"color_routing_smt_sat_complete",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        if (gate_level_layout.has_value())
        {
            re_route_and_log(benchmark, network, *gate_level_layout, routing_params, exact_stats, color_routing_exp);
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

    constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::one_bit_add_aoig &
                                            ~fiction_experiments::majority & ~fiction_experiments::newtag &
                                            ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        static color_routing_experiment color_routing_exp{"color_routing_ortho_sat_complete",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

void smt_sat_partial()
{
    exact_stats = {};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 40;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;
    routing_params.partial_sat             = true;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an SMT-based exact algorithm
        auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

        static color_routing_experiment color_routing_exp{"color_routing_smt_sat_complete",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        if (gate_level_layout.has_value())
        {
            re_route_and_log(benchmark, network, *gate_level_layout, routing_params, exact_stats, color_routing_exp);
        }
    }
}

void ortho_sat_partial()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 40;
    routing_params.engine                  = fiction::graph_coloring_engine::SAT;
    routing_params.partial_sat             = true;

    constexpr const uint64_t bench_select = fiction_experiments::all & ~fiction_experiments::one_bit_add_aoig &
                                            ~fiction_experiments::c17 & ~fiction_experiments::majority &
                                            ~fiction_experiments::newtag & ~fiction_experiments::clpl;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        static color_routing_experiment color_routing_exp{"color_routing_ortho_sat_partial",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

void smt_mcs()
{
    exact_stats = {};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.engine                  = fiction::graph_coloring_engine::MCS;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an SMT-based exact algorithm
        auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

        static color_routing_experiment color_routing_exp{"color_routing_smt_mcs",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        if (gate_level_layout.has_value())
        {
            re_route_and_log(benchmark, network, *gate_level_layout, routing_params, exact_stats, color_routing_exp);
        }
    }
}

void ortho_mcs()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 30;
    routing_params.engine                  = fiction::graph_coloring_engine::MCS;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        static color_routing_experiment color_routing_exp{"color_routing_ortho_mcs",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

void smt_lmxrlf()
{
    exact_stats = {};

    // parameters for SMT-based physical design
    fiction::exact_physical_design_params<gate_lyt> exact_params{};
    exact_params.scheme        = fiction::ptr<gate_lyt>(fiction::twoddwave_clocking<gate_lyt>(fiction::num_clks::FOUR));
    exact_params.crossings     = true;
    exact_params.border_io     = true;
    exact_params.desynchronize = true;
    exact_params.timeout       = 3'600'000;  // 1h in ms

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.engine                  = fiction::graph_coloring_engine::LMXRLF;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an SMT-based exact algorithm
        auto gate_level_layout = fiction::exact<gate_lyt>(network, exact_params, &exact_stats);

        static color_routing_experiment color_routing_exp{"color_routing_smt_lmxrlf",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        if (gate_level_layout.has_value())
        {
            re_route_and_log(benchmark, network, *gate_level_layout, routing_params, exact_stats, color_routing_exp);
        }
    }
}

void ortho_lmxrlf()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 30;
    routing_params.engine                  = fiction::graph_coloring_engine::LMXRLF;

    constexpr const uint64_t bench_select = fiction_experiments::all;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<fiction::tec_nt>(benchmark);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &ortho_stats);

        static color_routing_experiment color_routing_exp{"color_routing_ortho_lmxrlf",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

void ortho_large_mcs()
{
    ortho_stats = {};

    // parameters for SAT-based color routing
    fiction::color_routing_params routing_params{};
    routing_params.conduct_partial_routing = true;
    routing_params.crossings               = true;
    routing_params.path_limit              = 5;
    routing_params.engine                  = fiction::graph_coloring_engine::MCS;

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

    constexpr const uint64_t bench_select = experiments::c432;

    for (const auto& benchmark : experiments::all_benchmarks(bench_select))
    {
        const auto network = read_ntk<mockturtle::aig_network>("ISCAS85/" + benchmark);

        // rewrite network cuts using the given re-synthesis function
        const auto cut_network = mockturtle::cut_rewriting(network, resynthesis_function, cut_params);

        // perform technology mapping
        const auto mapped_network = mockturtle::map(cut_network, gate_lib, map_params);

        // perform layout generation with an OGD-based algorithm
        auto gate_level_layout = fiction::orthogonal<gate_lyt>(mapped_network, {}, &ortho_stats);

        static color_routing_experiment color_routing_exp{"color_routing_ortho_large_mcs",
                                                          "benchmark",
                                                          "inputs",
                                                          "outputs",
                                                          "nodes",
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

        re_route_and_log(benchmark, network, gate_level_layout, routing_params, ortho_stats, color_routing_exp);
    }
}

int main()
{
    const std::string layouts_folder = fmt::format("{}/color_routing/layouts", EXPERIMENTS_PATH);

    //    smt_sat_complete();
    //    ortho_sat_complete();
    //    smt_sat_partial();
    //    ortho_sat_partial();
    //    smt_mcs();
    //    ortho_mcs();
    //    smt_lmxrlf();
    //    ortho_lmxrlf();

    ortho_large_mcs();

    return 0;
}
