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
}

//  Mac M1, Sonoma 14.6.1, Apple clang version 15.0.0 (13.09.24)

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
//                                         159.512 ms    157.588 ms    161.348 ms
//                                         9.57403 ms    8.58416 ms    10.7949 ms
//
// graph_oriented_layout_design: high effort
//
//                                          1.43071 s     1.40021 s     1.46216 s
//                                         157.737 ms    136.972 ms    185.544 ms
//
// graph_oriented_layout_design: highest effort
//
//                                          3.98571 s     3.79708 s     4.18383 s
//                                         987.697 ms    887.173 ms     1.10834 s
//
//
// ===============================================================================
