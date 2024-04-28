//
// Created by marcel on 13.08.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/tt_reader.hpp>

#include <kitty/constructors.hpp>
#include <kitty/operations.hpp>

#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Read truth tables", "[tt-reader]")
{
    // from https://people.eecs.berkeley.edu/~alanmi/temp5/
    static constexpr const char* file = "0x6996000000006996 (!(f+e)*(d+c+b+a))\n"
                                        "0x0990900906606006 (!(e+d+c)*(f+b+a))\n"
                                        "0x0000966996690000 ((f+e)*!(d+c+b+a))\n"
                                        "0x6006066090090990 ((e+d+c)*!(f+b+a))";

    std::stringstream stream{std::string{file}};

    tt_reader<kitty::dynamic_truth_table> reader{stream};

    const auto check = [&reader](const auto num_vars, const std::string& hex)
    {
        auto read_tt = reader.next();
        REQUIRE(read_tt.has_value());

        kitty::dynamic_truth_table test_tt{num_vars};
        kitty::create_from_hex_string(test_tt, hex);
        CHECK(kitty::equal(*read_tt, test_tt));
    };

    check(6u, "6996000000006996");
    check(6u, "0990900906606006");
    check(6u, "0000966996690000");
    check(6u, "6006066090090990");
}
