//
// Created by Jan Drewniok on 04.05.23.
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/io/write_loc_sim_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace fiction;

int main(int argc, const char* argv[])  // NOLINT
{
    // This script uses the randomly generated layouts (hence, random_layout_generation.cpp should be executed first),
    // simulates them, and collects the simulation results as a text file. The text file has three columns: x,y, charge
    // state (as integer) of the ground state.

    std::unordered_map<std::string, std::variant<std::string>> options{{"--folder_name", "layout_random_cli/"},
                                                                       {"--mu_minus", "-0.32"}};

    // Parse command-line arguments
    for (auto i = 1u; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (options.count(arg) > 0)
        {
            if (i + 1 < argc)
            {
                options[arg] = argv[i + 1];
                ++i;  // Skip the next argument
            }
            else
            {
                std::cerr << "Error: Argument " << arg << " is missing a value." << std::endl;
                return 1;
            }
        }
    }

    // Folder name where the randomly generated layouts are located.
    const std::string folder_name = std::get<std::string>(options["--folder_name"]);
    // µ-value used for the simulation.
    const double mu = std::stod(std::get<std::string>(options["--mu_minus"]));

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

                        // When random layouts are generated with the script random_layout_generation.cpp, they have the
                        // following file name format: layout_xxx (xxx is the numbering). In the following code lines,
                        // the file name of the current file is extracted from the file path.
                        const std::string path  = benchmark.string();
                        const uint64_t    start = path.rfind("layout") + 7;
                        const uint64_t    end   = path.rfind(".sqd") - 1;
                        const std::string name  = path.substr(start, end - start + 1);

                        std::cout << benchmark << std::endl;

                        auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

                        const sidb_simulation_parameters params{2, units::energy::electron_volt_t{mu}};
                        const auto                       simulation_results =
                            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params);
                        std::stringstream ss;
                        const std::string file_path = fmt::format("{}/loc/{}_sim_µ_minus_{:.3f}.txt",
                                                                  folder.path().string(), name, -params.mu.value());

                        // Some layouts where positively charged SiDBs may occur cannot be simulated (i.e., no
                        // physically valid charge distribution is found) because the physical model currently works
                        // reliably for layouts with neutrally and negatively charged SiDBs.
                        if (!simulation_results.charge_distributions.empty())
                        {
                            write_loc_sim_result(simulation_results, file_path);
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
