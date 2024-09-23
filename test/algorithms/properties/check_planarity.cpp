//
// Created by benjamin on 17.07.24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/properties/check_planarity.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/extended_rank_view.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Check planarity aig", "[check-planarity]")
{
    mockturtle::aig_network aig{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();

    const auto f1 = aig.create_and(x1, x2);
    const auto f2 = aig.create_and(x2, x3);

    const auto x4 = aig.create_pi();
    const auto x5 = aig.create_not(x4);

    aig.create_po(f1);
    aig.create_po(f2);
    aig.create_po(x5);

    extended_rank_view tec_r(aig);

    const std::vector<mockturtle::aig_network::node> nodes_rank0{1, 2, 3, 6};
    const std::vector<mockturtle::aig_network::node> nodes_rank1{4, 5};

    tec_r.modify_rank(0, nodes_rank0);
    tec_r.modify_rank(1, nodes_rank1);

    const bool planar = check_planarity(tec_r);

    CHECK(planar == 1);
}

TEST_CASE("Check planarity technology network", "[check-planarity]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x2, x3);

    tec.create_po(f1);
    tec.create_po(f2);

    extended_rank_view tec_r(tec);

    const std::vector<technology_network::node> nodes_rank0{2, 3, 4};
    const std::vector<technology_network::node> nodes_rank1{5, 6};

    tec_r.modify_rank(0, nodes_rank0);
    tec_r.modify_rank(1, nodes_rank1);

    const bool planar = check_planarity(tec_r);

    CHECK(planar == 1);
}

TEST_CASE("Check non-planarity technology network", "[check-planarity]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x3);
    const auto f2 = tec.create_and(x2, x3);

    tec.create_po(f1);
    tec.create_po(f2);

    extended_rank_view tec_r(tec);

    const std::vector<technology_network::node> nodes_rank0{2, 3, 4};
    const std::vector<technology_network::node> nodes_rank1{5, 6};

    tec_r.modify_rank(0, nodes_rank0);
    tec_r.modify_rank(1, nodes_rank1);

    const bool planar = check_planarity(tec_r);

    CHECK(planar == 0);
}
