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

#include <cstdint>

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
//      Mac M1, Sequoia 15.2, Apple clang version 14.0.3 (16.12.24)
//
//      Before PR #602:
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
//
//      PR #602:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//    -------------------------------------------------------------------------------
//      QuickExact          100                 1                       1.68503 m
//                          1.01966 s           1.01725 s               1.02271 s
//                          13.7569 ms          11.465 ms               18.3512 ms
//
//      QuickSim            100                 1                       445.639 ms
//                          4.50754 ms          4.47813 ms              4.54016 ms
//                          158.347 us          137.998 us              187.498 us

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (15.01.2025)
//
//      Before PR #390:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//      -----------------------------------------------------------------------------
//      QuickExact          100                 1                       1.6412 m
//                          1.01839 s           1.01649 s               1.02095 s
//                          11.1488 ms          8.8668 ms               14.6936 ms
//
//      QuickSim            100                 1                       202.955 ms
//                          2.00015 ms          1.97047 ms              2.05573 ms
//                          199.837 us          129.863 us              357.338 us
//
//      After PR #390:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//      -----------------------------------------------------------------------------
//      QuickExact          100                 1                       1.65065 m
//                          1.01159 s           1.006 s                 1.0285 s
//                          45.175 ms           18.3951 ms              98.1386 ms
//
//      QuickSim            100                 1                       185.262 ms
//                          1.87537 ms          1.85862 ms              1.92868 ms
//                          138.199 us          51.2677 us              305.326 us
//
//      ClusterComplete     100                 1                       633.861 ms
//                          20.5306 ms          16.53 ms                26.2279 ms
//                          24.2954 ms          18.5671 ms              31.7567 ms

#if (FICTION_ALGLIB_ENABLED)
TEST_CASE("Benchmark ClusterComplete", "[benchmark]")
{
    // number of non-terminating segments of a diagonal wire
    const uint64_t i = 2;

    hex_odd_row_gate_clk_lyt lyt{{(i + 1) / 2, i + 1}};

    uint64_t signal = lyt.create_pi("a", {0, 0});

    for (uint64_t j = 1; j < i + 1; j++)
    {
        signal = lyt.create_buf(signal, {j / 2, j});
    }

    lyt.create_po(signal, "o", {(i + 1) / 2, i + 1});

    const lattice cl{convert_layout_to_siqad_coordinates(
        apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library, hex_odd_row_gate_clk_lyt>(lyt))};

    BENCHMARK("4 Segment Diagonal Bestagon Wire")
    {
        const clustercomplete_params<cell<lattice>> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(cl, sim_params);
    };
}
#endif  // FICTION_ALGLIB_ENABLED
//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (15.01.2025)
//
//      Without jemalloc:
//      benchmark name                      samples             iterations              est run time
//                                          mean                low mean                high mean
//                                          std dev             low std dev             high std dev
//      ---------------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                       2.39773 m
//                                          1.44927 s           1.44573 s               1.45397 s
//                                          20.5985 ms          16.1537 ms              32.1081 ms
