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

#include <cstdint>

using namespace fiction;

using lattice       = sidb_100_cell_clk_lyt;
using lattice_siqad = sidb_100_cell_clk_lyt_siqad;

TEST_CASE("Benchmark simulators", "[benchmark]")
{
    // crossing bestagon gate
    lattice_siqad lyt{};

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
        const quickexact_params<cell<lattice_siqad>> sim_params{sidb_simulation_parameters{2, -0.32}};
        return quickexact<lattice_siqad>(lyt, sim_params);
    };

    BENCHMARK("QuickSim")
    {
        const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.32}};
        return quicksim<lattice_siqad>(lyt, quicksim_params);
    };

#if (FICTION_ALGLIB_ENABLED)
    BENCHMARK("ClusterComplete (multi-threaded)")
    {
        const clustercomplete_params<cell<lattice_siqad>> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice_siqad>(lyt, sim_params);
    };

    BENCHMARK("ClusterComplete (single-threaded)")
    {
        const clustercomplete_params<cell<lattice_siqad>> sim_params{
            sidb_simulation_parameters{3, -0.32}, {}, {}, {}, {}, 1};
        return clustercomplete<lattice_siqad>(lyt, sim_params);
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
//      After PR #390 (without jemalloc):
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
//
//      After PR #390 (with jemalloc):
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//      -----------------------------------------------------------------------------
//      QuickExact          100                 1                       1.71685 m
//                          1.05373 s           1.05177 s               1.05608 s
//                          10.8409 ms          9.05006 ms              13.7004 ms
//
//      QuickSim            100                 1                       269.791 ms
//                          2.73239 ms          2.69164 ms              2.83162 ms
//                          299.43 us           79.7179 us              533.137 us
//
//      ClusterComplete     100                 1                       1.25202 s
//                          8.38247 ms          6.48053 ms              13.3237 ms
//                          14.5352 ms          5.93141 ms              29.8427 ms

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (11.02.2025)
//
//      Before PR #664 (with jemalloc)
//      benchmark name                     samples         iterations         est run time
//                                         mean            low mean           high mean
//                                         std dev         low std dev        high std dev
//      -----------------------------------------------------------------------------------
//      QuickExact                         100             1                  1.68635 m
//                                         1.07188 s       1.06547 s          1.07837 s
//                                         32.9658 ms      31.1111 ms         35.3383 ms
//
//      QuickSim                           100             1                  266.508 ms
//                                         2.69043 ms      2.65618 ms         2.77416 ms
//                                         253.13 us       66.3756 us         465.027 us
//
//      ClusterComplete (multi-threaded)   100             1                  639.07 ms
//                                         5.70678 ms      5.14756 ms         6.73039 ms
//                                         3.73416 ms      2.33282 ms         5.75455 ms
//
//      ClusterComplete (single-threaded)  100             1                  759.588 ms
//                                         7.61088 ms      7.59401 ms         7.62819 ms
//                                         87.2281 us      75.7313 us         104.133 us
//
//      After PR #664 (with jemalloc)
//      benchmark name                       samples       iterations         est run time
//                                           mean          low mean           high mean
//                                           std dev       low std dev        high std dev
//      -----------------------------------------------------------------------------------
//      QuickExact                         100             1                  1.69636 m
//                                         1.01288 s       1.01171 s          1.01426 s
//                                         6.44908 ms      5.43149 ms         7.60552 ms
//
//      QuickSim                           100             1                  266.627 ms
//                                         2.73651 ms      2.69581 ms         2.83154 ms
//                                         298.359 us      152.152 us         521.457 us
//
//      ClusterComplete (multi-threaded)   100             1                  1.57307 s
//                                         5.22269 ms      4.8304 ms          5.95271 ms
//                                         2.66566 ms      1.61691 ms         4.20681 ms
//
//      ClusterComplete (single-threaded)  100             1                  726.747 ms
//                                         7.25989 ms      7.25401 ms         7.2662 ms
//                                         31.1296 us      27.4805 us         36.3467 us

#if (FICTION_ALGLIB_ENABLED)
TEST_CASE("Benchmark ClusterComplete", "[benchmark]")
{
    // number of non-terminating segments of a diagonal wire
    const auto create_diagonal_wire_with_n_non_terminating_segments = [](const uint64_t n)
    {
        hex_odd_row_gate_clk_lyt lyt{{(n + 1) / 2, n + 1}};

        uint64_t signal = lyt.create_pi("a", {0, 0});

        for (uint64_t i = 1; i < n + 1; i++)
        {
            signal = lyt.create_buf(signal, {i / 2, i});
        }

        lyt.create_po(signal, "o", {(n + 1) / 2, n + 1});

        return lyt;
    };

    const lattice cl_4_seg{apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library, hex_odd_row_gate_clk_lyt>(
        create_diagonal_wire_with_n_non_terminating_segments(2))};

    BENCHMARK("4 Segment Diagonal Bestagon Wire (multi-threaded)")
    {
        const clustercomplete_params<> sim_params{sidb_simulation_parameters{3, -0.32}};
        return clustercomplete<lattice>(cl_4_seg, sim_params);
    };

    const lattice cl_3_seg{apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library, hex_odd_row_gate_clk_lyt>(
        create_diagonal_wire_with_n_non_terminating_segments(1))};

    BENCHMARK("3 Segment Diagonal Bestagon Wire (single-threaded)")
    {
        const clustercomplete_params<> sim_params{sidb_simulation_parameters{3, -0.32}, {}, {}, {}, {}, 1};
        return clustercomplete<lattice>(cl_3_seg, sim_params);
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
//
//      With jemalloc:
//      benchmark name                      samples             iterations              est run time
//                                          mean                low mean                high mean
//                                          std dev             low std dev             high std dev
//      ---------------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                       1.39364 m
//                                          835.42 ms           833.679 ms              837.076 ms
//                                          8.68306 ms          7.61487 ms              10.1187 ms

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (11.02.2025)
//
//      Before PR #664 (with jemalloc)
//      benchmark name                      samples             iterations              est run time
//                                          mean                low mean                high mean
//                                          std dev             low std dev             high std dev
//      ---------------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                       1.38135 m
//      (multi-threaded)                    837.966 ms          835.809 ms              841.715 ms
//                                          14.1689 ms          9.41514 ms              24.4521 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                   100                 1                       19.6147 s
//                                          192.324 ms          191.795 ms              192.866 ms
//                                          2.73183 ms          2.45956 ms              3.0714 ms
//
//      After PR #664 (with jemalloc)
//      benchmark name                      samples             iterations              est run time
//                                          mean                low mean                high mean
//                                          std dev             low std dev             high std dev
//      ---------------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire
//      (multi-threaded)                    100                 1                       1.41724 m
//                                          834.071 ms          832.759 ms              835.443 ms
//                                          6.87635 ms          6.03154 ms              8.19222 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                   100                 1                       19.7644 s
//                                          190.366 ms          189.739 ms              191.044 ms
//                                          3.32957 ms          2.89922 ms              3.89814 ms
