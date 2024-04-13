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
        const quickexact_params<lattice> sim_params{sidb_simulation_parameters{2, -0.32}};
        return quickexact<lattice>(lyt, sim_params);
    };

    BENCHMARK("QuickSim")
    {
        const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.32}};
        return quicksim<lattice>(lyt, quicksim_params);
    };

    BENCHMARK("ClusterComplete")
    {
        const clustercomplete_params<lattice> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(lyt, sim_params);
    };
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

//
//    benchmark name          samples       iterations    est run time
//                            mean          low mean      high mean
//                            std dev       low std dev   high std dev
//    -------------------------------------------------------------------------------
//    QuickExact              100           1             3.95783 m
//                            2.3555 s      2.33626 s     2.37767 s
//                            106.153 ms    90.4958 ms    135.246 ms
//
//    QuickSim                100           1             1.49393 s
//                            29.9329 ms    28.5082 ms    31.3671 ms
//                            7.31298 ms    6.70416 ms    8.12608 ms
//
//    ClusterComplete         100           1             17.9353 s
//                            151.302 ms     145.56 ms    157.719 ms
//                            30.9984 ms    27.1551 ms    35.8004 ms

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

    BENCHMARK("3 Segment Diagonal Bestagon Wire")
    {
        const clustercomplete_params<lattice> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(cl, sim_params);
    };
}
//  Intel Core i6700HQ (8 cores @ 2.60 GHz), Ubuntu 22.04.2 LTS, Ubuntu clang version 14.0.0 (13.04.24)

//      benchmark name                          samples       iterations    est run time
//                                              mean          low mean      high mean
//                                              std dev       low std dev   high std dev
//      -------------------------------------------------------------------------------
//      3 Segment Diagonal Bestagon Wire        100           1             3.99949 m
//                                              1.77163 s     1.75544 s     1.78914 s
//                                              85.8704 ms    71.0768 ms    111.453 ms
