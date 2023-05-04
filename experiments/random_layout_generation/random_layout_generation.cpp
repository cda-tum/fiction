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

int main()
{
    // This script generates random layouts as .sqd file.
    using cell_level_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
    std::string           folder_name = "random_layouts_test/";
    std::filesystem::path folder_path(EXPERIMENTS_PATH);
    folder_path /= folder_name;

    bool result = std::filesystem::create_directory(folder_path);
    if (result)
    {
        std::cout << "Folder *random_layouts* created successfully" << std::endl;
    }
    else
    {
        std::cout << "Failed to create folder" << std::endl;
    }

    for (uint64_t num_sidbs = 5; num_sidbs < 10; num_sidbs++)
    {
        std::filesystem::path dir_path = folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs);
        std::filesystem::path dir_path_sqd =
            folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs) + "/sqd";
        std::filesystem::path dir_path_loc =
            folder_path.string() + "number_sidbs_" + std::to_string(num_sidbs) + "/loc";
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

        std::vector<cell_level_layout>          all_layouts{};
        random_layout_params<cell_level_layout> params{{40, 40}, num_sidbs, true};
        for (uint64_t number_of_layouts = 0; number_of_layouts < 10; number_of_layouts++)
        {
            generate_random_layout<cell_level_layout>(
                params, dir_path_sqd.string() + "/layout_" + std::to_string(number_of_layouts) + ".sqd", all_layouts);
        }
    }
}
