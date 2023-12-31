//
// Created by Willem Lambooy on 12/12/2023.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/simulation/sidb/exact_cluster_iteration.hpp>
#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/potential_hierarchy.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <iostream>
#include <set>

using namespace fiction;

using layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("8 DB cluster hierarchy", "[potential-hierarchy]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);

    //    lyt.foreach_cell([&sidb_order](const auto& c1) { sidb_order.push_back(c1); });
    charge_distribution_surface<layout> cl{lyt};
    const std::vector<cell<layout>>&    sidb_order = cl.get_sidb_order();

    sidb_simulation_parameters ps{};

    SECTION("Small cone angle")
    {
        const auto h = potential_hierarchy<layout>{sidb_order, ps, 1}.make_cluster_hierarchy();

        CHECK(h->c.size() == sidb_order.size());
        CHECK(h->v.size() == 8);
    }

    SECTION("Medium cone angle")
    {
        const auto h = potential_hierarchy<layout>{sidb_order, ps, 20}.make_cluster_hierarchy();

        CHECK(h->c.size() == sidb_order.size());
        CHECK(h->v.size() == 4);

        std::multiset<int8_t> multiset = {-1, +1};
        charge_state_multiset m{};
        for (const int8_t i : multiset)
        {
            m.insert(sign_to_charge_state(i));
        }

        //        const auto res = generate_charge_assignments(h->c, m);
        for (const auto& h_ : h->v)
        {
            cluster_exact<layout> ce{cl};
            const double          x = ce.V_int_lower(h_->c, m);
            const double          y = ce.V_int_upper(h_->c, m);
            CHECK(x != y);
        }
    }

    SECTION("Large cone angle")
    {
        const auto h = potential_hierarchy<layout>{sidb_order, ps, 90}.make_cluster_hierarchy();

        CHECK(h->c.size() == sidb_order.size());
        CHECK(h->v.size() == 3);
    }

    //    SECTION("Very large cone angle")
    //    {
    //        const auto h = potential_hierarchy<layout>{sidb_order, ps, 160}.make_cluster_hierarchy();
    //
    //        CHECK(h->c.size() == sidb_order.size());
    //        CHECK(h->v.size() == 3);
    //    }
}

TEST_CASE("Influence region test", "[potential-hierarchy]")
{
    layout lyt = read_sqd_layout<layout>("influence_region.sqd");

    std::vector<cell<layout>> sidb_order{};
    lyt.foreach_coordinate(
        [&lyt, &sidb_order](const auto& c1)
        {
            if (!lyt.is_empty_cell(c1))
                sidb_order.push_back(c1);
        });

    const auto h = potential_hierarchy<layout>{sidb_order, sidb_simulation_parameters{}, 100}.make_cluster_hierarchy();

    REQUIRE(h->c.size() == sidb_order.size());

    for (const uint64_t i : h->c)
    {
        CHECK(i < h->c.size());
    }

    std::set<uint64_t> level_1_indices{};

    for (const auto& level_1_h : h->v)
    {
        for (const uint64_t i : level_1_h->c)
        {
            CHECK(level_1_indices.count(i) == 0);
            level_1_indices.emplace(i);
        }
    }
    //    std::transform(sidb_order.cbegin(), sidb_order.cend(), )
    CHECK(h->c == level_1_indices);
}

TEST_CASE("TEST", "[potential-hierarchy]")
{
    clustering            xv{{0, 1}, {2, 3}, {4, 5}};
    charge_state_multiset m{sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE,
                            sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE, sidb_charge_state::POSITIVE};
    charge_multiset_conf  cmc{xv, m};
    uint64_t              count = 1;

    while (cmc.next_conf())
    {
        count++;
    }

    CHECK(count == 15);
    //    layout lyt{};
    //
    //    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    //    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);
    //
    //    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    //    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    //
    //    lyt.assign_cell_type({23, 9, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({18, 11, 1}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({19, 8, 0}, sidb_technology::cell_type::NORMAL);
    //
    //    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    //    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    //
    //    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    //    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    //
    //    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
    //
    //    std::vector<cell<layout>> sidb_order{};
    //    lyt.foreach_cell([&sidb_order](const auto& c1) { sidb_order.push_back(c1); });
    //
    //    const potential_hierarchy<layout> ph{sidb_order, sidb_simulation_parameters{}, 90};
    //
    //    ph.make_cluster_hierarchy();
    //
    CHECK(true);
}

