//
// Created by marcel on 17.05.21.
//

#include "catch.hpp"
#include "clocked_layout.hpp"
#include "gate_level_layout.hpp"
#include "layout_blueprints.hpp"
#include "tile_based_layout.hpp"

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/traits.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("Simulation", "[mockturtle]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

    auto and_or_layout  = blueprints::and_or_gate_layout<gate_layout>();
    auto xor_maj_layout = blueprints::xor_maj_gate_layout<gate_layout>();

    kitty::dynamic_truth_table tt_maj(3u), tt_xor(2u), tt_and(2u), tt_or(2u);
    kitty::create_from_hex_string(tt_maj, "e8");
    kitty::create_from_hex_string(tt_xor, "6");
    kitty::create_from_hex_string(tt_and, "8");
    kitty::create_from_hex_string(tt_or, "e");

    auto and_or_tts = mockturtle::simulate<kitty::dynamic_truth_table>(
        and_or_layout,
        mockturtle::default_simulator<kitty::dynamic_truth_table>(static_cast<unsigned>(and_or_layout.num_pis())));
    
    REQUIRE(and_or_tts.size() == 2);
    CHECK(and_or_tts[0] == tt_and);
    CHECK(and_or_tts[1] == tt_or);

    auto xor_maj_tts = mockturtle::simulate<kitty::dynamic_truth_table>(
        xor_maj_layout,
        mockturtle::default_simulator<kitty::dynamic_truth_table>(static_cast<unsigned>(xor_maj_layout.num_pis())));

    kitty::dynamic_truth_table tt_3xor2(3u);
    kitty::create_from_hex_string(tt_3xor2, "66");  // 2-input XOR on three variables

    REQUIRE(xor_maj_tts.size() == 2);
    CHECK(xor_maj_tts[0] == tt_maj);
    CHECK(xor_maj_tts[1] == tt_3xor2);
}
