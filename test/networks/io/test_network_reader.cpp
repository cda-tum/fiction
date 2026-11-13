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
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/sequential.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <random>
#include <regex>
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
    CHECK_THAT((contents), Catch::Matchers::Equals("kept"));

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
    CHECK_THAT((retained), Catch::Matchers::Equals("kept"));
}

TEMPLATE_TEST_CASE("Read Verilog", "[network-reader]", named_aig_network, named_xag_network, named_mig_network,
                   named_technology_network)
{
    /** @brief Shared Verilog benchmark resolved from the test resource root. */
    const auto mux21_file_name = std::filesystem::path{TEST_PATH} / "../benchmarks/TOY/mux21.v";
    const auto mux21           = read_network<TestType>(mux21_file_name);

    SECTION("Equality")
    {
        check_eq(mux21, blueprints::mux21_network<TestType>());
    }
    SECTION("Name conservation")
    {
        REQUIRE(mux21.num_pis() == 3);
        REQUIRE(mux21.num_pos() == 1);

        // network name
        CHECK_THAT((mux21.get_network_name()), Catch::Matchers::Equals("mux21"));

        // PI names
        for (uint32_t i = 0; i < mux21.num_pis(); ++i)
        {
            CHECK_THAT((mux21.get_name(mux21.make_signal(mux21.pi_at(i)))),
                       Catch::Matchers::Equals("in" + std::to_string(i)));
        }

        // PO names
        CHECK_THAT((mux21.get_output_name(0)), Catch::Matchers::Equals("out"));
    }
}

TEST_CASE("Verilog reader removes unused gates", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-verilog-cleanup-" + std::to_string(std::random_device{}())};
    const auto path = directory.path() / "unused.v";
    {
        std::ofstream file{path, std::ios::binary};
        file << "module top(a,b,y);\r\n"
                "  input a,b;\r\n"
                "  output y;\r\n"
                "  wire unused;\r\n"
                "  assign unused = a | b;\r\n"
                "  assign y = a & b;\r\n"
                "endmodule\r\n";
        file.close();
        REQUIRE_FALSE(file.fail());
    }

    const auto network = read_network<named_technology_network>(path);
    CHECK(network.num_pis() == 2);
    REQUIRE(network.num_pos() == 1);
    CHECK(network.num_gates() == 1);
    const auto outputs = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{network.num_pis()});
    CHECK(kitty::get_bit(outputs.front(), 3));
    CHECK_FALSE(kitty::get_bit(outputs.front(), 1));
}

TEST_CASE("BLIF readers accept empty lines without changing logic", "[network-reader]")
{
    const auto whitespace = GENERATE(std::string{}, std::string{" \t\r"});
    const auto separator  = GENERATE(' ', '\t');
    CAPTURE(whitespace, separator);
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-whitespace-" + std::to_string(std::random_device{}())};
    const auto path = directory.path() / "whitespace.blif";
    {
        std::ofstream file{path};
        REQUIRE(file.is_open());
        file << whitespace << "\n# before model\n.model whitespace\n"
             << whitespace << "\n.inputs a\\\n"
             << whitespace << "\nb" << separator << "unused\n.outputs zero" << separator
             << "one inverted xor lut copy\n"
             << whitespace << "\n.names a zero\n"
             << whitespace << "\n.names one\n"
             << whitespace << "\n1\n# between gates\n.names a inverted\n"
             << whitespace << "\n0 1\n.names a" << separator << "b xor\n01 1\n"
             << whitespace << "\n# between cover rows\n10 1\n.names a b \\\n lut\n01 1\n.names a copy\n1 1\n.end\n"
             << whitespace << '\n';
        file.close();
        REQUIRE_FALSE(file.fail());
    }
    const auto network = read_network<named_technology_network>(path);
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
        CHECK_THAT((network.get_output_name(static_cast<uint32_t>(index))), Catch::Matchers::Equals(names.at(index)));
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
        file << "\n.model invalid\n \t\n.unsupported first\n\n.unsupported second\n.end\n";
        file.close();
        REQUIRE_FALSE(file.fail());
    }
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_technology_network>(path)),
                      Catch::Matchers::ContainsSubstring(path.string()) &&
                          Catch::Matchers::ContainsSubstring(".unsupported first") &&
                          Catch::Matchers::ContainsSubstring(".unsupported second"));
}

