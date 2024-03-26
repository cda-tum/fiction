//
// Created by Jan Drewniok on 12.02.24.
//

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("layout traits", "[sidb-lattice]", (sidb_100_cell_clk_lyt), (cds_sidb_100_cell_clk_lyt))
{
    using lattice_layout_100 = sidb_lattice<sidb_100_lattice, TestType>;
    CHECK(is_sidb_lattice_v<lattice_layout_100>);
    CHECK(is_cell_level_layout_v<lattice_layout_100>);

    using lattice_layout_111 = sidb_lattice<sidb_111_lattice, TestType>;
    CHECK(is_sidb_lattice_v<lattice_layout_111>);
    CHECK(is_cell_level_layout_v<lattice_layout_111>);
}

TEMPLATE_TEST_CASE("use different lattice orientations", "[sidb-lattice]", (sidb_cell_clk_lyt),
                   (cds_sidb_100_cell_clk_lyt))
{
    sidb_lattice<sidb_100_lattice, TestType> lyt_100{};
    lyt_100.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_100.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_100.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    CHECK(lyt_100.num_cells() == 3);

    using lattice_layout_111 = sidb_lattice<sidb_100_lattice, TestType>;
    lattice_layout_111 lyt_111{};
    lyt_111.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_111.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_111.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    CHECK(lyt_111.num_cells() == 3);
}
