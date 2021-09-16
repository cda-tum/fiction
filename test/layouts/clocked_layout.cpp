//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

using namespace fiction;

TEST_CASE("Traits", "[clocked]")
{
    using layout = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    CHECK(has_is_incoming_clocked_v<layout>);
    CHECK(has_is_outgoing_clocked_v<layout>);
}

TEST_CASE("Clocking", "[clocked]")
{
    using coordinate_layout = cartesian_layout<cartesian::ucoord_t>;

    clocked_layout<coordinate_layout> layout{coordinate_layout::aspect_ratio{1, 1, 0}, twoddwave_4_clocking};

    SECTION("2DDWave Clocking")
    {
        CHECK(layout.is_clocking_scheme(clock_name::twoddwave4));
        CHECK(!layout.is_clocking_scheme(clock_name::twoddwave3));
        CHECK(!layout.is_clocking_scheme(clock_name::res));
        CHECK(layout.is_regularly_clocked());
        CHECK(layout.num_clocks() == 4);

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 1);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);

        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(!layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));

        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
    }

    SECTION("Replace with USE")
    {
        layout.replace_clocking_scheme(use_4_clocking);

        CHECK(!layout.is_clocking_scheme(clock_name::twoddwave4));
        CHECK(layout.is_clocking_scheme(clock_name::use));
        CHECK(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 3);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);
    }
}

TEST_CASE("Iteration", "[clocked]")
{
    using coordinate_layout = cartesian_layout<cartesian::ucoord_t>;

    clocked_layout<coordinate_layout> layout{coordinate_layout::aspect_ratio{2, 2, 0}, twoddwave_4_clocking};

    CHECK(layout.incoming_clocked_zones<std::set<coordinate_layout::coordinate>>({0, 0}).empty());
    CHECK(layout.outgoing_clocked_zones<std::set<coordinate_layout::coordinate>>({2, 2}).empty());

    auto s1 = layout.incoming_clocked_zones<std::set<coordinate_layout::coordinate>>({1, 1});
    auto s2 = std::set<coordinate_layout::coordinate>{{{1, 0}, {0, 1}}};

    CHECK(s1 == s2);

    auto s3 = layout.outgoing_clocked_zones<std::set<coordinate_layout::coordinate>>({1, 1});
    auto s4 = std::set<coordinate_layout::coordinate>{{{1, 2}, {2, 1}}};

    CHECK(s3 == s4);
}

TEST_CASE("Structural properties", "[clocked]")
{
    using coordinate_layout = cartesian_layout<cartesian::ucoord_t>;

    SECTION("2DDWave Clocking")
    {
        clocked_layout<coordinate_layout> layout{coordinate_layout::aspect_ratio{2, 2, 0}, twoddwave_4_clocking};

        CHECK(layout.in_degree({0, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(0));
        CHECK(layout.in_degree({1, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.in_degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.out_degree({1, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(0));

        CHECK(layout.degree({0, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
    }
    SECTION("USE Clocking")
    {
        clocked_layout<coordinate_layout> layout{coordinate_layout::aspect_ratio{2, 2, 0}, use_4_clocking};

        CHECK(layout.in_degree({0, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.in_degree({1, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.in_degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.out_degree({1, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(1));

        CHECK(layout.degree({0, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clocked_layout<coordinate_layout>::degree_t>(2));
    }
}
