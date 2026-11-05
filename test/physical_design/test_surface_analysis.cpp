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

#include <fiction/physical_design/surface_analysis.hpp>

// Test dependencies follow the defining header to expose missing direct includes.
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>

#include <kitty/dynamic_truth_table.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::synthesis;
using namespace fiction::test;

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

/** @brief Test library with a cell at the right edge of a five-column tile. */
struct wide_gate_library : gate_library<sidb::layout, 5, 3>
{
    /** @brief Returns the test gate. @return Gate implementations by function. */
    static gate_functions get_functional_implementations()
    {
        return {{create_id_tt(), {WIRE}}};
    }

    /** @brief Returns the test gate's ports. @return Ports by gate. */
    static gate_ports<port_position> get_gate_ports()
    {
        return {{WIRE, {{{port_position(0, 0)}, {port_position(4, 0)}}}}};
    }

  private:
    /** @brief One occupied cell at the tile's right edge. */
    static constexpr gate WIRE{
        cell_list_to_gate<char>({{{' ', ' ', ' ', ' ', 'x'}, {' ', ' ', ' ', ' ', ' '}, {' ', ' ', ' ', ' ', ' '}}})};
};

/** @brief SiDB gate library without implementation or port lookup. */
struct empty_gate_library : gate_library<sidb::layout, 3, 3>
{};

/** @brief Gate library whose implementations have no port entries. */
struct missing_gate_ports_library : dummy_gate_library
{
    /** @brief Returns an empty port map. @return Empty port map. */
    static gate_ports<port_position> get_gate_ports()
    {
        return {};
    }
};

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
    /** @brief Checks both blocked gates and ports on every tile of the four-by-three grid. */
    const auto check_all_tiles = [&](const auto& black_list)
    {
        CHECK(black_list.size() == 12);
        for (auto y = 0; y < 3; ++y)
        {
            for (auto x = 0; x < 4; ++x)
            {
                const layout_base::coordinate tile{x, y};
                CAPTURE(tile);
                REQUIRE(black_list.contains(tile));
                CHECK(black_list.at(tile) == expected);
            }
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

        check_all_tiles(black_list);
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect(site_at_row(1, 1), defect{defect_type::SILOXANE});

        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.contains({0, 0}));
        CHECK(black_list.at({0, 0}) == expected);
    }
    SECTION("one obstructed function")
    {
        defect_layout.assign_defect(site_at_row(0, 0), defect{defect_type::SILOXANE});
        const auto neutral_spacing = std::pair<uint16_t, uint16_t>{0, 0};
        const auto black_list =
            surface_analysis<dummy_gate_library>(gate_lyt, defect_layout, std::nullopt, neutral_spacing);
        CHECK(black_list.size() == 1);
        REQUIRE(black_list.contains({0, 0}));
        const surface_black_list<port_position>::mapped_type blocked{{create_and_tt(), {y_ports}}};
        CHECK(black_list.at({0, 0}) == blocked);
    }
    SECTION("localized charged and neutral defects")
    {
        defect_layout.assign_defect(site_at_row(1, 1), defect{defect_type::SI_VACANCY});
        defect_layout.assign_defect(site_at_row(5, 4), defect{defect_type::SILOXANE});

        const auto charged_spacing = std::pair<uint16_t, uint16_t>{3, 0};
        const auto neutral_spacing = std::pair<uint16_t, uint16_t>{2, 0};
        const auto black_list =
            surface_analysis<dummy_gate_library>(gate_lyt, defect_layout, charged_spacing, neutral_spacing);

        CHECK(black_list.size() == 4);
        REQUIRE(black_list.contains({0, 0}));
        REQUIRE(black_list.contains({1, 0}));
        REQUIRE(black_list.contains({1, 1}));
        REQUIRE(black_list.contains({2, 1}));
        CHECK(black_list.at({0, 0}) == expected);
        CHECK(black_list.at({1, 0}) == expected);
        CHECK(black_list.at({1, 1}) == expected);
        CHECK(black_list.at({2, 1}) == expected);
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_defect(site_at_row(6, 2), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(6, 3), defect{defect_type::DB});
        defect_layout.assign_defect(site_at_row(3, 6), defect{defect_type::ETCH_PIT});
        defect_layout.assign_defect(site_at_row(7, 5), defect{defect_type::RAISED_SI});
        defect_layout.assign_defect(site_at_row(7, 6), defect{defect_type::RAISED_SI});

        const auto black_list = surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        check_all_tiles(black_list);
    }
}

