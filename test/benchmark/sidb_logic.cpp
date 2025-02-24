//
// Created by Willem Lambooy on 19/02/2025.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

using namespace fiction;

using lattice       = sidb_100_cell_clk_lyt;
using lattice_siqad = sidb_100_cell_clk_lyt_siqad;

TEST_CASE("Benchmark operational assessment using exact engines", "[benchmark]")
{

    const auto lyt_ha = convert_layout_to_fiction_coordinates<lattice>(blueprints::bestagon_ha<lattice_siqad>());
    const auto lyt_cx = convert_layout_to_fiction_coordinates<lattice>(blueprints::bestagon_crossing<lattice_siqad>());

    is_operational_params operational_params_qe{sidb_simulation_parameters{2, -0.32},
                                                sidb_simulation_engine::QUICKEXACT};

#if (FICTION_ALGLIB_ENABLED)
    is_operational_params operational_params_cc{sidb_simulation_parameters{2, -0.32},
                                                sidb_simulation_engine::CLUSTERCOMPLETE};
#endif  // FICTION_ALGLIB_ENABLED

    SECTION("Bestagon half adder, filter only")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon half adder, filter then simulation")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon half adder, simulation only")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_ha, create_half_adder_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing, filter only")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing, filter then simulation")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing, simulation only")
    {
        BENCHMARK("QuickExact")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return is_operational(lyt_cx, create_crossing_wire_tt(), operational_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }
}

TEST_CASE("Benchmark exact design using QuickCell", "[benchmark]")
{
    const auto lyt = convert_layout_to_fiction_coordinates<lattice>(
        blueprints::two_input_two_output_bestagon_skeleton<lattice_siqad>());

    design_sidb_gates_params<lattice> design_gate_params_qe{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT},
        design_sidb_gates_params<lattice>::design_sidb_gates_mode::QUICKCELL,
        {{16, 8, 0}, {22, 14, 0}},
        3};

#if (FICTION_ALGLIB_ENABLED)
    design_sidb_gates_params<lattice> design_gate_params_cc{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE},
        design_sidb_gates_params<lattice>::design_sidb_gates_mode::QUICKCELL,
        {{16, 8, 0}, {22, 14, 0}},
        3};
