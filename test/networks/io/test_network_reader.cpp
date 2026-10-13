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

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

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
#include <random>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::networks::io;

TEST_CASE("Read Verilog", "[network-reader]")
{
    /** @brief Shared Verilog benchmark resolved from the test resource root. */
    const auto mux21_file_name = (std::filesystem::path{TEST_PATH} / "../benchmarks/TOY/mux21.v").string();

    std::ostringstream os{};

    network_reader<aig_ptr> reader{mux21_file_name, os};

    // no error messages
    REQUIRE(os.str().empty());

    const auto nets = reader.get_networks();

    // exactly one net should have been created
    REQUIRE(nets.size() == 1);

    const auto mux21 = *nets.front();

    SECTION("Equality")
    {
        check_eq(mux21, blueprints::mux21_network<aig_nt>());
    }
    SECTION("Name conservation")
    {
        // network name
        CHECK(mux21.get_network_name() == "mux21");

        // PI names
        CHECK(mux21.get_name(mux21.make_signal(1)) == "in0");  // first PI
        CHECK(mux21.get_name(mux21.make_signal(2)) == "in1");  // second PI
        CHECK(mux21.get_name(mux21.make_signal(3)) == "in2");  // third PI

        // PO names
        CHECK(mux21.get_output_name(0) == "out");
    }
}

TEST_CASE("BLIF readers accept empty lines without changing logic", "[network-reader]")
{
    const auto whitespace = GENERATE(std::string{}, std::string{" \t\r"});
    const auto path       = std::filesystem::temp_directory_path() /
                            ("fiction-blif-whitespace-" + std::to_string(std::random_device{}()) + ".blif");
    {
        std::ofstream file{path};
        file << whitespace << "\n# before model\n.model whitespace\n"
             << whitespace << "\n.inputs a b \\\n"
             << whitespace << "\n unused\n.outputs zero one inverted xor lut copy\n"
             << whitespace << "\n.names zero\n"
             << whitespace << "\n.names one\n"
             << whitespace << "\n1\n# between gates\n.names a inverted\n"
             << whitespace << "\n0 1\n.names a b xor\n01 1\n"
             << whitespace << "\n# between cover rows\n10 1\n.names a b \\\n lut\n01 1\n.names a copy\n1 1\n.end\n"
             << whitespace << '\n';
    }
    std::ostringstream      diagnostics{};
    network_reader<tec_ptr> reader{path.string(), diagnostics};
    std::filesystem::remove(path);
    REQUIRE(diagnostics.str().empty());
    REQUIRE(reader.get_networks().size() == 1);
    const auto& network = *reader.get_networks().front();
    CHECK(network.num_pis() == 3);
    REQUIRE(network.num_pos() == 6);
    network.foreach_pi(
        [&](const auto pi, const auto index)
        { CHECK(network.get_name(network.make_signal(pi)) == std::array{"a", "b", "unused"}.at(index)); });
    const auto actual = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{3});
    const std::array functions{"00", "ff", "55", "66", "44", "aa"};
    const std::array names{"zero", "one", "inverted", "xor", "lut", "copy"};
    for (std::size_t index{}; index < functions.size(); ++index)
    {
        kitty::dynamic_truth_table expected{3};
        kitty::create_from_hex_string(expected, functions.at(index));
        CHECK(actual.at(index) == expected);
        CHECK(network.get_output_name(static_cast<uint32_t>(index)) == names.at(index));
    }
}

TEST_CASE("BLIF readers report invalid declarations after empty lines", "[network-reader]")
{
    const auto path = std::filesystem::temp_directory_path() /
                      ("fiction-blif-invalid-" + std::to_string(std::random_device{}()) + ".blif");
    {
        std::ofstream file{path};
        file << "\n.model invalid\n \t\n.unsupported declaration\n\n.end\n";
    }
    std::ostringstream      diagnostics{};
    network_reader<tec_ptr> reader{path.string(), diagnostics};
    std::filesystem::remove(path);
    CHECK(reader.get_networks().empty());
    CHECK(diagnostics.str().find("parsing error in " + path.string()) != std::string::npos);
}
