//
// Created by marcel on 14.02.24.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/physical_design/determine_clocking.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <bill/sat/interface/common.hpp>
#include <mockturtle/networks/aig.hpp>

using namespace fiction;

template <typename Lyt>
void remove_clocking(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    lyt.foreach_tile([&lyt](const auto& t) { lyt.assign_clock_number(t, 0); });
}

TEST_CASE("Benchmark SAT-based clocking determination", "[benchmark]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto lyt = orthogonal<gate_layout>(blueprints::nary_operation_network<mockturtle::aig_network>());
    remove_clocking(lyt);

    auto params = determine_clocking_params{};

    BENCHMARK("determine_clocking: ghack")
    {
        params.sat_engine = bill::solvers::ghack;

        return determine_clocking(lyt, params);
    };

    BENCHMARK("determine_clocking: glucose_41")
    {
        params.sat_engine = bill::solvers::glucose_41;

        return determine_clocking(lyt, params);
    };

    BENCHMARK("determine_clocking: bsat2")
    {
        params.sat_engine = bill::solvers::bsat2;

        return determine_clocking(lyt, params);
    };

#if !defined(BILL_WINDOWS_PLATFORM)
    BENCHMARK("determine_clocking: maple")
    {
        params.sat_engine = bill::solvers::maple;

        return determine_clocking(lyt, params);
    };

    BENCHMARK("determine_clocking: bmcg")
    {
        params.sat_engine = bill::solvers::bmcg;

        return determine_clocking(lyt, params);
    };
#endif
}

// AMD Ryzen 7 Pro 5850U @ 4.5 GHz, 32 GB DDR4 RAM, Manjaro Linux Kernel 6.5, g++ 13.2.1, February, 14, 2024

// -------------------------------------------------------------------------------
//  Benchmark SAT-based clocking determination
// -------------------------------------------------------------------------------
// / home/marcel/git/fiction/test/benchmark/determine_clocking.cpp:33
// ...............................................................................
//
//  benchmark name                       samples       iterations    est run time
//                                       mean          low mean      high mean
//                                       std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
//  determine_clocking: ghack                     100             1    252.841 ms
//                                         2.52136 ms    2.51465 ms    2.52896 ms
//                                         36.0955 us     31.418 us    45.6648 us
//
//  determine_clocking: glucose_41                100             1     260.79 ms
//                                         2.75101 ms    2.72807 ms    2.77364 ms
//                                         116.957 us    108.018 us    127.595 us
//
//  determine_clocking: bsat2                     100             1    242.718 ms
//                                         2.40227 ms    2.39682 ms    2.40785 ms
//                                         28.0132 us    24.5231 us    32.3819 us
//
//  determine_clocking: maple                     100             1    256.312 ms
//                                         2.57518 ms    2.56575 ms    2.59061 ms
//                                         60.3798 us    42.5418 us    104.924 us
//
//  determine_clocking: bmcg                      100             1     282.23 ms
//                                         2.80308 ms    2.79577 ms    2.82136 ms
//                                         55.3074 us    28.0592 us    115.087 us
//
//
// ===============================================================================
