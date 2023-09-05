//
// Created by Jan Drewniok on 04.05.23.
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/io/write_location_and_ground_state.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace fiction;

/**
 * This program reads randomly generated SiDB layouts from a specified folder, simulates them, and collects the
 * simulation results in text files. The simulation results include the x and y coordinates of SiDBs and their charge
 * state (as an integer) in the ground state. The simulation is parameterized by the µ-value, which influences the
 * charge distribution.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of C-style strings containing the command-line arguments.
 * @return Returns 0 on successful execution, or EXIT_FAILURE if an error occurs.
 *
 * Command-line Options:
 *   --folder_name <name>  Specifies the name of the folder containing randomly generated layouts.
 *   --mu_minus <value>    Sets the µ-value used for the simulation, which affects the charge distribution.
 *
 * Example Usage:
 *   To simulate layouts from the "random_sidb_layouts/" folder with a µ (mu) value of -0.32:
 *   ./sidb_location_and_ground_state_state_simulation --folder_name random_sidb_layouts/ --mu_minus -0.32
 */
int main(int argc, const char* argv[])  // NOLINT
{

    std::unordered_map<std::string, std::string> options{{"--folder_name", "layout_random_cli/"},
                                                         {"--mu_minus", "-0.32"}};

    std::vector<std::string> arguments(argv + 1, argv + argc);  // Convert argv to a vector of strings

    // Parse command-line arguments
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        const std::string& arg = arguments[i];
        if (options.count(arg) > 0)
        {
            if (i + 1 < arguments.size())
            {
                options[arg] = arguments[i + 1];
                ++i;  // Skip the next argument
            }
            else
            {
                std::cerr << "Error: Argument " << arg << " is missing a value." << std::endl;
                return EXIT_FAILURE;
            }
        }
    }

    // Folder name where the randomly generated layouts are located.
    const std::string folder_name = options["--folder_name"];
    // µ-value used for the simulation.
    const double mu = std::stod(options["--mu_minus"]);

    // Print the parsed values
    std::cout << "Folder name: " << folder_name << std::endl;
    std::cout << fmt::format("µ_minus: {}", mu) << std::endl;

    try
    {
        std::filesystem::path folder_path(EXPERIMENTS_PATH);
        folder_path /= folder_name;

        if (std::filesystem::exists(folder_path))
        {
            for (const auto& folder : std::filesystem::directory_iterator(folder_path))
            {
                if (std::filesystem::exists(folder_path))
                {
                    for (const auto& file : std::filesystem::directory_iterator(folder.path().string() + "/sqd"))
                    {
                        const auto& benchmark = file.path();

                        // When random layouts are generated with the script random_sidb_layout_generation.cpp, they
                        // have the following file name format: layout_xxx (xxx is the numbering). In the following code
                        // lines, the file name of the current file is extracted from the file path.
                        const std::string path  = benchmark.string();
                        const uint64_t    start = path.rfind("layout") + 7;
                        const uint64_t    end   = path.rfind(".sqd") - 1;
                        const std::string name  = path.substr(start, end - start + 1);

                        std::cout << benchmark << std::endl;

                        auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

                        const sidb_simulation_parameters params{2, mu};
                        const auto                       simulation_results =
                            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params);
                        const std::string file_path = fmt::format("{}/loc/{}_sim_µ_minus_{:.3f}.txt",
                                                                  folder.path().string(), name, -params.mu_minus);

                        // Some SiDB layouts where positively charged SiDBs may occur cannot be simulated (i.e., no
                        // physically valid charge distribution is found) because the physical model currently works
                        // reliably only for layouts with neutrally and negatively charged SiDBs.
                        if (!simulation_results.charge_distributions.empty())
                        {
                            write_location_and_ground_state(simulation_results, file_path);
                        }
                    }
                }
                else
                {
                    std::cout << "Folder */sqd* does not exist." << std::endl;
                }
            }
        }
        else
        {
            std::cout << fmt::format("Folder {} does not exist", folder_name) << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        std::cerr << "Error accessing folder: " << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
