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

    BENCHMARK("graph_oriented_layout_design: low effort")
    {
        params.high_effort_mode = false;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: high effort")
    {
        params.high_effort_mode = true;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };
}

//  Mac M1, Sonoma 14.5, Apple clang version 15.0.0 (09.08.24)

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
// graph_oriented_layout_design: low
//
//                                         157.227 ms    155.044 ms    159.725 ms
//                                         11.8494 ms    10.1235 ms    14.8552 ms
//
// graph_oriented_layout_design: high
//
//                                          1.34778 s     1.32165 s     1.37673 s
//                                          1.34778 s     1.32165 s     1.37673 s
//
//
// ===============================================================================
