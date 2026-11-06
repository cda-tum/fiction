/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/networks/io/network_reader.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/scoped_temp_directory.hpp"

#include <fiction/networks/io/network_reader.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::io;

TEST_CASE("Temporary test directories preserve occupied names and clean up", "[network-reader]")
{
    const fiction::test::scoped_temp_directory root{std::filesystem::temp_directory_path(),
                                                    "fiction-temp-test-" + std::to_string(std::random_device{}())};
    const auto                                 occupied = root.path() / "candidate-0";
    REQUIRE(std::filesystem::create_directory(occupied));
    const auto sentinel = occupied / "sentinel";
    {
        std::ofstream output{sentinel};
        REQUIRE(output.is_open());
        output << "kept";
        output.close();
        REQUIRE_FALSE(output.fail());
    }
    std::filesystem::path created{};
    {
        const fiction::test::scoped_temp_directory child{root.path(), "candidate"};
        created = child.path();
        CHECK(created == root.path() / "candidate-1");
        std::ofstream debris{created / "debris"};
        REQUIRE(debris.is_open());
    }
    CHECK_FALSE(std::filesystem::exists(created));
    std::ifstream input{sentinel};
    REQUIRE(input.is_open());
    std::string contents{};
    input >> contents;
    CHECK(contents == "kept");

    for (auto index = 1; index < 16; ++index)
    {
        REQUIRE(std::filesystem::create_directory(root.path() / ("candidate-" + std::to_string(index))));
    }
    CHECK_THROWS_AS((fiction::test::scoped_temp_directory{root.path(), "candidate"}), std::runtime_error);
    CHECK_THROWS_WITH((fiction::test::scoped_temp_directory{root.path(), "candidate"}),
                      Catch::Matchers::ContainsSubstring("Temporary directory candidates occupied:"));
    CHECK(std::filesystem::exists(sentinel));
    for (auto index = 0; index < 16; ++index)
    {
        CAPTURE(index);
        CHECK(std::filesystem::is_directory(root.path() / ("candidate-" + std::to_string(index))));
    }
    CHECK_THROWS_AS((fiction::test::scoped_temp_directory{root.path() / "missing", "candidate"}),
                    std::filesystem::filesystem_error);
    std::filesystem::path unwound{};
    CHECK_THROWS_AS((
                        [&]
                        {
                            const fiction::test::scoped_temp_directory child{root.path(), "unwound"};
                            unwound = child.path();
                            throw std::runtime_error{"test"};
                        }()),
                    std::runtime_error);
    REQUIRE(unwound == root.path() / "unwound-0");
    CHECK_FALSE(std::filesystem::exists(unwound));

    const auto occupied_file = root.path() / "file-candidate-0";
    {
        std::ofstream output{occupied_file};
        REQUIRE(output.is_open());
        output << "kept";
        output.close();
        REQUIRE_FALSE(output.fail());
    }
    {
        const fiction::test::scoped_temp_directory child{root.path(), "file-candidate"};
        CHECK(child.path() == root.path() / "file-candidate-1");
    }
    std::ifstream occupied_input{occupied_file};
    REQUIRE(occupied_input.is_open());
    std::string retained{};
    occupied_input >> retained;
    CHECK(retained == "kept");
}

TEMPLATE_TEST_CASE("Read Verilog", "[network-reader]", named_aig_network, named_xag_network, named_mig_network,
                   named_technology_network)
{
    /** @brief Shared Verilog benchmark resolved from the test resource root. */
    const auto mux21_file_name = (std::filesystem::path{TEST_PATH} / "../benchmarks/TOY/mux21.v").string();

    std::ostringstream os{};

    network_reader<std::shared_ptr<TestType>> reader{mux21_file_name, os};

    // no error messages
    REQUIRE(os.str().empty());

    const auto& nets = reader.get_networks();

    // exactly one net should have been created
    REQUIRE(nets.size() == 1);

    REQUIRE(nets.front());
    const auto& mux21 = *nets.front();

    SECTION("Equality")
    {
        check_eq(mux21, blueprints::mux21_network<TestType>());
    }
    SECTION("Name conservation")
    {
        REQUIRE(mux21.num_pis() == 3);
        REQUIRE(mux21.num_pos() == 1);

        // network name
        CHECK(mux21.get_network_name() == "mux21");

        // PI names
        for (uint32_t i = 0; i < mux21.num_pis(); ++i)
        {
            CHECK(mux21.get_name(mux21.make_signal(mux21.pi_at(i))) == "in" + std::to_string(i));
        }

        // PO names
        CHECK(mux21.get_output_name(0) == "out");
    }
}