TEST_CASE("BLIF readers return the first model", "[network-reader]")
{
    const auto [end, separator] =
        GENERATE(table<const char*, char>({{"", ' '}, {".end\n", ' '}, {"", '\f'}, {"", '\v'}}));
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-model-" + std::to_string(std::random_device{}())};
    const auto    path = directory.path() / "models.blif";
    std::ofstream file{path};
    file << ".model first\n.inputs a\n.outputs a\n"
         << end << ".model" << separator << "second\n.inputs b\n.outputs b\n.end\n";
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_technology_network>(path);
    REQUIRE(network.num_pis() == 1);
    REQUIRE(network.num_pos() == 1);
    CHECK(network.get_output_name(0) == "a");
}

TEST_CASE("BLIF readers accept empty port lists", "[network-reader]")
{
    const auto [body, inputs, outputs] = GENERATE(table<const char*, uint32_t, uint32_t>(
        {{".inputs\n.outputs y\n.names y\n1\n", 0, 1}, {".inputs a\n.outputs\n", 1, 0}}));
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-empty-ports-" + std::to_string(std::random_device{}())};
    const auto    path = directory.path() / "empty-ports.blif";
    std::ofstream file{path};
    file << ".model top\n" << body << ".end\n";
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_technology_network>(path);
    CHECK(network.num_pis() == inputs);
    CHECK(network.num_pos() == outputs);
}

TEST_CASE("BLIF readers accept carriage-return line endings", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{std::filesystem::temp_directory_path(),
                                                         "fiction-blif-cr-" + std::to_string(std::random_device{}())};
    const auto                                 path = directory.path() / "carriage-return.blif";
    std::ofstream                              file{path, std::ios::binary};
    file << "# unnamed model\r\r.inputs \\\ra\\\r# end of the input declaration\r.outputs y\r.names a y\r1 1";
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_technology_network>(path);
    REQUIRE(network.num_pis() == 1);
    REQUIRE(network.num_pos() == 1);
    CHECK(network.get_output_name(0) == "y");
    const auto outputs = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{1});
    kitty::dynamic_truth_table identity{1};
    kitty::create_from_hex_string(identity, "2");
    CHECK(outputs == std::vector{identity});
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(path)),
                      Catch::Matchers::ContainsSubstring("requires a network that supports truth-table nodes"));
}

TEST_CASE("Read network rejects sequential results", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-blif-latch-" + std::to_string(std::random_device{}())};
    const auto    path = directory.path() / "sequential.blif";
    std::ofstream file{path};
    file << ".inputs a\n.outputs y\n.latch a y\n.end\n";
    file.close();
    REQUIRE_FALSE(file.fail());

    CHECK_THROWS_WITH(static_cast<void>(read_network<mockturtle::sequential<technology_network>>(path)),
                      Catch::Matchers::ContainsSubstring("not combinational"));
}

TEST_CASE("Directory readers retain successful networks and report failed files", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-network-batch-" + std::to_string(std::random_device{}())};
    const auto& root  = directory.path();
    const auto  empty = read_networks<named_aig_network>(root);
    CHECK(empty.networks.empty());
    CHECK(empty.failures.empty());
    CHECK_THROWS_AS(static_cast<void>(read_network<named_aig_network>(root)), network_read_error);

    const auto fixture = std::filesystem::path{TEST_PATH} / "../benchmarks/TOY/mux21.v";
    REQUIRE(std::filesystem::copy_file(fixture, root / "z.v"));
    REQUIRE(std::filesystem::copy_file(fixture, root / "b.V"));
    REQUIRE(std::filesystem::copy_file(fixture, root / "c.data"));
    REQUIRE(std::filesystem::create_directory(root / "ignored.v"));
    {
        std::ofstream broken{root / "a.v"};
        broken << "module broken(\n";
        broken.close();
        REQUIRE_FALSE(broken.fail());
    }

    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(root / "missing.v")),
                      Catch::Matchers::ContainsSubstring("missing.v"));
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(root / "c.data")),
                      Catch::Matchers::ContainsSubstring("unsupported network extension"));
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(root / "b.V", "unknown")),
                      Catch::Matchers::ContainsSubstring("unsupported network format"));
    auto embedded_null = (root / "b.V").native();
    embedded_null += std::filesystem::path::value_type{};
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(embedded_null)),
                      Catch::Matchers::ContainsSubstring("null character"));

    const auto batch = read_networks<named_aig_network>(root);
    REQUIRE(batch.networks.size() == 2);
    CHECK(batch.networks[0].get_network_name() == "b");
    CHECK(batch.networks[1].get_network_name() == "z");
    REQUIRE(batch.failures.size() == 1);
    CHECK(batch.failures.front().path == root / "a.v");
    CHECK_THAT(batch.failures.front().message, Catch::Matchers::ContainsSubstring("parsing error"));
    CHECK(read_network<named_aig_network>(root).get_network_name() == "b");
    CHECK(read_network<named_aig_network>(root / "c.data", ".V").get_network_name() == "c");
    CHECK(read_networks<named_aig_network>(root, "v").networks.size() == 3);
    CHECK_THROWS_AS(static_cast<void>(read_networks<named_aig_network>(fixture)), network_read_error);
    REQUIRE(std::filesystem::remove(root / "b.V"));
    REQUIRE(std::filesystem::remove(root / "z.v"));
    CHECK_THROWS_WITH(static_cast<void>(read_network<named_aig_network>(root)),
                      Catch::Matchers::ContainsSubstring("a.v") && Catch::Matchers::ContainsSubstring("parsing error"));
}

