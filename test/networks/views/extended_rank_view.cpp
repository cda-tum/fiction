//
// Created by benjamin on 18.06.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/extended_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/buffered.hpp>
#include <mockturtle/networks/cover.hpp>
#include <mockturtle/networks/crossed.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>

#include <functional>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("traits", "[extended_rank_view]", mockturtle::aig_network, mockturtle::mig_network,
                   mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   mockturtle::cover_network, mockturtle::buffered_aig_network, mockturtle::buffered_mig_network,
                   mockturtle::crossed_klut_network, mockturtle::buffered_crossed_klut_network)
{
    CHECK(mockturtle::is_network_type_v<TestType>);
    CHECK(!mockturtle::has_rank_position_v<TestType>);
    CHECK(!mockturtle::has_at_rank_position_v<TestType>);
    CHECK(!mockturtle::has_width_v<TestType>);
    CHECK(!mockturtle::has_sort_rank_v<TestType>);
    CHECK(!mockturtle::has_foreach_node_in_rank_v<TestType>);
    CHECK(!mockturtle::has_foreach_gate_in_rank_v<TestType>);
    CHECK(!mockturtle::is_topologically_sorted_v<TestType>);

    using rank_ntk = extended_rank_view<TestType>;

    CHECK(mockturtle::is_network_type_v<rank_ntk>);
    CHECK(mockturtle::has_rank_position_v<rank_ntk>);
    CHECK(mockturtle::has_at_rank_position_v<rank_ntk>);
    CHECK(mockturtle::has_width_v<rank_ntk>);
    CHECK(mockturtle::has_sort_rank_v<rank_ntk>);
    CHECK(mockturtle::has_foreach_node_in_rank_v<rank_ntk>);
    CHECK(mockturtle::has_foreach_gate_in_rank_v<rank_ntk>);
    CHECK(mockturtle::is_topologically_sorted_v<rank_ntk>);

    using rank_rank_ntk = extended_rank_view<rank_ntk>;

    CHECK(mockturtle::is_network_type_v<rank_rank_ntk>);
    CHECK(mockturtle::has_rank_position_v<rank_rank_ntk>);
    CHECK(mockturtle::has_at_rank_position_v<rank_rank_ntk>);
    CHECK(mockturtle::has_width_v<rank_rank_ntk>);
    CHECK(mockturtle::has_sort_rank_v<rank_rank_ntk>);
    CHECK(mockturtle::has_foreach_node_in_rank_v<rank_rank_ntk>);
    CHECK(mockturtle::has_foreach_gate_in_rank_v<rank_rank_ntk>);
    CHECK(mockturtle::is_topologically_sorted_v<rank_rank_ntk>);
}

TEST_CASE("Constructors", "[extended-rank-view]")
{
    technology_network                     tec{};
    extended_rank_view<technology_network> tec_r;

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto a1 = tec.create_and(x1, x2);
    tec.create_po(a1);

    const auto x1_r = tec_r.create_pi();
    const auto x2_r = tec_r.create_pi();
    const auto a1_r = tec_r.create_and(x1_r, x2_r);
    tec_r.create_po(a1_r);
    tec_r.update_ranks();

    const auto                                                           tec_r_cpy(tec_r);
    const auto                                                           tec_r_eq     = tec_r;
    const auto                                                           tec_r_create = extended_rank_view(tec);
    const std::vector<std::vector<mockturtle::node<technology_network>>> ranks        = {{2, 3}, {4}};
    const auto                                                           tec_r_vector = extended_rank_view(tec, ranks);
    CHECK(tec_r_vector.rank_position(2) == tec_r.rank_position(2));
    CHECK(tec_r_vector.rank_position(3) == tec_r.rank_position(3));
    CHECK(tec_r_vector.rank_position(4) == tec_r.rank_position(4));

    CHECK(tec_r_create.check_validity() == 1);
    CHECK(tec_r_vector.check_validity() == 1);
    CHECK(tec_r.check_validity() == 1);
    CHECK(tec_r_cpy.check_validity() == 1);
    CHECK(tec_r_eq.check_validity() == 1);
}

