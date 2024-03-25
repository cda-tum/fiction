//
// Created by marcel on 24.09.21.
//

#include <catch2/catch_test_macros.hpp>

#if (MUGEN)

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/one_pass_synthesis.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

using namespace fiction;

one_pass_synthesis_params configuration() noexcept
{
    one_pass_synthesis_params ps{};

    ps.enable_and   = true;
    ps.enable_not   = true;
    ps.enable_or    = true;
    ps.enable_wires = true;

    return ps;
}

one_pass_synthesis_params&& twoddwave(one_pass_synthesis_params&& ps) noexcept
{
    ps.scheme = "2DDWave";

    return std::move(ps);
}

one_pass_synthesis_params&& use(one_pass_synthesis_params&& ps) noexcept
{
    ps.scheme = "USE";

    return std::move(ps);
}

one_pass_synthesis_params&& res(one_pass_synthesis_params&& ps) noexcept
{
    ps.scheme     = "RES";
    ps.enable_maj = true;

    return std::move(ps);
}

one_pass_synthesis_params&& crossings(one_pass_synthesis_params&& ps) noexcept
{
    ps.crossings = true;

    return std::move(ps);
}

one_pass_synthesis_params&& maj(one_pass_synthesis_params&& ps) noexcept
{
    ps.enable_maj = true;

    return std::move(ps);
}

one_pass_synthesis_params&& async(const std::size_t t, one_pass_synthesis_params&& ps) noexcept
{
#if !defined(__APPLE__)
    ps.num_threads = t;
#endif
    return std::move(ps);
}

void check_stats(const one_pass_synthesis_stats& st) noexcept
{
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(st.time_total).count() > 0);
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Lyt, typename Ntk>
Lyt generate_layout(const Ntk& ntk, const one_pass_synthesis_params& ps)
{
    one_pass_synthesis_stats stats{};

    const auto layout = one_pass_synthesis<Lyt>(ntk, ps, &stats);

    REQUIRE(layout.has_value());
    check_stats(stats);

    print_gate_level_layout(std::cout, *layout);

    return *layout;
}

template <typename Lyt>
void apply_gate_library(const Lyt& lyt)
{
    CHECK_NOTHROW(apply_gate_library<qca_cell_clk_lyt, qca_one_library>(lyt));
}

template <typename Lyt, typename Ntk>
void check(const Ntk& ntk, const one_pass_synthesis_params& ps)
{
    const auto layout = generate_layout<Lyt>(ntk, ps);

    check_eq(ntk, layout);
    apply_gate_library(layout);
}

TEST_CASE("One-pass synthesis", "[one-pass]")
{
    SECTION("2DDWave clocking")
    {
        check<cart_gate_clk_lyt>(blueprints::and_or_network<mockturtle::mig_network>(),
                                 twoddwave(crossings(configuration())));
    }
    SECTION("USE clocking")
    {
        check<cart_gate_clk_lyt>(blueprints::and_or_network<mockturtle::mig_network>(),
                                 use(crossings(configuration())));
    }
    SECTION("RES clocking")
    {
        check<cart_gate_clk_lyt>(blueprints::and_or_network<mockturtle::mig_network>(),
                                 res(crossings(configuration())));
    }
    SECTION("Planar")
    {
        check<cart_gate_clk_lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
                                 twoddwave(configuration()));
    }
    SECTION("MAJ network")
    {
        check<cart_gate_clk_lyt>(blueprints::maj1_network<mockturtle::mig_network>(), res(maj(configuration())));
    }
    SECTION("Constant input MAJ network")
    {
        check<cart_gate_clk_lyt>(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>(),
                                 twoddwave(crossings(configuration())));
    }
    SECTION("Multi-output network")
    {
        check<cart_gate_clk_lyt>(blueprints::multi_output_and_network<mockturtle::aig_network>(),
                                 twoddwave(crossings(configuration())));
    }
    // #if !defined(__APPLE__)
    //     SECTION("Async")
    //     {
    //         check<cart_gate_clk_lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
    //                                  res(crossings(async(2, configuration()))));
    //     }
    // #endif
}

TEST_CASE("One-pass synthesis timeout", "[one-pass]")
{
    auto timeout_config    = use(configuration());
    timeout_config.timeout = 1u;  // allow only one second to find a solution; this will fail (and is tested for)

    const auto half_adder = blueprints::full_adder_network<mockturtle::aig_network>();
    const auto layout     = one_pass_synthesis<cart_gate_clk_lyt>(half_adder, timeout_config);

    // since a full adder cannot be synthesized in just one second, layout should not have a value
    CHECK(!layout.has_value());
}

TEST_CASE("Name conservation after one-pass synthesis", "[one-pass]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto layout = one_pass_synthesis<cart_gate_clk_lyt>(maj, res(configuration()));

    REQUIRE(layout.has_value());

    // network name
    CHECK(layout->get_layout_name() == "maj");

    // PI names
    CHECK(layout->get_name(layout->pi_at(0)) == "a");  // first PI
    CHECK(layout->get_name(layout->pi_at(1)) == "b");  // second PI
    CHECK(layout->get_name(layout->pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout->get_output_name(0) == "f");
}

#else  // MUGEN

TEST_CASE("One-pass synthesis", "[one-pass]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // MUGEN
