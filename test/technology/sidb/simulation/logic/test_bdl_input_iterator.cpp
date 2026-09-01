//
// Created by marcel on 24.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/layout_utils.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/surfaces/lattice.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

using namespace fiction;

TEST_CASE("BDL Input Iterator Traits", "[bdl-input-iterator]")
{
    using layout = sidb_100_cell_clk_lyt_siqad;

    CHECK(std::is_same_v<std::iterator_traits<sidb::simulation::logic::bdl_input_iterator<layout>>::iterator_category,
                         std::random_access_iterator_tag>);

    CHECK(
        std::is_same_v<std::iterator_traits<sidb::simulation::logic::bdl_input_iterator<layout>>::value_type, layout>);

    CHECK(std::is_same_v<std::iterator_traits<sidb::simulation::logic::bdl_input_iterator<layout>>::difference_type,
                         int64_t>);
}

TEST_CASE("Operators", "[bdl-input-iterators]")
{
    using layout = sidb_100_cell_clk_lyt_siqad;

    const layout lyt{};

    sidb::simulation::logic::bdl_input_iterator<layout> bii{lyt};

    CHECK(bii == 0ull);
    CHECK(bii != 1ull);

    CHECK(bii < 1ull);
    CHECK(bii <= 1ull);

    CHECK(bii >= 0ull);

    // increment
    ++bii;

    CHECK(bii == 1ull);
    CHECK(bii != 0ull);

    CHECK(bii < 2ull);
    CHECK(bii <= 2ull);

    CHECK(bii > 0ull);
    CHECK(bii >= 0ull);

    // decrement
    --bii;

    CHECK(bii == 0ull);
    CHECK(bii != 1ull);

    CHECK(bii < 1ull);
    CHECK(bii <= 1ull);

    CHECK(bii >= 0ull);

    // increment assignment
    bii += 2ull;

    CHECK(bii == 2ull);
    CHECK(bii != 1ull);

    CHECK(bii < 3ull);
    CHECK(bii <= 3ull);

    CHECK(bii > 1ull);
    CHECK(bii >= 1ull);

    const auto bii_cp = bii;

    // decrement assignment
    bii -= 2ull;

    CHECK(bii == 0ull);
    CHECK(bii != 1ull);

    CHECK(bii < 1ull);
    CHECK(bii <= 1ull);

    CHECK(bii >= 0ull);

    // difference
    CHECK(bii_cp - bii == 2);

    // subscript
    CHECK(bii[0] == 0ull);
    CHECK(bii[1] == 1ull);
    CHECK(bii[2] == 2ull);
    CHECK(bii[3] == 3ull);
    CHECK(bii[4] == 4ull);
}

TEST_CASE("Empty layout iteration", "[bdl-input-iterator]")
{
    using layout = sidb_100_cell_clk_lyt_siqad;

    const layout lyt{};

    sidb::simulation::logic::bdl_input_iterator<layout> bii{lyt};

    CHECK(bii.num_input_pairs() == 0);
    CHECK((*bii).num_cells() == 0);

    // increment

    ++bii;

    CHECK(bii.num_input_pairs() == 0);
    CHECK((*bii).num_cells() == 0);

    auto bii_cp = bii++;

    CHECK(bii.num_input_pairs() == 0);
    CHECK(bii_cp.num_input_pairs() == 0);
    CHECK((*bii).num_cells() == 0);
    CHECK((*bii_cp).num_cells() == 0);

    // decrement

    --bii;

    CHECK(bii.num_input_pairs() == 0);
    CHECK((*bii).num_cells() == 0);

    auto bii_cm = bii--;

    CHECK(bii.num_input_pairs() == 0);
    CHECK((*bii).num_cells() == 0);

    CHECK(bii_cm.num_input_pairs() == 0);
    CHECK((*bii_cm).num_cells() == 0);
}

