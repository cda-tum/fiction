//
// Created by benjamin on 14.06.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>

#include <cstdint>

using namespace fiction;

TEST_CASE("Check loops", "[virtual-pi-network]")
{
    virtual_pi_network<technology_network> vpi{};

    CHECK(is_virtual_network_type_v<decltype(vpi)>);

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto c  = vpi.create_virtual_pi(a);
    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);

    vpi.create_po(f1);
    vpi.create_po(f2);

    CHECK(vpi.get_real_pi(c) == a);

    uint32_t size_real_pis    = 0;
    uint32_t size_virtual_pis = 0;
    uint32_t size_real_cis    = 0;
    uint32_t size_virtual_cis = 0;
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

TEST_CASE("Copy technology network and size consistency", "[virtual-pi-network]")
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
    CHECK(vpi.num_real_pis() == vpi.num_real_cis());
    CHECK(vpi.num_virtual_pis() == vpi.num_virtual_cis());
}

TEMPLATE_TEST_CASE("Copy networks and size consistency", "[virtual-pi-network]", mockturtle::aig_network,
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
    CHECK(vpi.num_real_pis() == vpi.num_real_cis());
    CHECK(vpi.num_virtual_pis() == vpi.num_virtual_cis());
}
