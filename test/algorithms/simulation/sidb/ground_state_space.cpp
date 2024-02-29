//
// Created by Willem Lambooy on 06/02/2024.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/ground_state_space.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_set>

using namespace fiction;

// TEMPLATE_TEST_CASE(
//     "Ground space space of a layout comprising of 13 SiDBs", "[ground-state-space]",
//     (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
//     (charge_distribution_surface<cell_level_layout<sidb_technology,
//     clocked_layout<cartesian_layout<siqad::coord_t>>>>))
//{
//     // initialize SiDB layout
//     TestType lyt{};
//
//     // design random SiDB layout
//     lyt.assign_cell_type({11, 15, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({37, 8, 0}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({17, 6, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({37, 19, 1}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({4, 2, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({0, 17, 0}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({39, 10, 1}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({22, 19, 1}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({17, 2, 0}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({27, 13, 1}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({27, 16, 1}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({8, 10, 1}, TestType::cell_type::NORMAL);
//
//     ground_state_space gss{lyt};
//
//     gss.update_charge_spaces();
// }
//
// TEMPLATE_TEST_CASE(
//     "Ground space space of a layout where a positive charge occurs", "[ground-state-space]",
//     (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
//     (charge_distribution_surface<cell_level_layout<sidb_technology,
//     clocked_layout<cartesian_layout<siqad::coord_t>>>>))
//{
//     TestType lyt{};
//
//     lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({11, 0, 1}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({12, 0, 1}, TestType::cell_type::NORMAL);
//
//     lyt.assign_cell_type({18, 0, 0}, TestType::cell_type::NORMAL);
//     lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
//
//     ground_state_space gss{lyt};
//     //    while (!gss.sweep());
//
//     int k = 0;
//
//     CHECK(k == 0);
//
//     //    REQUIRE(gss.cl_min.get_phys_params().mu_minus == -0.32);
// }
//// template <typename Lyt, bound_calculation_mode bound, sidb_charge_state onto_cs>
//// static void verify_recv_pot_bound(const ground_state_space<Lyt>& gss) noexcept
////{
////     for (const sidb_cluster_ptr& c : gss.clustering)
////     {
////         if (ground_state_space<Lyt>::template onto_cs_pruned<onto_cs>(c))
////         {
////             continue;
////         }
////
////         double recv_pot_bound = 0;
////         for (const sidb_cluster_ptr& other_c : gss.clustering)
////         {
////             if (other_c != c)
////             {
////                 recv_pot_bound += ground_state_space<Lyt>::template get_proj_bound<bound, onto_cs>(other_c, c).V;
////             }
////         }
////
////         recv_pot_bound -= c->get_recv_ext_pot_bound<bound, onto_cs>();
////
////         CHECK_THAT(recv_pot_bound, Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
////         if (std::abs(recv_pot_bound) > physical_constants::POP_STABILITY_ERR)
////         {
////             std::cout << "BIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIP\n\n"
////                       << (bound == bound_calculation_mode::LOWER ? "LB" : "UB") << " ONTO "
////                       << charge_configuration_to_string(std::vector{onto_cs}) << "\nc: " << c->uid
////                       << "\nrecv_pot_bound: " << recv_pot_bound + c->get_recv_ext_pot_bound<bound, onto_cs>()
////                       << "\nstored_bound: " << c->get_recv_ext_pot_bound<bound, onto_cs>() << "\nclustering:\n";
////
////             for (const sidb_cluster_ptr& c_ : gss.clustering)
////             {
////                 std::cout << c_->uid << std::endl;
////             }
////
////             std::cout << std::endl;
////         }
////     }
//// }

