//
// Created by Jan Drewniok on 22.11.23.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

using namespace fiction;

using lattice = sidb_100_cell_clk_lyt_siqad;

TEST_CASE("Benchmark simulators", "[benchmark]")
{
    // crossing bestagon gate
    lattice lyt{};

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 11, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    BENCHMARK("QuickExact")
    {
        const quickexact_params<cell<lattice>> sim_params{sidb_simulation_parameters{2, -0.32}};
        return quickexact<lattice>(lyt, sim_params);
    };

    BENCHMARK("QuickSim")
    {
        const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.32}};
        return quicksim<lattice>(lyt, quicksim_params);
    };

#if (FICTION_ALGLIB_ENABLED)
    BENCHMARK("ClusterComplete")
    {
        const clustercomplete_params<cell<lattice>> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(lyt, sim_params);
    };
#endif  // FICTION_ALGLIB_ENABLED
}
//  Mac M1, Ventura 13.0, Apple clang version 14.0.0 (22.11.23)

//    Before PR #388:
//    benchmark name    samples          iterations          est run time
//                      mean             low mean            high mean
//                      std dev          low std dev         high std dev
//    -------------------------------------------------------------------------------
//    QuickExact        100             1                    1.75185 m
//                      1.03471 s       1.03254 s            1.03694 s
//                      11.2095 ms      10.2241 ms           12.5137 ms
//
//    QuickSim          100              1                   449.61 ms
//                      4.49349 ms      4.47125 ms           4.51652 ms
//                      115.682 us      101.563 us           135.051 us

//    PR #388:
//    benchmark name    samples          iterations          est run time
//                      mean             low mean            high mean
//                      std dev          low std dev         high std dev
//    -------------------------------------------------------------------------------
//    QuickExact        100             1                    1.69339 m
//                      1.03102 s       1.028 s              1.03517 s
//                      17.8754  ms     14.3439 ms           27.4865 ms
//
//    QuickSim          100             1                   437.706 ms
//                      4.32637 ms      4.29823 ms          4.358931 ms
//                      154.258 us      132.162 us          194.857 us

//  Intel Core i6700HQ (8 cores @ 2.60 GHz), Ubuntu 22.04.2 LTS, Ubuntu clang version 14.0.0 (13.04.24)

//    before "Add and subtract parent potential only once"
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.20383 m
//                            1.87035 s     1.85331 s     1.89046 s
//                            94.185 ms     79.5784 ms    114.325 ms
//
//    QuickSim                100           1             1.01174 s
//                            10.3755 ms    10.2221 ms    10.5144 ms
//                            742.128 us    579.253 us    977.237 us
//
//    ClusterComplete         100           1             6.36677 s
//                            68.1879 ms    66.8025 ms    69.7052 ms
//                            7.4455 ms     6.55368 ms    8.6877 ms

//    after "Add and subtract parent potential only once"
//    before "Store composition bounds to subtract after recursion"
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.05071 m
//                            1.86217 s     1.84601 s     1.88107 s
//                            88.7133 ms    77.0255 ms    114.574 ms
//
//    QuickSim                100           1             966.661 ms
//                            9.82326 ms    9.69725 ms    9.95392 ms
//                            655.91 us     533.255 us    871.692 us
//
//    ClusterComplete         100           1             5.99807 s
//                            62.7459 ms    61.3404 ms    64.3648 ms
//                            7.71925 ms    6.65408 ms    9.13036 ms

//    after "Store composition bounds to subtract after recursion"
//    before "Restructured stores of potential bounds"
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.09518 m
//                            1.86457 s     1.84747 s     1.88346 s
//                            91.8324 ms    82.2694 ms    102.996 ms
//
//    QuickSim                100           1             941.808 ms
//                            9.84096 ms    9.75242 ms    9.94587 ms
//                            491.07 us     387.717 us    673.361 us
//
//    ClusterComplete         100           1             4.60825 s
//                            45.0607 ms    44.2967 ms    45.9833 ms
//                            4.25241 ms    3.59799 ms    5.09686 ms

