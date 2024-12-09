//
// Created by benjamin on 14.06.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>

using namespace fiction;

TEST_CASE("Check name conservation", "[virtual-pi-view]")
{
    virtual_pi_network<mockturtle::names_view<technology_network>> vpi{};

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto f1 = vpi.create_and(a, b);
    // delete_virtual_pis returns the ntk unchanged if no virtual PIs are present.
    const auto test_del = delete_virtual_pis(vpi);
    CHECK(test_del.num_virtual_pis() == 0);
    // continue ntk creation
    const auto c  = vpi.create_virtual_pi(a);
    const auto d  = vpi.create_virtual_pi(b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    vpi.set_network_name("vpi");

    const auto non_vpi = delete_virtual_pis(vpi);
    // network name
    CHECK(non_vpi.get_network_name() == "vpi");
}

TEST_CASE("Check loops", "[virtual-pi-view]")
{
    virtual_pi_network<technology_network> vpi{};

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto c  = vpi.create_virtual_pi(a);
    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);

    vpi.create_po(f1);
    vpi.create_po(f2);

    std::uint32_t size_real_pis    = 0;
    std::uint32_t size_virtual_pis = 0;
    std::uint32_t size_real_cis    = 0;
    std::uint32_t size_virtual_cis = 0;
    vpi.foreach_real_pi(
        [&](const auto& pi)
        {
            if (vpi.is_real_pi(pi))
            {
                ++size_real_pis;
            }
        });
    vpi.foreach_virtual_pi(
        [&](const auto& pi)
        {
            if (vpi.is_virtual_pi(pi))
            {
                ++size_virtual_pis;
            }
        });
    vpi.foreach_real_ci(
        [&](const auto& ci)
        {
            if (vpi.is_real_ci(ci))
            {
                ++size_real_cis;
            }
        });
    vpi.foreach_virtual_ci(
        [&](const auto& ci)
        {
            if (vpi.is_virtual_pi(ci))
            {
                ++size_virtual_cis;
            }
        });

    CHECK(size_real_pis == size_real_cis);
    CHECK(size_virtual_pis == size_virtual_cis);
    CHECK(size_real_pis == vpi.num_real_pis());
    CHECK(size_virtual_pis == vpi.num_virtual_pis());
}

TEST_CASE("Copy technology network and size consistency", "[virtual-pi-view]")
{
    virtual_pi_network<technology_network> vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();

    const auto f1 = vpi.create_and(a, b);

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    // This is the size with the virtual PIs
    CHECK(vpi.size() == 9);
    // This is the size disregarding virtual PIs. So this corresponds to the size in a normal technology_network
    CHECK(vpi.real_size() == 7);
    // check cloning
    const auto clone = vpi.clone();
    CHECK(clone.size() == vpi.size());
    // check virtual PI removal
    const auto non_vpi = delete_virtual_pis(vpi);
    // After removing/remapping the virtual PIs to their real PIs the sizes are equal
    CHECK(non_vpi.real_size() == non_vpi.size());
    CHECK(non_vpi.num_pis() == vpi.num_real_pis());
    CHECK(non_vpi.size() == vpi.size() - vpi.num_virtual_pis());
    CHECK(non_vpi.size() == 7);
    //
    CHECK(vpi.num_real_pis() == vpi.num_real_cis());
    CHECK(vpi.num_virtual_pis() == vpi.num_virtual_cis());
}

TEMPLATE_TEST_CASE("Copy networks and size consistency", "[virtual-pi-view]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    virtual_pi_network<TestType> vpi{};

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto f1 = vpi.create_and(a, b);
    // create virtual PIs
    const auto c  = vpi.create_virtual_pi(a);
    const auto d  = vpi.create_virtual_pi(b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);
    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    // This is the size with the virtual PIs
    CHECK(vpi.size() == 8);
    // This is the size disregarding virtual PIs. So this corresponds to the size in a normal technology_network
    CHECK(vpi.real_size() == 6);
    // check cloning
    const auto clone = vpi.clone();
    CHECK(clone.size() == vpi.size());
    // check virtual PI removal
    const auto non_vpi = delete_virtual_pis(vpi);
    // After removing/remapping the virtual PIs to their real PIs the sizes are equal
    CHECK(non_vpi.real_size() == non_vpi.size());
    // Minus one, since AIG nodes get hashed when creating the new AIG
    CHECK(non_vpi.size() == vpi.size() - vpi.num_virtual_pis() - 1);
    CHECK(non_vpi.size() == 5);
    //
    CHECK(vpi.num_real_pis() == vpi.num_real_cis());
    CHECK(vpi.num_virtual_pis() == vpi.num_virtual_cis());
}

