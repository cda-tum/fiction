//
// Created by Jan Drewniok on 04.05.23.
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"

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
    std::string           folder_name = "random_layouts_test/";
    std::filesystem::path folder_path(EXPERIMENTS_PATH);
    folder_path /= folder_name;

    for (const auto& folder : std::filesystem::directory_iterator(folder_path))
    {
        for (const auto& file : std::filesystem::directory_iterator(folder.path().string() + "/sqd"))
        {
            const auto& benchmark = file.path();

            const std::string path  = benchmark.string();
            const uint64_t    start = path.rfind("layout") + 7;
            const uint64_t    end   = path.rfind(".sqd") - 1;
            const std::string name  = path.substr(start, end - start + 1);

            std::cout << benchmark << std::endl;

            auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

            const sidb_simulation_parameters    params{2, -0.32};
            exgs_stats<sidb_cell_clk_lyt_siqad> exgs_stats{};
            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params, &exgs_stats);

            auto min_energy = minimum_energy<sidb_cell_clk_lyt_siqad>(exgs_stats.valid_lyts);

            std::vector<charge_distribution_surface<sidb_cell_clk_lyt_siqad>> ground_state_layouts{};
            for (const auto& valid_layout : exgs_stats.valid_lyts)
            {
                if (std::abs(valid_layout.get_system_energy() - min_energy) < physical_constants::POP_STABILITY_ERR)
                {
                    ground_state_layouts.emplace_back(
                        charge_distribution_surface<sidb_cell_clk_lyt_siqad>{valid_layout});
                }
            }
            if (ground_state_layouts.size() > 1)
            {
                std::cout << "degeneracy detected!" << std::endl;
            }
            if (!ground_state_layouts.empty())
            {
                const auto        sidbs              = ground_state_layouts.front().get_all_sidb_cells();
                const auto        physical_parameter = ground_state_layouts.front().get_phys_params();
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << -params.mu;
                std::ofstream outFile(folder.path().string() + "/loc/" + name + "_sim_µ_minus_" + ss.str() + ".txt");
                outFile << std::fixed << std::setprecision(3);
                outFile << "x;"
                        << "y;";
                for (uint64_t i = 0; i < ground_state_layouts.size(); i++)
                {
                    outFile << std::to_string(i) << ";";
                }
                outFile << std::endl;

                for (const auto& sidb : sidbs)
                {
                    auto pos = sidb_nm_position<sidb_cell_clk_lyt_siqad>(physical_parameter, sidb);
                    outFile << pos.first << ";" << pos.second << ";";
                    for (const auto& valid_layout : ground_state_layouts)
                    {
                        outFile << std::to_string(charge_state_to_sign(valid_layout.get_charge_state(sidb))) << ";";
                    }
                    outFile << std::endl;
                }
            }
            else
            {
                std::cout << "no groundstate found" << std::endl;
            }
        }
    }

    return EXIT_SUCCESS;
}