//    after "Restructured stores of potential bounds"
//    before "Compute external projections for each composition in GSS"
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.16303 m
//                            1.8573 s      1.83863 s     1.87994 s
//                            104.263 ms    86.6128 ms    131.582 ms
//
//    QuickSim                100           1             928.817 ms
//                            9.50481 ms    9.38153 ms    9.67599 ms
//                            731.198 us    519.946 us    1.15811 ms
//
//    ClusterComplete         100           1             2.29913 s
//                            22.6208 ms    22.2895 ms    23.0093 ms
//                            1.82446 ms    1.58868 ms    2.0759 ms

//    after "Compute external projections for each composition in GSS"
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.07802 m
//                            1.82677 s     1.81125 s     1.84519 s
//                            85.8776 ms    73.6874 ms    102.415 ms
//
//    QuickSim                100           1             949.075 ms
//                            9.6784 ms     9.55572 ms    9.81532 ms
//                            659.103 us    529.837 us    908.398 us
//
//    ClusterComplete         100           1             1.78456 s
//                            17.2877 ms    17.0146 ms    17.5923 ms
//                            1.46908 ms    1.29383 ms    1.70859 ms

#if (FICTION_ALGLIB_ENABLED)
TEST_CASE("Benchmark ClusterComplete", "[benchmark]")
{
    // number of non-terminating segments of a diagonal wire
    const uint64_t i = 1;

    hex_odd_row_gate_clk_lyt lyt{{(i + 1) / 2, i + 1}};

    uint64_t signal = lyt.create_pi("a", {0, 0});

    for (uint64_t j = 1; j < i + 1; j++)
    {
        signal = lyt.create_buf(signal, {j / 2, j});
    }

    lyt.create_po(signal, "o", {(i + 1) / 2, i + 1});

    const lattice cl{convert_to_siqad_coordinates(
        apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library, hex_odd_row_gate_clk_lyt>(lyt))};

#if (FICTION_ALGLIB_ENABLED)
    BENCHMARK("3 Segment Diagonal Bestagon Wire")
    {
        const clustercomplete_params<cell<lattice>> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(cl, sim_params);
    };
#endif  // FICTION_ALGLIB_ENABLED
}
#endif  // FICTION_ALGLIB_ENABLED
//  Intel Core i6700HQ (8 cores @ 2.60 GHz), Ubuntu 22.04.2 LTS, Ubuntu clang version 14.0.0 (13.04.24)

//    before "Add and subtract parent potential only once"
//    benchmark name                          samples       iterations    est run time
//                                            mean          low mean      high mean
//                                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    3 Segment Diagonal Bestagon Wire        100           1             2.86089 m
//                                            1.57859 s     1.56147 s     1.60054 s
//                                            98.2992 ms    79.0445 ms    133.074 ms

//    after "Add and subtract parent potential only once"
//    before "Store composition bounds to subtract after recursion"
//    benchmark name                          samples       iterations    est run time
//                                            mean          low mean      high mean
//                                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    3 Segment Diagonal Bestagon Wire        100           1             2.43878 m
//                                            1.43439 s     1.42121 s     1.44845 s
//                                            69.1732 ms    60.7787 ms    80.0373 ms

//    after "Store composition bounds to subtract after recursion"
//    before "Restructured stores of potential bounds"
//    benchmark name                          samples       iterations    est run time
//                                            mean          low mean      high mean
//                                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    3 Segment Diagonal Bestagon Wire        100           1             1.88129 m
//                                            1.12828 s     1.11773 s     1.14043 s
//                                            57.5402 ms    48.6612 ms    71.5112 ms

//    after "Restructured stores of potential bounds"
//    before "Compute external projections for each composition in GSS"
//    benchmark name                          samples       iterations    est run time
//                                            mean          low mean      high mean
//                                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    3 Segment Diagonal Bestagon Wire        100           1             51.5535 s
//                                            579.269 ms    571.672 ms    588.093 ms
//                                            41.8846 ms    35.4194 ms    51.1304 ms

//    after "Compute external projections for each composition in GSS"
//    benchmark name                          samples       iterations    est run time
//                                            mean          low mean      high mean
//                                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    3 Segment Diagonal Bestagon Wire        100           1             44.3262 s
//                                            454.74 ms     448.366 ms    461.915 ms
//