TEST_CASE("Directory reads continue after recoverable reader errors", "[network-reader]")
{
    /** @brief Simulates reader errors without depending on platform-specific limits. */
    struct failing_reader_network : named_aig_network
    {
        /** @brief Construct an empty test network. */
        failing_reader_network() = default;

        /** @brief Assign the name or simulate a reader error for the selected input. */
        void set_name(const signal& input_signal, const std::string& name)
        {
            if (name == "complexity")
            {
                throw std::regex_error{std::regex_constants::error_complexity};
            }
            if (name == "depth")
            {
                throw std::regex_error{std::regex_constants::error_stack};
            }
            if (name == "allocation")
            {
                throw std::regex_error{std::regex_constants::error_space};
            }
            if (name == "range_error")
            {
                throw std::out_of_range{"conversion failed"};
            }
            if (name == "io_error")
            {
                throw std::ios_base::failure{"stream failed"};
            }
            named_aig_network::set_name(input_signal, name);
        }
    };

    const auto input =
        GENERATE(std::string_view{"complexity"}, std::string_view{"depth"}, std::string_view{"allocation"},
                 std::string_view{"range_error"}, std::string_view{"io_error"});
    const fiction::test::scoped_temp_directory directory{std::filesystem::temp_directory_path(),
                                                         "fiction-network-reader-error-" +
                                                             std::to_string(std::random_device{}())};
    const auto                                 limited = directory.path() / "a.v";
    {
        std::ofstream file{limited};
        file << "module top(" << input << ",y); input " << input << "; output y; assign y = " << input
             << "; endmodule\n";
        file.close();
        REQUIRE_FALSE(file.fail());
    }
    const auto fixture = std::filesystem::path{TEST_PATH} / "../benchmarks/TOY/mux21.v";
    REQUIRE(std::filesystem::copy_file(fixture, directory.path() / "b.v"));

    if (input == "allocation")
    {
        CHECK_THROWS_AS(static_cast<void>(read_networks<failing_reader_network>(directory.path())), std::regex_error);
        return;
    }

    const auto batch = read_networks<failing_reader_network>(directory.path());
    REQUIRE(batch.failures.size() == 1);
    CHECK(batch.failures.front().path == limited);
    const std::string reason{input == "io_error" ? "read error" : "parsing error"};
    CHECK_THAT(batch.failures.front().message, Catch::Matchers::ContainsSubstring(reason));
    REQUIRE(batch.networks.size() == 1);
    CHECK(batch.networks.front().get_network_name() == "b");
    CHECK(read_network<failing_reader_network>(directory.path()).get_network_name() == "b");
}