TEST_CASE("SiDB Bestagon gate library with simple defects", "[sidb-surface-analysis]")
{
    /** @brief Single 60-by-46-cell Bestagon gate tile. */
    static const hexagonal_gate_layout gate_lyt{
        arrangement::EVEN_COLUMN, hexagonal_gate_layout::extent{1, 1, 1}};  // 1 x 1 tiles of size 60 x 46 cells each
    static const layout cell_lyt{};  // makes for exactly one gate of the Bestagon library

    layout defect_layout{cell_lyt};

    /** @brief Collects all blocked functions without imposing map iteration order. */
    const auto blocked_functions = [](const auto& functions)
    {
        std::vector<kitty::dynamic_truth_table> keys{};
        keys.reserve(functions.size());
        for (const auto& [function, ports] : functions)
        {
            keys.push_back(function);
        }
        return keys;
    };

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
        CHECK_THAT(blocked_functions(black_list.at({0, 0})),
                   Catch::Matchers::UnorderedRangeEquals(std::vector<kitty::dynamic_truth_table>{
                       create_id_tt(), create_not_tt(), create_and_tt(), create_or_tt(), create_nand_tt(),
                       create_nor_tt(), create_xor_tt(), create_xnor_tt()}));
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_defect(site_at_row(30, 45), defect{defect_type::SILOXANE});

        const auto black_list = surface_analysis<bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.contains({0, 0}));
        CHECK_THAT(blocked_functions(black_list.at({0, 0})),
                   Catch::Matchers::UnorderedRangeEquals(
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
        CHECK_THAT(blocked_functions(black_list.at({0, 0})),
                   Catch::Matchers::UnorderedRangeEquals(std::vector<kitty::dynamic_truth_table>{
                       create_id_tt(), create_not_tt(), create_and_tt(), create_or_tt(), create_nand_tt(),
                       create_nor_tt(), create_xor_tt(), create_xnor_tt()}));
    }
}

TEST_CASE("Surface analysis rejects affected gates without ports", "[sidb-surface-analysis]")
{
    const cartesian_gate_layout gates{{1, 1, 1}};
    sidb::layout                surface{};
    surface.assign_defect(site_at_row(1, 1), defect{defect_type::SILOXANE});

    CHECK_THROWS_AS(surface_analysis<missing_gate_ports_library>(gates, surface), std::out_of_range);
}

TEST_CASE("Surface analysis propagates allocation failure and recovers", "[sidb-surface-analysis]")
{
    require_allocation_failure_support();
    const auto extent = GENERATE(uint16_t{1}, uint16_t{4}, uint16_t{32});
    CAPTURE(extent);
    const auto spacing = std::pair{extent, extent};
    /** @brief Single tile with both dummy-library gates obstructed at their center cell. */
    const cartesian_gate_layout gates{{1, 1, 1}};
    /** @brief Defect canary that must survive every failed and successful analysis. */
    sidb::layout surface{};
    surface.assign_defect(site_at_row(1, 1), defect{defect_type::SILOXANE});
    /** @brief Complete independently expected blacklist, including each gate's ports. */
    const surface_black_list<port_position> expected{
        {{0, 0},
         {{create_id_tt(), {{{port_position(0, 1)}, {port_position(2, 1)}}}},
          {create_and_tt(), {{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}}}}}}};
    // Initialize the test library's static fixtures before injecting allocation failures.
    CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::nullopt, spacing) == expected);
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        try
        {
            allocation_budget = failure;
            const auto result = surface_analysis<dummy_gate_library>(gates, surface, std::nullopt, spacing);
            allocation_budget.reset();
            CHECK(result == expected);
            break;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
            CHECK(surface.num_defects() == 1);
            CHECK(surface.get_defect(site_at_row(1, 1)).type == defect_type::SILOXANE);
            CHECK(gates.is_empty());
            CHECK(gates.get_extent() == layout_base::extent{1, 1, 1});
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
    }
    CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::nullopt, spacing) == expected);
    CHECK(surface.num_defects() == 1);
    CHECK(surface.get_defect(site_at_row(1, 1)).type == defect_type::SILOXANE);
    CHECK(gates.is_empty());
    CHECK(gates.get_extent() == layout_base::extent{1, 1, 1});
}

