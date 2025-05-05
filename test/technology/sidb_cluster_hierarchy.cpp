//
// Created by Willem Lambooy on 04.02.2024.
//

#if (FICTION_ALGLIB_ENABLED)

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_cluster_hierarchy.hpp>
#include <fiction/types.hpp>

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
#include <set>
#else
#include <phmap.h>
#endif

#include <cstdint>

using namespace fiction;

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
using set_container = std::set<uint64>;
#else
using set_container = phmap::flat_hash_set<uint64_t>;
#endif

TEMPLATE_TEST_CASE("SiDB cluster hierarchy of a Y-shape SiDB OR gate with input 01", "[sidb-cluster-hierarchy]",
                   sidb_cell_clk_lyt_siqad, charge_distribution_surface<sidb_cell_clk_lyt_siqad>)
{
    TestType lyt{};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    // check for spooky non-determinism in alglib
    for (int8_t i = 0; i < 100; ++i)
    {
        const sidb_binary_cluster_hierarchy_node& h = sidb_cluster_hierarchy(lyt);

        REQUIRE(h.c.size() == 8);
        REQUIRE(h.sub.at(0) != nullptr);
        REQUIRE(h.sub.at(1) != nullptr);
        REQUIRE(h.sub.at(0)->c.size() == 3);
        REQUIRE(h.sub.at(1)->c.size() == 5);
        CHECK(h.sub.at(0)->c == set_container{5, 6, 7});
        CHECK(h.sub.at(1)->c == set_container{0, 1, 2, 3, 4});
        REQUIRE(h.sub.at(0)->sub.at(0) != nullptr);
        REQUIRE(h.sub.at(0)->sub.at(1) != nullptr);
        REQUIRE(h.sub.at(1)->sub.at(0) != nullptr);
        REQUIRE(h.sub.at(1)->sub.at(1) != nullptr);
        CHECK(h.sub.at(0)->sub.at(0)->c == set_container{7});
        CHECK(h.sub.at(0)->sub.at(1)->c == set_container{5, 6});
        CHECK(h.sub.at(1)->sub.at(0)->c == set_container{1, 3});
        CHECK(h.sub.at(1)->sub.at(1)->c == set_container{0, 2, 4});
        REQUIRE(h.sub.at(1)->sub.at(1)->sub.at(0) != nullptr);
        REQUIRE(h.sub.at(1)->sub.at(1)->sub.at(1) != nullptr);
        CHECK(h.sub.at(1)->sub.at(1)->sub.at(0)->c == set_container{4});
        CHECK(h.sub.at(1)->sub.at(1)->sub.at(1)->c == set_container{0, 2});
    }
}

TEMPLATE_TEST_CASE("SiDB cluster hierarchy of an 8 DB layout with separated groups of SiDBs",
                   "[sidb-cluster-hierarchy]", sidb_cell_clk_lyt_siqad,
                   charge_distribution_surface<sidb_cell_clk_lyt_siqad>)
{
    TestType lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);

    const sidb_binary_cluster_hierarchy_node& h = sidb_cluster_hierarchy(lyt);

    REQUIRE(h.c.size() == 8);
    REQUIRE(h.sub.at(0) != nullptr);
    REQUIRE(h.sub.at(1) != nullptr);
    REQUIRE(h.sub.at(0)->c.size() == 4);
    REQUIRE(h.sub.at(1)->c.size() == 4);

    CHECK(h.sub.at(0)->c == set_container{2, 3, 4, 5});
    CHECK(h.sub.at(1)->c == set_container{0, 1, 6, 7});

    REQUIRE(h.sub.at(0)->sub.at(0) != nullptr);
    REQUIRE(h.sub.at(0)->sub.at(1) != nullptr);
    REQUIRE(h.sub.at(1)->sub.at(0) != nullptr);

    CHECK(h.sub.at(0)->sub.at(0)->c == set_container{4, 5});
    CHECK(h.sub.at(0)->sub.at(1)->c == set_container{2, 3});
    CHECK(h.sub.at(1)->sub.at(0)->c == set_container{0, 1});
    CHECK(h.sub.at(1)->sub.at(1)->c == set_container{6, 7});
}

#else  // FICTION_ALGLIB_ENABLED

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SiDB Cluster Hierarchy", "[sidb-cluster-hierarchy]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_ALGLIB_ENABLED
