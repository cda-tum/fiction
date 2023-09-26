//
// Created by Simon Hofmann on 26.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/io/read_gate_level_layout.hpp>
#include <fiction/io/write_gate_level_layout.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
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

    CHECK(wlyt.num_pis() == rlyt.num_pis());
    CHECK(wlyt.num_pos() == rlyt.num_pos());
}

TEST_CASE("Write empty gate_level layout", "[sqd]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    const gate_layout layout{{}, "empty"};

    std::stringstream layout_stream{};

    write_gate_level_layout(layout, layout_stream);

    const auto read_layout = read_gate_level_layout<sidb_layout>(layout_stream, "empty");

    compare_written_and_read_layout(layout, read_layout);
}