TEST_CASE("Remove PIs and check equivalence technology_network", "[virtual-pi-view]")
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

    virtual_pi_network<technology_network> vpi{};

    const auto a   = vpi.create_pi();
    const auto b   = vpi.create_pi();
    const auto c   = vpi.create_pi();
    const auto d   = vpi.create_pi();
    const auto d_v = vpi.create_virtual_pi(d);
    const auto e   = vpi.create_pi();
    const auto f7  = vpi.create_not(b);
    const auto f8  = vpi.create_and(a, b);
    const auto f9  = vpi.create_and(c, d);
    const auto f10 = vpi.create_buf(c);
    const auto f11 = vpi.create_and(d_v, e);
    const auto f12 = vpi.create_buf(f7);
    const auto f13 = vpi.create_and(f8, f9);
    const auto f14 = vpi.create_and(f10, f11);
    vpi.create_po(f12);
    vpi.create_po(f13);
    vpi.create_po(f14);
    // delete virtualPIs
    auto non_vpi = delete_virtual_pis(vpi);
    CHECK(non_vpi.size() == vpi.size() - vpi.num_virtual_pis());
    // check equivalence
    auto maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi);
    if (!maybe_miter.has_value())
    {
        CHECK(false);
    }
    mockturtle::equivalence_checking_stats st;
    const auto                             maybe_cec_m = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
    if (!maybe_cec_m.has_value())
    {
        CHECK(false);
    }
    CHECK(maybe_cec_m.value() == 1);
    // check cloning
    const auto clone         = vpi.clone();
    auto       non_vpi_clone = delete_virtual_pis(clone);
    // check equivalence
    maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi);
    if (!maybe_miter.has_value())
    {
        CHECK(false);
    }
    const auto maybe_cec_c = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
    if (!maybe_cec_c.has_value())
    {
        CHECK(false);
    }
    CHECK(maybe_cec_c.value() == 1);
}

TEMPLATE_TEST_CASE("Remove PIs and check equivalence", "[virtual-pi-view]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    TestType tec{};

    const auto a_t  = tec.create_pi();
    const auto b_t  = tec.create_pi();
    const auto f1_t = tec.create_and(a_t, b_t);
    const auto f2_t = tec.create_and(b_t, a_t);
    const auto f3_t = tec.create_and(a_t, b_t);
    tec.create_po(f1_t);
    tec.create_po(f2_t);
    tec.create_po(f3_t);

    virtual_pi_network<TestType> vpi{};

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto c  = vpi.create_virtual_pi(a);
    const auto d  = vpi.create_virtual_pi(b);
    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_and(a, d);
    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    /*When creating the AIG, the nodes will be hashed. Since all AND nodes get mapped to the same two PIs after deleting
     * the virtual PIs, only one node will be created.*/
    auto non_vpi = delete_virtual_pis(vpi);
    // check size consistency
    CHECK(non_vpi.size() == vpi.size() - vpi.num_virtual_pis() - 2);
    CHECK(non_vpi.real_size() == non_vpi.size());
    // check equivalence
    auto maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi);
    if (!maybe_miter.has_value())
    {
        CHECK(false);
    }
    mockturtle::equivalence_checking_stats st;
    const auto                             maybe_cec_m = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
    if (!maybe_cec_m.has_value())
    {
        CHECK(false);
    }
    CHECK(maybe_cec_m.value() == 1);
    // check cloning
    const auto clone         = vpi.clone();
    auto       non_vpi_clone = delete_virtual_pis(clone);
    // check equivalence
    maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi_clone);
    if (!maybe_miter.has_value())
    {
        CHECK(false);
    }
    const auto maybe_cec_c = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
    if (!maybe_cec_c.has_value())
    {
        CHECK(false);
    }
    CHECK(maybe_cec_c.value() == 1);
}
