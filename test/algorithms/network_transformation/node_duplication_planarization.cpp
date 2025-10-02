//
// Created by benjamin on 11.06.24.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/network_transformation/network_balancing.hpp"

#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>

using namespace fiction;

TEST_CASE("Planarize technology ntk", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_or({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto tec_b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(tec), ps);

    const auto vpi_r = fiction::mutable_rank_view(tec_b);

    auto planarized_maj = node_duplication_planarization(vpi_r);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, planarized_maj), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);
}

TEST_CASE("Planarize ntk with 3-ary node", "[node-duplication-planarization]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_or({x3, x4, x5});
    const auto f4 = tec.create_maj(x1, x2, f3);
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);
    tec.create_po(f4);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto tec_b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(tec), ps);

    const auto vpi_r = fiction::mutable_rank_view(tec_b);

    auto planarized_maj = node_duplication_planarization(vpi_r);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, planarized_maj), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);
}

TEST_CASE("Buffer AIG and planarize technology_network", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto x3 = aig.create_pi();
    const auto x4 = aig.create_pi();
    const auto x5 = aig.create_pi();
    const auto f1 = aig.create_not(x2);
    const auto f2 = aig.create_nary_and({x1, x2, x3, x4});
    const auto f3 = aig.create_nary_or({x3, x4, x5});
    const auto f4 = aig.create_maj(x1, x2, f3);
    aig.create_po(f1);
    aig.create_po(f2);
    aig.create_po(f3);
    aig.create_po(f4);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto aig_b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(aig), ps);

    const auto vpi_r = fiction::mutable_rank_view(aig_b);

    auto planarized_maj = node_duplication_planarization(vpi_r);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(aig, planarized_maj), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);
}

TEST_CASE("Buffer AIG and planarize technology_network 2", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();
    const auto f1 = aig.create_and(x1, x2);
    const auto f2 = aig.create_or(x1, x2);
    aig.create_po(f1);
    aig.create_po(f2);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto aig_b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(aig), ps);

    const auto vpi_r = fiction::mutable_rank_view(aig_b);

    auto planarized_maj = node_duplication_planarization(vpi_r);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(aig, planarized_maj), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);
}

TEST_CASE("Planarize multi output network", "[node-duplication-planarization]")
{
    mockturtle::aig_network aig{};

    const auto x1 = aig.create_pi();
    const auto x2 = aig.create_pi();

    const auto a1 = aig.create_and(x1, x2);

    aig.create_po(a1);
    aig.create_po(a1);
    aig.create_po(a1);
    aig.create_po(a1);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto aig_b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(aig), ps);

    const auto vpi_r = fiction::mutable_rank_view(aig_b);

    auto planarized_maj = node_duplication_planarization(vpi_r);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(aig, planarized_maj), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(*cec_m == 1);
}
