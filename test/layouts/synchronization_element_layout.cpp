//
// Created by marcel on 30.08.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

using namespace fiction;

TEST_CASE("Synchronization element layout traits", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    CHECK(has_synchronization_elements_v<se_layout>);
}

TEST_CASE("Deep copy synchronization element layout", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout original{{5, 5, 0}, twoddwave_clocking<se_layout>()};
    original.assign_synchronization_element({0, 0}, 1);
    original.assign_synchronization_element({1, 0}, 2);

    auto copy = original.clone();

    copy.resize(aspect_ratio_t<se_layout>{10, 10, 1});
    copy.replace_clocking_scheme(use_clocking<se_layout>());
    copy.assign_synchronization_element({0, 0}, 2);
    copy.assign_synchronization_element({1, 0}, 3);

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);
    CHECK(original.is_clocking_scheme(clock_name::TWODDWAVE));
    CHECK(original.get_synchronization_element({0, 0}) == 1);
    CHECK(original.get_synchronization_element({1, 0}) == 2);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
    CHECK(copy.is_clocking_scheme(clock_name::USE));
    CHECK(copy.get_synchronization_element({0, 0}) == 2);
    CHECK(copy.get_synchronization_element({1, 0}) == 3);
}

TEST_CASE("Shifted clocking with synchronization elements", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    layout.assign_synchronization_element({1, 1}, 1);

    CHECK(layout.is_clocking_scheme(clock_name::TWODDWAVE));
    CHECK(layout.is_regularly_clocked());
    CHECK(layout.num_clocks() == 4);

    CHECK(layout.get_clock_number({0, 0}) == 0);
    CHECK(layout.get_clock_number({1, 0}) == 1);
    CHECK(layout.get_clock_number({0, 1}) == 1);
    CHECK(layout.get_clock_number({1, 1}) == 2);
    CHECK(layout.get_clock_number({2, 1}) == 3);
    CHECK(layout.get_clock_number({1, 2}) == 3);
    CHECK(layout.get_clock_number({2, 2}) == 0);

    CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {2, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 2}));

    CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
    CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({2, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 2}, {1, 1}));
}

TEST_CASE("Iteration over synchronization elements", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    layout.assign_synchronization_element({0, 1}, 1);
    layout.assign_synchronization_element({1, 0}, 1);
    layout.assign_synchronization_element({1, 2}, 1);
    layout.assign_synchronization_element({2, 1}, 1);

    CHECK(layout.incoming_clocked_zones({0, 0}).size() == 2);
    CHECK(layout.outgoing_clocked_zones({2, 2}).size() == 2);

    const auto v1 = layout.incoming_clocked_zones({1, 1});
    const auto s1 = std::set<se_layout::coordinate>{v1.cbegin(), v1.cend()};
    const auto s2 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s1 == s2);

    const auto v3 = layout.outgoing_clocked_zones({1, 1});
    const auto s3 = std::set<se_layout::coordinate>{v3.cbegin(), v3.cend()};
    const auto s4 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s3 == s4);
}

TEST_CASE("Synchronization element layout properties", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 0}, 0);
    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 1}, 1);
    CHECK(layout.num_se() == 1);
    layout.assign_synchronization_element({1, 0}, 1);
    CHECK(layout.num_se() == 2);
    layout.assign_synchronization_element({1, 2}, 2);
    CHECK(layout.num_se() == 3);
    layout.assign_synchronization_element({2, 1}, 2);
    CHECK(layout.num_se() == 4);

    CHECK(layout.is_synchronization_element({0, 1}));
    CHECK(layout.is_synchronization_element({1, 0}));
    CHECK(layout.is_synchronization_element({1, 2}));
    CHECK(layout.is_synchronization_element({2, 1}));

    CHECK(!layout.is_synchronization_element({0, 0}));
    CHECK(!layout.is_synchronization_element({1, 1}));
    CHECK(!layout.is_synchronization_element({2, 0}));
    CHECK(!layout.is_synchronization_element({0, 2}));
    CHECK(!layout.is_synchronization_element({2, 2}));

    CHECK(layout.get_synchronization_element({0, 1}) == 1);
    CHECK(layout.get_synchronization_element({1, 0}) == 1);
    CHECK(layout.get_synchronization_element({1, 2}) == 2);
    CHECK(layout.get_synchronization_element({2, 1}) == 2);

    CHECK(layout.get_synchronization_element({0, 0}) == 0);
    CHECK(layout.get_synchronization_element({1, 1}) == 0);
    CHECK(layout.get_synchronization_element({2, 0}) == 0);
    CHECK(layout.get_synchronization_element({0, 2}) == 0);
    CHECK(layout.get_synchronization_element({2, 2}) == 0);

    CHECK(layout.in_degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.in_degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 1}) == static_cast<se_layout::degree_t>(4));

    CHECK(layout.out_degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.out_degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.out_degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.out_degree({2, 2}) == static_cast<se_layout::degree_t>(2));

    CHECK(layout.degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 2}) == static_cast<se_layout::degree_t>(2));
}
