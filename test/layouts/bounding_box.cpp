//
// Created by marcel on 13.01.22.
//
#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("2D bounding box around an empty gate-level layout", "[bounding-box]")
{
    const auto lyt = cart_gate_clk_lyt{};
    const auto bb  = bounding_box_2d<cart_gate_clk_lyt>{lyt};
    CHECK(bb.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb.get_max() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb.get_x_size() == 0);
    CHECK(bb.get_y_size() == 0);
}

TEST_CASE("Initialize 2D gate-level bounding box", "[bounding-box]")
{
    const auto lyt_xor_maj = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

    const bounding_box_2d bb_xor_maj{lyt_xor_maj};

    CHECK(bb_xor_maj.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_xor_maj.get_max() == tile<cart_gate_clk_lyt>{3, 2});
    CHECK(bb_xor_maj.get_x_size() == 3);
    CHECK(bb_xor_maj.get_y_size() == 2);

    const auto lyt_or_not = blueprints::or_not_gate_layout<cart_gate_clk_lyt>();

    const bounding_box_2d bb_or_not{lyt_or_not};

    CHECK(bb_or_not.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_or_not.get_max() == tile<cart_gate_clk_lyt>{2, 2});
    CHECK(bb_or_not.get_x_size() == 2);
    CHECK(bb_or_not.get_y_size() == 2);

    const auto lyt_crossing = blueprints::crossing_layout<cart_gate_clk_lyt>();

    const bounding_box_2d bb_crossing{lyt_crossing};

    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 2});
    CHECK(bb_crossing.get_x_size() == 3);
    CHECK(bb_crossing.get_y_size() == 2);
}

TEST_CASE("Update 2D gate-level bounding box", "[bounding-box]")
{
    auto lyt_crossing = blueprints::crossing_layout<cart_gate_clk_lyt>();

    bounding_box_2d bb_crossing{lyt_crossing};

    // resize the layout to size of 6 x 6 tiles
    lyt_crossing.resize(aspect_ratio_type_t<cart_gate_clk_lyt>{5, 5});

    // move the PO from tile (3, 2) to tile (2, 3) but keep its child on tile (2, 2)
    lyt_crossing.move_node(lyt_crossing.get_node({3, 2}), {2, 3},
                           {lyt_crossing.make_signal(lyt_crossing.get_node({2, 2}))});

    // still the old bounding box
    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 2});

    bb_crossing.update_bounding_box();

    // new bounding box
    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 3});
    CHECK(bb_crossing.get_x_size() == 3);
    CHECK(bb_crossing.get_y_size() == 3);
}

TEST_CASE("2D bounding box around an empty cell-level layout", "[bounding-box]")
{
    const auto lyt = qca_cell_clk_lyt{};
    const auto bb  = bounding_box_2d<qca_cell_clk_lyt>{lyt};
    CHECK(bb.get_min() == cell<qca_cell_clk_lyt>{0, 0});
    CHECK(bb.get_max() == cell<qca_cell_clk_lyt>{0, 0});
    CHECK(bb.get_x_size() == 0);
    CHECK(bb.get_y_size() == 0);
}

TEST_CASE("Initialize 2D cell-level bounding box", "[bounding-box]")
{
    const auto lyt_and = blueprints::single_layer_qca_and_gate<qca_cell_clk_lyt>();

    const bounding_box_2d bb_and{lyt_and};

    CHECK(bb_and.get_min() == cell<qca_cell_clk_lyt>{0, 0});
    CHECK(bb_and.get_max() == cell<qca_cell_clk_lyt>{4, 4});
    CHECK(bb_and.get_x_size() == 4);
    CHECK(bb_and.get_y_size() == 4);
}

