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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/layouts/cell_grid.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <cstdint>
#include <vector>

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

/**
 * @brief Checks whether a cell type can form a grid.
 * @tparam CellType Cell type to test.
 */
template <typename CellType>
concept grid_cell_type = requires { typename cell_grid<CellType>; };

static_assert(grid_cell_type<test_cell>);
static_assert(!grid_cell_type<int>);

}  // namespace

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

    CHECK_FALSE(g.is_empty());
    CHECK(g.num_cells() == 4);
    CHECK(g.num_pis() == 2);
    CHECK(g.num_pos() == 1);
    CHECK(g.is_pi({0, 2}));
    CHECK(g.is_po({4, 2}));
    CHECK_FALSE(g.is_pi({2, 2}));
    CHECK(g.get_cell_type({2, 2}) == test_cell::NORMAL);
    CHECK(g.get_cell_type({1, 1}) == test_cell::EMPTY);
    CHECK(g.is_empty_cell({1, 1}));
    CHECK_FALSE(g.is_empty_cell({2, 2}));
    CHECK(g.get_cell_name({2, 4}) == "b");
    CHECK(g.get_cell_name({2, 2}).empty());

    std::vector<layout_base::coordinate> pis{};
    g.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
    CHECK_THAT(pis, Catch::Matchers::UnorderedRangeEquals(std::vector<layout_base::coordinate>{{0, 2}, {2, 4}}));

    std::vector<layout_base::coordinate> pos{};
    g.foreach_po([&pos](const auto& c) { pos.push_back(c); });
    CHECK_THAT(pos, Catch::Matchers::UnorderedRangeEquals(std::vector<layout_base::coordinate>{{4, 2}}));

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

TEST_CASE("Cell traversal stops early and positions enumerate in order", "[cell-grid]")
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

    std::vector<layout_base::coordinate> positions{};
    g.foreach_cell_position([&positions](const auto& c) { positions.push_back(c); });
    CHECK_THAT(
        positions,
        Catch::Matchers::RangeEquals(
            std::vector<
                layout_base::coordinate>{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}, {0, 2}, {1, 2}, {2, 2}}));
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
    CHECK(original.width() == 5);
    CHECK(original.layers() == 1);
    CHECK(original.get_layout_name() == "original");
    CHECK(original.get_cell_type({0, 2}) == test_cell::NORMAL);
    CHECK(original.get_cell_name({0, 2}).empty());

    grid assigned{};
    assigned = copy;
    CHECK(assigned == copy);
    assigned.resize({1, 1});
    CHECK(copy.width() == 10);
}
