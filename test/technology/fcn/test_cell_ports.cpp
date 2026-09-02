/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 03.07.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>

#include <unordered_set>

using namespace fiction;

TEST_CASE("Port positions", "[cell-ports]")
{
    fcn::port_position p_00{0, 0}, p_10{1, 0}, p_01{0, 1};

    CHECK(p_00 == fcn::port_position{});
    CHECK(p_10 == fcn::port_position{1, 0});
    CHECK(p_01 == fcn::port_position{0, 1});
    CHECK(!(p_00 == p_01));
    CHECK(!(p_00 == p_10));
    CHECK(!(p_10 == p_01));
    CHECK(p_00 < p_10);
    CHECK(p_00 < p_01);
    CHECK(p_01 < p_10);

    std::unordered_set<fcn::port_position> port_set{};

    port_set.insert(p_00);
    port_set.insert(p_10);
    port_set.insert(p_01);

    CHECK(port_set.size() == 3);

    port_set.insert(p_00);
    port_set.insert(p_10);
    port_set.insert(p_01);

    CHECK(port_set.size() == 3);
}

TEST_CASE("Port directions", "[cell-ports]")
{
    fcn::port_direction p_n{fcn::port_direction::cardinal::NORTH}, p_e{fcn::port_direction::cardinal::EAST},
        p_se{fcn::port_direction::cardinal::SOUTH_EAST};

    CHECK(p_n == fcn::port_direction{});
    CHECK(p_e == fcn::port_direction{fcn::port_direction::cardinal::EAST});
    CHECK(p_se == fcn::port_direction{fcn::port_direction::cardinal::SOUTH_EAST});
    CHECK(!(p_n == p_e));
    CHECK(!(p_n == p_se));
    CHECK(!(p_se == p_e));
    CHECK(p_n < p_se);
    CHECK(p_n < p_e);
    CHECK(p_e < p_se);

    std::unordered_set<fcn::port_direction> port_set{};

    port_set.insert(p_n);
    port_set.insert(p_se);
    port_set.insert(p_e);

    CHECK(port_set.size() == 3);

    port_set.insert(p_n);
    port_set.insert(p_se);
    port_set.insert(p_e);

    CHECK(port_set.size() == 3);
}

TEST_CASE("Port lists", "[cell-ports]")
{
    SECTION("Port positions")
    {
        fcn::port_position p_02{0, 2}, p_24{2, 4};

        fcn::port_list<fcn::port_position> p_list_02_24{{p_02}, {p_24}};
        fcn::port_list<fcn::port_position> p_list_24_02{{p_24}, {p_02}};

        CHECK(!(p_list_02_24 == p_list_24_02));
        CHECK(!(fcn::port_list<fcn::port_position>{} == p_list_24_02));
        CHECK(!(p_list_02_24 == fcn::port_list<fcn::port_position>{}));

        CHECK(p_list_02_24 == fcn::port_list<fcn::port_position>{{p_02}, {p_24}});
        CHECK(p_list_24_02 == fcn::port_list<fcn::port_position>{{p_24}, {p_02}});
    }
    SECTION("Port directions")
    {
        fcn::port_direction p_sw{fcn::port_direction::cardinal::SOUTH_WEST}, p_w{fcn::port_direction::cardinal::WEST};

        fcn::port_list<fcn::port_direction> p_list_sw_w{{p_sw}, {p_w}};
        fcn::port_list<fcn::port_direction> p_list_w_sw{{p_w}, {p_sw}};

        CHECK(!(p_list_sw_w == p_list_w_sw));
        CHECK(!(fcn::port_list<fcn::port_direction>{} == p_list_w_sw));
        CHECK(!(p_list_sw_w == fcn::port_list<fcn::port_direction>{}));

        CHECK(p_list_sw_w == fcn::port_list<fcn::port_direction>{{p_sw}, {p_w}});
        CHECK(p_list_w_sw == fcn::port_list<fcn::port_direction>{{p_w}, {p_sw}});
    }
}
