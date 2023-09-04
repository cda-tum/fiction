//
// Created by Jan Drewniok on 04.05.23.
//

#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace fiction;

/**
 * This program generates random SiDB layouts and saves them as .sqd files (provide SiQAD compatibility) in a specified
 * folder. It allows configuring various layout parameters through command-line arguments, including the folder name,
 * layout dimensions, allowed charges (2- or 3-states), number of layouts for each SiDB count, and step size for SiDB
 * count increments.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of C-style strings containing the command-line arguments.
 * @return Returns 0 on successful execution, or EXIT_FAILURE if an error occurs.
 *
 * Command-line Options:
 *   --folder_name <name>       Specifies the name of the folder to store generated layouts.
 *   --NW_x <value>             Sets the x-coordinate of the north-west cell corner.
 *   --NW_y <value>             Sets the y-coordinate of the north-west cell corner.
 *   --SE_x <value>             Sets the x-coordinate of the south-east cell corner.
 *   --SE_y <value>             Sets the y-coordinate of the south-east cell corner.
 *   --positive_charges <type>  Specifies whether positively charged SiDBs are allowed ("ALLOWED") or forbidden
 * ("FORBIDDEN").
 *   --lower <value>            Sets the number of SiDBs for the first bunch of layouts.
 *   --upper <value>            Sets the number of SiDBs for the last bunch of layouts.
 *   --num_layouts <value>      Sets the number of layouts to generate for each SiDB count.
 *   --step <value>             Sets the step size for incrementing the SiDB count from lower to upper limit.
 *
 * Example Usage:
 *   To generate layouts with default options and save them in a folder named "layout_random_cli/":
 *   ./random_sidb_layout_generation
 *
 *   To specify custom options:
 *   ./random_sidb_layout_generation --folder_name my_layouts --NW_x 0 --NW_y 0 --SE_x 20 --SE_y 20 --positive_charges
 * ALLOWED --lower 5 --upper 10 --num_layouts 10 --step 1
 */
int main(int argc, const char* argv[])  // NOLINT
{
    std::unordered_map<std::string, std::string> options{{"--folder_name", "layout_random_cli/"},
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
                return EXIT_FAILURE;
            }
        }
    }

    // specifies the name of the folder to store generated layouts
    const std::string folder_name = options["--folder_name"];
    // sets the x-coordinate of the north-west cell corner
    const int32_t nw_x = std::stoi(options["--NW_x"]);
    // sets the y-coordinate of the north-west cell corner
    const int32_t nw_y = std::stoi(options["--NW_y"]);
    // sets the x-coordinate of the south-east cell corner
    const int32_t se_x = std::stoi(options["--SE_x"]);
    // sets the y-coordinate of the south-east cell corner
    const int32_t se_y = std::stoi(options["--SE_y"]);
    // specifies whether positively charged SiDBs are allowed ("ALLOWED") or forbidden ("FORBIDDEN")
    const std::string charges_str = options["--positive_charges"];
    // specifies whether positively charged SiDBs are allowed ("ALLOWED") or forbidden ("FORBIDDEN")
    const positive_charges charges =
        (charges_str == "ALLOWED") ? positive_charges::ALLOWED : positive_charges::FORBIDDEN;
    // sets the number of SiDBs for the first bunch of layouts
    const uint64_t lower_limit = std::stoull(options["--lower"]);
    // sets the number of SiDBs for the last bunch of layouts
    const uint64_t upper_limit = std::stoull(options["--upper"]);
    // sets the number of layouts to generate for each SiDB count
    const uint64_t number_of_layouts = std::stoull(options["--num_layouts"]);
    // sets the step size for incrementing the SiDB count from lower to upper limit
    const uint64_t step = std::stoull(options["--step"]);

    // print the parsed values
    std::cout << "Folder name: " << folder_name << std::endl;
    std::cout << fmt::format("NW: {} | {}", nw_x, nw_y) << std::endl;
    std::cout << fmt::format("SE: {} | {}", se_x, se_y) << std::endl;
    std::cout << fmt::format("positive_charges: {}", charges_str) << std::endl;
    std::cout << "lower_limit: " << lower_limit << std::endl;
    std::cout << "upper_limit: " << upper_limit << std::endl;
    std::cout << "number_of_layouts: " << number_of_layouts << std::endl;
    std::cout << "step: " << step << std::endl;

    // generates random layouts as .sqd file
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

                const generate_random_sidb_layout_params<cell_level_layout> params{
                    {{nw_x, nw_y}, {se_x, se_y}}, place_sidbs, charges, 2, uint64_t(10E6), number_of_layouts};
                const auto unique_lyts =
                    generate_multiple_random_sidb_layouts<cell_level_layout>(cell_level_layout{}, params);
                for (auto i = 0u; i < unique_lyts.size(); i++)
                {
                    write_sqd_layout(unique_lyts[i], fmt::format("{}/layout_{}.sqd", dir_path_sqd.string(), i));
                }
            }
            catch (const std::filesystem::filesystem_error& ex)
            {
                // exception is handled
                std::cerr << "Filesystem error: " << ex.what() << std::endl;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        // exception occurred, handle it here
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
