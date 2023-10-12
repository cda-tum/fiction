//
// Created by marcel on 10.10.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/physical_design/sa_placement.hpp>
#include <fiction/algorithms/verification/design_rule_violations.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <type_traits>

using namespace fiction;

void check_stats(const sa_placement_stats& st) noexcept
{
    CHECK(st.initial_cost > 0);
    CHECK(st.final_cost > 0);
    CHECK(st.final_cost <= st.initial_cost);
    CHECK(mockturtle::to_seconds(st.time_total) > 0);
}

template <typename Lyt>
void check_layout(const Lyt& lyt) noexcept
{
    gate_level_drv_params params{};
    params.unplaced_nodes           = true;
    params.placed_dead_nodes        = true;
    params.non_adjacent_connections = false;
    params.missing_connections      = false;
    params.crossing_gates           = false;
    params.clocked_data_flow        = false;
    params.has_io                   = true;
    params.empty_io                 = true;
    params.io_pins                  = true;
    params.border_io                = false;

    gate_level_drv_stats stats{};

    gate_level_drvs(lyt, params, &stats);

    CHECK(stats.drvs == 0);
    CHECK(stats.warnings == 0);
}

template <typename Lyt, typename Ntk>
void place_and_check(const Ntk& ntk) noexcept
{
    const sa_placement_params params{5, 5, "2DDWave", temperature_schedule::LINEAR, 10000, 1, 100};
    sa_placement_stats        stats{};

    const auto lyt = sa_placement<Lyt>(ntk, params, &stats);

    CHECK(lyt.num_pis() == ntk.num_pis());
    CHECK(lyt.num_gates() == ntk.num_gates());
    CHECK(lyt.num_pos() == ntk.num_pos());

    check_layout(lyt);
    check_stats(stats);
}

TEMPLATE_TEST_CASE(
    "Small network placement", "[sa-placement]",
    // (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    // (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>)
    // (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>)
)
{
    place_and_check<TestType>(blueprints::unbalanced_and_inv_network<technology_network>());
}

// TEST_CASE("Name conservation after placement", "[sa-placement]")
//{
//     using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
//
//     auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
//     maj.set_network_name("maj");
//
//     const auto layout = orthogonal<gate_layout>(maj);
//
//     // network name
//     CHECK(layout.get_layout_name() == "maj");
//
//     // PI names
//     CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
//     CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
//     CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI
//
//     // PO names
//     CHECK(layout.get_output_name(0) == "f");
// }