TEST_CASE("Smaller cluster charge multiset assignment iteration", "[multiset-assignment-iteration]")
{
    clustering            xv{{0, 1, 2, 3, 4}, {5, 6, 7, 8}};
    charge_state_multiset m = {sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL,
                               sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL};
    charge_multiset_conf  cmc{xv, m};

    uint64_t count = 1;

    while (cmc.next_conf())
    {
        count++;
    }

    CHECK(count == 9);
}

TEST_CASE("Bigger cluster charge multiset assignment iteration", "[multiset-assignment-iteration]")
{
    clustering            xv{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};
    charge_state_multiset m = {sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL,
                               sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL};
    charge_multiset_conf  cmc{xv, m};

    uint64_t count = 1;

    while (cmc.next_conf())
    {
        count++;
    }
    std::cout << count << std::endl;

    //    CHECK(count == 105);
}

TEST_CASE("Very large cluster charge multiset assignment iteration", "[multiset-assignment-iteration]")
{
    clustering xv{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15, 16}, {17}, {18, 19}, {20, 21, 22}};

    charge_state_multiset m = {sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL,
                               sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,
                               sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::POSITIVE,
                               sidb_charge_state::POSITIVE, sidb_charge_state::NEUTRAL,  sidb_charge_state::POSITIVE,
                               sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL};
    charge_multiset_conf  cmc{xv, m};

    uint64_t count = 1;

    while (cmc.next_conf())
    {
        count++;
    }
    std::cout << count << std::endl;

    //    CHECK(count == 247363);
}

TEST_CASE("All charge state multisets of length 3", "[multiset-iteration]")
{
    clustering xv{{0, 1, 2}};

    charge_state_multiset m{sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE, sidb_charge_state::NEGATIVE,
                            sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,  sidb_charge_state::NEUTRAL,
                            sidb_charge_state::POSITIVE, sidb_charge_state::POSITIVE, sidb_charge_state::POSITIVE};

    charge_multiset_conf cmc{xv, m};

    uint64_t count = 1;

    while (cmc.next_conf())
    {
        count++;
    }
    std::cout << count << std::endl;
}

TEST_CASE("13 DB test layout cone angle", "[potential-hierarchy]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);

    const charge_distribution_surface<layout> cl{lyt};

    const auto h = potential_hierarchy<layout>{cl.get_sidb_order(), cl.get_phys_params(), 45}.make_cluster_hierarchy();

    CHECK(h->c.size() == cl.num_cells());
    CHECK(h->v.size() == 3);
}

