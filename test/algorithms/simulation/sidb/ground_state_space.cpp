//
// Created by Willem Lambooy on 06/02/2024.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/ground_state_space.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>

#include <algorithm>
#include <limits>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Ground space space of a layout comprising of 13 SiDBs", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    // initialize SiDB layout
    TestType lyt{};

    // design random SiDB layout
    lyt.assign_cell_type({11, 15, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({37, 8, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({37, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 17, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({39, 10, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({22, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({17, 2, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({27, 13, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({27, 16, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 10, 1}, TestType::cell_type::NORMAL);

    ground_state_space gss{lyt};

    gss.update_charge_spaces();

    sidb_cluster_charge_state_space s{
        sidb_cluster_charge_state{sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL},
        sidb_cluster_charge_state{sidb_charge_state::NEUTRAL, sidb_charge_state::NEGATIVE},
        sidb_cluster_charge_state{sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE},
        sidb_cluster_charge_state{sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL},
        sidb_cluster_charge_state{sidb_charge_state::NEGATIVE, sidb_charge_state::POSITIVE}};

    CHECK(s.size() == 4);
    //    CHECK(*s.cbegin() == sidb_cluster_charge_state{sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE});
    //    CHECK(*s.crbegin() == sidb_cluster_charge_state{sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL});

    sidb_cluster_charge_state m{sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL};
    sidb_cluster_charge_state m_{sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL};
    //    CHECK(*--s.crbegin().base() == m);
    //    CHECK(*--(--s.crbegin().base()) ==
    //          sidb_cluster_charge_state{sidb_charge_state::POSITIVE, sidb_charge_state::NEGATIVE});
    //    sidb_cluster_charge_state_space s{};

    //    while (!gss.sweep());
    //
    //    REQUIRE(gss.cl_min.get_phys_params().mu_minus == -0.32);
    //
    //    gss.cl_min.foreach_cell(
    //        [&](const auto& cell)
    //        {
    //            if (cell != coordinate<TestType>{5, 3, 0})
    //            {
    //                CHECK(gss.cl_min.get_charge_state(cell) == sidb_charge_state::NEGATIVE);
    //            }
    //            else
    //            {
    //                CHECK(gss.cl_min.get_charge_state({5, 3, 0}) == sidb_charge_state::NEUTRAL);
    //            }
    //        });
    //
    //    gss.cl_max.foreach_cell(
    //        [&](const auto& cell)
    //        {
    //            if (cell != coordinate<TestType>{5, 3, 0})
    //            {
    //                CHECK(gss.cl_max.get_charge_state(cell) == sidb_charge_state::NEGATIVE);
    //            }
    //            else
    //            {
    //                CHECK(gss.cl_max.get_charge_state({5, 3, 0}) == sidb_charge_state::NEUTRAL);
    //            }
    //        });
    //
    //    for (uint64_t i = 0; i < gss.possible_assignments.size(); ++i)
    //    {
    //        REQUIRE(gss.possible_assignments[i].size() == 1);
    //
    //        if (gss.cl_min.get_sidb_order()[i] != coordinate<TestType>{5, 3, 0})
    //        {
    //            CHECK(*gss.possible_assignments[i].cbegin() == sidb_charge_state::NEGATIVE);
    //        }
    //        else
    //        {
    //            CHECK(*gss.possible_assignments[i].cbegin() == sidb_charge_state::NEUTRAL);
    //        }
    //    }
}

TEMPLATE_TEST_CASE(
    "Ground space space of a layout where a positive charge occurs", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({11, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);

    ground_state_space gss{lyt};
    //    while (!gss.sweep());

    int k = 0;

    CHECK(k == 0);

    //    REQUIRE(gss.cl_min.get_phys_params().mu_minus == -0.32);
}

TEMPLATE_TEST_CASE(
    "Ground space space a 7 DB layout", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);  // neut
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);  // neut
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);

    //    const auto res = quickexact(lyt);

    ground_state_space gss{lyt};
    gss.debug = true;
    while (!gss.update_charge_spaces())
        ;

    for (const sidb_cluster_ptr& c : gss.clustering)
    {
        CHECK(std::find(c->charge_space.cbegin(), c->charge_space.cend(),
                        sidb_cluster_charge_state{sidb_charge_state::POSITIVE}) == c->charge_space.cend());
        if (c->uid == 1 || c->uid == 2 || c->uid == 4 || c->uid == 5)
        {
            CHECK(c->charge_space.size() == 2);
        }
        else
        {
            CHECK(c->charge_space.size() == 1);
        }
    }

    const sidb_cluster& top = *gss.compute_ground_state_space();

    CHECK(top.sidbs.size() == 7);

    CHECK_THAT(top.local_pot_bounds.first, Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
    CHECK_THAT(top.local_pot_bounds.second, Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));

    REQUIRE(top.charge_space.size() == 1);

    CHECK(top.charge_space.cbegin()->neg_count == 5);
    CHECK(top.charge_space.cbegin()->pos_count == 0);

    REQUIRE(top.charge_space.cbegin()->decompositions.size() == 1);
    REQUIRE(top.charge_space.cbegin()->decompositions.cbegin()->size() == 2);

    CHECK(top.charge_space.cbegin()->decompositions.cbegin()->cbegin()->first->sidbs == std::set{0ul, 1ul, 2ul});
    CHECK((top.charge_space.cbegin()->decompositions.cbegin()->cbegin()->second >> 32ull) == 2);
    CHECK(((top.charge_space.cbegin()->decompositions.cbegin()->cbegin()->second << 32ull) >> 32ull) == 0);

    CHECK(std::next(top.charge_space.cbegin()->decompositions.cbegin()->cbegin(), 1)->first->sidbs ==
          std::set{3ul, 4ul, 5ul, 6ul});
    CHECK((std::next(top.charge_space.cbegin()->decompositions.cbegin()->cbegin(), 1)->second >> 32ull) == 3);
    CHECK(((std::next(top.charge_space.cbegin()->decompositions.cbegin()->cbegin(), 1)->second << 32ull) >> 32ull) ==
          0);

    const charge_distribution_surface<TestType> cds{lyt};

    const std::set<uint64_t> neg_sidbs  = {0, 1, 3, 5, 6};
    const std::set<uint64_t> neut_sidbs = {2, 4};

    double min_neg_recv = -std::numeric_limits<double>::infinity();
    double max_neg_recv = std::numeric_limits<double>::infinity();

    for (const uint64_t i : neg_sidbs)
    {
        double sum = 0;
        for (const uint64_t j : neg_sidbs)
        {
            sum -= cds.get_chargeless_potential_by_indices(i, j);
        }
        min_neg_recv = std::max(min_neg_recv, sum);
        max_neg_recv = std::min(max_neg_recv, sum);
    }

    CHECK(min_neg_recv <=
          gss.template internal_pot_bound_value<ground_state_space<TestType>::bound_calculation_mode::LOWER,
                                                sidb_charge_state::NEGATIVE>((*top.children.cbegin())->parent,
                                                                             *top.charge_space.cbegin()));
    CHECK(max_neg_recv >=
          gss.template internal_pot_bound_value<ground_state_space<TestType>::bound_calculation_mode::UPPER,
                                                sidb_charge_state::NEGATIVE>((*top.children.cbegin())->parent,
                                                                             *top.charge_space.cbegin()));

    double min_neut_recv = -std::numeric_limits<double>::infinity();
    double max_neut_recv = std::numeric_limits<double>::infinity();

    for (const uint64_t i : neut_sidbs)
    {
        double sum = 0;
        for (const uint64_t j : neg_sidbs)
        {
            sum -= cds.get_chargeless_potential_by_indices(i, j);
        }
        min_neut_recv = std::max(min_neut_recv, sum);
        max_neut_recv = std::min(max_neut_recv, sum);
    }

    CHECK(min_neut_recv <=
          gss.template internal_pot_bound_value<ground_state_space<TestType>::bound_calculation_mode::LOWER>(
              (*top.children.cbegin())->parent, *top.charge_space.cbegin()));
    CHECK(max_neut_recv >=
          gss.template internal_pot_bound_value<ground_state_space<TestType>::bound_calculation_mode::UPPER>(
              (*top.children.cbegin())->parent, *top.charge_space.cbegin()));
}