TEST_CASE("Surface analysis handles maximum defect spacing", "[sidb-surface-analysis]")
{
    const cartesian_gate_layout gates{{2, 2, 1}};
    sidb::layout                surface{};
    surface.assign_defect(site_at_row(-3, 1), defect{defect_type::SI_VACANCY});
    constexpr auto maximum    = std::numeric_limits<uint16_t>::max();
    const auto     black_list = surface_analysis<dummy_gate_library>(gates, surface, std::pair{maximum, maximum});
    const port_list<port_position> line_ports{{port_position(0, 1)}, {port_position(2, 1)}};
    const port_list<port_position> y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};
    const surface_black_list<port_position>::mapped_type expected{{create_id_tt(), {line_ports}},
                                                                  {create_and_tt(), {y_ports}}};
    CHECK(black_list.size() == 4);
    for (auto y = 0; y < 2; ++y)
    {
        for (auto x = 0; x < 2; ++x)
        {
            const layout_base::coordinate tile{x, y};
            CAPTURE(tile);
            REQUIRE(black_list.contains(tile));
            CHECK(black_list.at(tile) == expected);
        }
    }
}

TEST_CASE("Surface analysis retains outside defects whose influence reaches a tile", "[sidb-surface-analysis]")
{
    sidb::layout surface{};
    surface.assign_defect({-10, 0, 0}, defect{defect_type::SI_VACANCY});
    const port_list<port_position>          line_ports{{port_position(0, 1)}, {port_position(2, 1)}};
    const port_list<port_position>          y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};
    const surface_black_list<port_position> expected{
        {{0, 0}, {{create_id_tt(), {line_ports}}, {create_and_tt(), {y_ports}}}}};
    /** @brief Checks the complete blacklist for one layout geometry. */
    const auto check = [&](const auto& gates)
    { CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::pair<uint16_t, uint16_t>{12, 32}) == expected); };
    SECTION("Cartesian")
    {
        check(cartesian_gate_layout{{1, 1, 1}});
    }
    SECTION("Shifted Cartesian")
    {
        for (const auto a :
             std::array{arrangement::EVEN_ROW, arrangement::ODD_ROW, arrangement::EVEN_COLUMN, arrangement::ODD_COLUMN})
        {
            CAPTURE(static_cast<int>(a));
            check(shifted_cartesian_gate_layout{a, {1, 1, 1}});
        }
    }
    SECTION("Hexagonal")
    {
        for (const auto a :
             std::array{arrangement::EVEN_ROW, arrangement::ODD_ROW, arrangement::EVEN_COLUMN, arrangement::ODD_COLUMN})
        {
            CAPTURE(static_cast<int>(a));
            check(hexagonal_gate_layout{a, {1, 1, 1}});
        }
    }
}

TEST_CASE("Surface analysis excludes defects beyond the layout", "[sidb-surface-analysis]")
{
    sidb::layout surface{};
    surface.assign_defect({20, 0, 0}, defect{defect_type::SI_VACANCY});
    /** @brief Checks that a distant defect blocks no gates. */
    const auto check = [&](const auto& gates)
    { CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::pair<uint16_t, uint16_t>{12, 32}).empty()); };
    check(cartesian_gate_layout{{2, 2, 1}});
    for (const auto a :
         std::array{arrangement::EVEN_ROW, arrangement::ODD_ROW, arrangement::EVEN_COLUMN, arrangement::ODD_COLUMN})
    {
        CAPTURE(static_cast<int>(a));
        check(shifted_cartesian_gate_layout{a, {2, 2, 1}});
        check(hexagonal_gate_layout{a, {2, 2, 1}});
    }
}

