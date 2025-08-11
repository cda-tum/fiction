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

#include <mockturtle/networks/aig.hpp>

using namespace fiction;

TEST_CASE("Benchmark Graph-Oriented Layout Design", "[benchmark]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto ntk    = blueprints::mux21_network<mockturtle::aig_network>();
    auto params = fiction::graph_oriented_layout_design_params{};

    BENCHMARK("graph_oriented_layout_design: high-efficiency")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: high-effort")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: highest-effort")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;

        return graph_oriented_layout_design<gate_layout>(ntk, params);
    };

    BENCHMARK("graph_oriented_layout_design: maximum-effort")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.seed = 12345;

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

//  Mac M1, Sequoia 15.5, Apple clang version 17.0.0 (19.05.25)
// -------------------------------------------------------------------------------
// Benchmark Graph-Oriented Layout Design
// -------------------------------------------------------------------------------
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// graph_oriented_layout_design: high-efficiency
//
//                                         154.203 ms    152.139 ms    157.968 ms
//                                           13.77 ms    8.91136 ms    25.9232 ms
//
// graph_oriented_layout_design: high-effort
//
//                                          1.35733 s     1.32885 s     1.38707 s
//                                         148.219 ms    132.249 ms    167.853 ms
//
// graph_oriented_layout_design: highest-effort
//
//                                            5.009 s     4.84578 s     5.19168 s
//                                         880.273 ms    771.159 ms     1.03008 s
//
// graph_oriented_layout_design: maximum-effort
//
//                                          19.0867 s      18.346 s     19.8665 s
//                                          3.88072 s     3.37165 s     4.53847 s
//
// graph_oriented_layout_design: single-threading
//
//                                          2.30311 s     2.28982 s     2.31924 s
//                                         74.3541 ms    61.6422 ms    101.801 ms
//
// graph_oriented_layout_design: multi-threading
//
//                                         623.524 ms    614.624 ms     633.33 ms
//                                         47.7563 ms    41.6035 ms    55.7239 ms
//
//
// ===============================================================================
