//
// Created by Jan Drewniok on 04.05.23.
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/io/write_txt_sim_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace fiction;

int main()  // NOLINT
{
    // This script uses the randomly generated layouts (hence, random_layout_generation.cpp should be executed first),
    // simulates them, and collects the simulation results as a text file. The text file has three columns: x,y, charge
    // state (as integer).
    try
    {
        const std::string     folder_name = "random_layouts_test_new/";
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

                        // Randomly generated layouts have the following file name format: layout_xxx (xxx is the
                        // numbering). In the following code lines, the file name of the current file is extracted from
                        // the file path.
                        const std::string path  = benchmark.string();
                        const uint64_t    start = path.rfind("layout") + 7;
                        const uint64_t    end   = path.rfind(".sqd") - 1;
                        const std::string name  = path.substr(start, end - start + 1);

                        std::cout << benchmark << std::endl;

                        auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

                        const sidb_simulation_parameters params{2, -0.32};
                        const auto                       simulation_results =
                            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params);
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(3) << -params.mu;
                        std::string const file_path =
                            folder.path().string() + "/loc/" + name + "_sim_µ_minus_" + ss.str() + ".txt";
                        write_txt_sim_result(simulation_results, file_path);
                    }
                }
                else
                {
                    std::cout << "Folder" + std::string("/sqd") + "does not exist." << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Folder" + folder_name + "does not exist." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        std::cerr << "Error accessing folder: " << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
