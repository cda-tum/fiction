//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <map>
#include <sstream>
#include <string>

using namespace fiction;

template <typename WLyt, typename RLyt>
void compare_written_and_read_layout(const WLyt& wlyt, const RLyt& rlyt) noexcept
{
    CHECK(wlyt.get_layout_name() == rlyt.get_layout_name());

    const bounding_box_2d<WLyt> wbb{wlyt};
    const bounding_box_2d<RLyt> rbb{wlyt};

    CHECK(wbb.get_min() == rbb.get_min());
    CHECK(wbb.get_max() == rbb.get_max());

    CHECK(wbb.get_x_size() == rbb.get_x_size());
    CHECK(wbb.get_y_size() == rbb.get_y_size());

    CHECK(wlyt.num_cells() == rlyt.num_cells());
    CHECK(wlyt.num_pis() == rlyt.num_pis());
    CHECK(wlyt.num_pos() == rlyt.num_pos());

    wlyt.foreach_cell([&wlyt, &rlyt](const auto& c) { CHECK(wlyt.get_cell_type(c) == rlyt.get_cell_type(c)); });

    if constexpr (has_foreach_sidb_defect_v<WLyt> && has_get_sidb_defect_v<RLyt>)
    {
        wlyt.foreach_sidb_defect(
            [&rlyt](const auto& cd)
            {
                const auto& wc = cd.first;
                const auto& wd = cd.second;

                const auto& rd = rlyt.get_sidb_defect(wc);

                CHECK(wd.type == rd.type);
                CHECK(wd.charge == rd.charge);
                CHECK(wd.epsilon_r == rd.epsilon_r);
                CHECK(wd.lambda_tf == rd.lambda_tf);
            });
    }
}

