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

    const auto                      ntk    = blueprints::parity_network<mockturtle::aig_network>();
    const auto                      layout = orthogonal<gate_layout>(ntk);
    post_layout_optimization_params params{};

    BENCHMARK("post_layout_optimization: full optimization")
    {
        post_layout_optimization<gate_layout>(layout.clone());
    };

    params.max_gate_relocations = 0;

    BENCHMARK("post_layout_optimization: wiring reduction only")
    {
        post_layout_optimization<gate_layout>(layout.clone(), params);
    };
}

//  Mac M1, Sonoma 14.6.1, Apple clang version 15.0.0 (14.10.24)
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
// optimization                                   100             1     2.85441 m
//                                          1.72478 s     1.72069 s     1.73066 s
//                                         24.7886 ms    18.9018 ms    39.0736 ms
//
// post_layout_optimization: wiring
// reduction only                                 100             1     3.15921 s
//                                         31.5594 ms    31.2989 ms    32.6047 ms
//                                         2.37185 ms    392.966 us    5.59301 ms
//
//
// ===============================================================================