#endif  // FICTION_ALGLIB_ENABLED

    SECTION("Bestagon half adder")
    {
        BENCHMARK("QuickExact")
        {
            return design_sidb_gates(lyt, std::vector<tt>{create_half_adder_tt()}, design_gate_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return design_sidb_gates(lyt, std::vector<tt>{create_half_adder_tt()}, design_gate_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }

    SECTION("Bestagon crossing")
    {
        BENCHMARK("QuickExact")
        {
            return design_sidb_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, design_gate_params_qe);
        };

#if (FICTION_ALGLIB_ENABLED)
        BENCHMARK("ClusterComplete")
        {
            return design_sidb_gates(lyt, std::vector<tt>{create_crossing_wire_tt()}, design_gate_params_cc);
        };
#endif  // FICTION_ALGLIB_ENABLED
    }
}

// AMD Ryzen Threadripper PRO 5955X, Ubuntu 20.04, Ubuntu clang version 18.1.3 (19.02.2025)
//
//
//   BEFORE #552 (-DFICTION_ENABLE_JEMALLOC=ON) ==================================
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, filter only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.61372 s
//                                         36.1154 ms    36.0746 ms    36.1633 ms
//                                         225.865 us    192.438 us    276.626 us
//
// ClusterComplete                                100             1      20.042 s
//                                          48.413 ms    41.4015 ms    58.1081 ms
//                                         41.6985 ms    32.7763 ms    54.8126 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, filter then simulation
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.65567 s
//                                          36.848 ms    36.7097 ms     36.985 ms
//                                         702.456 us    652.708 us    760.232 us
//
// ClusterComplete                                100             1     5.71742 s
//                                         52.0939 ms    42.4464 ms    65.7572 ms
//                                         57.9337 ms    44.4928 ms    74.7396 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, simulation only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.64339 s
//                                         36.5662 ms    36.4448 ms    36.6976 ms
//                                         646.176 us    592.023 us    703.154 us
//
// ClusterComplete                                100             1     7.97753 s
//                                         73.4741 ms    58.6144 ms    99.1509 ms
//                                         97.0133 ms    63.7442 ms    149.867 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, filter only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     29.6623 s
//                                         302.549 ms    301.342 ms    304.089 ms
//                                         6.91644 ms    5.68371 ms    8.01302 ms
//
// ClusterComplete                                100             1     5.42756 s
//                                         52.2006 ms    43.9063 ms    68.0955 ms
//                                         56.4932 ms    35.8826 ms    103.684 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, filter then simulation
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     30.6831 s
//                                         299.977 ms     299.57 ms     300.65 ms
//                                         2.61126 ms    1.82812 ms    4.39419 ms
//
// ClusterComplete                                100             1     11.2909 s
//                                         38.2897 ms    32.5868 ms     47.593 ms
//                                         36.3324 ms    25.8208 ms    59.0547 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, simulation only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     29.9357 s
//                                         301.806 ms    300.921 ms    303.025 ms
//                                         5.23905 ms    3.98772 ms    6.54099 ms
//
// ClusterComplete                                100             1     5.55833 s
//                                         49.1383 ms    41.3757 ms    59.5775 ms
//                                         45.8248 ms    36.1941 ms    57.3929 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon half adder
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     10.9851 s
//                                         102.632 ms    102.234 ms    103.064 ms
//                                         2.12151 ms    1.85788 ms    2.52999 ms
//
// ClusterComplete                                100             1     10.1796 s
//                                         103.164 ms    102.743 ms    103.617 ms
//                                          2.2432 ms    1.95342 ms    2.65687 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon crossing
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1      10.782 s
//                                         103.754 ms    103.333 ms    104.233 ms
//                                         2.28478 ms    1.97222 ms    2.74259 ms
//
// ClusterComplete                                100             1     10.8237 s
//                                         104.066 ms    103.655 ms    104.505 ms
//                                         2.16288 ms    1.90859 ms    2.49953 ms
//
//
//
//   AFTER #552 (-DFICTION_ENABLE_JEMALLOC=ON) ===================================
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, filter only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.58351 s
//                                         35.7141 ms    35.6905 ms    35.7399 ms
//                                         125.743 us    109.463 us    146.677 us
//
// ClusterComplete                                100             1     5.39978 s
//                                         61.0824 ms    51.1992 ms    74.6826 ms
//                                          58.576 ms    46.6999 ms     80.838 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, filter then simulation
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.57956 s
//                                         35.6716 ms    35.6494 ms    35.6971 ms
//                                         121.081 us    104.599 us    140.825 us
//
// ClusterComplete                                100             1     2.89808 s
//                                         57.2165 ms    47.1633 ms    71.0956 ms
//                                         59.7114 ms    46.5993 ms    76.3067 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon half adder, simulation only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     3.58077 s
//                                         35.7433 ms    35.7033 ms    35.8556 ms
//                                         314.086 us    141.496 us    673.349 us
//
// ClusterComplete                                100             1     8.95055 s
//                                          60.249 ms    50.3837 ms    74.4514 ms
//                                           59.66 ms    45.9172 ms    81.7567 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, filter only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     29.6191 s
//                                         298.495 ms    297.799 ms    299.994 ms
//                                         4.99546 ms    2.35073 ms    8.36629 ms
//
// ClusterComplete                                100             1     6.25514 s
//                                         54.3169 ms    44.2527 ms    69.1531 ms
//                                         61.4579 ms    46.4648 ms     83.323 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, filter then simulation
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     29.6573 s
//                                         299.916 ms    298.689 ms    301.854 ms
//                                          7.7258 ms    5.34909 ms    10.4406 ms
//
// ClusterComplete                                100             1     5.44163 s
//                                         48.5948 ms    41.1548 ms    59.3492 ms
//                                         45.2397 ms    34.6753 ms     63.186 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark operational assessment using exact engines
//   Bestagon crossing, simulation only
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     29.6576 s
//                                         296.978 ms    296.867 ms    297.113 ms
//                                         622.364 us    524.333 us    787.205 us
//
// ClusterComplete                                100             1      2.7908 s
//                                         46.4762 ms    39.9876 ms    58.5954 ms
//                                         43.5899 ms    25.3335 ms    70.2295 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon half adder
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     11.1668 s
//                                         102.741 ms    102.353 ms    103.152 ms
//                                         2.03127 ms      1.803 ms    2.31067 ms
//
// ClusterComplete                                100             1     10.2805 s
//                                         102.878 ms    102.542 ms    103.256 ms
//                                         1.81781 ms    1.58851 ms    2.13815 ms
//
//
// -------------------------------------------------------------------------------
// Benchmark exact design using QuickCell
//   Bestagon crossing
// -------------------------------------------------------------------------------
// ...............................................................................
//
// benchmark name                       samples       iterations    est run time
//                                      mean          low mean      high mean
//                                      std dev       low std dev   high std dev
// -------------------------------------------------------------------------------
// QuickExact                                     100             1     10.7396 s
//                                         103.523 ms    103.151 ms    103.933 ms
//                                         1.99474 ms    1.72231 ms    2.46009 ms
//
// ClusterComplete                                100             1     10.6287 s
//                                         103.967 ms    103.558 ms     104.48 ms
//                                         2.32589 ms    1.85219 ms    3.14282 ms
