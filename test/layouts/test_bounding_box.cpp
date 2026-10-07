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

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Occupied bounds include outside objects and clear after removal", "[bounding-box]")
{
    gate_level_layout<cartesian_layout> lyt{{1000000000, 1000000000}};
    bounding_box_2d                     bounds{lyt};
    CHECK_FALSE(bounds.get_min().has_value());
    CHECK(bounds.get_x_size() == 0);
    const auto a = lyt.create_pi("a", {-5, 12});
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
