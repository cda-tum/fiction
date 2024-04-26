//
// Created by Jan Drewniok 01.01.23
//

// pre-defined types suitable for the FCN domain
#include "mockturtle/utils/stopwatch.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability_sidb_gate.hpp>
#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_minimum_defect_influence_distance.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_defect_operational_domain.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace fiction;

int main()  // NOLINT
{
    using Lyt = sidb_cell_clk_lyt_siqad;

    static const std::string folder =
        fmt::format("{}skeleton_bestagons_with_tags/skeleton_hex_inputsdbp_2i1o.sqd", EXPERIMENTS_PATH);

    static const std::string solution_folder = fmt::format("{}metric_data_final/", EXPERIMENTS_PATH);

    const auto skeleton = read_sqd_layout<Lyt>(folder);

    design_sidb_gates_params<Lyt> params{sidb_simulation_parameters{2, -0.32},
                                         design_sidb_gates_params<Lyt>::design_sidb_gates_mode::EXHAUSTIVE,
                                         {{17, 7, 0}, {21, 11, 0}},
                                         1,
                                         sidb_simulation_engine::QUICKEXACT};

    params.maximal_random_solutions = 100;

        const auto truth_tables = std::vector<std::vector<tt>>{std::vector<tt>{create_and_tt()},
        std::vector<tt>{create_xor_tt()}, std::vector<tt>{create_or_tt()},
            std::vector<tt>{create_xnor_tt()}, std::vector<tt>{create_nor_tt()}, std::vector<tt>{create_nand_tt()},
            std::vector<tt>{create_lt_tt()},   std::vector<tt>{create_gt_tt()},  std::vector<tt>{create_le_tt()},
            std::vector<tt>{create_ge_tt()}};

        const std::vector<std::string> gate_names = {"and", "xor", "or", "xnor", "nor", "nand", "lt", "gt", "le", "ge"};

    //    const auto truth_tables =
    //        std::vector<std::vector<tt>>{std::vector<tt>{create_lt_tt()}, std::vector<tt>{create_gt_tt()},
    //                                     std::vector<tt>{create_le_tt()}, std::vector<tt>{create_ge_tt()}};

    // const auto truth_tables = std::vector<std::vector<tt>>{std::vector<tt>{create_nand_tt()}};

    const critical_temperature_params ct_params{sidb_simulation_parameters{
        2, params.phys_params.mu_minus, params.phys_params.epsilon_r, params.phys_params.lambda_tf}};

    operational_domain_params op_domain_params{sidb_simulation_parameters{
        2, params.phys_params.mu_minus, params.phys_params.epsilon_r, params.phys_params.lambda_tf}};
    op_domain_params.x_min  = 4.0;
    op_domain_params.x_max  = 6;
    op_domain_params.x_step = 0.2;

    op_domain_params.y_min  = 4.0;
    op_domain_params.y_max  = 6;
    op_domain_params.y_step = 0.2;

    const assess_physical_population_stability_params_sidb_gate assess_params{
        assess_physical_population_stability_params{sidb_simulation_parameters{
            2, params.phys_params.mu_minus, params.phys_params.epsilon_r, params.phys_params.lambda_tf}}};

    const maximum_defect_influence_distance_params defect_avoidance_params_arsenic{
        sidb_defect{sidb_defect_type::UNKNOWN, 1, 9.7, 2.1},
        params.phys_params,
        {50, 20}};

    const maximum_defect_influence_distance_params defect_avoidance_params_vacancy{
        sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9},
        params.phys_params,
        {50, 20}};

    uint64_t truth_counter = 0;

    for (const auto& truth_table : truth_tables)
    {
        std::cout << fmt::format("truth counter: {}", truth_counter) << '\n';
        for (auto num_sidbs = 5u; num_sidbs < 7; num_sidbs++)
        {
            std::cout << fmt::format("num sidbs: {}", num_sidbs) << '\n';
            params.number_of_sidbs = num_sidbs;

            mockturtle::stopwatch<>::duration time_total{};

            std::vector<Lyt>    all_gate{};
            std::vector<double> temps                             = {};
            std::vector<double> op_domains                        = {};
            std::vector<double> defect_influence_arsenic          = {};
            std::vector<double> defect_influence_vacancy          = {};
            std::vector<double> pop_stability_neutral_to_negative = {};
            std::vector<double> pop_stability_negative_to_neutral = {};
            std::vector<double> runtime                           = {};
            {
                mockturtle::stopwatch stop{time_total};

                design_sidb_gates_stats stats{};

                all_gate = design_sidb_gates<Lyt, tt>(skeleton, truth_table, params, &stats);

                std::cout << fmt::format("gate design finished and {} gates were found", all_gate.size()) << '\n';

                // Define mutex for synchronization
                std::mutex mtx;

                const auto calculate_metric_values = [&](const auto& gate_chunk)
                {
                    // Local vectors to store calculated metrics
                    std::vector<double> temps_local;
                    temps_local.reserve(gate_chunk.size());
                    std::vector<double> op_domains_local;
                    op_domains_local.reserve(gate_chunk.size());
                    std::vector<double> defect_influence_arsenic_local;
                    defect_influence_arsenic_local.reserve(gate_chunk.size());
                    std::vector<double> defect_influence_vacancy_local;
                    defect_influence_vacancy_local.reserve(gate_chunk.size());
                    std::vector<double> pop_stability_neutral_to_negative_local;
                    pop_stability_neutral_to_negative_local.reserve(gate_chunk.size());
                    std::vector<double> pop_stability_negative_to_neutral_local;
                    pop_stability_negative_to_neutral_local.reserve(gate_chunk.size());
                    std::vector<double> runtime_local;
                    runtime.reserve(gate_chunk.size());

                    for (const auto& gate : gate_chunk)
                    {
                        //write_sqd_layout(gate, solution_folder + "/bug/17_7_21_11_numdbs_{}.sqd");
                        temps_local.push_back(critical_temperature_gate_based(gate, truth_table, ct_params));
                        operational_domain_stats op_stats{};
                        const auto               op_domain =
                            operational_domain_flood_fill(gate, truth_table, 0, op_domain_params,
                                                          operational_domain::parameter_point{5.6, 5}, &op_stats);
                        op_domains_local.push_back(op_stats.percentual_operational_area);

                        defect_influence_stats arsenic_stats{};
                        const auto siqad_gate = convert_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(gate);
                        const auto defect_influence_domain_arsenic = defect_influence_operational_domain_contour_tracing(
                                siqad_gate, truth_table, 300,
                                defect_influence_params{
                                        defect_avoidance_params_arsenic,
                                        is_operational_params{defect_avoidance_params_arsenic.simulation_parameters}},
                                &arsenic_stats);
                        std::cout << fmt::format("runtime: {}", mockturtle::to_seconds(arsenic_stats.time_total))
                                  << '\n';
                        runtime_local.push_back(mockturtle::to_seconds(arsenic_stats.time_total));
                        //                        if (mockturtle::to_seconds(arsenic_stats.time_total) < 0.4)
                        //                        {
                        //                            write_operational_domain_params write_params{};
                        //                            write_params.operational_tag     = "1";
                        //                            write_params.non_operational_tag = "0";
                        //                            write_operational_domain(
                        //                                defect_influence_domain_arsenic,
                        //                                "/Users/jandrewniok/CLionProjects/fiction_copy/fiction/experiments/test_defect_op.csv",
                        //                                write_params);
                        //                            write_sqd_layout(
                        //                                gate,
                        //                                "/Users/jandrewniok/CLionProjects/fiction_copy/fiction/experiments/test_defect_op.sqd");
                        //                        }

                        defect_influence_arsenic_local.push_back(
                                maximum_minimum_defect_influence_distance(siqad_gate, defect_influence_domain_arsenic));

                        defect_influence_stats vacancy_stats{};
                        const auto defect_influence_domain_vacancy = defect_influence_operational_domain_contour_tracing(
                                siqad_gate, truth_table, 300,
                                defect_influence_params{
                                        defect_avoidance_params_vacancy,
                                        is_operational_params{defect_avoidance_params_arsenic.simulation_parameters}},
                                &vacancy_stats);
                        defect_influence_vacancy_local.push_back(
                                maximum_minimum_defect_influence_distance(siqad_gate, defect_influence_domain_vacancy));

                        pop_stability_neutral_to_negative_local.push_back(
                            assess_physical_population_stability_sidb_gate(gate, truth_table, assess_params, -1));
                        pop_stability_negative_to_neutral_local.push_back(
                            assess_physical_population_stability_sidb_gate(gate, truth_table, assess_params, 1));
                    }

                    // Acquire lock before modifying shared vectors
                    const std::lock_guard<std::mutex> lock(mtx);

                    // Append local vectors to global vectors
                    runtime.insert(runtime.end(), runtime_local.begin(), runtime_local.end());
                    temps.insert(temps.end(), temps_local.begin(), temps_local.end());
                    op_domains.insert(op_domains.end(), op_domains_local.begin(), op_domains_local.end());
                    defect_influence_arsenic.insert(defect_influence_arsenic.end(),
                                                    defect_influence_arsenic_local.begin(),
                                                    defect_influence_arsenic_local.end());
                    defect_influence_vacancy.insert(defect_influence_vacancy.end(),
                                                    defect_influence_vacancy_local.begin(),
                                                    defect_influence_vacancy_local.end());
                    pop_stability_neutral_to_negative.insert(pop_stability_neutral_to_negative.end(),
                                                             pop_stability_neutral_to_negative_local.begin(),
                                                             pop_stability_neutral_to_negative_local.end());
                    pop_stability_negative_to_neutral.insert(pop_stability_negative_to_neutral.end(),
                                                             pop_stability_negative_to_neutral_local.begin(),
                                                             pop_stability_negative_to_neutral_local.end());
                };

                // Define the number of threads to use
                const size_t num_threads = std::thread::hardware_concurrency();
                const size_t chunk_size  = (all_gate.size() + num_threads - 1) / num_threads;  // Calculate chunk size

                // A vector to store threads
                std::vector<std::thread> threads;
                threads.reserve(num_threads);

                // Split the vector into chunks and process each chunk in its own thread
                auto gate_it = all_gate.begin();
                for (size_t i = 0; i < num_threads; ++i)
                {
                    auto chunk_start = gate_it;
                    auto chunk_end   = std::min(gate_it + chunk_size, all_gate.end());

                    threads.emplace_back(calculate_metric_values, std::vector<Lyt>(chunk_start, chunk_end));

                    gate_it = chunk_end;
                }

                // Wait for all threads to finish
                for (auto& thread : threads)
                {
                    thread.join();
                }

                std::cout << "determination is finished" << std::endl;

                // Start asynchronous tasks to process combinations in parallel
                //                for (const auto& gate : all_gate)
                //                {
                //                    futures.emplace_back(std::async(std::launch::async, calculate_metric_values,
                //                    gate));
                //                }

                //                for (const auto& gate : all_gate)
                //                {
                //                    calculate_metric_values(gate);
                //                }

                // for each y value in parallel
                //                std::for_each(FICTION_EXECUTION_POLICY_PAR all_gate.cbegin(), all_gate.cend(),
                //                                  [&](const sidb_cell_clk_lyt_siqad &gate) {
                //                                  calculate_metric_values(gate);
                //                              });

                // Wait for all tasks to finish
                //                for (auto& future : futures)
                //                {
                //                    future.wait();
                //                }

                for (auto l = 0u; l < all_gate.size(); l++)
                {
                    const auto choose_gate = all_gate[l];
                    write_sqd_layout(choose_gate,
                                     fmt::format(solution_folder + "/sqd/17_7_21_11_numdbs_{}_{}_{}.sqd",
                                                 num_sidbs, gate_names[truth_counter], l));
                }

                // Open a file for writing
                std::ofstream csvFile(
                    fmt::format(solution_folder + "/csv/17_7_21_11_numdbs_{}_{}.csv", num_sidbs, gate_names[truth_counter]));

                // Check if the file is open
                if (!csvFile.is_open())
                {
                    std::cerr << "Error opening output.csv\n";
                    return 1;
                }

                // Write headers to the CSV file
                csvFile << "temp,op,popstab_neg_to_neu,popstab_neu_to_neg,defect_influence_arsenic,defect_influence_"
                           "vacancy,runtime\n";

                // Write data to the CSV file and calculate mean, max, min
                double meanTemp = 0.0, maxTemp = std::numeric_limits<double>::min(),
                       minTemp       = std::numeric_limits<double>::max();
                double meanOpDomains = 0.0, maxOpDomains = std::numeric_limits<double>::min(),
                       minOpDomains               = std::numeric_limits<double>::max();
                double meanDefectInfluenceArsenic = 0.0, maxDefectInfluenceArsenic = std::numeric_limits<double>::min(),
                       minDefectInfluenceArsenic  = std::numeric_limits<double>::max();
                double meanDefectInfluenceVacancy = 0.0, maxDefectInfluenceVacancy = std::numeric_limits<double>::min(),
                       minDefectInfluenceVacancy = std::numeric_limits<double>::max();
                double meanPopStabilityNegToNeu = 0.0, maxPopStabilityNegToNeu = std::numeric_limits<double>::min(),
                       minPopStabilityNegToNeu  = std::numeric_limits<double>::max();
                double meanPopStabilityNeuToNeg = 0.0, maxPopStabilityNeuToNeg = std::numeric_limits<double>::min(),
                       minPopStabilityNeuToNeg = std::numeric_limits<double>::max();

                for (std::size_t k = 0; k < temps.size(); ++k)
                {
                    csvFile << temps[k] << "," << op_domains[k] << "," << pop_stability_negative_to_neutral[k] << ","
                            << pop_stability_neutral_to_negative[k] << "," << defect_influence_arsenic[k] << ","
                            << defect_influence_vacancy[k] << "," << runtime[k] << "\n";

                    // Update mean, max, min for temps
                    meanTemp += temps[k];
                    maxTemp = std::max(maxTemp, temps[k]);
                    minTemp = std::min(minTemp, temps[k]);

                    // Update mean, max, min for op_domains
                    meanOpDomains += op_domains[k];
                    maxOpDomains = std::max(maxOpDomains, op_domains[k]);
                    minOpDomains = std::min(minOpDomains, op_domains[k]);

                    // Update mean, max, min for defect_influence_arsenic
                    meanDefectInfluenceArsenic += defect_influence_arsenic[k];
                    maxDefectInfluenceArsenic = std::max(maxDefectInfluenceArsenic, defect_influence_arsenic[k]);
                    minDefectInfluenceArsenic = std::min(minDefectInfluenceArsenic, defect_influence_arsenic[k]);

                    // Update mean, max, min for defect_influence_vacancy
                    meanDefectInfluenceVacancy += defect_influence_vacancy[k];
                    maxDefectInfluenceVacancy = std::max(maxDefectInfluenceVacancy, defect_influence_vacancy[k]);
                    minDefectInfluenceVacancy = std::min(minDefectInfluenceVacancy, defect_influence_vacancy[k]);

                    // Update mean, max, min for pop_stability_negative_to_neutral
                    meanPopStabilityNegToNeu += pop_stability_negative_to_neutral[k];
                    maxPopStabilityNegToNeu = std::max(maxPopStabilityNegToNeu, pop_stability_negative_to_neutral[k]);
                    minPopStabilityNegToNeu = std::min(minPopStabilityNegToNeu, pop_stability_negative_to_neutral[k]);

                    // Update mean, max, min for pop_stability_neutral_to_negative
                    meanPopStabilityNeuToNeg += pop_stability_neutral_to_negative[k];
                    maxPopStabilityNeuToNeg = std::max(maxPopStabilityNeuToNeg, pop_stability_neutral_to_negative[k]);
                    minPopStabilityNeuToNeg = std::min(minPopStabilityNeuToNeg, pop_stability_neutral_to_negative[k]);
                }

                // Calculate mean for temps, op_domains, and other metrics
                meanTemp /= static_cast<double>(temps.size());
                meanOpDomains /= static_cast<double>(op_domains.size());
                meanDefectInfluenceArsenic /= static_cast<double>(defect_influence_arsenic.size());
                meanDefectInfluenceVacancy /= static_cast<double>(defect_influence_vacancy.size());
                meanPopStabilityNegToNeu /= static_cast<double>(pop_stability_negative_to_neutral.size());
                meanPopStabilityNeuToNeg /= static_cast<double>(pop_stability_neutral_to_negative.size());

                // Output mean, max, min for temps
                std::cout << "temp: " << meanTemp << ";" << maxTemp << ";" << minTemp << '\n';

                // Output mean, max, min for op_domains
                std::cout << "op: " << meanOpDomains << ";" << maxOpDomains << ";" << minOpDomains << '\n';

                // Output mean, max, min for temps
                std::cout << "ars: " << meanDefectInfluenceArsenic << ";" << maxDefectInfluenceArsenic << ";"
                          << minDefectInfluenceArsenic << '\n';

                // Output mean, max, min for op_domains
                std::cout << "vac: " << meanDefectInfluenceVacancy << ";" << maxDefectInfluenceVacancy << ";"
                          << minDefectInfluenceVacancy << '\n';

                // Output mean, max, min for temps
                std::cout << "pop_neu_to_neg: " << meanPopStabilityNeuToNeg << ";" << maxPopStabilityNeuToNeg << ";"
                          << minPopStabilityNeuToNeg << '\n';

                // Output mean, max, min for op_domains
                std::cout << "pop_neg_to_neu: " << meanPopStabilityNegToNeu << ";" << maxPopStabilityNegToNeu << ";"
                          << minPopStabilityNegToNeu << '\n';

                // Close the file
                csvFile.close();
            }
        }
        truth_counter++;
    }
    return EXIT_SUCCESS;
}
