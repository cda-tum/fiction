//
// Created by benjamin on 6/18/24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/extended_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/views/rank_view.hpp>

using namespace fiction;

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

    auto blc = is_balanced(tec);
    std::cout << "Balanced: " << blc << "\n";

    network_balancing_params ps;
    ps.unify_outputs = true;

    auto tec_balanced = network_balancing<technology_network>(tec, ps);
    blc = is_balanced(tec_balanced);
    std::cout << "Balanced: " << blc << "\n";

    auto vpi_r = extended_rank_view(tec_balanced);

    std::vector<technology_network::node> nodes = {13, 10};
    vpi_r.modify_rank(2, nodes);

    vpi_r.foreach_po([&](const auto& nd) {
                           std::cout << "Nd:" << nd << "\n";
                           /*if (vpi_r.is_inv(nd))
                           {
                               std::cout << "is not" << "\n";
                           }
                           if (vpi_r.is_buf(nd))
                           {
                               std::cout << "is buf" << "\n";
                           }
                           vpi_r.foreach_fanin(nd, [&](const auto& fi) { std::cout << "Fis:" << fi << "\n"; });*/
                           auto rnk = vpi_r.rank_position(nd);
                           auto lvl = vpi_r.level(nd);
                           std::cout << "Level: " << lvl << "\n";
                           std::cout << "Rank: " << rnk << "\n";
                       });

    std::cout << "Valid: " << vpi_r.check_validity() << std::endl;

    CHECK(1 == 1);
}

TEST_CASE("Check equivalence checking", "[extended-rank-view]")
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

    std::cout << "Num Cis" << vpi_r.num_cis() << std::endl;

    vpi_r.remove_virtual_input_nodes<virtual_pi_network>();

    std::cout << "Num Cis" << vpi_r.num_cis() << std::endl;

    mockturtle::equivalence_checking_stats st;
    bool cec_m = *mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, vpi), {}, &st);
    CHECK(cec_m == 1);

    CHECK(1 == 1);
}