TEST_CASE("13 DB exact cluster simulation", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);
    cluster_exact<layout> ce{lyt};
    const auto&           res = ce.FindGroundState(45);
    std::cout << "CC Store size: " << ce.charge_conf_store.size() << " ; PB Store size: " << ce.pot_bounds_store.size()
              << std::endl;
    CHECK(!res.charge_distributions.empty());
    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("13 DB exact cluster simulation (ExGS)", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);

    const auto& res = exhaustive_ground_state_simulation(lyt);
    CHECK(!res.charge_distributions.empty());
    std::cout << "NUM CELLS: " << lyt.num_cells() << std::endl;
    std::cout << "EXGS RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("13 DB exact cluster simulation (QuickExact)", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);

    const auto& res = quickexact(lyt);
    std::cout << res.simulation_runtime.count() << std::endl;
    CHECK(!res.charge_distributions.empty());
    std::cout << "NUM CELLS: " << lyt.num_cells() << std::endl;
    std::cout << "QUICKEXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("16 DB exact cluster simulation", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({52, -7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, -7, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);
    cluster_exact<layout> ce{lyt};
    const auto&           res = ce.FindGroundState(45);
    std::cout << "CC Store size: " << ce.charge_conf_store.size() << " ; PB Store size: " << ce.pot_bounds_store.size()
              << std::endl;
    REQUIRE(res.charge_distributions.size() == 1);
    print_layout(res.charge_distributions[0]);
    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("16 DB exact cluster simulation (ExGS)", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({52, -7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, -7, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);

    const auto& res = exhaustive_ground_state_simulation(lyt);
    REQUIRE(res.charge_distributions.size() == 1);
    print_layout(res.charge_distributions[0]);

    std::cout << "NUM CELLS: " << lyt.num_cells() << std::endl;
    std::cout << "RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("16 DB exact cluster simulation (QuickExact)", "[exact-cluster-simulation]")
{
    layout lyt{};

    lyt.assign_cell_type({50, -11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({54, -9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({52, -7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, -5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, -5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, -7, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-2, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, -2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({53, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({53, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({48, 12, 1}, sidb_technology::cell_type::NORMAL);

    const auto& res = quickexact(lyt);
    REQUIRE(res.charge_distributions.size() == 1);
    print_layout(res.charge_distributions[0]);
    std::cout << "NUM CELLS: " << lyt.num_cells() << std::endl;
    std::cout << "QUICKEXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon PI", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_pi({"x1"}, {0, 0});
    lyt.create_pi({"x2"}, {2, 2});

    cluster_exact<layout> ce{charge_distribution_surface<layout>{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))}};
    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    const auto& res = ce.FindGroundState(90);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    }

    std::cout << "CC Store size: " << ce.charge_conf_store.size() << " ; PB Store size: " << ce.pot_bounds_store.size()
              << std::endl;
    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon PI (QuickExact)", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_pi({"x1"}, {0, 0});
    lyt.create_pi({"x2"}, {2, 2});

    charge_distribution_surface<layout> cds{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))};
    const auto& res = quickexact(cds);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    };
    std::cout << "NUM CELLS: " << cds.num_cells() << std::endl;
    std::cout << "QUICKEXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon NAND", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_nand({}, {}, {0, 0});
    lyt.create_nand({}, {}, {2, 2});

    cluster_exact<layout> ce{charge_distribution_surface<layout>{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))}};
    std::cout << "(CE) NUM CELLS: " << ce.cl.num_cells() << std::endl;
    const auto& res = ce.FindGroundState(90);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    }
    //    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    std::cout << "CC Store size: " << ce.charge_conf_store.size() << " ; PB Store size: " << ce.pot_bounds_store.size()
              << std::endl;
    std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon NAND (QuickExact)", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_nand({}, {}, {0, 0});
    lyt.create_nand({}, {}, {2, 2});

    charge_distribution_surface<layout> cds{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))};

    std::cout << "(QE) NUM CELLS: " << cds.num_cells() << std::endl;

    const auto& res = quickexact(cds);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    };

    //    std::cout << "NUM CELLS: " << cds.num_cells() << std::endl;
    std::cout << "RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon OR", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_or({}, {}, {0, 0});
    lyt.create_or({}, {}, {2, 2});

    cluster_exact<layout> ce{charge_distribution_surface<layout>{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))}};
    std::cout << "(CE) NUM CELLS: " << ce.cl.num_cells() << std::endl;
    const auto& res = ce.FindGroundState(90);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    }
    //    std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
    std::cout << "CC Store size: " << ce.charge_conf_store.size() << " ; PB Store size: " << ce.pot_bounds_store.size()
              << std::endl;
    std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon OR (QuickExact)", "[potential-hierarchy]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
    lyt.create_or({}, {}, {0, 0});
    lyt.create_or({}, {}, {2, 2});

    charge_distribution_surface<layout> cds{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))};

    std::cout << "(QE) NUM CELLS: " << cds.num_cells() << std::endl;

    const auto& res = quickexact(cds);

    //    for (const auto& cl : res.charge_distributions)
    //    {
    //        print_layout(cl);
    //    };

    //    std::cout << "NUM CELLS: " << cds.num_cells() << std::endl;
    std::cout << "RUNTIME: " << res.simulation_runtime.count() << std::endl;
    std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
    std::cout << "GROUND STATE ENERGY: "
              << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl;
}

