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

    ntk                 = blueprints::parity_network<mockturtle::aig_network>();
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

//  Mac M1, Sonoma 14.6.1, Apple clang version 15.0.0 (25.09.24)
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
//                                         148.492 ms    146.396 ms    150.925 ms
//                                         11.4994 ms    9.88837 ms    13.6356 ms
//
// graph_oriented_layout_design: high effort
//
//                                          1.23503 s     1.20168 s     1.27147 s
//                                         178.104 ms    159.325 ms    202.161 ms
//
// graph_oriented_layout_design: highest effort
//
//                                           3.2585 s      3.0587 s     3.47545 s
//                                          1.06581 s    963.829 ms     1.19622 s
//
// graph_oriented_layout_design: singlethreading
//
//                                          2.49857 s     2.46866 s     2.52899 s
//                                         152.931 ms     137.96 ms     170.69 ms
//
// graph_oriented_layout_design: multithreading
//
//                                         601.433 ms    593.498 ms    609.969 ms
//                                         42.0223 ms    36.1444 ms    50.4232 ms
//
//
// ===============================================================================
