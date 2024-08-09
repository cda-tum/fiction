//
// Created by marcel on 17.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/layouts/coordinates.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/technology_mapping_library.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/io/genlib_reader.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/tech_library.hpp>

using namespace fiction;

TEST_CASE("Simulation", "[mockturtle]")
{
    // adapted from mockturtle/test/networks/klut.cpp

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

    auto and_or_layout   = blueprints::and_or_gate_layout<gate_layout>();
    auto xor_maj_layout  = blueprints::xor_maj_gate_layout<gate_layout>();
    auto crossing_layout = blueprints::crossing_layout<gate_layout>();

    kitty::dynamic_truth_table tt_maj(3u), tt_and(2u), tt_or(2u);
    kitty::create_from_hex_string(tt_maj, "e8");
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
    kitty::create_from_hex_string(tt_3xor2, "66");  // 2-input XOR in three variables (1 xor 2)

    REQUIRE(xor_maj_tts.size() == 2);
    CHECK(xor_maj_tts[0] == tt_maj);
    CHECK(xor_maj_tts[1] == tt_3xor2);

    auto crossing_tts = mockturtle::simulate<kitty::dynamic_truth_table>(
        crossing_layout,
        mockturtle::default_simulator<kitty::dynamic_truth_table>(static_cast<unsigned>(crossing_layout.num_pis())));

    kitty::dynamic_truth_table tt_and12(4u), tt_and34(4u);

    kitty::create_from_hex_string(tt_and12, "8888");  // 2-input AND in four variables (1 and 2)
    kitty::create_from_hex_string(tt_and34, "f000");  // 2-input AND in four variables (3 and 4)

    REQUIRE(crossing_tts.size() == 2);
    CHECK(crossing_tts[0] == tt_and12);
    CHECK(crossing_tts[1] == tt_and34);

    SECTION("Synchronization elements")
    {
        using se_layout = gate_level_layout<
            synchronization_element_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>>;

        REQUIRE(mockturtle::has_compute_v<se_layout, kitty::dynamic_truth_table>);

        auto se_or_layout = blueprints::se_gate_layout<se_layout>();

        auto se_or_tts = mockturtle::simulate<kitty::dynamic_truth_table>(
            se_or_layout,
            mockturtle::default_simulator<kitty::dynamic_truth_table>(static_cast<unsigned>(se_or_layout.num_pis())));

        REQUIRE(se_or_tts.size() == 1);
        CHECK(se_or_tts[0] == tt_or);
    }
}

template <typename Ntk, typename Lib>
technology_network map(const Ntk& ntk, const Lib& lib)
{
    return mockturtle::map<technology_network>(ntk, lib);
}

template <typename Ntk, typename Lib>
void check(const Ntk& ntk, const Lib& lib)
{
    check_eq(ntk, map(ntk, lib));
}

template <typename Lib>
void check_all(const Lib& lib)
{
    check(blueprints::maj4_network<mockturtle::aig_network>(), lib);
    check(blueprints::nary_operation_network<mockturtle::aig_network>(), lib);
    check(blueprints::full_adder_network<mockturtle::aig_network>(), lib);
}

TEST_CASE("Technology mapping", "[mockturtle]")
{
    // instantiate a technology mapping library
    std::stringstream library_stream{};

    std::vector<mockturtle::gate> gates{};

    SECTION("QCA ONE library")
    {
        library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                       << fiction::GATE_AND2 << fiction::GATE_OR2 << fiction::GATE_MAJ3 << fiction::DECAY_MAJ3;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        mockturtle::tech_library<3> gate_lib{gates};

        check_all(gate_lib);
    }
    SECTION("Bestagon library")
    {
        library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                       << fiction::GATE_AND2 << fiction::GATE_NAND2 << fiction::GATE_OR2 << fiction::GATE_NOR2
                       << fiction::GATE_XOR2 << fiction::GATE_XNOR2;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        mockturtle::tech_library<2> gate_lib{gates};

        check_all(gate_lib);
    }
    SECTION("Marakkalage 3-input library")
    {
        library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                       << fiction::GATE_AND3 << fiction::GATE_XOR_AND << fiction::GATE_OR_AND << fiction::GATE_ONEHOT
                       << fiction::GATE_MAJ3 << fiction::GATE_GAMBLE << fiction::GATE_DOT << fiction::GATE_MUX
                       << fiction::GATE_AND_XOR << fiction::DECAY_AND3 << fiction::DECAY_XOR_AND
                       << fiction::DECAY_OR_AND << fiction::DECAY_ONEHOT << fiction::DECAY_MAJ3 << fiction::DECAY_GAMBLE
                       << fiction::DECAY_DOT << fiction::DECAY_MUX << fiction::DECAY_AND_XOR;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        mockturtle::tech_library<3> gate_lib{gates};

        check_all(gate_lib);
    }
}