//TEST_CASE("Exact Cluster Simulation of 2 Bestagon PI (differing min-size)", "[potential-hierarchy]")
//{
//    gate_level_layout<hex_even_row_gate_clk_lyt> lyt{{2, 2}};
//    lyt.create_pi({"x1"}, {0, 0});
//    lyt.create_pi({"x2"}, {2, 2});
//
//    SECTION("Min-size 11")
//    {
//        cluster_exact<layout> ce2{charge_distribution_surface<layout>{convert_to_siqad_coordinates(
//                                      apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))},
//                                  11};
//        std::cout << "NUM CELLS: " << ce2.cl.num_cells() << std::endl;
//        const auto& res2 = ce2.FindGroundState(90);
//        std::cout << "CC Store size: " << ce2.charge_conf_store.size()
//                  << " ; PB Store size: " << ce2.pot_bounds_store.size() << std::endl;
//        std::cout << "NUM CELLS: " << ce2.cl.num_cells() << std::endl;
//        std::cout << "CLUSTEREXACT RUNTIME: " << res2.simulation_runtime.count() << std::endl;
//        std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res2.charge_distributions.size() << std::endl;
//        std::cout << "GROUND STATE ENERGY: "
//                  << minimum_energy(res2.charge_distributions.cbegin(), res2.charge_distributions.cend()) << std::endl
//                  << std::endl;
//    }
//    SECTION("Min-size 14")
//    {
//        cluster_exact<layout> ce{charge_distribution_surface<layout>{
//            convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))}, 14};
//        std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
//        const auto& res = ce.FindGroundState(90);
//        std::cout << "CC Store size: " << ce.charge_conf_store.size()
//                  << " ; PB Store size: " << ce.pot_bounds_store.size() << std::endl;
//        std::cout << "NUM CELLS: " << ce.cl.num_cells() << std::endl;
//        std::cout << "CLUSTEREXACT RUNTIME: " << res.simulation_runtime.count() << std::endl;
//        std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res.charge_distributions.size() << std::endl;
//        std::cout << "GROUND STATE ENERGY: "
//                  << minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()) << std::endl
//                  << std::endl;
//    }
//    SECTION("Min-size 18")
//    {
//        cluster_exact<layout> ce3{charge_distribution_surface<layout>{convert_to_siqad_coordinates(
//                                      apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))},
//                                  18};
//        std::cout << "NUM CELLS: " << ce3.cl.num_cells() << std::endl;
//        const auto& res3 = ce3.FindGroundState(90);
//        std::cout << "CC Store size: " << ce3.charge_conf_store.size()
//                  << " ; PB Store size: " << ce3.pot_bounds_store.size() << std::endl;
//        std::cout << "NUM CELLS: " << ce3.cl.num_cells() << std::endl;
//        std::cout << "CLUSTEREXACT RUNTIME: " << res3.simulation_runtime.count() << std::endl;
//        std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res3.charge_distributions.size() << std::endl;
//        std::cout << "GROUND STATE ENERGY: "
//                  << minimum_energy(res3.charge_distributions.cbegin(), res3.charge_distributions.cend()) << std::endl
//                  << std::endl;
//    }
//    SECTION("Min-size 22")
//    {
//        cluster_exact<layout> ce4{charge_distribution_surface<layout>{convert_to_siqad_coordinates(
//                                      apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(lyt))},
//                                  22};
//        std::cout << "NUM CELLS: " << ce4.cl.num_cells() << std::endl;
//        const auto& res4 = ce4.FindGroundState(90);
//
//        std::cout << "CC Store size: " << ce4.charge_conf_store.size()
//                  << " ; PB Store size: " << ce4.pot_bounds_store.size() << std::endl;
//        std::cout << "NUM CELLS: " << ce4.cl.num_cells() << std::endl;
//        std::cout << "CLUSTEREXACT RUNTIME: " << res4.simulation_runtime.count() << std::endl;
//        std::cout << "PHYSICALLY VALID CHARGE LAYOUTS: " << res4.charge_distributions.size() << std::endl;
//        std::cout << "GROUND STATE ENERGY: "
//                  << minimum_energy(res4.charge_distributions.cbegin(), res4.charge_distributions.cend()) << std::endl;
//    }
//}
