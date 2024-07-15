//
// Created by benjamin on 6/17/24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/views/rank_view.hpp>

using namespace fiction;

TEST_CASE("Copy network and size consistency", "[virtual-pi-view]")
{
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();

    const auto f1 = tec.create_and(a, b);

    virtual_pi_network vpi{tec};
    const auto         c = vpi.create_virtual_pi(a);
    const auto         d = vpi.create_virtual_pi(b);

    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    // virtual_pi_network is a deep copy. THe original network won't be affected by any changes after the copy
    CHECK(tec.size() == 5);
    // This is the size disregarding virtual PIs. So this would correspond to the size in a normal technology_network
    CHECK(vpi.size_real() == 7);
    // This is the size with the virtual PIs
    CHECK(vpi.size() == 9);
    vpi.remove_virtual_input_nodes();
    // After removing/remapping the virtual PIs to their real PIs the sizes are equal
    CHECK(vpi.size_real() == vpi.size());
    CHECK(vpi.size() == 7);
}

TEST_CASE("Remove PIs and check equivalence", "[virtual-pi-view]")
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

    vpi.foreach_node(
       [&](const auto& nd)
       {
           std::cout << "Nd:" << nd << "\n";
           if (vpi.is_pi_real(nd))
           {
               std::cout << "is pi real" << "\n";
           }
           if (vpi.is_pi_virtual(nd))
           {
               std::cout << "is pi virtual" << "\n";
           }
           if (vpi.is_inv(nd))
           {
               std::cout << "is not" << "\n";
           }
           if (vpi.is_buf(nd))
           {
               std::cout << "is buf" << "\n";
           }
           if (vpi.is_po(nd))
           {
               std::cout << "is po" << "\n";
           }
       });

    vpi.remove_virtual_input_nodes();

    vpi.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Nd:" << nd << "\n";
            if (vpi.is_pi_real(nd))
            {
                std::cout << "is pi real" << "\n";
            }
            if (vpi.is_pi_virtual(nd))
            {
                std::cout << "is pi virtual" << "\n";
            }
            if (vpi.is_inv(nd))
            {
                std::cout << "is not" << "\n";
            }
            if (vpi.is_buf(nd))
            {
                std::cout << "is buf" << "\n";
            }
            if (vpi.is_po(nd))
            {
                std::cout << "is po" << "\n";
            }
        });

    mockturtle::equivalence_checking_stats st;
    bool cec_m = *mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, vpi), {}, &st);
    CHECK(cec_m == 1);
}

TEST_CASE("Rank View on virtual PIs", "[virtual-rank-view]")
{
    virtual_pi_network vpi{};

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

    std::cout << "real tec num_gates: " << vpi.size() << std::endl;
    std::cout << "virt tec num_gates: " << vpi.size_real() << std::endl;

    /*vpi.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Nd:" << nd << "\n";
            vpi.foreach_fanin(nd, [&](const auto& fi) { std::cout << "Fis:" << fi << "\n"; });
            if (vpi.is_virtual_pi(nd))
            {
                std::cout << "Is virtual PI \n";
            }
        });*/

    // vpi.remove_virtual_input_nodes();

    mockturtle::rank_view vpi_r(vpi);
   /* vpi_r.foreach_pi([&](const auto& nd) {
                           std::cout << "Nd:" << nd << "\n";
                           auto rnk = vpi_r.rank_position(nd);
                           auto lvl = vpi_r.level(nd);
                           std::cout << "Level: " << lvl << "\n";
                           std::cout << "Rank: " << rnk << "\n";
                       });*/
    std::cout << "r pis: " <<vpi_r.num_pis_virtual() << "\n";
    std::cout << "n pis: " <<vpi.num_pis_virtual() << "\n";
    vpi.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi:" << nd << "\n";
        });
    vpi_r.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi_r:" << nd << "\n";
        });

    vpi_r.remove_virtual_input_nodes();

    std::cout << "r pis: " << vpi_r.num_pis_virtual() << "\n";
    std::cout << "n pis: " << vpi.num_pis_virtual() << "\n";

    vpi.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi:" << nd << "\n";
        });
    vpi_r.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi_r:" << nd << "\n";
        });

    vpi.remove_virtual_input_nodes();

    std::cout << "r pis: " <<vpi_r.num_pis_virtual() << "\n";
    std::cout << "n pis: " <<vpi.num_pis_virtual() << "\n";

    vpi.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi:" << nd << "\n";
        });
    vpi_r.foreach_node(
        [&](const auto& nd)
        {
            std::cout << "Out_vpi_r:" << nd << "\n";
        });
}

TEST_CASE("Remove PIs and check equivalence second", "[virtual-pi-view]")
{
    technology_network tec{};

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

    virtual_pi_network vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();
    const auto c = vpi.create_pi();
    const auto d = vpi.create_pi();

    const auto d_v = vpi.create_virtual_pi(d);

    const auto e = vpi.create_pi();

    // const auto d_v = vpi.create_virtual_pi(d);

    const auto f7 = vpi.create_not(b);
    const auto f8 = vpi.create_and(a, b);
    const auto f9 = vpi.create_and(c, d);
    const auto f10 = vpi.create_buf(c);
    const auto f11 = vpi.create_and(d_v, e);

    const auto f12 = vpi.create_buf(f7);
    const auto f13 = vpi.create_and(f8, f9);
    const auto f14 = vpi.create_and(f10, f11);

    vpi.create_po(f12);
    vpi.create_po(f13);
    vpi.create_po(f14);

    vpi.remove_virtual_input_nodes();

    mockturtle::equivalence_checking_stats st;
    bool cec_m = *mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, vpi), {}, &st);
    CHECK(cec_m == 1);
}
