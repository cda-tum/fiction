//
// Created by marcel on 24.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Empty layout iteration", "[bdl-input-iterator]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    bdl_input_iterator<layout> bii{lyt};

    CHECK((*bii).num_cells() == 0);

    ++bii;

    CHECK((*bii).num_cells() == 0);

    auto bii_cp = bii++;

    CHECK((*bii).num_cells() == 0);
    CHECK((*bii_cp).num_cells() == 0);
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

    bdl_input_iterator<layout> bii{lyt};

    // layout at input state 0
    const auto& lyt_0 = *bii;

    // the iterator should have toggled the second input cell
    CHECK(lyt_0.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt_0.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::EMPTY);

    ++bii;

    // layout at input state 1
    const auto& lyt_1 = *bii;

    // the iterator should have toggled the first input cell
    CHECK(lyt_1.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::EMPTY);
    CHECK(lyt_1.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::INPUT);

    // doing another iteration should overflow and set it back to 0
    ++bii;

    const auto& lyt_2 = *bii;

    CHECK(lyt_2.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt_2.get_cell_type({2, 0, 0}) == sidb_technology::cell_type::EMPTY);
}

TEST_CASE("SiQAD's AND gate iteration", "[bdl-input-iterator]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    bdl_input_iterator<layout> bii{lyt};

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
