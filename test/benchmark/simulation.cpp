//
// Created by Jan Drewniok on 22.11.23.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exact_cluster_iteration.hpp>
#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <cstdint>
#include <string>
#include <utility>

using namespace fiction;

TEST_CASE("Benchmark simulators", "[benchmark]")
{
    // crossing bestagon gate
    sidb_cell_clk_lyt_siqad lyt{};

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
        const quickexact_params<sidb_cell_clk_lyt_siqad> sim_params{sidb_simulation_parameters{2, -0.32}};
        return quickexact<sidb_cell_clk_lyt_siqad>(lyt, sim_params);
    };

    BENCHMARK("QuickSim")
    {
        const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.32}};
        return quicksim<sidb_cell_clk_lyt_siqad>(lyt, quicksim_params);
    };
}

//  Mac M1, Ventura 13.0, Apple clang version 14.0.0 (22.11.23)

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

TEST_CASE("Benchmark exact simulators", "[benchmark]")
{
    using sidb_lyt = sidb_cell_clk_lyt_siqad;

    const uint64_t min_sidbs = 25;
    const uint64_t max_sidbs = 40;

    const uint64_t min_trials = 5;
    const uint64_t max_trials = 50;

    const uint64_t exgs_limit = 0;

    const uint64_t min_y = 10;
    const uint64_t max_y = 40;

//    const auto compute_trials = [&](const uint64_t num_sidbs)
//    {
//        return max_trials -
//               static_cast<uint64_t>(std::round(static_cast<double>(max_trials - min_trials) *
//                                                std::pow((num_sidbs - min_sidbs) / (min_sidbs - max_sidbs), 2)));
//    };

    for (uint64_t y = min_y; y <= max_y; y += 5)
    {
        const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{40, y}};

        BENCHMARK("-=-=-=-=-=- Y: " + std::to_string(y) +
                  " ; AREA (nm): " + std::to_string(0.768 * static_cast<double>(y) * 0.384 * 40.0))
        {
            return std::pow(1, 2);
        };

        for (uint64_t num_sidbs = min_sidbs; num_sidbs <= max_sidbs; num_sidbs++)
        {

            generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
            rlg_ps.coordinate_pair                    = layout_dimensions;
            rlg_ps.number_of_sidbs                    = num_sidbs;
            rlg_ps.number_of_unique_generated_layouts = max_trials;  // compute_trials(num_sidbs);

            //        std::stringst << "\n\nNumber of SiDBs: " << num_sidbs
            //                  << "\nRandom layouts: " << rlg_ps.number_of_unique_generated_layouts << '\n' << std::endl;

            BENCHMARK("NUMBER OF SIDBS: " + std::to_string(num_sidbs) +
                      " ; RANDOM LAYOUTS: " + std::to_string(rlg_ps.number_of_unique_generated_layouts))
            {
                return std::pow(1, 2);
            };

            const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

            for (uint64_t i = 0; i < lyts.size(); ++i)
            {
//                BENCHMARK("LAYOUT NUMBER: " + std::to_string(i + 1))
//                {
//                    return std::pow(1, 2);
//                };

//                if (num_sidbs <= exgs_limit)
//                {
//                    BENCHMARK("ExGS")
//                    {
//                        return exhaustive_ground_state_simulation<sidb_lyt>(lyts[i]);
//                    };
//                }
                const double time = cluster_exact<sidb_lyt>{lyts[i]}.FindGroundState(45).simulation_runtime.count();
                BENCHMARK("LAYOUT NUMBER: " + std::to_string(i + 1) + " ; RUNTIME: " + std::to_string(time))
                {
//                    return cluster_exact<sidb_lyt>{lyts[i]}.FindGroundState(45);
                    return std::pow(1, 2);
                };

//                BENCHMARK("QuickExact")
//                {
//                    return quickexact<sidb_lyt>(lyts[i]);
//                };
            }
        }
    }
}
