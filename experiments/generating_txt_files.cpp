//
// Created by Jan Drewniok on 05.04.23.
//

//
// Created by Jan Drewniok 01.01.23
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/io/read_sqd_layout.hpp"  // reader for SiDB layouts including surface scan data
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>  // output formatting
#include <mockturtle/utils/stopwatch.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <string>

using namespace fiction;
namespace fs = std::filesystem;

int main()  // NOLINT
{

    fs::path dir_path = "/Users/jandrewniok/Documents/PhD/random_layouts";

    for (const auto& folder : fs::directory_iterator(dir_path))
    {
        for (const auto& file : std::filesystem::directory_iterator(folder.path().string() + "/sqd"))
        {
            const auto& benchmark = file.path();

            std::string path  = benchmark.string();
            uint64_t    start = path.rfind("layout") + 7;
            uint64_t    end   = path.rfind(".sqd") - 1;
            std::string name  = path.substr(start, end - start + 1);

            std::cout << benchmark << std::endl;

            auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

            exgs_stats<sidb_cell_clk_lyt_siqad> exgs_stats{};
            const sidb_simulation_parameters    params{2, -0.32};
            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params, &exgs_stats);

            auto min_energy = minimum_energy<sidb_cell_clk_lyt_siqad>(exgs_stats.valid_lyts);

            std::vector<charge_distribution_surface<sidb_cell_clk_lyt_siqad>> ground_state_layouts{};
            for (const auto& lyt : exgs_stats.valid_lyts)
            {
                if (std::abs(lyt.get_system_energy() - min_energy) < physical_constants::POP_STABILITY_ERR)
                {
                    ground_state_layouts.push_back(charge_distribution_surface<sidb_cell_clk_lyt_siqad>{lyt});
                }
            }

            if (!ground_state_layouts.empty())
            {
                const auto    sidb_location_nm   = ground_state_layouts.front().get_all_sidb_location_in_nm();
                const auto    sidbs              = ground_state_layouts.front().get_all_sidbs();
                const auto    physical_parameter = ground_state_layouts.front().get_phys_params();
                std::ofstream outFile(folder.path().string() + "/loc/" + name + "_sim.txt");
                outFile << std::fixed << std::setprecision(3);
                outFile << "x;"
                        << "y;";
                for (uint64_t i = 0; i < ground_state_layouts.size() - 1; i++)
                {
                    outFile << std::to_string(i) << ";";
                }
                outFile << std::to_string(ground_state_layouts.size()) << std::endl;

                for (const auto& sidb : sidbs)
                {
                    auto pos = sidb_nm_position<sidb_cell_clk_lyt_siqad>(physical_parameter, sidb);
                    outFile << pos.first * 10E9 << ";" << pos.second * 10E9 << ";";
                    for (const auto& lyt : ground_state_layouts)
                    {
                        outFile << std::to_string(charge_state_to_sign(lyt.get_charge_state(sidb))) << ";";
                    }
                    outFile << std::endl;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