TEST_CASE("BDL wire iteration", "[bdl-input-iterator]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 0}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb::sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb::sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb::sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb::sidb_technology::cell_type::OUTPUT);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb::simulation::logic::bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{lat};

    CHECK((*bii).num_cells() == 7);  // 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs

    CHECK(bii.num_input_pairs() == 1);

    CHECK(bii == 0ull);

    // start by incrementing over all input states

    // layout at input state 0
    const auto& lyt_0 = *bii;

    // the iterator should have toggled the second input cell
    CHECK(lyt_0.get_cell_type({0, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);
    CHECK(lyt_0.get_cell_type({2, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);

    ++bii;
    CHECK(bii == 1ull);

    // layout at input state 1
    const auto& lyt_1 = *bii;

    // the iterator should have toggled the first input cell
    CHECK(lyt_1.get_cell_type({0, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1.get_cell_type({2, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);

    // doing another iteration should overflow and set it back to 0
    ++bii;
    CHECK(bii == 2ull);

    const auto& lyt_2 = *bii;

    CHECK(lyt_2.get_cell_type({0, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);
    CHECK(lyt_2.get_cell_type({2, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);

    // finally, decrement back to the initial state, doing another wrap-around

    --bii;
    CHECK(bii == 1ull);

    const auto& lyt_1_1 = *bii;

    CHECK(lyt_1_1.get_cell_type({0, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1_1.get_cell_type({2, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);

    --bii;
    CHECK(bii == 0ull);

    const auto& lyt_0_1 = *bii;

    CHECK(lyt_0_1.get_cell_type({0, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);
    CHECK(lyt_0_1.get_cell_type({2, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);
}

TEST_CASE("Mirrored BDL wire iteration", "[bdl-input-iterator]")
{
    // the same wire as in "BDL wire iteration", but with the input pair at the right end and the wire running to the
    // left, so that the dot closer to the end of the wire is the opposite one of the two. Under
    // `PERTURBER_DISTANCE_ENCODED`, an input of `1` places the perturber closer to the wire, so both orientations
    // together cover both outcomes of the distance comparison
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 0}, "mirrored BDL wire"};

    lyt.assign_cell_type({20, 0, 0}, sidb::sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 0, 0}, sidb::sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({14, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({2, 0, 0}, sidb::sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb::sidb_technology::cell_type::OUTPUT);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb::simulation::logic::bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{lat};

    REQUIRE(bii.num_input_pairs() == 1);

    // input state 0 keeps the perturber that is farther away from the wire, which is the right-hand dot here
    const auto& lyt_0 = *bii;

    CHECK(lyt_0.get_cell_type({20, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);
    CHECK(lyt_0.get_cell_type({18, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);

    ++bii;

    // input state 1 keeps the perturber that is closer to the wire, which is the left-hand dot here
    const auto& lyt_1 = *bii;

    CHECK(lyt_1.get_cell_type({20, 0, 0}) == sidb::sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1.get_cell_type({18, 0, 0}) == sidb::sidb_technology::cell_type::INPUT);
}

TEST_CASE("SiQAD's AND gate iteration", "[bdl-input-iterator]")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    const sidb::simulation::logic::detect_bdl_wires_params params{.threshold_bdl_interdistance = 2.0};

    SECTION("SiQAD coordinates, encode input 0 with the absence of perturbers")
    {
        sidb::simulation::logic::bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{
            lat, sidb::simulation::logic::bdl_input_iterator_params{
                     .bdl_wire_params  = params,
                     .input_bdl_config = sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::
                         PERTURBER_ABSENCE_ENCODED}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_0.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_0.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        }
    }

    SECTION("SiQAD coordinates")
    {
        sidb::simulation::logic::bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{
            lat, sidb::simulation::logic::bdl_input_iterator_params{.bdl_wire_params = params}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 1, 1}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        }
    }

    SECTION("cube coordinates")
    {
        const auto layout_cube = layouts::convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);
        sidb::simulation::logic::bdl_input_iterator bii{sidb_100_cell_clk_lyt_cube{layout_cube}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        }
    }

    SECTION("offset coordinates")
    {
        const auto layout_offset = layouts::convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);
        sidb::simulation::logic::bdl_input_iterator bii{sidb_100_cell_clk_lyt_cube{layout_offset}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 1}) == sidb::sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 3}) == sidb::sidb_technology::cell_type::INPUT);

                    break;
                }
                default:
                {
                    CHECK(false);
                }
            }
        }
    }
}

TEST_CASE("Generate BDL input pattern layouts", "[bdl-input-iterator]")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    const sidb::simulation::logic::bdl_input_iterator_params params{
        .bdl_wire_params = sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 2.0}};

    SECTION("One layout per input pattern, matching the iterator")
    {
        const auto layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(lat, params);

        REQUIRE(layouts.size() == 4);

        sidb::simulation::logic::bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{lat, params};

        for (uint64_t i = 0; i < layouts.size(); ++i, ++bii)
        {
            const auto& expected = *bii;

            CHECK(layouts[i].num_cells() == expected.num_cells());

            expected.foreach_cell([&layouts, &expected, i](const auto& c)
                                  { CHECK(layouts[i].get_cell_type(c) == expected.get_cell_type(c)); });
        }
    }

    SECTION("Pre-detected input wires yield the same layouts")
    {
        const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
            lat, params.bdl_wire_params, sidb::simulation::logic::bdl_wire_selection::INPUT);

        const auto layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(lat, params);
        const auto layouts_with_wires =
            sidb::simulation::logic::generate_bdl_input_pattern_layouts(lat, params, input_wires);

        REQUIRE(layouts.size() == layouts_with_wires.size());

        for (uint64_t i = 0; i < layouts.size(); ++i)
        {
            layouts[i].foreach_cell([&layouts, &layouts_with_wires, i](const auto& c)
                                    { CHECK(layouts_with_wires[i].get_cell_type(c) == layouts[i].get_cell_type(c)); });
        }
    }

    SECTION("The layouts are independent deep copies")
    {
        // a shallow copy would make all entries share cell storage, which passes every other check here but
        // corrupts the layouts as soon as they are read concurrently
        auto layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(lat, params);

        REQUIRE(layouts.size() == 4);

        const cell<sidb_100_cell_clk_lyt_siqad> probe{100, 100, 0};

        REQUIRE(layouts[0].get_cell_type(probe) == sidb::sidb_technology::cell_type::EMPTY);

        layouts[0].assign_cell_type(probe, sidb::sidb_technology::cell_type::NORMAL);

        CHECK(layouts[0].get_cell_type(probe) == sidb::sidb_technology::cell_type::NORMAL);

        for (uint64_t i = 1; i < layouts.size(); ++i)
        {
            CHECK(layouts[i].get_cell_type(probe) == sidb::sidb_technology::cell_type::EMPTY);
        }
    }
}