TEST_CASE("Surface analysis retains defects reaching shifted rightmost cells", "[sidb-surface-analysis]")
{
    sidb::layout surface{};
    surface.assign_defect({18, 0, 0}, defect{defect_type::SI_VACANCY});
    const port_list<port_position>          y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};
    const surface_black_list<port_position> expected{{{1, 0}, {{create_and_tt(), {y_ports}}}}};
    /** @brief Checks the shifted rightmost-cell blacklist. */
    const auto check = [&](const auto& gates)
    { CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::pair<uint16_t, uint16_t>{12, 32}) == expected); };
    SECTION("Shifted Cartesian")
    {
        check(shifted_cartesian_gate_layout{arrangement::EVEN_ROW, {2, 2, 1}});
    }
    SECTION("Hexagonal")
    {
        check(hexagonal_gate_layout{arrangement::EVEN_ROW, {2, 2, 1}});
    }
}

TEST_CASE("Surface analysis keeps the shifted edge of wide gates", "[sidb-surface-analysis]")
{
    sidb::layout surface{};
    surface.assign_defect({11, 0, 0}, defect{defect_type::SI_VACANCY});
    const port_list<port_position>          ports{{port_position(0, 0)}, {port_position(4, 0)}};
    const surface_black_list<port_position> expected{{{1, 0}, {{create_id_tt(), {ports}}}}};
    /** @brief Checks the shifted edge for one layout geometry. */
    const auto check = [&](const auto& gates)
    { CHECK(surface_analysis<wide_gate_library>(gates, surface, std::pair<uint16_t, uint16_t>{0, 32}) == expected); };
    SECTION("Shifted Cartesian")
    {
        check(shifted_cartesian_gate_layout{arrangement::EVEN_ROW, {2, 1, 1}});
    }
    SECTION("Hexagonal")
    {
        check(hexagonal_gate_layout{arrangement::EVEN_ROW, {2, 1, 1}});
    }
}

TEST_CASE("Surface analysis scans defects beyond the row budget", "[sidb-surface-analysis]")
{
    const cartesian_gate_layout gates{{2, 2, 1}};
    sidb::layout                surface{};
    surface.assign_defect({0, -1000, 0}, defect{defect_type::SI_VACANCY});
    // These 1,040 rectangles and the charged defect consume 65,521 of 65,536 cached rows.
    for (int32_t x = -1041; x <= -2; ++x)
    {
        surface.assign_defect({x, 0, 0}, defect{defect_type::SILOXANE});
    }
    surface.assign_defect({1, 0, 0}, defect{defect_type::SILOXANE});

    const port_list<port_position> line_ports{{port_position(0, 1)}, {port_position(2, 1)}};
    const port_list<port_position> y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};
    const surface_black_list<port_position>::mapped_type blocked{{create_id_tt(), {line_ports}},
                                                                 {create_and_tt(), {y_ports}}};
    const surface_black_list<port_position>              expected{{{0, 0}, blocked}, {{0, 1}, blocked}};

    CHECK(surface_analysis<dummy_gate_library>(gates, surface, std::pair<uint16_t, uint16_t>{2000, 0},
                                               std::pair<uint16_t, uint16_t>{0, 31}) == expected);
}