TEST_CASE("Malformed network files report their path", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-network-invalid-" + std::to_string(std::random_device{}())};
    const auto [extension, content] = GENERATE(table<std::string, std::string>({
        {".aig", ""},
        {".aig", "aag 1 1 0 1 0\n2\n2\n"},
        {".aig", "aig 2 1 0 1 0\n2\n"},
        {".aig", "aig 2 1 1 1 0\n2 2\n4\n"},
        {".aig", "aig 1 1 0 1 0 0 0 1\n2\n100\n"},
        {".aig", "aig 2 1 0 1 1\n4\n\x04\x05"},
        {".aig", "aig 2 1 0 1 1\n4\n\x80"},
        {".aig", "aig 1 1 0 1 0\n4\n"},
        {".aig", "aig 1 1 0 1 0\n2\ni1 invalid\n"},
        {".aig", "aig 1 1 0 1 0\n2\nc999999999999999999999999 invalid\n"},
        {".blif", "# no network\n"},
        {".blif", ".end\n"},
        {".blif", ".model broken\n.inputs a\n.outputs undriven\n.end\n"},
        {".blif", ".model broken\n.inputs a \\\n"},
        {".blif", ".model broken\n.inputs a\n.outputs y\n.names a y\n11 1\n.end\n"},
        {".blif", ".model broken\n.inputs a\n.outputs y\n.names a y\n1 1 garbage\n.end\n"},
        {".blif", ".inputs a\n.outputs y\n.names a y\n1 1\n0 0\n.end\n"},
        {".blif", ".inputs a\n.outputs y\n.names a y\nx 1\n.end\n"},
        {".blif", ".inputs a\n.outputs a\n.names a\n1\n.end\n"},
        {".blif", ".outputs y\n.names y\n1\n0\n.end\n"},
        {".blif", ".inputs a\n.outputs y\n.latch a y\n.end\n"},
        {".v", "module top(a,y); input [0:1] a; output y; assign y = a[0]; endmodule\n"},
        {".v", "module top(a,y); input [1'h2:0] a; output y; assign y = a[0]; endmodule\n"},
        {".v", "module top(a,y); input [2'hx:0] a; output y; assign y = a[0]; endmodule\n"},
        {".v", "module top(a,y); input [:0] a; output y; assign y = a[0]; endmodule\n"},
        {".v", "module helper(a,y); input [99999999999999999999:0] a; output y; assign y = a[0]; endmodule\n"
               "module top(a,y); input a; output y; assign y = a; endmodule\n"},
        {".v", "module helper(a); input [1:2:3] a; endmodule\n"
               "module top(a,y); input a; output y; assign y = a; endmodule\n"},
        {".v", "// no network\n"},
        {".v", "module helper(a,y); input a; output y; assign y = a; endmodule\n"},
        {".v", "module top(a,y); input a; output y; endmodule\n"},
        {".v", "module helper(a,z); input a; output z; assign z = a; endmodule\n"
               "module top(a,y); input a; output y; assign y = z; endmodule\n"},
        {".v", "module helper(a,y); input a; output y; assign y = a; endmodule\n"
               "module top(a,y); input a; output y; helper h(.a(a),.y(y)); endmodule\n"},
        {".v", "module ripple_carry_adder(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input a,b; output y; ripple_carry_adder #(invalid) add(.a(a),.b(b),.s(y)); "
               "endmodule\n"},
        {".v", "module ripple_carry_adder(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input a,b; output y; ripple_carry_adder add(.a(a),.b(b),.s(y)); endmodule\n"},
        {".v", "module ripple_carry_adder(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input [1:0] a,b; output [2:0] y; "
               "ripple_carry_adder #(2) add(.a(a),.a(b),.s(y)); endmodule\n"},
        {".v", "module ripple_carry_adder(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input [1:0] a,b; output [2:0] y; "
               "ripple_carry_adder #(0) add(.a(a),.b(b),.s(y)); endmodule\n"},
        {".v", "module montgomery_multiplier(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input [1:0] a,b; output [1:0] y; "
               "montgomery_multiplier #(4'h0,3,1) multiply(.a(a),.b(b),.s(y)); endmodule\n"},
        {".v", "module montgomery_multiplier(a,b,s); input a,b; output s; endmodule\n"
               "module top(a,b,y); input a,b; output y; montgomery_multiplier #(2,8'h03,invalid) "
               "multiply(.a(a),.b(b),.s(y)); endmodule\n"},
        {".v", "module top(a,y); input a; output y; assign y = a; endmodule\n"
               "module top(b,z); input b; output z; assign z = b; endmodule\n"},
    }));
    CAPTURE(content);
    const auto    path = directory.path() / ("malformed" + extension);
    std::ofstream file{path, std::ios::binary};
    file << content;
    file.close();
    REQUIRE_FALSE(file.fail());

    CHECK_THROWS_WITH(static_cast<void>(read_network<named_technology_network>(path)),
                      Catch::Matchers::ContainsSubstring(path.string()));
}

