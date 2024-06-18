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
    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);


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

    vpi.remove_virtual_input_nodes();

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

    vpi.foreach_node([&](const auto& nd)
                     {
                         std::cout << "Nd:" << nd << "\n";
                         vpi.foreach_fanin(nd, [&](const auto& fi)
                                           {
                                               std::cout << "Fis:" << fi << "\n";
                                           });
                         if (vpi.is_virtual_pi(nd)){
                             std::cout << "Is virtual PI \n";
                         }
                     });

    //vpi.remove_virtual_input_nodes();

    mockturtle::rank_view vpi_r(vpi);

}