TEST_CASE("Write empty SQD layout", "[sqd]")
{
    using sidb_layout = sidb_cell_clk_lyt;

    const sidb_layout layout{{}, "empty"};

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lattice_layout{layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream, "empty");

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write single-dot SQD layout", "[sqd]")
{
    using sidb_layout = sidb_cell_clk_lyt;

    sidb_layout layout{{2, 2}};
    layout.assign_cell_type({1, 2}, sidb_technology::cell_type::NORMAL);

    const sidb_lattice<sidb_100_lattice, sidb_layout> lattice_layout{layout};

    std::stringstream layout_stream{};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write single-dot SQD layout with SiQAD coordinates", "[sqd]")
{
    sidb_cell_clk_lyt_siqad layout{{2, 2}};
    layout.assign_cell_type({1, 2}, sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    write_sqd_layout(sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>{layout}, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>>(layout_stream);

    CHECK(read_layout.get_cell_type({1, 4}) == sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    CHECK(layout.get_cell_type({1, 2}) == sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
}

TEST_CASE("Write multi-dot SQD layout", "[sqd]")
{
    using sidb_layout = sidb_cell_clk_lyt;

    sidb_layout layout{{4, 4}};
    layout.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 1}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 2}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 3}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 4}, sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lattice_layout{layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write multi-dot SQD layout with differing dot types", "[sqd]")
{
    using sidb_layout = sidb_cell_clk_lyt;

    sidb_layout layout{{4, 4}};
    layout.assign_cell_type({0, 0}, sidb_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 1}, sidb_technology::cell_type::OUTPUT);
    layout.assign_cell_type({0, 2}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 3}, sidb_technology::cell_type::OUTPUT);
    layout.assign_cell_type({4, 4}, sidb_technology::cell_type::INPUT);

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lattice_layout{layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write Bestagon SQD layout", "[sqd]")
{
    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;
    using sidb_layout = sidb_cell_clk_lyt;

    auto g_layout = blueprints::row_clocked_and_xor_gate_layout<gate_layout>();
    g_layout.set_layout_name("Bestagon");

    const auto c_layout = apply_gate_library<sidb_layout, sidb_bestagon_library>(g_layout);

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lattice_layout{c_layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream, "Bestagon");

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write defective surface SQD layout", "[sqd]")
{
    static const std::map<cell<sidb_cell_clk_lyt>, sidb_defect> defect_map{
        {{{0, 0}, sidb_defect{sidb_defect_type::NONE}},
         {{0, 1}, sidb_defect{sidb_defect_type::DB}},
         {{0, 2}, sidb_defect{sidb_defect_type::SI_VACANCY}},
         {{0, 3}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}},
         {{0, 4}, sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}},
         {{0, 5}, sidb_defect{sidb_defect_type::ONE_BY_ONE}},
         {{0, 6}, sidb_defect{sidb_defect_type::THREE_BY_ONE}},
         {{0, 7}, sidb_defect{sidb_defect_type::SILOXANE}},
         {{0, 8}, sidb_defect{sidb_defect_type::RAISED_SI}},
         {{0, 9}, sidb_defect{sidb_defect_type::MISSING_DIMER}},
         {{0, 10}, sidb_defect{sidb_defect_type::ETCH_PIT}},
         {{0, 11}, sidb_defect{sidb_defect_type::STEP_EDGE}},
         {{0, 12}, sidb_defect{sidb_defect_type::GUNK}},
         {{0, 13}, sidb_defect{sidb_defect_type::UNKNOWN}}}};

    const sidb_cell_clk_lyt lyt{aspect_ratio<sidb_cell_clk_lyt>{0, defect_map.size() - 1}};

    sidb_defect_surface<sidb_cell_clk_lyt> defect_layout{lyt};

    // assign defects
    for (const auto& [c, d] : defect_map)
    {
        defect_layout.assign_sidb_defect(c, d);
    }

    const sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt>> lattice_layout{defect_layout};

    std::stringstream layout_stream{};

    write_sqd_layout(sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt>>{defect_layout},
                     layout_stream);

    const auto read_layout =
        read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt>>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write multi-dot SQD layout based on SiQAD coordinates", "[sqd]")
{
    sidb_cell_clk_lyt_siqad layout{{4, 4}};
    layout.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 1}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 2}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 3}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 4}, sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad> lattice_layout{layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}

TEST_CASE("Write defective surface SQD layout based on SiQAD coordinates", "[sqd]")
{
    static const std::map<cell<sidb_cell_clk_lyt_siqad>, sidb_defect> defect_map{
        {{{0, 0, 1}, sidb_defect{sidb_defect_type::NONE}},
         {{0, 1}, sidb_defect{sidb_defect_type::DB}},
         {{0, 2}, sidb_defect{sidb_defect_type::SI_VACANCY}},
         {{0, 3}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}},
         {{0, 4, 1}, sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}},
         {{0, 5, 1}, sidb_defect{sidb_defect_type::ONE_BY_ONE}},
         {{0, 6}, sidb_defect{sidb_defect_type::THREE_BY_ONE}},
         {{0, 7}, sidb_defect{sidb_defect_type::SILOXANE}},
         {{0, 8}, sidb_defect{sidb_defect_type::RAISED_SI}},
         {{0, 9}, sidb_defect{sidb_defect_type::MISSING_DIMER}},
         {{0, 10, 1}, sidb_defect{sidb_defect_type::ETCH_PIT}},
         {{0, 11}, sidb_defect{sidb_defect_type::STEP_EDGE}},
         {{0, 12, 0}, sidb_defect{sidb_defect_type::GUNK}},
         {{0, 13}, sidb_defect{sidb_defect_type::UNKNOWN}}}};

    const sidb_cell_clk_lyt_siqad lyt{aspect_ratio<sidb_cell_clk_lyt_siqad>{0, defect_map.size() - 1}};

    sidb_defect_surface<sidb_cell_clk_lyt_siqad> defect_layout{lyt};

    // assign defects
    for (const auto& [c, d] : defect_map)
    {
        defect_layout.assign_sidb_defect(c, d);
    }

    std::stringstream layout_stream{};

    const sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>> lattice_layout{defect_layout};

    write_sqd_layout(lattice_layout, layout_stream);

    const auto read_layout =
        read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>>(layout_stream);

    compare_written_and_read_layout(lattice_layout, read_layout);
}
