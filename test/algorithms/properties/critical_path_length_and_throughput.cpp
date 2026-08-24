//
// Created by marcel on 05.11.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/print_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/verification/critical_path_length_and_throughput.hpp>

#include <mockturtle/views/depth_view.hpp>

#include <type_traits>

using namespace fiction;

template <typename Lyt>
void check_critical_path_length(const Lyt& lyt, const verification::cp_and_tp& cp_tp) noexcept
{
    mockturtle::depth_view depth_lyt{lyt};

    CHECK(cp_tp.critical_path_length == depth_lyt.depth() + 1);  // + 1 because depth_view does not count POs
}

template <typename Lyt>
void check(const Lyt& lyt, const uint64_t throughput) noexcept
{
    const auto cp_tp = verification::critical_path_length_and_throughput(lyt);

    check_critical_path_length(lyt, cp_tp);
    CHECK(cp_tp.throughput == throughput);
}

TEST_CASE("Balanced layout", "[throughput]")
{
    using gate_layout = layouts::gate_level_layout<
        layouts::clocked_layout<layouts::tile_based_layout<layouts::cartesian_layout<layouts::coords::offset>>>>;

    check(blueprints::and_or_gate_layout<gate_layout>(), 1);
    check(blueprints::xor_maj_gate_layout<gate_layout>(), 1);
    check(blueprints::or_not_gate_layout<gate_layout>(), 1);
    check(blueprints::fanout_layout<gate_layout>(), 1);
    check(blueprints::crossing_layout<gate_layout>(), 1);

    SECTION("Synchronization Elements")
    {
        using se_gate_layout = layouts::gate_level_layout<layouts::synchronization_element_layout<
            layouts::clocked_layout<layouts::tile_based_layout<layouts::cartesian_layout<layouts::coords::offset>>>>>;

        check(blueprints::se_gate_layout<se_gate_layout>(), 1);
    }
}

TEST_CASE("Unbalanced layout", "[throughput]")
{
    using gate_layout = layouts::gate_level_layout<
        layouts::clocked_layout<layouts::tile_based_layout<layouts::cartesian_layout<layouts::coords::offset>>>>;

    check(blueprints::unbalanced_and_layout<gate_layout>(), 2);
}
