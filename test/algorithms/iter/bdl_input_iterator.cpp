//
// Created by marcel on 24.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

using namespace fiction;

TEST_CASE("Traits", "[bdl-input-iterator]")
{
    using layout = sidb_100_cell_clk_lyt_siqad;

    CHECK(std::is_same_v<std::iterator_traits<bdl_input_iterator<layout>>::iterator_category,
                         std::random_access_iterator_tag>);

    CHECK(std::is_same_v<std::iterator_traits<bdl_input_iterator<layout>>::value_type, layout>);

    CHECK(std::is_same_v<std::iterator_traits<bdl_input_iterator<layout>>::difference_type, int64_t>);
}

TEST_CASE("Operators", "[bdl-input-iterators]")
{
    using layout = sidb_100_cell_clk_lyt_siqad;

    const layout lyt{};

    bdl_input_iterator<layout> bii{lyt};

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

    bdl_input_iterator<layout> bii{lyt};

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

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{lat};

    CHECK((*bii).num_cells() == 7);  // 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs

    CHECK(bii.num_input_pairs() == 1);

    CHECK(bii == 0ull);

    // start by incrementing over all input states

    // layout at input state 0
    const auto& lyt_0 = *bii;

    // the iterator should have toggled the second input cell
    CHECK(lyt_0.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt_0.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::EMPTY);

    ++bii;
    CHECK(bii == 1ull);

    // layout at input state 1
    const auto& lyt_1 = *bii;

    // the iterator should have toggled the first input cell
    CHECK(lyt_1.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::INPUT);

    // doing another iteration should overflow and set it back to 0
    ++bii;
    CHECK(bii == 2ull);

    const auto& lyt_2 = *bii;

    CHECK(lyt_2.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt_2.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::EMPTY);

    // finally, decrement back to the initial state, doing another wrap-around

    --bii;
    CHECK(bii == 1ull);

    const auto& lyt_1_1 = *bii;

    CHECK(lyt_1_1.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1_1.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::INPUT);

    --bii;
    CHECK(bii == 0ull);

    const auto& lyt_0_1 = *bii;

    CHECK(lyt_0_1.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt_0_1.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::EMPTY);
}

TEST_CASE("SiQAD's AND gate iteration", "[bdl-input-iterator]")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    const detect_bdl_wires_params params{2.0};

    SECTION("SiQAD coordinates, encode input 0 with the absence of perturbers")
    {
        bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{
            lat, bdl_input_iterator_params{
                     params, bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_0.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_0.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::INPUT);

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
        bdl_input_iterator<sidb_100_cell_clk_lyt_siqad> bii{lat, bdl_input_iterator_params{params}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 1, 1}) == sidb_technology::cell_type::INPUT);

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
        const auto         layout_cube = convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);
        bdl_input_iterator bii{sidb_100_cell_clk_lyt_cube{layout_cube}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 3}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 3}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 3}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 3}) == sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 3}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 3}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 3}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 3}) == sidb_technology::cell_type::INPUT);

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
        const auto         layout_offset = convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);
        bdl_input_iterator bii{sidb_100_cell_clk_lyt_cube{layout_offset}};

        for (auto i = 0; bii < 4; ++bii, ++i)
        {
            switch (i)
            {
                case 0:
                {
                    const auto& lyt_0 = *bii;

                    CHECK(lyt_0.get_cell_type({0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({2, 3}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_0.get_cell_type({20, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_0.get_cell_type({18, 3}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 1:
                {
                    const auto& lyt_1 = *bii;

                    CHECK(lyt_1.get_cell_type({0, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_1.get_cell_type({2, 3}) == sidb_technology::cell_type::EMPTY);

                    CHECK(lyt_1.get_cell_type({20, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_1.get_cell_type({18, 3}) == sidb_technology::cell_type::INPUT);

                    break;
                }
                case 2:
                {
                    const auto& lyt_2 = *bii;

                    CHECK(lyt_2.get_cell_type({0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_2.get_cell_type({2, 3}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_2.get_cell_type({20, 1}) == sidb_technology::cell_type::INPUT);
                    CHECK(lyt_2.get_cell_type({18, 3}) == sidb_technology::cell_type::EMPTY);

                    break;
                }
                case 3:
                {
                    const auto& lyt_3 = *bii;

                    CHECK(lyt_3.get_cell_type({0, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({2, 3}) == sidb_technology::cell_type::INPUT);

                    CHECK(lyt_3.get_cell_type({20, 1}) == sidb_technology::cell_type::EMPTY);
                    CHECK(lyt_3.get_cell_type({18, 3}) == sidb_technology::cell_type::INPUT);

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
