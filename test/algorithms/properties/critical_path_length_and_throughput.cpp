//
// Created by marcel on 05.11.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>

#include <mockturtle/views/depth_view.hpp>

#include <type_traits>

using namespace fiction;

template <typename Lyt>
void check_critical_path_length(const Lyt& lyt, const critical_path_length_and_throughput_stats& st) noexcept
{
    mockturtle::depth_view depth_lyt{lyt};

    CHECK(st.critical_path_length == depth_lyt.depth() + 1);  // + 1 because depth_view does not count POs
}

template <typename Lyt>
void check(const Lyt& lyt, const uint64_t throughput) noexcept
{
    critical_path_length_and_throughput_stats st{};
    critical_path_length_and_throughput(lyt, &st);

    check_critical_path_length(lyt, st);
    CHECK(st.throughput == throughput);
}

TEST_CASE("Balanced layout", "[throughput]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    check(blueprints::and_or_gate_layout<gate_layout>(), 1);
    check(blueprints::xor_maj_gate_layout<gate_layout>(), 1);
    check(blueprints::or_not_gate_layout<gate_layout>(), 1);
    check(blueprints::fanout_layout<gate_layout>(), 1);
    check(blueprints::crossing_layout<gate_layout>(), 1);

    SECTION("Synchronization Elements")
    {
        using se_gate_layout = gate_level_layout<
            synchronization_element_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>>;

        check(blueprints::se_gate_layout<se_gate_layout>(), 1);
    }
}

TEST_CASE("Unbalanced layout", "[throughput]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    check(blueprints::unbalanced_and_layout<gate_layout>(), 2);
}