TEST_CASE("Update 2D cell-level bounding box", "[bounding-box]")
{
    auto lyt_and = blueprints::single_layer_qca_and_gate<qca_cell_clk_lyt>();

    bounding_box_2d bb_and{lyt_and};

    // resize the layout to size of 8 x 8 cells
    lyt_and.resize(aspect_ratio_type_t<qca_cell_clk_lyt>{7, 7});

    // erase an input cell and the constant cell
    lyt_and.assign_cell_type({0, 2}, qca_technology::cell_type::EMPTY);
    lyt_and.assign_cell_type({2, 0}, qca_technology::cell_type::EMPTY);

    // add a wire segment below
    lyt_and.assign_cell_type({1, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({2, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({3, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({4, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({5, 6}, qca_technology::cell_type::NORMAL);

    // still the old bounding box
    CHECK(bb_and.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_and.get_max() == tile<cart_gate_clk_lyt>{4, 4});

    bb_and.update_bounding_box();

    // new bounding box
    CHECK(bb_and.get_min() == tile<cart_gate_clk_lyt>{1, 1});
    CHECK(bb_and.get_max() == tile<cart_gate_clk_lyt>{5, 6});
    CHECK(bb_and.get_x_size() == 4);
    CHECK(bb_and.get_y_size() == 5);
}

TEMPLATE_TEST_CASE("2D bounding box for siqad layout", "[bounding-box]", sidb_cell_clk_lyt_siqad,
                   sidb_111_cell_clk_lyt_siqad, sidb_100_cell_clk_lyt_siqad)
{
    SECTION("empyt layout")
    {
        const TestType lyt{};

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t(0, 0, 0));
        CHECK(se == siqad::coord_t(0, 0, 0));
    }

    SECTION("one cell")
    {
        TestType lyt{};
        lyt.assign_cell_type({1, 0, 0}, TestType::technology::NORMAL);

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{1, 0, 0});
        CHECK(se == siqad::coord_t{1, 0, 0});
    }

    SECTION("three cells as input, switched correct order")
    {
        TestType lyt{};
        lyt.assign_cell_type({0, 1, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({10, 0, 1}, TestType::technology::NORMAL);
        lyt.assign_cell_type({5, 8, 0}, TestType::technology::NORMAL);

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{0, 0, 1});
        CHECK(se == siqad::coord_t{10, 8, 0});
    }

    SECTION("two cells as input, on the same height in y-direction")
    {
        TestType lyt{};
        lyt.assign_cell_type({-3, 0, 1}, TestType::technology::NORMAL);
        lyt.assign_cell_type({3, 0, 1}, TestType::technology::NORMAL);

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{-3, 0, 1});
        CHECK(se == siqad::coord_t{3, 0, 1});
    }

    SECTION("four cells as input, three on the same dimer")
    {
        TestType lyt{};
        lyt.assign_cell_type({3, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({0, 3, 1}, TestType::technology::NORMAL);
        lyt.assign_cell_type({5, 3, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({10, 3, 1}, TestType::technology::NORMAL);

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{0, 0, 0});
        CHECK(se == siqad::coord_t{10, 3, 1});
    }

    SECTION("four cells as input, two on the same dimer")
    {
        TestType lyt{};
        lyt.assign_cell_type({0, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({1, 0, 1}, TestType::technology::NORMAL);
        lyt.assign_cell_type({-2, 4, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({2, 4, 1}, TestType::technology::NORMAL);

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{-2, 0, 0});
        CHECK(se == siqad::coord_t{2, 4, 1});
    }
}

TEMPLATE_TEST_CASE("2D bounding box for siqad layout with atomic defect", "[bounding-box]",
                   sidb_defect_cell_clk_lyt_siqad, sidb_111_cell_clk_lyt_siqad, sidb_defect_100_cell_clk_lyt_siqad)
{
    SECTION("empyt layout")
    {
        const TestType lyt{};

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t(0, 0, 0));
        CHECK(se == siqad::coord_t(0, 0, 0));
    }

    SECTION("one cell and one defect")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({2, 0, 0}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{1, 0, 0});
        CHECK(se == siqad::coord_t{2, 0, 0});
    }

    SECTION("two cell and two defect")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({-2, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({2, 0, 0}, sidb_defect{});
        lyt.assign_sidb_defect({2, 0, 1}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == siqad::coord_t{-2, 0, 0});
        CHECK(se == siqad::coord_t{2, 0, 1});
    }
}

TEMPLATE_TEST_CASE("2D bounding box for layout with atomic defect", "[bounding-box]", sidb_defect_cell_clk_lyt)
{
    SECTION("empyt layout")
    {
        const TestType lyt{};

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{0, 0, 0});
        CHECK(se == cell<TestType>{0, 0, 0});
    }

    SECTION("one cell and one defect")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({2, 0, 0}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{1, 0, 0});
        CHECK(se == cell<TestType>{2, 0, 0});
    }

    SECTION("two cell and two defect")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({3, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({2, 0}, sidb_defect{});
        lyt.assign_sidb_defect({2, 0}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{1, 0});
        CHECK(se == cell<TestType>{3, 0});
    }
}

TEMPLATE_TEST_CASE("2D bounding box for cube layout with atomic defect", "[bounding-box]", sidb_cell_clk_lyt_cube,
                   sidb_111_cell_clk_lyt_cube, sidb_100_cell_clk_lyt_cube)
{
    SECTION("empyt layout")
    {
        const TestType lyt{};

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{0, 0});
        CHECK(se == cell<TestType>{0, 0});
    }

    SECTION("one cell and one defect")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({2, 0}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{1, 0});
        CHECK(se == cell<TestType>{2, 0});
    }

    SECTION("two cell and two defect, include defects")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({-3, 0}, sidb_defect{});
        lyt.assign_sidb_defect({2, 0}, sidb_defect{});

        const bounding_box_2d bb{lyt};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{-3, 0});
        CHECK(se == cell<TestType>{2, 0});
    }

    SECTION("two cell and two defect, exclude defects")
    {
        sidb_defect_surface<TestType> lyt{TestType{}};
        lyt.assign_cell_type({1, 0}, TestType::technology::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::technology::NORMAL);
        lyt.assign_sidb_defect({-3, 0}, sidb_defect{});
        lyt.assign_sidb_defect({2, 0}, sidb_defect{});

        const bounding_box_2d bb{static_cast<TestType>(lyt)};
        const auto            nw = bb.get_min();
        const auto            se = bb.get_max();

        CHECK(nw == cell<TestType>{1, 0, 0});
        CHECK(se == cell<TestType>{2, 0, 0});
    }
}
