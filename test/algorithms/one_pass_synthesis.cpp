//
// Created by marcel on 24.09.21.
//

#if (MUGEN)

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/apply_gate_library.hpp>
#include <fiction/algorithms/one_pass_synthesis.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

using namespace fiction;

std::vector<one_pass_synthesis_params> configurations() noexcept
{
    one_pass_synthesis_params twoddwave_config{};

    twoddwave_config.scheme       = std::make_shared<clocking_scheme<coord_t>>(twoddwave_4_clocking);
    twoddwave_config.enable_and   = true;
    twoddwave_config.enable_not   = true;
    twoddwave_config.enable_or    = true;
    twoddwave_config.enable_wires = true;
    twoddwave_config.crossings    = true;

    one_pass_synthesis_params use_config{};

    use_config.scheme       = std::make_shared<clocking_scheme<coord_t>>(use_4_clocking);
    use_config.enable_and   = true;
    use_config.enable_not   = true;
    use_config.enable_or    = true;
    use_config.enable_wires = true;
    use_config.crossings    = true;

    one_pass_synthesis_params res_config{};

    res_config.scheme       = std::make_shared<clocking_scheme<coord_t>>(res_4_clocking);
    res_config.enable_and   = true;
    res_config.enable_not   = true;
    res_config.enable_or    = true;
    res_config.enable_maj   = true;
    res_config.enable_wires = true;
    res_config.crossings    = true;

#if !defined(__APPLE__)
    one_pass_synthesis_params async_config{};

    async_config.scheme       = std::make_shared<clocking_scheme<coord_t>>(twoddwave_4_clocking);
    async_config.enable_and   = true;
    async_config.enable_not   = true;
    async_config.enable_or    = true;
    async_config.enable_wires = true;
    async_config.crossings    = true;
    async_config.num_threads  = 4ul;
#endif

    return {{twoddwave_config, use_config, res_config
#if !defined(__APPLE__)
             ,
             async_config
#endif
    }};
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
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;
    CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(lyt));
}

template <typename Ntk>
void check_all(const Ntk& ntk)
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coord_t>>>>;

    for (const auto& ps : configurations())
    {
        const auto layout = generate_layout<gate_layout>(ntk, ps);

        check_eq(ntk, layout);
        apply_gate_library(layout);
    }
}

TEST_CASE("One-pass synthesis", "[one-pass]")
{
    check_all(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_all(blueprints::maj1_network<mockturtle::mig_network>());
    check_all(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>());
    check_all(blueprints::multi_output_and_network<mockturtle::aig_network>());
    check_all(blueprints::half_adder_network<mockturtle::aig_network>());
    check_all(blueprints::se_coloring_corner_case_network<mockturtle::aig_network>());
}

TEST_CASE("Timeout", "[one-pass]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coord_t>>>>;

    one_pass_synthesis_params timeout_config{};

    timeout_config.scheme       = std::make_shared<clocking_scheme<coord_t>>(use_4_clocking);
    timeout_config.enable_and   = true;
    timeout_config.enable_not   = true;
    timeout_config.enable_or    = true;
    timeout_config.enable_wires = true;
    timeout_config.crossings    = true;
    timeout_config.timeout      = 1u;  // allow only one second to find a solution; this will fail (and is tested for)

    const auto half_adder = blueprints::half_adder_network<mockturtle::aig_network>();
    const auto layout = one_pass_synthesis<gate_layout>(half_adder, timeout_config);

    // since a half adder cannot be synthesized in just one second, layout should not have a value
    CHECK(!layout.has_value());
}

#endif  // MUGEN
