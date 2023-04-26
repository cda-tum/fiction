//
// Created by Jan Drewniok on 05.04.23.
//

#include "fiction/algorithms/simulation/sidb/random_layout_generator.hpp"

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"

#include <filesystem>

using namespace fiction;

int main()
{
    using cell_level_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
    std::string           folder_name = "random_layouts_paper_KI/";
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

    for (uint64_t num_sidbs = 32; num_sidbs < 33; num_sidbs++)
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

        std::vector<cell_level_layout> all_layouts{};
        for (uint64_t i = 0; i < 10000; i++)
        {
            generate_random_layouts<cell_level_layout>({40, 40}, num_sidbs, "layout_" + std::to_string(i) + ".sqd",
                                                       dir_path_sqd.string() + "/", 10E8, true, all_layouts);
        }
    }
}
