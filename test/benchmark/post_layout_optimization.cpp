//
// Created by simon on 14.10.24.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>

#include <mockturtle/networks/aig.hpp>

using namespace fiction;

TEST_CASE("Benchmark Post-Layout Optimization", "[benchmark]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    const auto ntk    = blueprints::parity_network<mockturtle::aig_network>();
    const auto layout = orthogonal<gate_layout>(ntk);

    post_layout_optimization_params full_optimization_params{};

    full_optimization_params.max_gate_relocations = (layout.x() + 1) * (layout.y() + 1);
    full_optimization_params.optimize_pos_only    = false;
    full_optimization_params.planar_optimization  = false;
    full_optimization_params.timeout              = 100000;

    BENCHMARK("post_layout_optimization: full optimization")
    {
        post_layout_optimization<gate_layout>(layout.clone(), full_optimization_params);
    };

    post_layout_optimization_params wiring_reduction_only_params{};

    wiring_reduction_only_params.max_gate_relocations = 0;
    wiring_reduction_only_params.optimize_pos_only    = false;
    wiring_reduction_only_params.planar_optimization  = false;
    wiring_reduction_only_params.timeout              = 100000;

    BENCHMARK("post_layout_optimization: wiring reduction only")
    {
        post_layout_optimization<gate_layout>(layout.clone(), wiring_reduction_only_params);
    };
}

//  Mac M1, Sonoma 14.6.1, Apple clang version 15.0.0 (07.11.24)
// -------------------------------------------------------------------------------
// Benchmark Post-Layout Optimization
// -------------------------------------------------------------------------------
// /Users/simonhofmann/Documents/fiction_fork/fiction/test/benchmark/post_layout_optimization.cpp:24
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// post_layout_optimization: full
// optimization                                   100             1     2.72845 m
//                                           1.6211 s     1.61291 s     1.64963 s
//                                          69.565 ms    21.7338 ms    158.472 ms
//
// post_layout_optimization: wiring
// reduction only                                 100             1     2.89888 s
//                                         30.5434 ms    30.3498 ms    30.9227 ms
//                                         1.32943 ms    827.313 us    2.53012 ms
//
//
// ===============================================================================
