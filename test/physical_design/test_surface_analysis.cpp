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
 * @brief Tests for `fiction/physical_design/surface_analysis.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/surface_analysis.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>

#include <kitty/dynamic_truth_table.hpp>

#include <initializer_list>
#include <vector>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::synthesis;

/**
 * This is simply a test library, its gates do not represent real entities that would work when fabricated.
 */
class dummy_gate_library : public gate_library<sidb::layout, 3, 3>
{
  public:
    dummy_gate_library() = delete;

    /**
     * @brief Returns the test library's gate implementations.
     * @return Gate implementations grouped by Boolean function.
     */
    static gate_functions get_functional_implementations()
    {
        static const gate_functions implementations{{{create_id_tt(), {LINE}}, {create_and_tt(), {Y}}}};

        return implementations;
    }

    /**
     * @brief Returns the ports of the test library's gates.
     * @return Port lists grouped by gate implementation.
     */
    static gate_ports<port_position> get_gate_ports()
    {
        static const gate_ports<port_position> ports{
            {{LINE, {{{port_position(0, 1)}, {port_position(2, 1)}}}},
             {Y, {{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}}}}}};

        return ports;
    }

  private:
    static constexpr const gate LINE{cell_list_to_gate<char>({{
        {' ', 'x', ' '},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};

    static constexpr const gate Y{cell_list_to_gate<char>({{
        {'x', ' ', 'x'},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};
};

/** @brief SiDB gate library without implementation or port lookup. */
struct empty_gate_library : gate_library<sidb::layout, 3, 3>
{};

/** @brief Gate library with an incompatible implementation lookup result. */
struct wrong_implementations_library : dummy_gate_library
{
    /** @brief Returns an incompatible implementation result. */
    static int get_functional_implementations();
};

/** @brief Gate library with an incompatible port lookup result. */
struct wrong_ports_library : dummy_gate_library
{
    /** @brief Returns an incompatible port result. */
    static int get_gate_ports();
};

/**
 * @brief Checks whether a gate library can analyze a defective SiDB surface.
 * @tparam Library Gate library to test.
 */
template <typename Library>
concept surface_analyzable = requires(const cartesian_gate_layout& gates, const sidb::layout& surface) {
    surface_analysis<Library>(gates, surface);
};

static_assert(surface_analyzable<dummy_gate_library>);
static_assert(surface_analyzable<bestagon_library>);
static_assert(!surface_analyzable<empty_gate_library>);
static_assert(!surface_analyzable<wrong_implementations_library>);
static_assert(!surface_analyzable<wrong_ports_library>);

TEST_CASE("Dummy gate library simple defects", "[sidb-surface-analysis]")
{
    /** @brief Four-by-three grid of three-by-three-cell gate tiles. */
    static const cartesian_gate_layout gate_lyt{cartesian_gate_layout::extent{4, 3, 1}};
    static const layout                cell_lyt{};  // makes for 12 x 9 cells

    static const port_list<port_position> line_ports{{port_position(0, 1)}, {port_position(2, 1)}};
    static const port_list<port_position> y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};

    /** @brief Complete function and port lists for each checked dummy-library tile. */
    const surface_black_list<port_position>::mapped_type expected{{create_id_tt(), {line_ports}},
                                                                  {create_and_tt(), {y_ports}}};
    /** @brief Checks the original tile samples without discarding repeated port lists. */
    const auto check_tiles = [&](const auto& black_list, const std::initializer_list<layout_base::coordinate> tiles)
    {
        for (const auto& tile : tiles)
        {
            CAPTURE(tile);
            REQUIRE(black_list.contains(tile));
            CHECK(black_list.at(tile) == expected);
        }
    };

    layout defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_defect(site_at_row(6, 3), defect{defect_type::SI_VACANCY});

        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 12);

        check_tiles(black_list, {{1, 0}, {2, 0}, {1, 1}, {2, 1}, {3, 1}});
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect(site_at_row(1, 1), defect{defect_type::SILOXANE});

        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        check_tiles(black_list, {{0, 0}});
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_defect(site_at_row(6, 2), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(6, 3), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(3, 6), defect{defect_type::ETCH_PIT});
        defect_layout.assign_defect(site_at_row(7, 5), defect{defect_type::RAISED_SI});
        defect_layout.assign_defect(site_at_row(7, 6), defect{defect_type::RAISED_SI});

        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 12);

        check_tiles(black_list, {{1, 0}, {2, 0}, {3, 0}, {1, 1}, {2, 1}, {3, 1}, {0, 2}, {1, 2}, {2, 2}});
    }
}

TEST_CASE("SiDB Bestagon gate library with simple defects", "[sidb-surface-analysis]")
{
    /** @brief Single 60-by-46-cell Bestagon gate tile. */
    static const hexagonal_gate_layout gate_lyt{
        arrangement::EVEN_COLUMN, hexagonal_gate_layout::extent{1, 1, 1}};  // 1 x 1 tiles of size 60 x 46 cells each
    static const layout cell_lyt{};  // makes for exactly one gate of the Bestagon library

    layout defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        const auto black_list = surface_analysis<bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_defect(site_at_row(30, 45), defect{defect_type::SI_VACANCY});

        const auto black_list = surface_analysis<bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.contains({0, 0}));
        /** @brief Observed blocked functions, with no iteration-order requirement. */
        std::vector<kitty::dynamic_truth_table> functions{};
        for (const auto& [function, ports] : black_list.at({0, 0}))
        {
            functions.push_back(function);
        }
        CHECK_THAT(functions, Catch::Matchers::UnorderedRangeEquals(std::vector<kitty::dynamic_truth_table>{
                                  create_id_tt(), create_not_tt(), create_and_tt(), create_or_tt(), create_nand_tt(),
                                  create_nor_tt(), create_xor_tt(), create_xnor_tt()}));
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect(site_at_row(30, 45), defect{defect_type::SILOXANE});

        const auto black_list = surface_analysis<bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.contains({0, 0}));
        /** @brief Observed blocked functions, with no iteration-order requirement. */
        std::vector<kitty::dynamic_truth_table> functions{};
        for (const auto& [function, ports] : black_list.at({0, 0}))
        {
            functions.push_back(function);
        }
        CHECK_THAT(functions, Catch::Matchers::UnorderedRangeEquals(
                                  std::vector<kitty::dynamic_truth_table>{create_id_tt(), create_not_tt()}));
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_defect(site_at_row(16, 43), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(30, 40), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(41, 32), defect{defect_type::ETCH_PIT});
        defect_layout.assign_defect(site_at_row(45, 33), defect{defect_type::RAISED_SI});
        defect_layout.assign_defect(site_at_row(45, 34), defect{defect_type::RAISED_SI});

        const auto black_list = surface_analysis<bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.contains({0, 0}));
        /** @brief Observed blocked functions, with no iteration-order requirement. */
        std::vector<kitty::dynamic_truth_table> functions{};
        for (const auto& [function, ports] : black_list.at({0, 0}))
        {
            functions.push_back(function);
        }
        CHECK_THAT(functions, Catch::Matchers::UnorderedRangeEquals(std::vector<kitty::dynamic_truth_table>{
                                  create_id_tt(), create_not_tt(), create_and_tt(), create_or_tt(), create_nand_tt(),
                                  create_nor_tt(), create_xor_tt(), create_xnor_tt()}));
    }
}
