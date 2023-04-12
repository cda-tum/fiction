//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include "fiction/utils/debug/network_writer.hpp"

#include <iostream>

using namespace fiction;

TEST_CASE("TEST", "[ordering]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto mux21 = blueprints::test_inv_flag<mockturtle::names_view<technology_network>>();

    orthogonal_physical_design_stats stats{};

    const auto layout = orthogonal_ordering_network<gate_layout>(mux21, {}, &stats);

    fiction::debug::write_dot_layout(layout);

    std::cout << "PROCESSING ORDERING_NW "<< std::endl;
    std::cout << "Size: " <<(stats.x_size) * (stats.y_size) << std::endl;
    std::cout << "w: " <<(stats.x_size)<< std::endl;
    std::cout << "h: " <<(stats.y_size) << std::endl;
    std::cout << "Time: " << mockturtle::to_seconds(stats.time_total) << std::endl;
    std::cout << "Num Gates: "  << stats.num_gates << std::endl;
    std::cout << "Num Wires: " << stats.num_wires<< std::endl;

    gate_level_drvs(layout);
}