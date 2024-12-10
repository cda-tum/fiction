//
// Created by benjamin on 14.06.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/algorithms/network_transformation/delete_virtual_pis.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/names_view.hpp>
#include <mockturtle/views/rank_view.hpp>

using namespace fiction;

TEST_CASE("Check name conservation", "[delete-virtual-pis]")
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

    CHECK(vpi.get_real_pi(c) == a);

    const auto non_vpi = delete_virtual_pis(vpi);
    // network name
    CHECK(non_vpi.get_network_name() == "vpi");
}

TEST_CASE("Discard views after virtual PI removal", "[delete-virtual-pis]")
{
    virtual_pi_network<mockturtle::names_view<technology_network>> vpi{};

    const auto a  = vpi.create_pi();
    const auto b  = vpi.create_pi();
    const auto f1 = vpi.create_and(a, b);
    vpi.create_po(f1);

    auto vpi_r = mockturtle::rank_view(vpi);
    CHECK(mockturtle::has_rank_position_v<decltype(vpi_r)>);
    // delete_virtual_pis returns the ntk unchanged if no virtual PIs are present.
    const auto test_del = delete_virtual_pis(vpi_r);
    CHECK(!mockturtle::has_rank_position_v<decltype(test_del)>);
}

TEST_CASE("Remove PIs and check equivalence technology_network", "[delete-virtual-pis]")
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
    REQUIRE(maybe_miter.has_value());
    if (maybe_miter.has_value())
    {
        mockturtle::equivalence_checking_stats st;
        const auto                             maybe_cec_m = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
        REQUIRE(maybe_cec_m.has_value());
        if (maybe_cec_m.has_value())
        {
            CHECK(maybe_cec_m.value() == 1);
            // check cloning
            const auto clone         = vpi.clone();
            auto       non_vpi_clone = delete_virtual_pis(clone);
            // check equivalence
            maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi);
            REQUIRE(maybe_miter.has_value());
            if (maybe_miter.has_value())
            {
                const auto maybe_cec_c = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
                REQUIRE(maybe_cec_c.has_value());
                if (maybe_cec_c.has_value())
                {
                    CHECK(maybe_cec_c.value() == 1);
                }
            }
        }
    }
}

TEMPLATE_TEST_CASE("Remove PIs and check equivalence", "[delete-virtual-pis]", mockturtle::aig_network,
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
    REQUIRE(maybe_miter.has_value());
    if (maybe_miter.has_value())
    {
        mockturtle::equivalence_checking_stats st;
        const auto                             maybe_cec_m = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
        REQUIRE(maybe_cec_m.has_value());
        if (maybe_cec_m.has_value())
        {
            CHECK(maybe_cec_m.value() == 1);
            // check cloning
            const auto clone         = vpi.clone();
            auto       non_vpi_clone = delete_virtual_pis(clone);
            // check equivalence
            maybe_miter = mockturtle::miter<technology_network>(tec, non_vpi_clone);
            REQUIRE(maybe_miter.has_value());
            if (maybe_miter.has_value())
            {
                const auto maybe_cec_c = mockturtle::equivalence_checking(*maybe_miter, {}, &st);
                REQUIRE(maybe_cec_c.has_value());
                if (maybe_cec_c.has_value())
                {
                    CHECK(maybe_cec_c.value() == 1);
                }
            }
        }
    }
}
