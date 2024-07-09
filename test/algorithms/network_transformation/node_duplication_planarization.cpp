//
// Created by benjamin on 6/11/24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include <mockturtle/views/rank_view.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>

using namespace fiction;

TEST_CASE("First test", "[node-duplication-planarization]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x1, x2);
    const auto f3 = tec.create_and(x2, x3);
    const auto f4 = tec.create_not(x3);
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);
    tec.create_po(f4);

    // auto maj = blueprints::maj1_network<mockturtle::aig_network>();

    const auto planarized_maj = node_duplication_planarization<technology_network>(tec);

    CHECK(1 == 1);
}

TEST_CASE("Multi Level test", "[node-duplication-planarization]")
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

    // auto maj = blueprints::maj1_network<mockturtle::aig_network>();

    const auto planarized_maj = node_duplication_planarization<technology_network>(tec);

    CHECK(1 == 1);
}

// Implement reverse balancing function. Also it should propagate PIs all to the last level
TEST_CASE("2-ary Balancing Test", "[node-duplication-planarization]")
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
    const auto f3 = tec.create_nary_or({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    auto blc = is_balanced(tec);
    std::cout << "Balanced: " << blc << "\n";

    network_balancing_params ps;
    ps.unify_outputs = true;

    auto tec_b = network_balancing<technology_network>(tec, ps);
    blc = is_balanced(tec_b);
    std::cout << "Balanced: " << blc << "\n";

    const auto vpi_r = mockturtle::rank_view(tec_b);
    /*vpi_r.foreach_node([&](const auto& nd) {
                         std::cout << "Nd:" << nd << "\n";
                         if (vpi_r.is_inv(nd))
                         {
                             std::cout << "is not" << "\n";
                         }
                         if (vpi_r.is_buf(nd))
                         {
                             std::cout << "is buf" << "\n";
                         }
                         vpi_r.foreach_fanin(nd, [&](const auto& fi) { std::cout << "Fis:" << fi << "\n"; });
                         auto rnk = vpi_r.rank_position(nd);
                         auto lvl = vpi_r.level(nd);
                         std::cout << "Level: " << lvl << "\n";
                         std::cout << "Rank: " << rnk << "\n";
                     });*/

    auto planarized_maj = node_duplication_planarization<technology_network>(tec_b);

    planarized_maj.foreach_node([&](const auto& nd) {
                         std::cout << "Nd:" << nd << "\n";
                         if (planarized_maj.is_pi_real(nd))
                         {
                             std::cout << "is pi real" << "\n";
                         }
                         if (planarized_maj.is_pi_virtual(nd))
                         {
                             std::cout << "is pi virtual" << "\n";
                         }
                         if (planarized_maj.is_inv(nd))
                         {
                             std::cout << "is not" << "\n";
                         }
                         if (planarized_maj.is_buf(nd))
                         {
                             std::cout << "is buf" << "\n";
                         }
                         if (planarized_maj.is_and(nd))
                         {
                             std::cout << "is and" << "\n";
                         }
                         if (planarized_maj.is_po(nd))
                         {
                             std::cout << "is po" << "\n";
                         }
                         planarized_maj.foreach_fanin(nd, [&](const auto& fi) { std::cout << "Fis:" << fi << "\n"; });
                     });

    planarized_maj.remove_virtual_input_nodes();

    planarized_maj.foreach_node([&](const auto& nd) {
                                    std::cout << "Nd:" << nd << "\n";
                                    if (planarized_maj.is_pi_real(nd))
                                    {
                                        std::cout << "is pi real" << "\n";
                                    }
                                    if (planarized_maj.is_pi_virtual(nd))
                                    {
                                        std::cout << "is pi virtual" << "\n";
                                    }
                                    if (planarized_maj.is_inv(nd))
                                    {
                                        std::cout << "is not" << "\n";
                                    }
                                    if (planarized_maj.is_buf(nd))
                                    {
                                        std::cout << "is buf" << "\n";
                                    }
                                    if (planarized_maj.is_and(nd))
                                    {
                                        std::cout << "is and" << "\n";
                                    }
                                    if (planarized_maj.is_po(nd))
                                    {
                                        std::cout << "is po" << "\n";
                                    }
                                    planarized_maj.foreach_fanin(nd, [&](const auto& fi) { std::cout << "Fis:" << fi << "\n"; });
                                });

    mockturtle::equivalence_checking_stats st;
    bool cec_m = *mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, planarized_maj), {}, &st);
    CHECK(cec_m == 1);

    CHECK(1 == 1);
}

TEST_CASE("3-ary Balancing Test", "[node-duplication-planarization]")
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
    const auto f3 = tec.create_nary_or({x3, x4, x5});
    const auto f4 = tec.create_maj(x1, x2, f3);
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);
    tec.create_po(f4);

    auto blc = is_balanced(tec);

    network_balancing_params ps;
    ps.unify_outputs = true;

    auto tec_b = network_balancing<technology_network>(tec, ps);
    blc = is_balanced(tec_b);

    const auto vpi_r = mockturtle::rank_view(tec_b);

    auto planarized_maj = node_duplication_planarization<technology_network>(tec_b);

    planarized_maj.remove_virtual_input_nodes();

    mockturtle::equivalence_checking_stats st;
    bool cec_m = *mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, planarized_maj), {}, &st);
    CHECK(cec_m == 1);

    CHECK(1 == 1);
}