TEST_CASE("Verilog port checks apply to the network being read", "[network-reader]")
{
    const auto* const connections = GENERATE(".a(a),.b(b),.s(y)", ".s(y),.b(b),.a(a)");
    CAPTURE(connections);
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-verilog-top-" + std::to_string(std::random_device{}())};
    const auto    path = directory.path() / "modules.v";
    std::ofstream file{path};
    file << "module ripple_carry_adder(a,b,s); input a,b; output s; endmodule\n"
         << "module top(a,b,y); input [1:0] a,b; output [4'h2:0] y; "
            "ripple_carry_adder #(4'h2) add("
         << connections << "); endmodule\n"
         << "module helper(a,b,y); input [0:1] a,b; output y; "
            "ripple_carry_adder #(invalid) add(.a(a),.b(b),.s(y)); endmodule\n";
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_aig_network>(path);
    CHECK(network.num_pis() == 4);
    REQUIRE(network.num_pos() == 3);
    CHECK(network.get_output_name(0) == "y[0]");
    const auto outputs = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{4});
    const std::array functions{"5a5a", "936c", "ec80"};
    for (std::size_t index{}; index < functions.size(); ++index)
    {
        kitty::dynamic_truth_table expected{4};
        kitty::create_from_hex_string(expected, functions.at(index));
        CHECK(outputs.at(index) == expected);
    }
}

TEST_CASE("Read binary AIGER interfaces and output functions", "[network-reader]")
{
    const auto [content, output_name, functions] = GENERATE(table<std::string, std::string, std::vector<std::string>>({
        {"aig 3 2 0 1 1\n6\n\x02\x02i0 a\ni1 b\no0 y", "y", {"8"}},
        {"aig 3 1 1 0 1 1\n6 4\n4\n\x02\x02i0 a\nl0 state\nb0 bad\n", "bad", {"c", "8"}},
        {"aig 3 2 0 1 1 0 1 1 1\n6\n2\n1\n4\n2\n\x02\x02\ni0 a\ni1 b\no0 y\nc0 c\nj0 j\nf0 f\nc\ncomment\n",
         "y",
         {"8"}},
    }));
    CAPTURE(output_name);
    const fiction::test::scoped_temp_directory directory{std::filesystem::temp_directory_path(),
                                                         "fiction-aiger-" + std::to_string(std::random_device{}())};
    const auto                                 path = directory.path() / "interfaces.aig";
    std::ofstream                              file{path, std::ios::binary};
    file << content;
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_aig_network>(path);
    CHECK(network.num_pis() == 2);
    REQUIRE(network.num_pos() == functions.size());
    CHECK(network.get_output_name(0) == output_name);
    const auto outputs = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{2});
    REQUIRE(outputs.size() == functions.size());
    for (std::size_t index{}; index < functions.size(); ++index)
    {
        kitty::dynamic_truth_table expected{2};
        kitty::create_from_hex_string(expected, functions.at(index));
        CHECK(outputs.at(index) == expected);
    }
}

/** @brief Check that the multiplier uses the low bits of its parameters. */
TEST_CASE("Read Verilog multipliers with sized literals", "[network-reader]")
{
    const fiction::test::scoped_temp_directory directory{
        std::filesystem::temp_directory_path(), "fiction-verilog-multiplier-" + std::to_string(std::random_device{}())};
    const auto [width, modulus] = GENERATE(table<std::string, std::string>(
        {{"2", "3"}, {std::to_string(std::numeric_limits<unsigned long>::max()), "3"}, {"8", "ff"}}));
    const auto    path          = directory.path() / "multiplier.v";
    std::ofstream file{path};
    file << "module montgomery_multiplier(a,b,s); input a,b; output s; endmodule\n"
         << "module top(a,b,y); input [1:0] a,b; output [1:0] y; montgomery_multiplier #(2," << width << "'h" << modulus
         << ',' << width << "'h1) multiply(.a(a),.b(b),.s(y)); endmodule\n";
    file.close();
    REQUIRE_FALSE(file.fail());

    const auto network = read_network<named_aig_network>(path);
    REQUIRE(network.num_pis() == 4);
    REQUIRE(network.num_pos() == 2);
    const auto outputs = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{4});
    for (uint32_t a{}; a < 3; ++a)
    {
        for (uint32_t b{}; b < 3; ++b)
        {
            CAPTURE(a, b, width, modulus);
            const auto input = a | (b << 2u);
            const auto value = (a * b) % 3;
            CHECK(kitty::get_bit(outputs.at(0), input) == ((value & 1u) != 0));
            CHECK(kitty::get_bit(outputs.at(1), input) == ((value & 2u) != 0));
        }
    }
}
