//
// Created by Jan Drewniok on 22.11.23.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

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
}
//      Mac M1, Ventura 13.0, Apple clang version 14.0.0 (24.07.24)
//
//      Before PR #483:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//      -----------------------------------------------------------------------------
//      QuickExact          100                 1                       1.7502 m
//                          1.06044 s           1.05536 s               1.06813 s
//                          31.4551 ms          23.1414 ms              44.911 ms
//
//      QuickSim            100                 1                       570.56 ms
//                          6.02459 ms          5.92537 ms              6.12262 ms
//                          505.788 us          450.737 us              574.389 us

//      PR #483:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//      -----------------------------------------------------------------------------
//      QuickExact          100                 1                       1.69216 m
//                          1.01658 s           1.01384 s               1.02149 s
//                          18.1717 ms          11.4147 ms              28.4481 ms
//
//      QuickSim            100                 1                       492.005 ms
//                          4.85343 ms          4.80376 ms              4.98192 ms
//                          381.332 us          184.008 us              801.102 us