TEST_CASE("Surface analysis agrees with materialized defect influence", "[sidb-surface-analysis]")
{
    const cartesian_gate_layout        gates{{2, 2, 1}};
    const port_list<port_position>     line_ports{{port_position(0, 1)}, {port_position(2, 1)}};
    const port_list<port_position>     y_ports{{port_position(0, 0), port_position(0, 2)}, {port_position(2, 1)}};
    std::mt19937                       rng{1937};  // NOLINT(cert-msc32-c,cert-msc51-cpp): Reproduce differential cases.
    std::uniform_int_distribution<int> coordinate{-4, 8};
    std::uniform_int_distribution<int> spacing{0, 3};
    for (auto trial = 0; trial < 40; ++trial)
    {
        CAPTURE(trial);
        sidb::layout surface{};
        for (auto i = 0; i < 6; ++i)
        {
            surface.assign_defect(site_at_row(coordinate(rng), coordinate(rng)),
                                  defect{i % 2 == 0 ? defect_type::SI_VACANCY : defect_type::SILOXANE});
        }
        surface.assign_defect(site_at_row(-1, 1), defect{defect_type::SILOXANE});
        surface.assign_defect(site_at_row(-1, 1), defect{defect_type::SILOXANE, -1});
        if (trial < 4)
        {
            for (auto i = 0; i < 512; ++i)
            {
                surface.assign_defect(site_at_row(i - 256, trial % 2 == 0 ? i - 256 : 0),
                                      defect{defect_type::SILOXANE});
            }
        }
        surface.assign_defect({std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min(), 0},
                              defect{defect_type::DB});
        surface.assign_defect({std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), 1},
                              defect{defect_type::SILOXANE});
        auto charged = std::pair{static_cast<uint16_t>(spacing(rng)), static_cast<uint16_t>(spacing(rng))};
        auto neutral = std::pair{static_cast<uint16_t>(spacing(rng)), static_cast<uint16_t>(spacing(rng))};
        if (trial < 4)
        {
            const auto boundary = static_cast<uint16_t>(trial + 1);
            charged             = {boundary, boundary};
            neutral             = {boundary, boundary};
        }
        else if (trial == 4)
        {
            charged = {1, 0};
            neutral = {0, 26};
        }
        else if (trial == 5)
        {
            charged = {0, 26};
            neutral = {1, 0};
        }
        else if (trial == 6)
        {
            charged = {0, 250};
            neutral = {0, 1};
        }
        else if (trial == 7)
        {
            surface = sidb::layout{};
            for (auto i = 0; i < 101; ++i)
            {
                surface.assign_defect(site_at_row(i - 50, i % 8),
                                      defect{i == 0 ? defect_type::SI_VACANCY : defect_type::SILOXANE});
            }
            charged = {0, 250};
            neutral = {1, 0};
        }
        else if (trial == 8)
        {
            surface = sidb::layout{};
            surface.assign_defect(site_at_row(0, 0), defect{defect_type::SI_VACANCY});
            surface.assign_defect(site_at_row(4, 1), defect{defect_type::SILOXANE});
            charged = {0, 1000};
            neutral = {0, 0};
        }
        else if (trial == 9)
        {
            surface = sidb::layout{};
            surface.assign_defect(site_at_row(-39, 0), defect{defect_type::SILOXANE});
            neutral = {40, 0};
        }
        else if (trial == 10)
        {
            surface = sidb::layout{};
            surface.assign_defect({0, 0, 0}, defect{defect_type::SILOXANE});
            surface.assign_defect({1, 0, 0}, defect{defect_type::SI_VACANCY});
            charged = {4, 0};
            neutral = {2, 31};
        }
        else if (trial == 11)
        {
            surface = sidb::layout{};
            surface.assign_defect({0, -1000, 0}, defect{defect_type::SI_VACANCY});
            surface.assign_defect({0, 0, 0}, defect{defect_type::SILOXANE});
            surface.assign_defect({2, 0, 0}, defect{defect_type::SILOXANE});
            charged = {2000, 0};
            neutral = {0, 31};
        }
        const auto                        affected = surface.all_affected_sidbs(charged, neutral);
        surface_black_list<port_position> expected{};
        for (auto y = 0; y < 2; ++y)
        {
            for (auto x = 0; x < 2; ++x)
            {
                const layout_base::coordinate tile{x, y};
                /** @brief Checks a gate cell against the materialized influence sites. */
                const auto hit = [&](const int col, const int row)
                { return affected.contains(site_at_row((3 * x) + col, (3 * y) + row)); };
                if (hit(1, 0) || hit(1, 1) || hit(1, 2))
                {
                    expected[tile][create_id_tt()].push_back(line_ports);
                }
                if (hit(0, 0) || hit(2, 0) || hit(1, 1) || hit(1, 2))
                {
                    expected[tile][create_and_tt()].push_back(y_ports);
                }
            }
        }
        CHECK(surface_analysis<dummy_gate_library>(gates, surface, charged, neutral) == expected);
    }
}

TEST_CASE("Surface analysis rejects an invalid defect basis", "[sidb-surface-analysis]")
{
    const cartesian_gate_layout gates{{1, 1, 1}};
    sidb::layout                surface{};
    lattice_site                invalid{};
    invalid.z = 2;
    surface.assign_defect(invalid, defect{defect_type::SI_VACANCY});
    CHECK_THROWS_AS(surface_analysis<dummy_gate_library>(gates, surface), std::out_of_range);
}
