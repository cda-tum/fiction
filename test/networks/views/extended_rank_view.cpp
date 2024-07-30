//
// Created by benjamin on 18.06.24.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/extended_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/buffered.hpp>
#include <mockturtle/networks/cover.hpp>
#include <mockturtle/networks/crossed.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/rank_view.hpp>

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

    const std::vector<technology_network::node> nodes = {13, 10};
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
    const auto maybe_cec_m = mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(ntk, ntk_r), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    const bool cec_m = *maybe_cec_m;
    CHECK(cec_m == 1);
    CHECK(ntk_r.check_validity() == 1);
}

TEST_CASE("Check equivalence checking for virtual PIs", "[extended-rank-view]")
{
    technology_network tec{};
    virtual_pi_network vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();

    const auto a_t = tec.create_pi();
    const auto b_t = tec.create_pi();

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    const auto f1_t = tec.create_and(a_t, b);
    const auto f2_t = tec.create_and(b_t, a_t);
    const auto f3_t = tec.create_or(a_t, b_t);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    tec.create_po(f1_t);
    tec.create_po(f2_t);
    tec.create_po(f3_t);

    network_balancing_params ps;
    ps.unify_outputs = true;

    auto vpi_r = extended_rank_view(vpi);

    vpi_r.remove_virtual_input_nodes<virtual_pi_network>();

    mockturtle::equivalence_checking_stats st;
    const auto maybe_cec_m = mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, vpi_r), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    const bool cec_m = *maybe_cec_m;
    CHECK(cec_m == 1);
    CHECK(vpi_r.check_validity() == 1);
}
