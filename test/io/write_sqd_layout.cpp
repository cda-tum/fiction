//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_surface.hpp>
#include <fiction/traits.hpp>

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

    wlyt.foreach_cell([&rlyt](const auto& c) { CHECK(rlyt.get_cell_type(c) == sidb_technology::cell_type::NORMAL); });

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
    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const sidb_layout layout{{}, "empty"};

    std::stringstream layout_stream{};

    write_sqd_layout(layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_layout>(layout_stream, "empty");

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write single-dot SQD layout", "[sqd]")
{
    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    sidb_layout layout{{2, 2}};
    layout.assign_cell_type({1, 2}, sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    write_sqd_layout(layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_layout>(layout_stream);

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write multi-dot SQD layout", "[sqd]")
{
    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    sidb_layout layout{{4, 4}};
    layout.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 1}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 2}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({0, 3}, sidb_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 4}, sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    write_sqd_layout(layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_layout>(layout_stream);

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write bestagon SQD layout", "[sqd]")
{
    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;
    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    auto g_layout = blueprints::row_clocked_and_xor_gate_layout<gate_layout>();
    g_layout.set_layout_name("bestagon");

    const auto c_layout = apply_gate_library<sidb_layout, sidb_bestagon_library>(g_layout);

    std::stringstream layout_stream{};

    write_sqd_layout(c_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_layout>(layout_stream, "bestagon");

    compare_written_and_read_layout(c_layout, read_layout);
}

TEST_CASE("Write defective surface SQD layout", "[sqd]")
{
    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    static const std::map<cell<sidb_layout>, sidb_defect> defect_map{
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

    const sidb_layout lyt{aspect_ratio<sidb_layout>{0, defect_map.size() - 1}};

    sidb_surface<sidb_layout> defect_layout{lyt};

    // assign defects
    for (const auto& [c, d] : defect_map)
    {
        defect_layout.assign_sidb_defect(c, d);
    }

    std::stringstream layout_stream{};

    write_sqd_layout(defect_layout, layout_stream);

    const auto read_layout = read_sqd_layout<sidb_surface<sidb_layout>>(layout_stream);

    compare_written_and_read_layout(defect_layout, read_layout);
}