TEMPLATE_TEST_CASE(
    "Ground state space of a 7 DB layout", "[ground-state-space]",
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

    //    ground_state_space gss_recv_pot{lyt};
    //
    //    while (gss_recv_pot.clustering.size() > 1)
    //    {
    //        while (!gss_recv_pot.update_charge_spaces())
    //        {
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEGATIVE>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(gss_recv_pot);
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEUTRAL>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(gss_recv_pot);
    //        }
    //        while (gss_recv_pot.move_up_hierarchy())
    //        {
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEGATIVE>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(gss_recv_pot);
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEUTRAL>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(gss_recv_pot);
    //        }
    //    }

    //    ground_state_space gss{lyt};

    //    while (!gss.update_charge_spaces())
    //        ;
    //
    //    for (const sidb_cluster_ptr& c : gss.clustering)
    //    {
    //        CHECK(std::find(c->charge_space.cbegin(), c->charge_space.cend(),
    //                        sidb_cluster_charge_state{sidb_charge_state::POSITIVE}) == c->charge_space.cend());
    //        if (c->uid == 1 || c->uid == 2 || c->uid == 4 || c->uid == 5)
    //        {
    //            CHECK(c->charge_space.size() == 2);
    //        }
    //        else
    //        {
    //            CHECK(c->charge_space.size() == 1);
    //        }
    //    }

    const auto& [top, time] = ground_state_space{lyt}.compute_ground_state_space();

    std::cout << "RUNTIME: " << (time.count() * 1000) << " ms" << std::endl;

    CHECK(top->sidbs.size() == 7);

    //    CHECK_THAT(top->local_pot_bounds[0], Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
    //    CHECK_THAT(top->local_pot_bounds[1], Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));

    REQUIRE(top->charge_space.size() == 1);

    CHECK(top->charge_space.cbegin()->neg_count == 5);
    CHECK(top->charge_space.cbegin()->pos_count == 0);

    REQUIRE(top->charge_space.cbegin()->compositions.size() == 1);
    REQUIRE(top->charge_space.cbegin()->compositions.cbegin()->size() == 2);

    if (top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.cluster->sidbs.size() == 3)
    {
        CHECK(top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.cluster->sidbs ==
              std::unordered_set{0ul, 1ul, 2ul});
        CHECK((top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.multiset_conf >> 32ull) == 2);
        CHECK(((top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.multiset_conf << 32ull) >> 32ull) ==
              0);

        CHECK(std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.cluster->sidbs ==
              std::unordered_set{3ul, 4ul, 5ul, 6ul});
        CHECK((std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.multiset_conf >>
               32ull) == 3);
        CHECK(((std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.multiset_conf
                << 32ull) >>
               32ull) == 0);
    }
    else
    {
        CHECK(std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.cluster->sidbs ==
              std::unordered_set{0ul, 1ul, 2ul});
        CHECK((std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.multiset_conf >>
               32ull) == 2);
        CHECK(((std::next(top->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)->first.multiset_conf
                << 32ull) >>
               32ull) == 0);

        CHECK(top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.cluster->sidbs ==
              std::unordered_set{3ul, 4ul, 5ul, 6ul});
        CHECK((top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.multiset_conf >> 32ull) == 3);
        CHECK(((top->charge_space.cbegin()->compositions.cbegin()->cbegin()->first.multiset_conf << 32ull) >> 32ull) ==
              0);
    }
    //
    //    const charge_distribution_surface<TestType> cds{lyt};
    //
    //    const std::set<uint64_t> neg_sidbs  = {0, 1, 3, 5, 6};
    //    const std::set<uint64_t> neut_sidbs = {2, 4};
    //
    //    double min_neg_recv = -std::numeric_limits<double>::infinity();
    //    double max_neg_recv = std::numeric_limits<double>::infinity();
    //
    //    for (const uint64_t i : neg_sidbs)
    //    {
    //        double sum = 0;
    //        for (const uint64_t j : neg_sidbs)
    //        {
    //            sum -= cds.get_chargeless_potential_by_indices(i, j);
    //        }
    //        min_neg_recv = std::max(min_neg_recv, sum);
    //        max_neg_recv = std::min(max_neg_recv, sum);
    //    }
    //
    //    //    CHECK(min_neg_recv <=
    //    //          gss.template internal_pot_bound_value<bound_calculation_mode::LOWER,
    //    // sidb_charge_state::NEGATIVE>((*top->children.cbegin())->parent,
    //    //                                                                             *top->charge_space.cbegin()));
    //    //    CHECK(max_neg_recv >=
    //    //          gss.template internal_pot_bound_value<bound_calculation_mode::UPPER,
    //    // sidb_charge_state::NEGATIVE>((*top->children.cbegin())->parent,
    //    //                                                                             *top->charge_space.cbegin()));
    //
    //    double min_neut_recv = -std::numeric_limits<double>::infinity();
    //    double max_neut_recv = std::numeric_limits<double>::infinity();
    //
    //    for (const uint64_t i : neut_sidbs)
    //    {
    //        double sum = 0;
    //        for (const uint64_t j : neg_sidbs)
    //        {
    //            sum -= cds.get_chargeless_potential_by_indices(i, j);
    //        }
    //        min_neut_recv = std::max(min_neut_recv, sum);
    //        max_neut_recv = std::min(max_neut_recv, sum);
    //    }
    //
    //    //    CHECK(min_neut_recv <=
    //    //          gss.template internal_pot_bound_value<bound_calculation_mode::LOWER>(
    //    //              (*top->children.cbegin())->parent, *top->charge_space.cbegin()));
    //    //    CHECK(max_neut_recv >=
    //    //          gss.template internal_pot_bound_value<bound_calculation_mode::UPPER>(
    //    //              (*top->children.cbegin())->parent, *top->charge_space.cbegin()));
}

