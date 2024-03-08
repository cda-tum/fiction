//
// Created by Willem Lambooy on 06/02/2024.
//

#if (FICTION_ALGLIB_ENABLED)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/ground_state_space.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/types.hpp>

#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <cstdint>
#include <iterator>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Empty layout Ground State Space construction", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    const ground_state_space_result& res = ground_state_space(lyt);

    CHECK(!res.top_cluster);
    CHECK(mockturtle::to_seconds(res.runtime) == 0.0);
    CHECK(res.pruned_top_level_multisets == 0);
    CHECK(res.maximum_top_level_multisets == 0);
}

TEMPLATE_TEST_CASE(
    "Ground State Space construction of two SiDBs directly next to each other", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);

    SECTION("Base 2")
    {
        const ground_state_space_result& res = ground_state_space(lyt, 6, sidb_simulation_parameters{2});
        CHECK(res.pruned_top_level_multisets == res.maximum_top_level_multisets);
    }

    SECTION("Base 3")
    {
        const ground_state_space_result& res = ground_state_space(lyt, 6, sidb_simulation_parameters{3});
        CHECK(res.pruned_top_level_multisets != res.maximum_top_level_multisets);
    }
}

TEMPLATE_TEST_CASE(
    "Ground State Space construction of a 7 DB layout", "[ground-state-space]",
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

    const ground_state_space_result& gss_res = ground_state_space(lyt, 6, sidb_simulation_parameters{2});

    CHECK(mockturtle::to_seconds(gss_res.runtime) > 0.0);

    CHECK(gss_res.pruned_top_level_multisets == 7);

    CHECK(gss_res.top_cluster->sidbs.size() == 7);

    CHECK(gss_res.top_cluster->charge_space.size() ==
          gss_res.maximum_top_level_multisets - gss_res.pruned_top_level_multisets);

    for (uint64_t i = 0; i < 7; ++i)
    {
        CHECK_THAT(gss_res.top_cluster->recv_ext_pot_bounds[i][0],
                   Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(gss_res.top_cluster->recv_ext_pot_bounds[i][1],
                   Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
    }

    REQUIRE(gss_res.top_cluster->charge_space.size() == 1);

    CHECK(gss_res.top_cluster->charge_space.cbegin()->neg_count == 5);
    CHECK(gss_res.top_cluster->charge_space.cbegin()->pos_count == 0);

    REQUIRE(gss_res.top_cluster->charge_space.cbegin()->compositions.size() == 1);
    REQUIRE(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->size() == 2);

    if (gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.cluster->sidbs.size() == 3)
    {
        CHECK(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.cluster->sidbs ==
              phmap::flat_hash_set<uint64_t>{0ul, 1ul, 2ul});
        CHECK((gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.multiset_conf >>
               32ull) == 2);
        CHECK(((gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.multiset_conf
                << 32ull) >>
               32ull) == 0);

        CHECK(std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                  ->proj_st.cluster->sidbs == phmap::flat_hash_set<uint64_t>{3ul, 4ul, 5ul, 6ul});
        CHECK((std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                   ->proj_st.multiset_conf >>
               32ull) == 3);
        CHECK(((std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                    ->proj_st.multiset_conf
                << 32ull) >>
               32ull) == 0);
    }
    else
    {
        CHECK(std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                  ->proj_st.cluster->sidbs == phmap::flat_hash_set<uint64_t>{0ul, 1ul, 2ul});
        CHECK((std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                   ->proj_st.multiset_conf >>
               32ull) == 2);
        CHECK(((std::next(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin(), 1)
                    ->proj_st.multiset_conf
                << 32ull) >>
               32ull) == 0);

        CHECK(gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.cluster->sidbs ==
              phmap::flat_hash_set<uint64_t>{3ul, 4ul, 5ul, 6ul});
        CHECK((gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.multiset_conf >>
               32ull) == 3);
        CHECK(((gss_res.top_cluster->charge_space.cbegin()->compositions.cbegin()->cbegin()->proj_st.multiset_conf
                << 32ull) >>
               32ull) == 0);
    }
}

TEMPLATE_TEST_CASE(
    "Ground state space construction of a 14 DB layout", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    // V1   V2      num
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({0, 7, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 5, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 8, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 7, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);

    const ground_state_space_result& gss_res = ground_state_space(lyt);

    CHECK(gss_res.top_cluster->sidbs.size() == 14);
    CHECK(gss_res.top_cluster->charge_space.size() == 5);

    for (uint64_t i = 0; i < 14; ++i)
    {
        CHECK_THAT(gss_res.top_cluster->recv_ext_pot_bounds[i][0],
                   Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
        CHECK_THAT(gss_res.top_cluster->recv_ext_pot_bounds[i][1],
                   Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
    }
}

template <typename Lyt>
static bool verify_ground_state_space_result(const charge_distribution_surface<Lyt>& valid_cl,
                                             const sidb_cluster_ptr&                 gss_node) noexcept
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

    for (const sidb_cluster_state_composition& composition : found_m_conf->compositions)
    {
        bool composition_has_correct_charge_conf = true;

        for (const sidb_cluster_state& cst : composition)
        {
            composition_has_correct_charge_conf &= verify_ground_state_space_result(valid_cl, cst.proj_st.cluster);
        }

        found_charge_conf |= composition_has_correct_charge_conf;
    }

    return found_charge_conf;
}

TEMPLATE_TEST_CASE(
    "Ground State Space construction of sub-10 DB layouts", "[ground-state-space]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    const auto& verify_layout = [](const TestType& lyt)
    {
        const sidb_simulation_result<TestType>& qe_res   = quickexact(lyt);
        const sidb_simulation_result<TestType>& exgs_res = exhaustive_ground_state_simulation(lyt);
        const ground_state_space_result&        gss_res  = ground_state_space(lyt);

        for (const charge_distribution_surface<TestType>& cl : qe_res.charge_distributions)
        {
            const bool verification = verify_ground_state_space_result<TestType>(cl, gss_res.top_cluster);
            CHECK(verification);
        }

        for (const charge_distribution_surface<TestType>& cl : exgs_res.charge_distributions)
        {
            const bool verification = verify_ground_state_space_result<TestType>(cl, gss_res.top_cluster);
            CHECK(verification);
        }
    };

    SECTION("Layout 1 (4 DBs)")
    {
        TestType lyt{};
        lyt.assign_cell_type({1, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 2, 0}, TestType::cell_type::NORMAL);

        verify_layout(lyt);
    }

    SECTION("Layout 2 (4 DBs)")
    {
        TestType lyt{};
        lyt.assign_cell_type({2, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 1, 0}, TestType::cell_type::NORMAL);

        verify_layout(lyt);
    }

    SECTION("Layout 3 (4 DBs)")
    {
        TestType lyt{};
        lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 2, 0}, TestType::cell_type::NORMAL);

        verify_layout(lyt);
    }

    SECTION("Layout 4 (7 DBs)")
    {
        TestType lyt{};
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 2, 0}, TestType::cell_type::NORMAL);

        verify_layout(lyt);
    }
}

#else  // FICTION_ALGLIB_ENABLED

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Ground State Space", "[ground-state-space]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_ALGLIB_ENABLED