TEST_CASE("Foreach", "[extended-rank-view]")
{
    extended_rank_view<technology_network> tec_r;

    const auto x1_r = tec_r.create_pi();
    const auto x2_r = tec_r.create_pi();
    const auto a1_r = tec_r.create_and(x1_r, x2_r);
    tec_r.create_po(a1_r);
    CHECK(has_update_ranks_v<decltype(tec_r)>);
    tec_r.update_ranks();

    std::vector<mockturtle::node<technology_network>> pis{};
    std::vector<mockturtle::node<technology_network>> cis{};
    tec_r.foreach_pi([&pis](const auto& n) { pis.push_back(n); });

    tec_r.foreach_ci([&cis](const auto& n) { cis.push_back(n); });
    CHECK(pis == cis);

    cis.clear();
    tec_r.foreach_node_in_rank(0, [&cis](const auto& n) { cis.push_back(n); });
    CHECK(pis == cis);

    std::vector<mockturtle::node<technology_network>> node_vector{};
    tec_r.foreach_node([&node_vector](const auto& n) { node_vector.push_back(n); });
    CHECK(node_vector.size() == tec_r.size() - 2);

    pis.clear();
    tec_r.foreach_gate_in_rank(0, [&pis](const auto& n) { pis.push_back(n); });
    CHECK(pis.empty());

    node_vector.clear();
    tec_r.foreach_gate([&node_vector](const auto& n) { node_vector.push_back(n); });
    CHECK(node_vector.size() == tec_r.num_gates());
}

TEST_CASE("Construct extended_rank_view with given rank order", "[extended-rank-view]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_and({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    const std::vector<std::vector<technology_network::node>> rank_array = {{2, 4, 3, 5, 6}, {7, 8, 9, 11}, {10, 12}};

    auto vpi_r = extended_rank_view(tec, rank_array);

    CHECK(vpi_r.rank_width(0) == 5);
    CHECK(vpi_r.rank_width(1) == 4);
    CHECK(vpi_r.rank_width(2) == 2);
    CHECK(vpi_r.width() == 5);
    CHECK(vpi_r.at_rank_position(0, 2) == 3);

    vpi_r.swap(3, 4);
    CHECK(vpi_r.at_rank_position(0, 2) == 4);

    vpi_r.swap(3, 4);
    CHECK(vpi_r.at_rank_position(0, 2) == 3);
    vpi_r.sort_rank(0u, std::greater<mockturtle::node<technology_network>>{});
    CHECK(vpi_r.at_rank_position(0, 2) == 4);
}

TEST_CASE("Check modify ranks", "[extended-rank-view]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_and({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto tec_balanced = network_balancing<technology_network>(tec, ps);

    auto vpi_r = extended_rank_view(tec_balanced);

    const std::vector<technology_network::node> nodes = {13, 10, 14};
    vpi_r.modify_rank(2, nodes);

    CHECK(vpi_r.check_validity() == 1);
}

TEMPLATE_TEST_CASE("Check equivalence checking", "[extended_rank_view]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   mockturtle::buffered_aig_network, mockturtle::buffered_mig_network, mockturtle::crossed_klut_network,
                   mockturtle::buffered_crossed_klut_network)
{
    TestType ntk{};

    const auto a = ntk.create_pi();
    const auto b = ntk.create_pi();

    const auto a_t = ntk.create_pi();
    const auto b_t = ntk.create_pi();

    const auto f1 = ntk.create_and(a, b);

    const auto f1_t = ntk.create_and(a_t, b);
    const auto f2_t = ntk.create_and(b_t, a_t);
    const auto f3_t = ntk.create_or(a_t, b_t);

    ntk.create_po(f1);

    ntk.create_po(f1_t);
    ntk.create_po(f2_t);
    ntk.create_po(f3_t);

    network_balancing_params ps;
    ps.unify_outputs = true;

    const auto ntk_r = extended_rank_view(ntk);

    mockturtle::equivalence_checking_stats st;
    const auto                             maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(ntk, ntk_r), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    const bool cec_m = *maybe_cec_m;
    CHECK(cec_m == 1);
    CHECK(ntk_r.check_validity() == 1);
}

TEST_CASE("Check equivalence checking for virtual PIs", "[extended-rank-view]")
{
    technology_network                     tec{};
    virtual_pi_network<technology_network> vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    const auto a_t = tec.create_pi();
    const auto b_t = tec.create_pi();

    const auto f1_t = tec.create_and(a_t, b_t);
    const auto f2_t = tec.create_and(b_t, a_t);
    const auto f3_t = tec.create_or(a_t, b_t);

    tec.create_po(f1_t);
    tec.create_po(f2_t);
    tec.create_po(f3_t);

    auto vpi_r = extended_rank_view(vpi);

    mockturtle::equivalence_checking_stats st;
    const auto                             maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, vpi), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    const bool cec_m = *maybe_cec_m;
    CHECK(cec_m == 1);
    CHECK(vpi_r.check_validity() == 1);
}
