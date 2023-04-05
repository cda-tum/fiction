//
// Created by Jan Drewniok on 05.04.23.
//

#include "fiction/algorithms/simulation/sidb/generated_random_layouts.hpp"

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"

#include <filesystem>

using namespace fiction;

int main()
{
    std::string path        = "/Users/jandrewniok/Documents/PhD/random_layouts/";
    using cell_level_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    for (uint64_t num_sidbs = 1; num_sidbs < 30; num_sidbs++)
    {
        std::filesystem::path dir_path = path + "number_sidbs_" + std::to_string(num_sidbs);
        if (!std::filesystem::exists(dir_path))
        {
            std::filesystem::create_directory(dir_path);
            std::cout << "Folder created." << std::endl;
        }
        else
        {
            std::cout << "Folder already exists." << std::endl;
        }

        for (uint64_t i = 0; i < 1000; i++)
        {
            generate_random_layouts<cell_level_layout>({30, 30}, num_sidbs, "layout_" + std::to_string(i) + ".sqd",
                                                       dir_path.string() + "/");
        }
    }
}
