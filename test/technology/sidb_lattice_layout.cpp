//
// Created by Jan Drewniok on 12.02.24.
//

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/sidb_lattice_layout.hpp>
#include <fiction/technology/sidb_lattice_properties.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("layout traits", "[sidb-lattice-layout]", (sidb_cell_clk_lyt), (cds_sidb_cell_clk_lyt))
{
    using lattice_layout = sidb_lattice_layout<TestType, si_lattice_orientations>;
    CHECK(is_sidb_lattice_layout_v<lattice_layout, si_lattice_orientations>);
    CHECK(is_cell_level_layout_v<lattice_layout>);
    CHECK(has_assign_lattice_orientation_v<lattice_layout, si_lattice_orientations>);
    CHECK(has_get_lattice_orientation_v<lattice_layout>);
}

TEST_CASE("Traits and construction", "[sidb-lattice-layout]")
{
    SECTION("Empty Cell-level layout based on offset coordinates")
    {
        sidb_lattice_layout<sidb_cell_clk_lyt> lattice{si_lattice_orientations::SI_111};
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_111);

        lattice.assign_lattice_orientation(si_lattice_orientations::SI_100);
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_100);
    }

    SECTION("Cell-level layout based on offset coordinates")
    {
        sidb_cell_clk_lyt lyt{};
        lyt.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);

        CHECK(lyt.num_cells() == 3);

        sidb_lattice_layout lattice{lyt, si_lattice_orientations::SI_111};
        CHECK(lattice.num_cells() == 3);
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_111);

        lattice.assign_lattice_orientation(si_lattice_orientations::SI_100);
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_100);
    }

    SECTION("Cell-level layout based on offset coordinates")
    {
        sidb_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
        lyt.assign_cell_type({5, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);

        CHECK(lyt.num_cells() == 3);

        sidb_lattice_layout lattice{lyt, si_lattice_orientations::SI_111};
        CHECK(lattice.num_cells() == 3);
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_111);

        lattice.assign_lattice_orientation(si_lattice_orientations::SI_100);
        CHECK(lattice.get_lattice_orientation() == si_lattice_orientations::SI_100);
    }
}