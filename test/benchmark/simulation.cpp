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
            sidb_simulation_parameters{3, -0.32}, {}, {}, 6, 6, 1};
        return clustercomplete<lattice_siqad>(lyt, sim_params);
    };
#endif  // FICTION_ALGLIB_ENABLED
}
//      Mac M1, Sequoia 15.2, Apple clang version 14.0.3 (16.12.24)
//
//      Before PR #679:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//    -------------------------------------------------------------------------------
//      QuickExact          100                 1                       1.7844 m
//                          1.07493 s           1.07269 s               1.07752 s
//                          12.2561 ms          10.7246 ms              13.8817 ms
//
//      QuickSim            100                 1                       445.05 ms
//                          4.43629 ms          4.41687 ms              4.46476 ms
//                          118.778 us          88.1746 us              158.552 us
//
//       ClusterComplete    100                 1                       299.125 ms
//                          3.00976 ms          2.98784 ms              3.03247 ms
//                          113.591 us          101.271 us              128.938 us
//      After PR #679:
//      benchmark name      samples             iterations              est run time
//                          mean                low mean                high mean
//                          std dev             low std dev             high std dev
//    -------------------------------------------------------------------------------
//      QuickExact          100                 1                       1.78579 m
//                          1.06699 s           1.06575 s               1.06858 s
//                          7.12851 ms          5.64407 ms              10.528 ms
//
//      QuickSim            100                 1                       551.093 ms
//                          5.54284 ms          5.52032 ms              5.57622 ms
//                          137.976 us          100.606 us              201.053 us
//
//      ClusterComplete     100                 1                       298.05 ms
//                          3.01338 ms          2.98089 ms              3.12774 ms
//                          278.749 us          79.1939 us              640.667 us

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
//      QuickExact                         100             1                  1.68023 m
//                                         1.02348 s       1.02048 s          1.02689 s
//                                         16.327 ms       14.0743 ms         19.8756 ms
//
//      QuickSim                           100             1                  274.735 ms
//                                         2.79275 ms      2.76267 ms         2.88513 ms
//                                         244.823 us      91.1395 us         536.164 us
//
//      ClusterComplete (multi-threaded)   100             1                  7.14016 s
//                                         7.43542 ms      6.28543 ms         9.75564 ms
//                                         8.07302 ms      4.73162 ms         14.0938 ms
//
//      ClusterComplete (single-threaded)  100             1                  726.747 ms
//                                         7.08628 ms      7.07611 ms         7.09851 ms
//                                         56.9189 us      47.144 us          73.1902 us

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (12.05.2025)
//
//      After PR #738 (without jemalloc)
//      benchmark name                       samples         iterations       est run time
//                                           mean            low mean         high mean
//                                           std dev         low std dev      high std dev
//      ------------------------------------------------------------------------------------
//      QuickExact                           100              1               1.62095 m
//                                           1.0263 s         1.02484 s       1.02932 s
//                                           10.2365 ms       6.13745 ms      16.0045 ms
//
//      QuickSim                             100              1               269.557 ms
//                                           2.74004 ms       2.71588 ms      2.80411 ms
//                                           185.301 us       74.8133 us      370.784 us
//
//      ClusterComplete (multi-threaded)     100              1               2.23624 s
//                                           9.91647 ms       7.37725 ms      15.7107 ms
//                                           18.5248 ms       7.67377 ms      31.9493 ms
//
//      ClusterComplete (single-threaded)    100              1               705.844 ms
//                                           7.06819 ms       7.05496 ms      7.08097 ms
//                                           66.2555 us       55.7839 us      80.9501 us
//
//      After PR #738 (with jemalloc)
//      benchmark name                       samples         iterations       est run time
//                                           mean            low mean         high mean
//                                           std dev         low std dev      high std dev
//      ------------------------------------------------------------------------------------
//      QuickExact                           100              1               1.61535 m
//                                           1.02306 s        1.02131 s       1.02654 s
//                                           12.1723 ms       6.84374 ms      22.3491 ms
//
//      QuickSim                             100              1               195.089 ms
//                                           2.00618 ms       1.97771 ms      2.07086 ms
//                                           211.811 us       112.749 us      346.251 us
//
//      ClusterComplete (multi-threaded)     100              1               791.414 ms
//                                           7.76209 ms       6.36529 ms      10.3966 ms
//                                           9.49968 ms       5.8101 ms       15.033 ms
//
//      ClusterComplete (single-threaded)    100              1               803.497 ms
//                                           8.06843 ms       8.05748 ms      8.07985 ms
//                                           56.9453 us       49.6048 us      67.6442 us

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
        const clustercomplete_params<> sim_params{sidb_simulation_parameters{3, -0.32}, {}, {}, 6, 6, 1};
        return clustercomplete<lattice>(cl_3_seg, sim_params);
    };
}
#endif  // FICTION_ALGLIB_ENABLED
//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (15.01.2025)
//
//      Without jemalloc:
//      benchmark name                      samples             iterations       est run time
//                                          mean                low mean         high mean
//                                          std dev             low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                2.39773 m
//                                          1.44927 s           1.44573 s        1.45397 s
//                                          20.5985 ms          16.1537 ms       32.1081 ms
//
//      With jemalloc:
//      benchmark name                      samples             iterations       est run time
//                                          mean                low mean         high mean
//                                          std dev             low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                1.39364 m
//                                          835.42 ms           833.679 ms       837.076 ms
//                                          8.68306 ms          7.61487 ms       10.1187 ms

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (11.02.2025)
//
//      Before PR #664 (with jemalloc)
//      benchmark name                      samples             iterations       est run time
//                                          mean                low mean         high mean
//                                          std dev             low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire    100                 1                1.38135 m
//      (multi-threaded)                    837.966 ms          835.809 ms       841.715 ms
//                                          14.1689 ms          9.41514 ms       24.4521 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                   100                 1                19.6147 s
//                                          192.324 ms          191.795 ms       192.866 ms
//                                          2.73183 ms          2.45956 ms       3.0714 ms
//
//      After PR #664 (with jemalloc)
//      benchmark name                      samples             iterations       est run time
//                                          mean                low mean         high mean
//                                          std dev             low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire
//      (multi-threaded)                    100                 1                1.35702 m
//                                          821.239 ms          819.725 ms       822.841 ms
//                                          7.96672 ms          7.01983 ms       9.21399 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                   100                 1                18.5153 s
//                                          187.65 ms           187.029 ms       188.333 ms
//                                          3.31611 ms          2.83437 ms       4.32337 ms

//      AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (12.05.2025)
//
//      After PR #738 (without jemalloc)
//      benchmark name                          samples         iterations       est run time
//                                              mean            low mean         high mean
//                                              std dev         low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire
//      (multi-threaded)                        100             1                1.39561 m
//                                              855.996 ms      852.867 ms       863.226 ms
//                                              22.9268 ms      12.1374 ms       45.2192 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                       100             1                19.4726 s
//                                              194.102 ms      193.282 ms       194.917 ms
//                                              4.18614 ms      3.77211 ms       4.69498 ms
//
//      After PR #738 (with jemalloc)
//
//      benchmark name                          samples         iterations       est run time
//                                              mean            low mean         high mean
//                                              std dev         low std dev      high std dev
//      --------------------------------------------------------------------------------------
//      4 Segment Diagonal Bestagon Wire
//      (multi-threaded)                        100              1               2.4024 m
//                                              1.47767 s        1.47438 s       1.48129 s
//                                              17.6153 ms       15.1925 ms      20.6301 ms
//
//      3 Segment Diagonal Bestagon Wire
//      (single-threaded)                       100              1               26.4334 s
//                                              268.317 ms       267.88 ms       268.789 ms
//                                              2.32011 ms       2.0288 ms       2.74071 ms