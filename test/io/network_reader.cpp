//
// Created by marcel on 27.09.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/io/network_reader.hpp>
#include <fiction/types.hpp>

#include <sstream>
#include <string>

using namespace fiction;

// adapted from https://stackoverflow.com/questions/44508228/c-how-to-check-if-ostringstream-is-empty
template <typename Stream>
bool is_stream_empty(Stream& stream)
{
    stream.flush();
    std::streampos pos = stream.tellp();    // store current location
    stream.seekp(0, std::ios_base::end);    // go to end
    bool is_empty = (stream.tellp() == 0);  // check size == 0 ?
    stream.seekp(pos);                      // restore location

    return is_empty;
}

TEST_CASE("Read Verilog", "[network-reader]")
{
    constexpr const char* mux21_file_name = "../../benchmarks/TOY/mux21.v";

    std::ostringstream os{};

    network_reader<aig_ptr> reader{mux21_file_name, os};

    // no error messages
    REQUIRE(is_stream_empty(os));

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
