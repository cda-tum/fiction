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
 * @brief Tests for `fiction/layouts/bounding_box.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <cstdint>
#include <limits>

using namespace fiction;
using namespace fiction::layouts;

/**
 * @brief Checks whether a layout has occupied bounds.
 * @tparam Lyt Layout type to test.
 */
template <typename Lyt>
concept bounded_layout = requires { typename bounding_box<Lyt>; };

static_assert(bounded_layout<cartesian_gate_layout>);
static_assert(bounded_layout<qca::layout>);
static_assert(!bounded_layout<cartesian_layout>);

TEMPLATE_TEST_CASE("Occupied bounds include outside objects and clear after removal", "[bounding-box]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    auto         lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {1000000000, 1000000000});
    bounding_box bounds{lyt};
    CHECK_FALSE(bounds.get_min().has_value());
    CHECK(bounds.get_x_size() == 0);
    const auto a = lyt.create_pi("a", {-5, 12});
    bounds.update_bounding_box();
    CHECK(bounds.get_min() == layout_base::coordinate{-5, 12});
    CHECK(bounds.get_max() == layout_base::coordinate{-5, 12});
    CHECK(bounds.get_x_size() == 1);
    CHECK(bounds.get_y_size() == 1);
    const auto b = lyt.create_pi("b", {9, -3});
    bounds.update_bounding_box();
    CHECK(bounds.get_min() == layout_base::coordinate{-5, -3});
    CHECK(bounds.get_max() == layout_base::coordinate{9, 12});
    CHECK(bounds.get_x_size() == 15);
    CHECK(bounds.get_y_size() == 16);
    lyt.remove(a);
    lyt.remove(b);
    bounds.update_bounding_box();
    CHECK_FALSE(bounds.get_max().has_value());
    CHECK(bounds.get_x_size() == 0);
    CHECK(bounds.get_y_size() == 0);
}

TEST_CASE("Occupied cell bounds span signed coordinates and ignore elevation", "[bounding-box]")
{
    qca::layout lyt{};
    lyt.assign_cell_type({std::numeric_limits<int32_t>::min(), 3, 1}, qca::cell_type::INPUT);
    lyt.assign_cell_type({std::numeric_limits<int32_t>::max(), -7}, qca::cell_type::OUTPUT);
    bounding_box bounds{lyt};
    CHECK(bounds.get_min() == layout_base::coordinate{std::numeric_limits<int32_t>::min(), -7});
    CHECK(bounds.get_max() == layout_base::coordinate{std::numeric_limits<int32_t>::max(), 3});
    CHECK(bounds.get_x_size() == 4294967296ull);
    CHECK(bounds.get_y_size() == 11);
}
