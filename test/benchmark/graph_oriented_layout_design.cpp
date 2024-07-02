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
    using Ntk         = mockturtle::aig_network;

    auto ntk    = blueprints::mux21_network<mockturtle::aig_network>();
    auto params = fiction::graph_oriented_layout_design_params{};

    BENCHMARK("graph_oriented_layout_design: low effort")
    {
        params.high_effort = false;

        return graph_oriented_layout_design<gate_layout, Ntk>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: high effort")
    {
        params.high_effort = true;
        return graph_oriented_layout_design<gate_layout, Ntk>(ntk, params);
    };
}

//  Mac M1, Sonoma 14.5, Apple clang version 15.0.0 (20.06.24)

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
//                                         192.194 ms    185.732 ms     200.42 ms
//                                         36.8634 ms    30.3139 ms    51.0752 ms
//
// graph_oriented_layout_design: high
//
//                                          1.53332 s     1.49446 s     1.57515 s
//                                         205.485 ms    179.755 ms    237.039 ms
//
//
// ===============================================================================
