//
// Created by simon on 19.06.24.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <bill/sat/interface/common.hpp>
#include <mockturtle/networks/aig.hpp>

using namespace fiction;

TEST_CASE("Benchmark Graph-Oriented Layout Design", "[benchmark]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto ntk    = blueprints::mux21_network<mockturtle::aig_network>();
    auto params = fiction::graph_oriented_layout_design_params{};

    BENCHMARK("graph_oriented_layout_design: high effiency")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: high effort")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: highest effort")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    ntk             = blueprints::parity_network<mockturtle::aig_network>();
    params.return_first = true;
    params.mode         = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
    BENCHMARK("graph_oriented_layout_design: single-threading")
    {
        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: multi-threading")
    {
        params.enable_multithreading = true;
        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };
}

//  Mac M1, Sonoma 14.6.1, Apple clang version 15.0.0 (23.09.24)
// -------------------------------------------------------------------------------
// Benchmark Graph-Oriented Layout Design
// -------------------------------------------------------------------------------
// /Users/simonhofmann/Documents/fiction_fork/fiction/test/benchmark/graph_oriented_layout_design.cpp:23
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// graph_oriented_layout_design: high efficiency
//
//                                         149.819 ms    148.027 ms    151.769 ms
//                                         9.54832 ms    8.51383 ms    10.8984 ms
//
// graph_oriented_layout_design: high effort
//
//                                          1.29868 s     1.26809 s      1.3326 s
//                                         164.453 ms    145.119 ms    192.831 ms
//
// graph_oriented_layout_design: highest effort
//
//                                          3.03282 s     2.90738 s     3.16135 s
//                                         644.494 ms    580.832 ms     717.08 ms
//
// graph_oriented_layout_design: singlethreading
//
//                                          2.44054 s     2.42026 s     2.46167 s
//                                         105.869 ms    94.1699 ms    121.157 ms
//
// graph_oriented_layout_design: multithreading
//
//                                          666.48 ms    654.382 ms    681.687 ms
//                                         69.0098 ms    56.0641 ms     86.388 ms
//
//
// ===============================================================================