TEMPLATE_TEST_CASE(
    "Ground state space of a 14 DB layout", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    // V1   V2      num
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({8, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({4, 7, 0}, TestType::cell_type::NORMAL);   //  0    -       11
    lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);   //  0    +       9
    lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);   //  -    -       13

    //    const auto res = quickexact(lyt);

    //    ground_state_space gss_recv_pot{lyt};
    //
    //    while (gss_recv_pot.clustering.size() > 1)
    //    {
    //        while (!gss_recv_pot.update_charge_spaces())
    //        {
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEGATIVE>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(gss_recv_pot);
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEUTRAL>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(gss_recv_pot);
    //        }
    //        while (gss_recv_pot.move_up_hierarchy())
    //        {
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEGATIVE>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(gss_recv_pot);
    //            verify_recv_pot_bound<TestType, bound_calculation_mode::LOWER,
    //            sidb_charge_state::NEUTRAL>(gss_recv_pot); verify_recv_pot_bound<TestType,
    //            bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(gss_recv_pot);
    //        }
    //    }

    ground_state_space gss{lyt};

    const auto& [top, time] = gss.compute_ground_state_space();

    std::cout << "RUNTIME: " << (time.count() * 1000) << " ms" << std::endl;
    std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;

    CHECK(top->sidbs.size() == 14);

    //    CHECK_THAT(top->local_pot_bounds[0], Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
    //    CHECK_THAT(top->local_pot_bounds[1], Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE(
    "Ground state space of a 28 DB layout", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    // V1   V2      num
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({8, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({4, 7, 0}, TestType::cell_type::NORMAL);   //  0    -       11
    lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);   //  0    +       9
    lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);   //  -    -       13

    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);   //  -    -       0
    lyt.assign_cell_type({10, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({12, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({8, 7, 0}, TestType::cell_type::NORMAL);   //  -    -       10
    lyt.assign_cell_type({9, 6, 0}, TestType::cell_type::NORMAL);   //  0    0       8
    lyt.assign_cell_type({14, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({12, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({12, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({13, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({18, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({16, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    //     const auto res = quickexact(lyt);

    ground_state_space gss{lyt};

    const auto& [top, time] = gss.compute_ground_state_space();

    std::cout << "RUNTIME: " << (time.count() * 1000) << " ms" << std::endl;
    std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;

    CHECK(top->sidbs.size() == 28);
}

TEMPLATE_TEST_CASE(
    "Ground state space of a 56 DB layout", "[ground-state-space-big]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    // V1   V2      num
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({8, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({4, 7, 0}, TestType::cell_type::NORMAL);   //  0    -       11
    lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);   //  0    +       9
    lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);   //  -    -       13

    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);   //  -    -       0
    lyt.assign_cell_type({10, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({12, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({8, 7, 0}, TestType::cell_type::NORMAL);   //  -    -       10
    lyt.assign_cell_type({9, 6, 0}, TestType::cell_type::NORMAL);   //  0    0       8
    lyt.assign_cell_type({14, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({12, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({12, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({13, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({18, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({16, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({16, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({18, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({20, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({16, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({17, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({22, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({20, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({22, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({24, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({20, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({21, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({26, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({24, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({24, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({26, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({28, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({24, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({25, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({30, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({28, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({28, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({30, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({28, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({29, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({34, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({32, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    ground_state_space gss{lyt};

    const auto& [top, time] = gss.compute_ground_state_space();

    std::cout << "RUNTIME: " << time.count() << " s" << std::endl;
    std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;

    CHECK(top->sidbs.size() == 56);
}

TEMPLATE_TEST_CASE(
    "Ground state space of a 112 DB layout", "[ground-state-space-big]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    // V1   V2      num
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({8, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({4, 7, 0}, TestType::cell_type::NORMAL);   //  0    -       11
    lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);   //  0    +       9
    lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);   //  -    -       13

    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);   //  -    -       0
    lyt.assign_cell_type({10, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({12, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({8, 7, 0}, TestType::cell_type::NORMAL);   //  -    -       10
    lyt.assign_cell_type({9, 6, 0}, TestType::cell_type::NORMAL);   //  0    0       8
    lyt.assign_cell_type({14, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({12, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({12, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({13, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({18, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({16, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({16, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({18, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({20, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({16, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({17, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({22, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({20, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({22, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({24, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({20, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({21, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({26, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({24, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({24, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({26, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({28, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({24, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({25, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({30, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({28, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({28, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({30, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({28, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({29, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({34, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({32, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13
    // V1   V2      num
    lyt.assign_cell_type({0 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({4 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({0 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({1 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({6 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({4 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({4 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({6 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({8 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({4 + 32, 7, 0}, TestType::cell_type::NORMAL);   //  0    -       11
    lyt.assign_cell_type({5 + 32, 6, 0}, TestType::cell_type::NORMAL);   //  0    +       9
    lyt.assign_cell_type({10 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({8 + 32, 8, 1}, TestType::cell_type::NORMAL);   //  -    -       13

    lyt.assign_cell_type({8 + 32, 0, 0}, TestType::cell_type::NORMAL);   //  -    -       0
    lyt.assign_cell_type({10 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({12 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({8 + 32, 7, 0}, TestType::cell_type::NORMAL);   //  -    -       10
    lyt.assign_cell_type({9 + 32, 6, 0}, TestType::cell_type::NORMAL);   //  0    0       8
    lyt.assign_cell_type({14 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({12 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({12 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({14 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({16 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({12 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({13 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({18 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({16 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({16 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({18 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({20 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({16 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({17 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({22 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({20 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({20 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({22 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({24 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({20 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({21 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({26 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({24 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    lyt.assign_cell_type({24 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({26 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({28 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  0    0       2

    lyt.assign_cell_type({24 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  -    -       10
    lyt.assign_cell_type({25 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    0       8
    lyt.assign_cell_type({30 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  0    -       6
    lyt.assign_cell_type({28 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    0       12

    lyt.assign_cell_type({28 + 32, 0, 0}, TestType::cell_type::NORMAL);  //  0    0       1
    lyt.assign_cell_type({30 + 32, 2, 0}, TestType::cell_type::NORMAL);  //  0    0       5
    lyt.assign_cell_type({32 + 32, 1, 0}, TestType::cell_type::NORMAL);  //  -    -       3

    lyt.assign_cell_type({28 + 32, 7, 0}, TestType::cell_type::NORMAL);  //  0    -       11
    lyt.assign_cell_type({29 + 32, 6, 0}, TestType::cell_type::NORMAL);  //  0    +       9
    lyt.assign_cell_type({34 + 32, 5, 1}, TestType::cell_type::NORMAL);  //  -    -       7
    lyt.assign_cell_type({32 + 32, 8, 1}, TestType::cell_type::NORMAL);  //  -    -       13

    ground_state_space gss{lyt};

    const auto& [top, time] = gss.compute_ground_state_space();

    std::cout << "RUNTIME: " << time.count() << " s" << std::endl;
    std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;

    CHECK(top->sidbs.size() == 112);
}

using sidb_lyt = sidb_cell_clk_lyt_siqad;

static bool verify_ground_state_space_result(const charge_distribution_surface<sidb_lyt>& valid_cl,
                                             const sidb_cluster_ptr&                      gss_node) noexcept
{
    sidb_cluster_charge_state cl_m_conf{};

    for (const uint64_t i : gss_node->sidbs)
    {
        cl_m_conf.add_charge(valid_cl.get_charge_state_by_index(i));
    }

    const sidb_cluster_charge_state_space::const_iterator found_m_conf = gss_node->charge_space.find(cl_m_conf);

    if (found_m_conf == gss_node->charge_space.cend())
    {
        return false;
    }
    else if (gss_node->sidbs.size() == 1)
    {
        return true;
    }

    bool found_charge_conf = false;

    for (const sidb_cluster_charge_state_composition& composition : found_m_conf->compositions)
    {
        bool composition_has_correct_charge_conf = true;

        for (const auto& [pst, _] : composition)
        {
            composition_has_correct_charge_conf &= verify_ground_state_space_result(valid_cl, pst.cluster);
        }

        found_charge_conf |= composition_has_correct_charge_conf;
    }

    return found_charge_conf;
}

TEST_CASE("Tiny fail 1", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({4, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({3, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  0    0       2
    lyt.assign_cell_type({4, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  0    0       2

    const sidb_simulation_result<sidb_lyt>& qe_res   = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyt);
    const auto& [top, time]                          = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }

    for (const charge_distribution_surface<sidb_lyt>& cl : exgs_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

TEST_CASE("Tiny fail 2", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({1, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -       0
    lyt.assign_cell_type({2, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       4
    lyt.assign_cell_type({2, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  0    0       2
    lyt.assign_cell_type({3, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  0    0       2

    const sidb_simulation_result<sidb_lyt>& qe_res   = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyt);
    const auto& [top, time]                          = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }

    for (const charge_distribution_surface<sidb_lyt>& cl : exgs_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

TEST_CASE("Tiny fail 3", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({2, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       0
    lyt.assign_cell_type({4, 0, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -       1
    lyt.assign_cell_type({2, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  -    +       2
    lyt.assign_cell_type({3, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  0    -       3

    const sidb_simulation_result<sidb_lyt>& qe_res   = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyt);
    const auto& [top, time]                          = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }

    for (const charge_distribution_surface<sidb_lyt>& cl : exgs_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

TEST_CASE("Tiny fail 4", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({2, 0, 0}, sidb_lyt::cell_type::NORMAL);  //  0    -       0
    lyt.assign_cell_type({1, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -       1
    lyt.assign_cell_type({3, 0, 0}, sidb_lyt::cell_type::NORMAL);  //  -    +       2
    lyt.assign_cell_type({4, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  0    -       3

    const sidb_simulation_result<sidb_lyt>& qe_res   = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyt);
    const auto& [top, time]                          = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }

    for (const charge_distribution_surface<sidb_lyt>& cl : exgs_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

TEST_CASE("Small fail", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({1, 0, 0}, sidb_lyt::cell_type::NORMAL);  //  0    -   0   0       0     -0
    lyt.assign_cell_type({4, 0, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       1     -
    lyt.assign_cell_type({0, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   0   +       2     -0+
    lyt.assign_cell_type({1, 1, 0}, sidb_lyt::cell_type::NORMAL);  //  0    +   0   -       3     -0+
    lyt.assign_cell_type({0, 1, 1}, sidb_lyt::cell_type::NORMAL);  //  +    +   0   -       4     -0+
    lyt.assign_cell_type({0, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   0       5     -0
    lyt.assign_cell_type({4, 2, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       6     -

    const sidb_simulation_result<sidb_lyt>& qe_res   = quickexact(lyt);
    const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyt);
    const auto& [top, time]                          = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }

    for (const charge_distribution_surface<sidb_lyt>& cl : exgs_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

TEST_CASE("Big fail", "[ground-state-space]")
{
    sidb_lyt lyt{};
    lyt.assign_cell_type({21, 0, 1}, sidb_lyt::cell_type::NORMAL);   //  0    -   0   0       0     -0
    lyt.assign_cell_type({24, 0, 1}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       1     -
    lyt.assign_cell_type({8, 2, 0}, sidb_lyt::cell_type::NORMAL);    //  -    -   0   +       2     -0+
    lyt.assign_cell_type({11, 3, 1}, sidb_lyt::cell_type::NORMAL);   //  0    +   0   -       3     -0+
    lyt.assign_cell_type({18, 3, 1}, sidb_lyt::cell_type::NORMAL);   //  +    +   0   -       4     -0+
    lyt.assign_cell_type({21, 6, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   0       5     -0
    lyt.assign_cell_type({7, 7, 1}, sidb_lyt::cell_type::NORMAL);    //  -    -   -   -       6     -
    lyt.assign_cell_type({16, 8, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({17, 8, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({9, 9, 0}, sidb_lyt::cell_type::NORMAL);    //  -    -   -   -       6     -
    lyt.assign_cell_type({14, 9, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({15, 9, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({6, 10, 1}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({9, 10, 1}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({2, 11, 0}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({10, 11, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       6     -
    lyt.assign_cell_type({3, 11, 1}, sidb_lyt::cell_type::NORMAL);   //  -    -   -   -       6     -
    lyt.assign_cell_type({24, 11, 1}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       6     -
    lyt.assign_cell_type({15, 12, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       6     -
    lyt.assign_cell_type({24, 12, 0}, sidb_lyt::cell_type::NORMAL);  //  -    -   -   -       6     -

    const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyt);
    const auto& [top, time]                        = ground_state_space{lyt}.compute_ground_state_space();

    for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
    {
        const bool verification = verify_ground_state_space_result(cl, top);
        CHECK(verification);
    }
}

static void verify_lyts(const std::vector<sidb_lyt>& lyts)
{
    for (uint64_t i = 0; i < lyts.size(); ++i)
    {
        std::cout << "LAYOUT NUMBER: " << i << std::endl;

        //        lyts[i].foreach_cell([](const cell<sidb_lyt>& c) { std::cout << fmt::format("{}", c) << std::endl; });

        try
        {
            const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyts[i]);

            std::cout << "QUICKEXACT RUNTIME: " << (qe_res.simulation_runtime.count() * 1000) << " ms" << std::endl;
            std::cout << "PHYSICALLY VALID CHARGE DISTRIBUTIONS: " << qe_res.charge_distributions.size() << std::endl;

            try
            {
                const auto& [top, time] = ground_state_space{lyts[i]}.compute_ground_state_space();

                std::cout << "RUNTIME: " << (time.count() * 1000) << " ms" << std::endl;
                std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;

                for (const charge_distribution_surface<sidb_lyt>& cl : qe_res.charge_distributions)
                {
                    const bool verification = verify_ground_state_space_result(cl, top);
                    CHECK(verification);
                    if (!verification)
                    {
                        //                    for (const cell<sidb_lyt>& c : cl.get_sidb_order())
                        //                    {
                        //                        std::cout << fmt::format("{}", c) << std::endl;
                        //                    }

                        const sidb_simulation_result<sidb_lyt>& exgs_res = exhaustive_ground_state_simulation(lyts[i]);
                        for (const charge_distribution_surface<sidb_lyt>& cl_exgs : exgs_res.charge_distributions)
                        {
                            const bool exgs_verification = verify_ground_state_space_result(cl_exgs, top);
                            CHECK(exgs_verification);
                            if (!exgs_verification)
                            {
                                std::cout << "EXGS AGREES with QUICKEXACT" << std::endl;
                            }
                        }
                    }
                }
            }
            catch (std::exception& e)
            {
                std::cout << "GROUND STATE SPACE CRASHED :(((((((((((((((((\n\n\n" << std::endl;
                REQUIRE(false);
            }
        }
        catch (std::exception& e)
        {
            std::cout << "QUICKEXACT CRASHED\n\n\n" << std::endl;
            CHECK(false);
        }

        std::cout << std::endl;
    }
}

TEST_CASE("Ground State Space verification on random layouts with 4 SiDBs", "[ground-state-space]")
{

    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{4, 2}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 4;
    rlg_ps.number_of_unique_generated_layouts = 1000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    verify_lyts(lyts);
}

TEST_CASE("Ground State Space verification on random layouts with 7 SiDBs", "[ground-state-space]")
{

    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{4, 2}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 7;
    rlg_ps.number_of_unique_generated_layouts = 1000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    verify_lyts(lyts);
}

TEST_CASE("Ground State Space verification on random layouts with 20 SiDBs", "[ground-state-space-big]")
{

    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{24, 12}};

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = 20;
    rlg_ps.number_of_unique_generated_layouts = 1000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    verify_lyts(lyts);
}

uint64_t C(uint64_t n, uint64_t r)
{
    if (r > n - r)
        r = n - r;  // because C(n, r) == C(n, n - r)
    uint64_t ans = 1;
    uint64_t i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void count_pruned_states(const uint64_t total_size, const sidb_cluster_ptr& c, uint64_t& open_states) noexcept
{
    const uint64_t max_combinations = C(c->size() + 2, 2);
    open_states -= (max_combinations - c->charge_space.size()) * static_cast<uint64_t>(std::pow(c->size(), 2ull)) *
                   static_cast<uint64_t>(std::pow(3ull, total_size - c->size()));
    for (const sidb_cluster_ptr& child : c->children)
    {
        count_pruned_states(total_size, child, open_states);
    }
}

TEST_CASE("Ground State Space state pruning comparison on random layouts with 40 SiDBs", "[ground-state-space-bench]")
{
    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{20, 10}};

    const uint64_t N = 40;

    generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
    rlg_ps.coordinate_pair                    = layout_dimensions;
    rlg_ps.number_of_sidbs                    = N;
    rlg_ps.number_of_unique_generated_layouts = 1000;

    const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

    const auto total_states = static_cast<uint64_t>(std::pow(3ull, N));

    for (uint64_t i = 0; i < lyts.size(); ++i)
    {
        ground_state_space gss{lyts[i]};
        const auto& [top, time] = gss.compute_ground_state_space();
        std::cout << "RUNTIME: " << time.count() << " s" << std::endl;
        std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;
        //        uint64_t open_states = gss.get_total_states();
        //        std::cout << "STATES PRUNED: "
        //                  << total_states - open_states << std::endl;
        //        std::cout << "OPEN STATES: " << open_states << std::endl;

        charge_distribution_surface<sidb_lyt> cds{lyts[i]};
        const uint64_t                        preassigned_neg_count = cds.negative_sidb_detection().size();
        cds.is_three_state_simulation_required();
        const uint64_t possibly_three_state_count = cds.get_positive_candidates().size();
        const uint64_t classical_open_states = std::pow(3ull, N - preassigned_neg_count - possibly_three_state_count) *
                                               std::pow(2ull, possibly_three_state_count);

        std::cout << "CLASSICAL SPACE PRUNING LEAVES " << classical_open_states << " STATES\n" << std::endl;
    }
}

// TEST_CASE("Compare modes 5", "[ground-state-space]")
//{
//     const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{4, 2}};
//
//     generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
//     rlg_ps.coordinate_pair                    = layout_dimensions;
//     rlg_ps.number_of_sidbs                    = 5;
//     rlg_ps.number_of_unique_generated_layouts = 1000;
//
//     const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);
//     for (const sidb_lyt& lyt : lyts)
//     {
//         std::cout << "NO MODE" << std::endl;
//         const auto& [top, time] = ground_state_space{lyt}.compute_ground_state_space();
//         std::cout << "RUNTIME: " << (time.count() * 1000) << " ms" << std::endl;
//         std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl;
//
//
//         std::cout << "\nMODE 0" << std::endl;
//         ground_state_space gss{lyt};
//         gss.mode = false;
//         const auto& [top0, time0] = gss.compute_ground_state_space();
//         std::cout << "RUNTIME: " << (time0.count() * 1000) << " ms" << std::endl;
//         std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top0->charge_space.size() << std::endl;
//
//         std::cout << "\nMODE 1" << std::endl;
//         ground_state_space gss1{lyt};
//         gss.mode = true;
//         const auto& [top1, time1] = gss1.compute_ground_state_space();
//         std::cout << "RUNTIME: " << (time1.count() * 1000) << " ms" << std::endl;
//         std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top1->charge_space.size() << std::endl;
//
//         if (top->charge_space.size() < top0->charge_space.size())
//         {
//             const sidb_simulation_result<sidb_lyt>& qe_res = quickexact(lyt);
//             for (const cell<sidb_lyt>& c : qe_res.charge_distributions.cbegin()->get_sidb_order())
//             {
//                 std::cout << fmt::format("{}", c) << std::endl;
//             }
//             REQUIRE(false);
//         }
//
//         std::cout << std::endl << std::endl;
//     }
// }

//(3,5,0)
//    (3,2,0)
//        (10,8,0)
//            (13,7,0)
//                (23,3,1)
//                    (8,2,1)
//                        (17,5,0)
//                            (6,5,0)
//                                (21,0,1)
//                                    (18,6,1)
//                                        (21,0,0)
//                                            (8,2,0)
//                                                (4,6,0)
//                                                    (5,11,0)
//                                                        (23,1,0)
//                                                            (24,12,0)
//                                                                (5,10,0)
//                                                                    (19,6,1)
//                                                                        (3,11,1)
//                                                                            (2,9,0)

TEST_CASE("bench20x10", "[gss]")
{
    const std::pair<cell<sidb_lyt>, cell<sidb_lyt>> layout_dimensions = {cell<sidb_lyt>{0, 0}, cell<sidb_lyt>{20, 10}};

    for (uint64_t N = 10; N <= 30; N += 5)
    {
        generate_random_sidb_layout_params<sidb_lyt> rlg_ps{};
        rlg_ps.coordinate_pair                    = layout_dimensions;
        rlg_ps.number_of_sidbs                    = N;
        rlg_ps.number_of_unique_generated_layouts = 50;

        const std::vector<sidb_lyt>& lyts = generate_multiple_random_sidb_layouts(sidb_lyt{}, rlg_ps);

        for (uint64_t i = 0; i < lyts.size(); ++i)
        {
            for (uint64_t lim = 6; lim <= 12; lim += 3)
            {
                ground_state_space gss{lyts[i], 9};
                const auto& [top, time] = gss.compute_ground_state_space();
                std::cout << "LAYOUT " << i << "  (N = " << N << ", LIM = " << lim << ")  |  RUNTIME: " << time.count()
                          << " s" << std::endl;
                std::cout << "TOP CLUSTER CHARGE SPACE SIZE: " << top->charge_space.size() << std::endl << std::endl;
            }

            std::cout << std::endl;
        }
    }
}