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
 * @brief Tests for `fiction/layouts/cell_grid.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cell_grid.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <set>

using namespace fiction;
using namespace fiction::layouts;

namespace
{

enum class test_cell : uint8_t
{
    EMPTY  = ' ',
    NORMAL = 'x',
    INPUT  = 'i',
    OUTPUT = 'o'
};

using grid = cell_grid<test_cell>;

}  // namespace

TEST_CASE("Cell grid traits", "[cell-grid]")
{
    CHECK(is_cartesian_layout_v<grid>);
    CHECK(has_get_layout_name_v<grid>);
    CHECK(has_set_layout_name_v<grid>);
}

TEST_CASE("Cell type and name assignment", "[cell-grid]")
{
    grid g{{4, 4}, "AND"};

    CHECK(g.is_empty());
    CHECK(g.get_layout_name() == "AND");

    g.assign_cell_type({0, 2}, test_cell::INPUT);
    g.assign_cell_type({2, 4}, test_cell::INPUT);
    g.assign_cell_type({2, 2}, test_cell::NORMAL);
    g.assign_cell_type({4, 2}, test_cell::OUTPUT);
    g.assign_cell_name({0, 2}, "a");
    g.assign_cell_name({2, 4}, "b");
    g.assign_cell_name({4, 2}, "f");

    CHECK(!g.is_empty());
    CHECK(g.num_cells() == 4);
    CHECK(g.num_pis() == 2);
    CHECK(g.num_pos() == 1);
    CHECK(g.is_pi({0, 2}));
    CHECK(g.is_po({4, 2}));
    CHECK(!g.is_pi({2, 2}));
    CHECK(g.get_cell_type({2, 2}) == test_cell::NORMAL);
    CHECK(g.get_cell_type({1, 1}) == test_cell::EMPTY);
    CHECK(g.is_empty_cell({1, 1}));
    CHECK(!g.is_empty_cell({2, 2}));
    CHECK(g.get_cell_name({2, 4}) == "b");
    CHECK(g.get_cell_name({2, 2}).empty());

    std::set<coords::offset> pis{};
    g.foreach_pi([&pis](const auto& c) { pis.insert(c); });
    CHECK(pis == std::set<coords::offset>{{0, 2}, {2, 4}});

    std::set<coords::offset> pos{};
    g.foreach_po([&pos](const auto& c) { pos.insert(c); });
    CHECK(pos == std::set<coords::offset>{{4, 2}});

    SECTION("empty type removes the cell and its name")
    {
        g.assign_cell_type({0, 2}, test_cell::EMPTY);

        CHECK(g.is_empty_cell({0, 2}));
        CHECK(g.get_cell_name({0, 2}).empty());
        CHECK(g.num_cells() == 3);
        CHECK(g.num_pis() == 1);
    }
    SECTION("empty name removes the name")
    {
        g.assign_cell_name({2, 4}, "");

        CHECK(g.get_cell_name({2, 4}).empty());
        CHECK(g.get_cell_type({2, 4}) == test_cell::INPUT);
    }
}

TEST_CASE("Cell traversal stops early", "[cell-grid]")
{
    grid g{{3, 3}};
    g.assign_cell_type({0, 0}, test_cell::NORMAL);
    g.assign_cell_type({1, 1}, test_cell::NORMAL);
    g.assign_cell_type({2, 2}, test_cell::NORMAL);

    auto visited = 0u;
    g.foreach_cell(
        [&visited](const auto&)
        {
            ++visited;
            return false;
        });
    CHECK(visited == 1);

    auto positions = 0u;
    g.foreach_cell_position([&positions](const auto&) { ++positions; });
    CHECK(positions == 16);
}

TEST_CASE("Cell grid copies are independent", "[cell-grid]")
{
    grid original{{5, 5}, "original"};
    original.assign_cell_type({0, 2}, test_cell::NORMAL);

    auto copy = original;
    CHECK(copy == original);

    copy.resize({10, 10, 1});
    copy.set_layout_name("copy");
    copy.assign_cell_type({0, 2}, test_cell::INPUT);
    copy.assign_cell_name({0, 2}, "a");

    CHECK(copy != original);
    CHECK(original.x() == 5);
    CHECK(original.z() == 0);
    CHECK(original.get_layout_name() == "original");
    CHECK(original.get_cell_type({0, 2}) == test_cell::NORMAL);
    CHECK(original.get_cell_name({0, 2}).empty());

    grid assigned{};
    assigned = copy;
    CHECK(assigned == copy);
    assigned.resize({1, 1});
    CHECK(copy.x() == 10);
}
