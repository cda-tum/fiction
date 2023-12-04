//
// Created by Jan Drewniok on 03.12.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/determine_influence_region.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>

#include <filesystem>

using namespace fiction;

const auto current_path = std::filesystem::current_path();

TEST_CASE("Three SiDBs with positive charge states", "[determine-influence-region]")
{
    sidb_cell_clk_lyt sublayout{{4, 5}};
    sublayout.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
    sublayout.assign_cell_type({2, 0}, sidb_technology::cell_type::NORMAL);
    sublayout.assign_cell_type({4, 0}, sidb_technology::cell_type::NORMAL);
    sublayout.assign_cell_type({3, 5}, sidb_technology::cell_type::NORMAL);

    sidb_cell_clk_lyt layout{{30, 11}};
    layout.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 5}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({7, 2}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({21, 3}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({8, 11}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 9}, sidb_technology::cell_type::NORMAL);

    SECTION("Default physical parameters")
    {
        const auto [nw, se] = determine_influence_region(layout, sublayout);

        CHECK(nw == cell<sidb_cell_clk_lyt>(0, 0));
        CHECK(se == cell<sidb_cell_clk_lyt>(8, 9));
    }
    SECTION("set µ value")
    {
        const quickexact_params<cell<sidb_cell_clk_lyt>> params{sidb_simulation_parameters{2, -0.25}};
        const auto [nw, se] = determine_influence_region(layout, sublayout, params);

        CHECK(nw == cell<sidb_cell_clk_lyt>(0, 0));
        CHECK(se == cell<sidb_cell_clk_lyt>(10, 11));
    }
}

TEST_CASE("Using xnor layout and xnor gate as sublayout", "[determine-influence-region]")
{
    const auto layout = read_sqd_layout<sidb_cell_clk_lyt>(current_path.string() + "/../../test/resources/xnor2.sqd");
    const auto sublayout = read_sqd_layout<sidb_cell_clk_lyt>(current_path.string() + "/../../test/resources/xnor.sqd");

    const auto [nw, se] = determine_influence_region(layout, sublayout);

    CHECK(nw == cell<sidb_cell_clk_lyt>(24, 28));
    CHECK(se == cell<sidb_cell_clk_lyt>(62, 72));
}

TEST_CASE("Using xnor layout and input wire as sublayout", "[determine-influence-region]")
{
    const auto layout = read_sqd_layout<sidb_cell_clk_lyt>(current_path.string() + "/../../test/resources/xnor2.sqd");
    const auto sublayout =
        read_sqd_layout<sidb_cell_clk_lyt>(current_path.string() + "/../../test/resources/wire_of_xnor2.sqd");

    const auto [nw, se] = determine_influence_region(layout, sublayout);

    CHECK(nw == cell<sidb_cell_clk_lyt>(0, 0));
    CHECK(se == cell<sidb_cell_clk_lyt>(32, 38));
}