TEST_CASE("BLIF readers accept empty lines without changing logic", "[network-reader]")
{
    const auto whitespace = GENERATE(std::string{}, std::string{" \t\r"});
    CAPTURE(whitespace);
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-whitespace-" + std::to_string(std::random_device{}())};
    const auto path = directory.path() / "whitespace.blif";
    {
        std::ofstream file{path};
        REQUIRE(file.is_open());
        file << whitespace << "\n# before model\n.model whitespace\n"
             << whitespace << "\n.inputs a b \\\n"
             << whitespace << "\n unused\n.outputs zero one inverted xor lut copy\n"
             << whitespace << "\n.names zero\n"
             << whitespace << "\n.names one\n"
             << whitespace << "\n1\n# between gates\n.names a inverted\n"
             << whitespace << "\n0 1\n.names a b xor\n01 1\n"
             << whitespace << "\n# between cover rows\n10 1\n.names a b \\\n lut\n01 1\n.names a copy\n1 1\n.end\n"
             << whitespace << '\n';
        file.close();
        REQUIRE_FALSE(file.fail());
    }
    std::ostringstream                                        diagnostics{};
    network_reader<std::shared_ptr<named_technology_network>> reader{path.string(), diagnostics};
    REQUIRE(diagnostics.str().empty());
    REQUIRE(reader.get_networks().size() == 1);
    REQUIRE(reader.get_networks().front());
    const auto& network = *reader.get_networks().front();
    CHECK(network.num_pis() == 3);
    REQUIRE(network.num_pos() == 6);
    std::vector<std::string> input_names{};
    std::vector<uint32_t>    input_indices{};
    network.foreach_pi(
        [&](const auto pi, const auto index)
        {
            input_names.push_back(network.get_name(network.make_signal(pi)));
            input_indices.push_back(index);
        });
    CHECK(input_names == std::vector<std::string>{"a", "b", "unused"});
    CHECK(input_indices == std::vector<uint32_t>{0, 1, 2});
    const auto actual = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{3});
    REQUIRE(actual.size() == 6);
    const std::array functions{"00", "ff", "55", "66", "44", "aa"};
    const std::array names{"zero", "one", "inverted", "xor", "lut", "copy"};
    for (std::size_t index{}; index < functions.size(); ++index)
    {
        CAPTURE(index, names.at(index));
        kitty::dynamic_truth_table expected{3};
        kitty::create_from_hex_string(expected, functions.at(index));
        CHECK(actual.at(index) == expected);
        CHECK(network.get_output_name(static_cast<uint32_t>(index)) == names.at(index));
    }
}

TEST_CASE("BLIF readers report invalid declarations after empty lines", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-invalid-" + std::to_string(std::random_device{}())};
    const auto path = directory.path() / "invalid.blif";
    {
        std::ofstream file{path};
        REQUIRE(file.is_open());
        file << "\n.model invalid\n \t\n.unsupported declaration\n\n.end\n";
        file.close();
        REQUIRE_FALSE(file.fail());
    }
    std::ostringstream                                        diagnostics{};
    network_reader<std::shared_ptr<named_technology_network>> reader{path.string(), diagnostics};
    CHECK(reader.get_networks().empty());
    CHECK(diagnostics.str().find("parsing error in " + path.string()) != std::string::npos);
}
