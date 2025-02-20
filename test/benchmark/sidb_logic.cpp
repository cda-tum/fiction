//
// Created by Willem Lambooy on 19/02/2025.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Benchmark exact operational assessment", "[benchmark]")
{
    SECTION("Bestagon half adder")
    {
        const auto lyt = blueprints::bestagon_ha<sidb_cell_clk_lyt_siqad>();

        BENCHMARK("QuickExact")
        {
            return is_operational(
                lyt, create_half_adder_tt(),
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT});
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(
                lyt, create_half_adder_tt(),
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE});
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing")
    {
        const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

        BENCHMARK("QuickExact")
        {
            return is_operational(
                lyt, create_crossing_wire_tt(),
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT});
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(
                lyt, create_crossing_wire_tt(),
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE});
        };
#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Benchmark exact design using QuickCell", "[benchmark]")
{
    const auto lyt = blueprints::two_input_two_output_bestagon_skeleton<sidb_100_cell_clk_lyt_siqad>();

    SECTION("Bestagon half adder")
    {
        BENCHMARK("QuickExact")
        {
            const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>> params{
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT},
                design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
                {{16, 8, 0}, {22, 14, 0}},
                3};

            return design_sidb_gates(lyt, std::vector<tt>{create_half_adder_tt()}, params);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>> params{
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE},
                design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
                {{16, 8, 0}, {22, 14, 0}},
                3};

            return design_sidb_gates(lyt, std::vector<tt>{create_half_adder_tt()}, params);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing")
    {
        BENCHMARK("QuickExact")
        {
            const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>> params{
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT},
                design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
                {{16, 8, 0}, {22, 14, 0}},
                3};

            return design_sidb_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, params);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>> params{
                is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE},
                design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
                {{16, 8, 0}, {22, 14, 0}},
                3};

            return design_sidb_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, params);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }
}

// AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (19.02.2025)
//
//
// BEFORE #552 (-DFICTION_ENABLE_JEMALLOC=ON)
//
// -------------------------------------------------------------------------------
// Benchmark exact operational assessment
//   Bestagon half adder
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.78555 s
//                                         37.6511 ms    37.5784 ms     37.849 ms
//                                         568.885 us    253.745 us    1.20599 ms
//
// ClusterComplete                                100             1     7.36722 s
//                                          64.115 ms    54.5074 ms    76.8617 ms
//                                         56.3622 ms    44.4604 ms       70.8 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact operational assessment
//   Bestagon crossing
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     30.9174 s
//                                         310.861 ms    310.752 ms    310.975 ms
//                                         568.035 us    504.324 us     646.06 us
//
// ClusterComplete                                100             1     3.28897 s
//                                         62.4506 ms    53.1562 ms    75.0693 ms
//                                         54.9345 ms    43.6231 ms    68.9101 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon half adder
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     1.30287 m
//                                         791.151 ms    787.981 ms    794.487 ms
//                                         16.5568 ms    14.8496 ms    18.5733 ms
//
// ClusterComplete                                100             1     8.16662 m
//                                         831.868 ms    828.488 ms    837.709 ms
//                                         22.1086 ms    14.7688 ms    40.4548 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon crossing
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     1.27554 m
//                                         763.535 ms    760.857 ms    766.287 ms
//                                          13.866 ms    12.2747 ms    15.8419 ms
//
// ClusterComplete                                100             1     1.26896 m
//                                          791.24 ms    774.817 ms    824.091 ms
//                                         113.249 ms    64.1338 ms    181.057 ms
//
//
// AFTER #552 (-DFICTION_ENABLE_JEMALLOC=ON)
//
// -------------------------------------------------------------------------------
// Benchmark exact operational assessment
//   Bestagon half adder
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.76167 s
//                                         37.6697 ms    37.6296 ms    37.7051 ms
//                                           190.5 us    149.566 us    282.061 us
//
// ClusterComplete                                100             1     2.71245 s
//                                         41.8305 ms    35.1286 ms    54.3935 ms
//                                         45.2892 ms    29.0476 ms    76.5903 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact operational assessment
//   Bestagon crossing
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     31.3176 s
//                                         315.491 ms    314.929 ms    316.207 ms
//                                          3.2295 ms    2.63159 ms     3.8207 ms
//
// ClusterComplete                                100             1     3.48814 s
//                                         43.6875 ms    38.5003 ms    50.7581 ms
//                                         30.6895 ms    24.0144 ms    39.2663 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon half adder
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     1.19416 m
//                                         729.032 ms    727.554 ms    730.625 ms
//                                         7.84468 ms    6.91439 ms    8.96589 ms
//
// ClusterComplete                                100             1     10.5641 m
//                                         779.777 ms    772.215 ms    809.633 ms
//                                         68.5302 ms    11.7131 ms    161.393 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon crossing
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     1.13369 m
//                                         703.847 ms    702.198 ms    705.588 ms
//                                         8.66388 ms    7.72657 ms    9.97905 ms
//
// ClusterComplete                                100             1     1.16481 m
//                                         705.672 ms    702.891 ms     709.29 ms
//                                         16.0869 ms    12.7729 ms    22.5207 ms
