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
 * @brief Tests for `fiction/technology/sidb/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <new>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::sidb;
using namespace fiction::sidb::model;

namespace
{
/**
 * Exercises every allocation failure in a layout update and checks that each failure preserves the layout.
 *
 * @param original Layout before the update.
 * @param update Mutation to exercise.
 * @return Layout after a successful update.
 */
layout check_allocation_failures(const layout& original, const std::function<void(layout&)>& update)
{
    require_allocation_failure_support();
    for (std::size_t failure = 0;; ++failure)
    {
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        auto candidate = original;
        try
        {
            allocation_budget = failure;
            update(candidate);
            allocation_budget.reset();
            return candidate;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
            CHECK(candidate == original);
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
    }
}
}  // namespace

TEST_CASE("Empty layout", "[layout]")
{
    const layout lyt{};

    CHECK(lyt.is_empty());
    CHECK(lyt.num_dots() == 0);
    CHECK(lyt.num_defects() == 0);
    CHECK(lyt.num_pis() == 0);
    CHECK(lyt.num_pos() == 0);
    CHECK(lyt.get_lattice() == lattice::si_100_2x1());
    CHECK(lyt.get_layout_name().empty());
    CHECK(lyt.is_empty_site({0, 0, 0}));
    CHECK(lyt.get_dot_tag({0, 0, 0}) == dot_tag::EMPTY);
    CHECK(lyt.get_defect({0, 0, 0}).type == defect_type::NONE);
    CHECK(!lyt.index_of({0, 0, 0}).has_value());
    CHECK(lyt.bounding_box() == std::pair{lattice_site{}, lattice_site{}});

    const layout named{lattice::si_111_1x1(), "named"};
    CHECK(named.get_lattice() == lattice::si_111_1x1());
    CHECK(named.get_layout_name() == "named");
}

TEST_CASE("Dots", "[layout]")
{
    layout lyt{};

    lyt.assign_sidb({3, 1, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 0, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({1, 0, 0});

    SECTION("lookup")
    {
        CHECK(!lyt.is_empty());
        CHECK(lyt.num_dots() == 4);
        CHECK(lyt.get_dot_tag({0, 0, 0}) == dot_tag::INPUT);
        CHECK(lyt.get_dot_tag({1, 0, 0}) == dot_tag::NORMAL);
        CHECK(lyt.get_dot_tag({2, 0, 1}) == dot_tag::NORMAL);
        CHECK(lyt.get_dot_tag({3, 1, 0}) == dot_tag::OUTPUT);
        CHECK(lyt.get_dot_tag({3, 1, 1}) == dot_tag::EMPTY);
        CHECK(lyt.is_empty_site({3, 1, 1}));
        CHECK(!lyt.is_empty_site({3, 1, 0}));
    }
    SECTION("canonical order and indices")
    {
        const std::vector<lattice_site> expected{{0, 0, 0}, {1, 0, 0}, {2, 0, 1}, {3, 1, 0}};
        CHECK(lyt.sidbs() == expected);

        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            CHECK(lyt.index_of(expected[i]) == i);
        }
        CHECK(!lyt.index_of({9, 9, 0}).has_value());

        std::vector<lattice_site> visited{};
        std::vector<std::size_t>  indices{};
        lyt.foreach_dot(
            [&](const auto& s, const auto i)
            {
                visited.push_back(s);
                indices.push_back(i);
            });
        CHECK(visited == expected);
        CHECK(indices == std::vector<std::size_t>{0, 1, 2, 3});

        std::size_t seen = 0;
        lyt.foreach_dot(
            [&seen](const auto&)
            {
                ++seen;
                return seen < 2;
            });
        CHECK(seen == 2);
    }
    SECTION("inputs and outputs")
    {
        CHECK(lyt.num_pis() == 1);
        CHECK(lyt.num_pos() == 1);
        CHECK(lyt.is_pi({0, 0, 0}));
        CHECK(!lyt.is_pi({1, 0, 0}));
        CHECK(lyt.is_po({3, 1, 0}));
        CHECK(lyt.dots_with_tag(dot_tag::NORMAL) == std::vector<lattice_site>{{1, 0, 0}, {2, 0, 1}});
        CHECK(lyt.num_dots_with_tag(dot_tag::NORMAL) == 2);

        std::vector<lattice_site> pis{};
        lyt.foreach_pi([&pis](const auto& s) { pis.push_back(s); });
        CHECK(pis == std::vector<lattice_site>{{0, 0, 0}});

        std::vector<lattice_site> pos{};
        lyt.foreach_po([&pos](const auto& s) { pos.push_back(s); });
        CHECK(pos == std::vector<lattice_site>{{3, 1, 0}});
    }
    SECTION("default tag replaces an existing tag")
    {
        lyt.assign_sidb({0, 0, 0});
        CHECK(lyt.num_dots() == 4);
        CHECK(lyt.get_dot_tag({0, 0, 0}) == dot_tag::NORMAL);
        CHECK(lyt.num_pis() == 0);
    }
    SECTION("overwrite and erase")
    {
        lyt.assign_sidb({1, 0, 0}, dot_tag::LOGIC);
        CHECK(lyt.num_dots() == 4);
        CHECK(lyt.get_dot_tag({1, 0, 0}) == dot_tag::LOGIC);

        lyt.assign_sidb({1, 0, 0}, dot_tag::EMPTY);
        CHECK(lyt.num_dots() == 3);
        CHECK(lyt.is_empty_site({1, 0, 0}));
        CHECK(lyt.sidbs() == std::vector<lattice_site>{{0, 0, 0}, {2, 0, 1}, {3, 1, 0}});
        CHECK(lyt.index_of({2, 0, 1}) == 1);

        lyt.assign_sidb({7, 7, 1}, dot_tag::EMPTY);
        CHECK(lyt.num_dots() == 3);
    }
    SECTION("value semantics")
    {
        auto copy = lyt;
        copy.assign_sidb({0, 0, 0}, dot_tag::EMPTY);

        CHECK(lyt.num_dots() == 4);
        CHECK(copy.num_dots() == 3);
        CHECK(lyt != copy);

        copy.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        CHECK(lyt == copy);
        CHECK(std::hash<layout>{}(lyt) == std::hash<layout>{}(copy));

        copy.set_layout_name("other");
        CHECK(lyt != copy);
    }
    SECTION("bounding box")
    {
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{3, 1, 0}});

        lyt.assign_sidb({-2, -1, 1}, dot_tag::NORMAL);
        CHECK(lyt.bounding_box() == std::pair{lattice_site{-2, -1, 1}, lattice_site{3, 1, 0}});
    }
}

TEST_CASE("Defects", "[layout]")
{
    layout lyt{};

    const defect charged{defect_type::SI_VACANCY, -1, 5.6, 5.0};
    const defect neutral{defect_type::SILOXANE, 0};
    const defect positive{defect_type::UNKNOWN, 1};

    lyt.assign_defect({5, 2, 0}, charged);
    lyt.assign_defect({1, 0, 1}, neutral);
    lyt.assign_defect({0, 0, 0}, positive);

    SECTION("lookup and counts")
    {
        CHECK(lyt.is_empty());
        CHECK(lyt.num_defects() == 3);
        CHECK(lyt.get_defect({5, 2, 0}) == charged);
        CHECK(lyt.get_defect({1, 0, 1}) == neutral);
        CHECK(lyt.get_defect({9, 9, 0}).type == defect_type::NONE);
        CHECK(lyt.num_charged_defects() == 2);
        CHECK(lyt.num_negatively_charged_defects() == 1);
        CHECK(lyt.num_positively_charged_defects() == 1);
        CHECK(lyt.num_neutral_defects() == 1);

        /** @brief Complete ordered defects observed by the callback. */
        std::vector<std::pair<lattice_site, defect>> visited{};
        /** @brief The canonical defects, independent of either callback overload. */
        const std::vector<std::pair<lattice_site, defect>> expected{{{0, 0, 0}, positive},
                                                                    {{1, 0, 1}, neutral},
                                                                    {{5, 2, 0}, charged}};
        lyt.foreach_defect([&visited](const auto& sd) { visited.push_back(sd); });
        CHECK(visited == expected);
        visited.clear();

        /** @brief Indices reported for the ordered defect visits. */
        std::vector<std::size_t> indices{};
        lyt.foreach_defect(
            [&](const auto& sd, const auto i)
            {
                visited.push_back(sd);
                indices.push_back(i);
            });
        CHECK(visited == expected);
        CHECK(indices == std::vector<std::size_t>{0, 1, 2});
    }
    SECTION("erase and move")
    {
        lyt.assign_defect({1, 0, 1}, defect{defect_type::NONE});
        CHECK(lyt.num_defects() == 2);
        CHECK(lyt.get_defect({1, 0, 1}).type == defect_type::NONE);

        lyt.move_defect({5, 2, 0}, {6, 2, 0});
        CHECK(lyt.num_defects() == 2);
        CHECK(lyt.get_defect({5, 2, 0}).type == defect_type::NONE);
        CHECK(lyt.get_defect({6, 2, 0}) == charged);
    }
    SECTION("charged defect types with zero charge also count as neutral")
    {
        lyt.assign_defect({5, 2, 0}, defect{defect_type::SI_VACANCY});
        CHECK(lyt.get_defect({5, 2, 0}).charge == 0);
        CHECK(lyt.num_charged_defects() == 2);
        CHECK(lyt.num_neutral_defects() == 2);
        CHECK(lyt.num_negatively_charged_defects() == 0);
        CHECK(lyt.num_positively_charged_defects() == 1);
    }
    SECTION("move using sites borrowed from the layout")
    {
        lyt.move_defect(lyt.defects()[0].first, lyt.defects()[1].first);
        CHECK(lyt.num_defects() == 2);
        CHECK(lyt.get_defect({0, 0, 0}).type == defect_type::NONE);
        CHECK(lyt.get_defect({1, 0, 1}) == positive);
        CHECK(lyt.get_defect({5, 2, 0}) == charged);
    }
    SECTION("moves from empty sites and to the same site preserve defects")
    {
        const auto original = lyt;
        lyt.move_defect({9, 9, 0}, {5, 2, 0});
        CHECK(lyt == original);
        lyt.move_defect({5, 2, 0}, {5, 2, 0});
        CHECK(lyt == original);
    }
    SECTION("affected SiDBs")
    {
        CHECK(lyt.affected_sidbs({9, 9, 0}).empty());

        const auto around_neutral = lyt.affected_sidbs({1, 0, 1});
        CHECK(around_neutral.size() == 3);
        CHECK(around_neutral.contains({0, 0, 1}));
        CHECK(around_neutral.contains({1, 0, 1}));
        CHECK(around_neutral.contains({2, 0, 1}));

        const auto around_charged = lyt.affected_sidbs({5, 2, 0});
        CHECK(around_charged.size() == (std::size_t{2} * 26 + 1) * (std::size_t{2} * 13 + 1));
        CHECK(around_charged.contains({5 - 26, 2 - 13, 0}));
        CHECK(around_charged.contains({5 + 26, 2 + 13, 0}));
        CHECK(!around_charged.contains({5, 2, 1}));
        CHECK(std::ranges::all_of(around_charged, [](const auto& s)
                                  { return s.z == 0 && s.x >= -21 && s.x <= 31 && s.y >= -11 && s.y <= 15; }));

        /** @brief The complete one-unit-cell rectangle retains the defect's basis index. */
        const std::unordered_set<lattice_site> expected_narrow{{4, 1, 0}, {5, 1, 0}, {6, 1, 0}, {4, 2, 0}, {5, 2, 0},
                                                               {6, 2, 0}, {4, 3, 0}, {5, 3, 0}, {6, 3, 0}};
        CHECK(lyt.affected_sidbs({5, 2, 0}, std::pair{uint16_t{1}, uint16_t{1}}) == expected_narrow);

        /** @brief Union of both charged rectangles and the neutral defect's site. */
        auto expected_all = expected_narrow;
        expected_all.insert({{-1, -1, 0},
                             {0, -1, 0},
                             {1, -1, 0},
                             {-1, 0, 0},
                             {0, 0, 0},
                             {1, 0, 0},
                             {-1, 1, 0},
                             {0, 1, 0},
                             {1, 1, 0},
                             {1, 0, 1}});
        const auto all =
            lyt.all_affected_sidbs(std::pair{uint16_t{1}, uint16_t{1}}, std::pair{uint16_t{0}, uint16_t{0}});
        CHECK(all == expected_all);
    }
    SECTION("bounding box covers defects")
    {
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{5, 2, 0}});

        lyt.assign_sidb({7, 0, 0}, dot_tag::NORMAL);
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{7, 2, 0}});
    }
    SECTION("coordinate limits")
    {
        constexpr auto     min_coordinate = std::numeric_limits<int32_t>::min();
        constexpr auto     max_coordinate = std::numeric_limits<int32_t>::max();
        const lattice_site first{min_coordinate, min_coordinate, 0};
        const lattice_site last{max_coordinate, max_coordinate, 1};
        lyt.assign_defect(first, charged);
        lyt.assign_defect(last, charged);
        CHECK(lyt.bounding_box() == std::pair{first, last});

        const auto lower = lyt.affected_sidbs(first, std::pair{uint16_t{1}, uint16_t{1}});
        CHECK(lower == std::unordered_set<lattice_site>{first,
                                                        {min_coordinate + 1, min_coordinate, 0},
                                                        {min_coordinate, min_coordinate + 1, 0},
                                                        {min_coordinate + 1, min_coordinate + 1, 0}});
        const auto upper = lyt.affected_sidbs(last, std::pair{uint16_t{1}, uint16_t{1}});
        CHECK(upper == std::unordered_set<lattice_site>{last,
                                                        {max_coordinate - 1, max_coordinate, 1},
                                                        {max_coordinate, max_coordinate - 1, 1},
                                                        {max_coordinate - 1, max_coordinate - 1, 1}});
    }
}

TEST_CASE("Layout updates preserve data when allocation fails", "[layout]")
{
    SECTION("copy assignment")
    {
        /** @brief Source with allocated name, dots, and defects. */
        layout source{lattice::si_111_1x1(), "source layout with an allocated name"};
        source.assign_sidb({0, 0, 0}, dot_tag::INPUT);
        source.assign_sidb({1, 0, 0}, dot_tag::OUTPUT);
        source.assign_sidb({2, 0, 1});
        source.assign_defect({3, 0, 0}, defect{defect_type::SI_VACANCY});

        /** @brief Populated destination retained when allocation fails. */
        layout original{};
        original.assign_sidb({5, 0, 0}, dot_tag::LOGIC);
        /** @brief Destination after successful copy assignment. */
        const auto result = check_allocation_failures(
            original,
            [&source](/** @brief Destination updated under an allocation budget. */ layout& lyt) { lyt = source; });
        CHECK(result == source);
        CHECK(result.get_dot_tag({0, 0, 0}) == dot_tag::INPUT);
        CHECK(result.get_dot_tag({1, 0, 0}) == dot_tag::OUTPUT);
        CHECK(result.get_dot_tag({2, 0, 1}) == dot_tag::NORMAL);
    }
    SECTION("SiDB insertion")
    {
        const auto result = check_allocation_failures(layout{}, [](layout& lyt) { lyt.assign_sidb({0, 0, 0}); });
        CHECK(result.num_dots() == 1);
        CHECK(result.get_dot_tag({0, 0, 0}) == dot_tag::NORMAL);
    }
    SECTION("defect movement")
    {
        layout       original{};
        const defect vacancy{defect_type::SI_VACANCY, -1, 5.6, 5.0};
        original.assign_defect({0, 0, 0}, vacancy);
        const auto result =
            check_allocation_failures(original, [](layout& lyt) { lyt.move_defect({0, 0, 0}, {1, 0, 0}); });
        CHECK(result.num_defects() == 1);
        CHECK(result.get_defect({0, 0, 0}).type == defect_type::NONE);
        CHECK(result.get_defect({1, 0, 0}) == vacancy);
    }
}

TEST_CASE("Layout assignment preserves value semantics", "[layout]")
{
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<layout>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<layout>);

    /** @brief Reference layout for copy and move checks. */
    layout original{lattice::si_111_1x1(), "named layout"};
    original.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    original.assign_defect({1, 0, 0}, defect{defect_type::SI_VACANCY});
    /** @brief Independent copy used for self-assignment. */
    auto copy = original;
    /** @brief Alias that exercises self-assignment. */
    const auto& same = copy;
    copy             = same;
    CHECK(copy == original);

    /** @brief Destination whose storage remains independent of the source. */
    layout assigned{};
    CHECK(&(assigned = copy) == &assigned);
    CHECK(assigned == original);
    copy.assign_sidb({0, 0, 0}, dot_tag::OUTPUT);
    CHECK(assigned.get_dot_tag({0, 0, 0}) == dot_tag::INPUT);

    /** @brief Destination that receives the assigned layout's storage. */
    auto moved = std::move(assigned);
    CHECK(moved == original);
    assigned = std::move(moved);
    CHECK(assigned == original);
}
