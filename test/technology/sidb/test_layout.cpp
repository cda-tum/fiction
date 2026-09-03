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

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;

TEST_CASE("Empty layout", "[layout]")
{
    const layout lyt{};

    CHECK(lyt.is_empty());
    CHECK(lyt.num_cells() == 0);
    CHECK(lyt.num_defects() == 0);
    CHECK(lyt.num_pis() == 0);
    CHECK(lyt.num_pos() == 0);
    CHECK(lyt.get_lattice() == lattice::si_100_2x1());
    CHECK(lyt.get_layout_name().empty());
    CHECK(lyt.is_empty_cell({0, 0, 0}));
    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::EMPTY);
    CHECK(lyt.get_defect({0, 0, 0}).type == defect_type::NONE);
    CHECK(!lyt.index_of({0, 0, 0}).has_value());
    CHECK(lyt.bounding_box() == std::pair{lattice_site{}, lattice_site{}});

    const layout named{lattice::si_111_1x1(), "named"};
    CHECK(named.get_lattice() == lattice::si_111_1x1());
    CHECK(named.get_layout_name() == "named");
}

TEST_CASE("Cells", "[layout]")
{
    layout lyt{};

    lyt.assign_cell_type({3, 1, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("lookup")
    {
        CHECK(!lyt.is_empty());
        CHECK(lyt.num_cells() == 4);
        CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
        CHECK(lyt.get_cell_type({1, 0, 0}) == sidb_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({2, 0, 1}) == sidb_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({3, 1, 0}) == sidb_technology::cell_type::OUTPUT);
        CHECK(lyt.get_cell_type({3, 1, 1}) == sidb_technology::cell_type::EMPTY);
        CHECK(lyt.is_empty_cell({3, 1, 1}));
        CHECK(!lyt.is_empty_cell({3, 1, 0}));
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
        lyt.foreach_cell(
            [&](const auto& s, const auto i)
            {
                visited.push_back(s);
                indices.push_back(i);
            });
        CHECK(visited == expected);
        CHECK(indices == std::vector<std::size_t>{0, 1, 2, 3});

        std::size_t seen = 0;
        lyt.foreach_cell(
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
        CHECK(lyt.cells_of_type(sidb_technology::cell_type::NORMAL) == std::vector<lattice_site>{{1, 0, 0}, {2, 0, 1}});
        CHECK(lyt.num_cells_of_type(sidb_technology::cell_type::NORMAL) == 2);

        std::vector<lattice_site> pis{};
        lyt.foreach_pi([&pis](const auto& s) { pis.push_back(s); });
        CHECK(pis == std::vector<lattice_site>{{0, 0, 0}});

        std::vector<lattice_site> pos{};
        lyt.foreach_po([&pos](const auto& s) { pos.push_back(s); });
        CHECK(pos == std::vector<lattice_site>{{3, 1, 0}});
    }
    SECTION("overwrite and erase")
    {
        lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::LOGIC);
        CHECK(lyt.num_cells() == 4);
        CHECK(lyt.get_cell_type({1, 0, 0}) == sidb_technology::cell_type::LOGIC);

        lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::EMPTY);
        CHECK(lyt.num_cells() == 3);
        CHECK(lyt.is_empty_cell({1, 0, 0}));
        CHECK(lyt.sidbs() == std::vector<lattice_site>{{0, 0, 0}, {2, 0, 1}, {3, 1, 0}});
        CHECK(lyt.index_of({2, 0, 1}) == 1);

        lyt.assign_cell_type({7, 7, 1}, sidb_technology::cell_type::EMPTY);
        CHECK(lyt.num_cells() == 3);
    }
    SECTION("value semantics")
    {
        auto copy = lyt;
        copy.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::EMPTY);

        CHECK(lyt.num_cells() == 4);
        CHECK(copy.num_cells() == 3);
        CHECK(lyt != copy);

        copy.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        CHECK(lyt == copy);
        CHECK(std::hash<layout>{}(lyt) == std::hash<layout>{}(copy));

        copy.set_layout_name("other");
        CHECK(lyt != copy);
    }
    SECTION("bounding box")
    {
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{3, 1, 0}});

        lyt.assign_cell_type({-2, -1, 1}, sidb_technology::cell_type::NORMAL);
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

        std::vector<lattice_site> visited{};
        lyt.foreach_defect([&visited](const auto& sd) { visited.push_back(sd.first); });
        CHECK(visited == std::vector<lattice_site>{{0, 0, 0}, {1, 0, 1}, {5, 2, 0}});
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

        const auto narrow = lyt.affected_sidbs({5, 2, 0}, std::pair<uint16_t, uint16_t>{1, 1});
        CHECK(narrow.size() == 9);

        const auto all =
            lyt.all_affected_sidbs(std::pair<uint16_t, uint16_t>{1, 1}, std::pair<uint16_t, uint16_t>{0, 0});
        CHECK(all.size() == 9 + 1 + 9);
    }
    SECTION("bounding box covers defects")
    {
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{5, 2, 0}});

        lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
        CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{7, 2, 0}});
    }
}
