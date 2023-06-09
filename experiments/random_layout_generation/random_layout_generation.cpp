//
// Created by Jan Drewniok on 04.05.23.
//

#include <fiction/algorithms/simulation/sidb/random_layout_generator.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>

using namespace fiction;

int main()  // NOLINT
{
    try
    {
        // This script generates random layouts as .sqd file.
        using cell_level_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
        const std::string     folder_name = "random_layouts_test_new/";
        std::filesystem::path folder_path(EXPERIMENTS_PATH);

        folder_path /= folder_name;

        if (std::filesystem::exists(folder_path))
        {
            std::cout << "Folder exists!" << std::endl;
        }
        else
        {

            if (std::filesystem::create_directory(folder_path))
            {
                std::cout << "Folder *random_layouts* created successfully" << std::endl;
            }
            else
            {
                std::cout << "Failed to create folder" << std::endl;
            }
        }
        // number of randomly generated layouts for a given number of SiDBs.
        const uint64_t number_of_layouts = 10;
        // number of SiDBs of the first bunch of randomly generated layouts.
        const uint64_t number_of_sidbs_lower_limit = 10;
        // number of SiDBs of the final bunch of randomly generated layouts
        const uint64_t number_of_sidbs_upper_limit = 11;

        for (uint64_t num_sidbs = number_of_sidbs_lower_limit; num_sidbs < number_of_sidbs_upper_limit; num_sidbs++)
        {
            const std::filesystem::path dir_path = folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs);
            const std::filesystem::path dir_path_sqd =
                folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs) + "/sqd";
            const std::filesystem::path dir_path_loc =
                folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs) + "/loc";
            try
            {
                if (!std::filesystem::exists(dir_path))
                {
                    std::filesystem::create_directory(dir_path);
                    std::cout << "Folder created." << std::endl;
                    std::filesystem::create_directory(dir_path_sqd);
                    std::filesystem::create_directory(dir_path_loc);
                }
                else
                {
                    std::cout << "Folder already exists." << std::endl;
                }

                std::vector<cell_level_layout>                all_layouts{};
                const random_layout_params<cell_level_layout> params{{20, 20}, num_sidbs, false};
                // Several randomly generated layouts are created (number of layouts is given by number_of_layouts).
                // They are stored as .sqd-file in the /sqd folder and the coordinates of all placed SiDBs in /loc.
                for (auto i = 0u; i < number_of_layouts; i++)
                {
                    const auto lyts = generate_random_layout<cell_level_layout>(
                        params, dir_path_sqd.string() + "/layout_" + std::to_string(i) + ".sqd", all_layouts);
                    all_layouts.push_back(lyts);
                }
            }
            catch (const std::filesystem::filesystem_error& ex)
            {
                // Exception is handled.
                std::cerr << "Filesystem error: " << ex.what() << std::endl;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        // Exception occurred, handle it here
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }
}
