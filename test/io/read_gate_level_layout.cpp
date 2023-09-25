//
// Created by simon on 25.09.23.
//


#include <catch2/catch_test_macros.hpp>

#include <fiction/io/read_gate_level_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/utils/name_utils.hpp>

#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

using namespace fiction;

TEST_CASE("Read empty FCN layout", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <clocking>2DDWave</clocking>\n"
                                              "  </layout>\n"
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(get_name(lyt) == "Test");
        CHECK(lyt.is_clocking_scheme(clock_name::TWODDWAVE));
    };

    using gate_layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    check(read_gate_level_layout<gate_layout>(layout_stream));
}
