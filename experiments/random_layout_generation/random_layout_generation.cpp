//
// Created by Jan Drewniok on 04.05.23.
//

#include <fiction/algorithms/simulation/sidb/random_layout_generator.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>

using namespace fiction;

int main(int argc, const char* argv[])  // NOLINT
{
    std::unordered_map<std::string, std::variant<std::string>> options{{"--folder_name", "layout_random_cli/"},
                                                                       {"--NW_x", "0"},
                                                                       {"--NW_y", "0"},
                                                                       {"--SE_x", "20"},
                                                                       {"--SE_y", "20"},
                                                                       {"--positive_charges", "FORBIDDEN"},
                                                                       {"--lower", "5"},
                                                                       {"--upper", "10"},
                                                                       {"--num_layouts", "10"},
                                                                       {"--step", "1"}};

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

    // Extract values from options map
    const std::string folder_name = std::get<std::string>(options["--folder_name"]);
    // x-coordinate of north-west cell.
    const int32_t nw_x = std::stoi(std::get<std::string>(options["--NW_x"]));
    // y-coordinate of north-west cell.
    const int32_t nw_y = std::stoi(std::get<std::string>(options["--NW_y"]));
    // x-coordinate of south-east cell.
    const int32_t se_x = std::stoi(std::get<std::string>(options["--SE_x"]));
    // y-coordinate of south-east cell.
    const int32_t     se_y        = std::stoi(std::get<std::string>(options["--SE_y"]));
    const std::string charges_str = std::get<std::string>(options["--positive_charges"]);
    // It labels if positively charged SiDBs are allowed.
    const positive_charges charges =
        (charges_str == "ALLOWED") ? positive_charges::ALLOWED : positive_charges::FORBIDDEN;
    // Number of SiDBs of the first bunch of layouts.
    const uint64_t lower_limit = std::stoull(std::get<std::string>(options["--lower"]));
    // Number of SiDBs of the last bunch of layouts.
    const uint64_t upper_limit = std::stoull(std::get<std::string>(options["--upper"]));
    // Number of layouts for each number of placed SiDBs (e.g., num_layouts = 10 means that 10 different layouts for a
    // given number of SiDBs is generated).
    const uint64_t number_of_layouts = std::stoull(std::get<std::string>(options["--num_layouts"]));
    // Step size for moving from lower limit to upper limit.
    const uint64_t step = std::stoull(std::get<std::string>(options["--step"]));

    // Print the parsed values
    std::cout << "Folder name: " << folder_name << std::endl;
    std::cout << fmt::format("NW: {} | {}", nw_x, nw_y) << std::endl;
    std::cout << fmt::format("SE: {} | {}", se_x, se_y) << std::endl;
    std::cout << fmt::format("positive_charges: {}", charges_str) << std::endl;
    std::cout << "lower_limit: " << lower_limit << std::endl;
    std::cout << "upper_limit: " << upper_limit << std::endl;
    std::cout << "number_of_layouts: " << number_of_layouts << std::endl;
    std::cout << "step: " << step << std::endl;

    // This script generates random layouts as .sqd file.
    using cell_level_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

    try
    {
        std::filesystem::path folder_path(EXPERIMENTS_PATH);

        folder_path /= folder_name;

        if (std::filesystem::exists(folder_path))
        {
            std::cout << "Folder *" << folder_path << "* exists!" << std::endl;
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

        for (uint64_t place_sidbs = lower_limit; place_sidbs <= upper_limit; place_sidbs += step)
        {
            const std::filesystem::path dir_path = folder_path.string() + "number_sidbs_" + std::to_string(place_sidbs);
            const std::filesystem::path dir_path_sqd =
                folder_path.string() + "number_sidbs_" + std::to_string(place_sidbs) + "/sqd";
            const std::filesystem::path dir_path_loc =
                folder_path.string() + "number_sidbs_" + std::to_string(place_sidbs) + "/loc";
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

                const generate_random_layout_params<cell_level_layout> params{
                    {{nw_x, nw_y}, {se_x, se_y}}, place_sidbs,      charges, 2,
                    static_cast<uint64_t>(10E6),  number_of_layouts};
                const auto unique_lyts =
                    generate_multiple_random_layouts<cell_level_layout>(cell_level_layout{}, params);
                for (auto i = 0u; i < unique_lyts.size(); i++)
                {
                    write_sqd_layout(unique_lyts[i], fmt::format("{}/layout_{}.sqd", dir_path_sqd.string(), i));
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
