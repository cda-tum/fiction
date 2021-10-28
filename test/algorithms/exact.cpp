//
// Created by marcel on 08.10.21.
//

#if (FICTION_Z3_SOLVER)

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/apply_gate_library.hpp>
#include <fiction/algorithms/design_rule_violations.hpp>
#include <fiction/algorithms/exact.hpp>
#include <fiction/algorithms/network_utils.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/debug/network_writer.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <chrono>
#include <memory>
#include <type_traits>
#include <vector>

using namespace fiction;

template <typename Lyt>
exact_physical_design_params<Lyt> configuration() noexcept
{
    return {};
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& twoddwave(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_clocking<Lyt>());

    return std::move(ps);
}
template <typename Lyt>
exact_physical_design_params<Lyt>&& use(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(use_clocking<Lyt>());

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& res(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(res_clocking<Lyt>());

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& crossings(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.crossings = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& border_io(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.border_io = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& async(const std::size_t t, exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.num_threads = t;

    return std::move(ps);
}

template <typename Lyt>
std::vector<exact_physical_design_params<Lyt>> all_configurations() noexcept
{
    return {{
        twoddwave(configuration<Lyt>()),
        use(configuration<Lyt>()),
        res(configuration<Lyt>()),
        twoddwave(crossings(border_io(configuration<Lyt>()))),
        use(crossings(border_io(configuration<Lyt>()))),
        res(crossings(border_io(configuration<Lyt>()))),
        twoddwave(crossings(async(4ul, configuration<Lyt>()))),
        use(crossings(async(4ul, configuration<Lyt>()))),
        res(crossings(async(4ul, configuration<Lyt>()))),
    }};
}

void check_stats(const exact_physical_design_stats& st)
{
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(st.time_total).count() > 0);
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Lyt>
void check_drvs(const Lyt& lyt)
{
    gate_level_drv_params ps{};
    std::stringstream     ss{};
    ps.out = &ss;
    gate_level_drv_stats st{};
    gate_level_drvs(lyt, ps, &st);

    REQUIRE(st.drvs == 0);
}

template <typename Lyt, typename Ntk>
Lyt generate_layout(const Ntk& ntk, const exact_physical_design_params<Lyt>& ps)
{
    exact_physical_design_stats stats{};

    const auto layout = exact<Lyt>(ntk, ps, &stats);

    REQUIRE(layout.has_value());

    if constexpr (Lyt::max_fanin_size == 3)
    {
        print_gate_level_layout(std::cout, *layout);
        debug::write_dot_layout<Lyt, gate_layout_cartesian_drawer<Lyt, true>>(*layout);
    }
    else if constexpr (Lyt::max_fanin_size == 5)
    {
        debug::write_dot_layout<Lyt, gate_layout_hexagonal_drawer<Lyt, true>>(*layout);
    }

    check_drvs(*layout);
    check_stats(stats);

    return *layout;
}

template <typename Lyt>
void apply_gate_library(const Lyt& lyt)
{
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
    CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(lyt));
}

template <typename Ntk, typename Lyt>
void check(const Ntk& ntk, const exact_physical_design_params<Lyt>& ps)
{
    const auto layout = generate_layout<Lyt>(ntk, ps);

    check_eq(ntk, layout);
    apply_gate_library(layout);
}

template <typename Ntk>
void check_all_hexagonal(const Ntk& ntk)
{
    for (const auto& ps : all_configurations<hex_odd_row_gate_clk_lyt>()) { check(ntk, ps); }
    for (const auto& ps : all_configurations<hex_even_row_gate_clk_lyt>()) { check(ntk, ps); }
    for (const auto& ps : all_configurations<hex_odd_col_gate_clk_lyt>()) { check(ntk, ps); }
    for (const auto& ps : all_configurations<hex_even_col_gate_clk_lyt>()) { check(ntk, ps); }
}

template <typename Ntk>
void check_all_cartesian(const Ntk& ntk)
{
    for (const auto& ps : all_configurations<cart_gate_clk_lyt>()) { check(ntk, ps); }
}

TEST_CASE("Exact Cartesian physical design", "[exact]")
{
    check_all_cartesian(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    //    check_all(blueprints::maj1_network<mockturtle::mig_network>());
    check_all_cartesian(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>());
    check_all_cartesian(blueprints::and_or_network<mockturtle::mig_network>());
    //    check_all(blueprints::multi_output_and_network<mockturtle::aig_network>());
    //    check_all(blueprints::half_adder_network<mockturtle::aig_network>());
    check_all_cartesian(blueprints::se_coloring_corner_case_network<mockturtle::aig_network>());
    //    check_all(blueprints::mux21_network<mockturtle::aig_network>());
}

TEST_CASE("Exact hexagonal physical design", "[exact]")
{
    check_all_hexagonal(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_all_hexagonal(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>());
    check_all_hexagonal(blueprints::and_or_network<mockturtle::mig_network>());
    check_all_hexagonal(blueprints::se_coloring_corner_case_network<mockturtle::aig_network>());
}

TEST_CASE("High degree input networks", "[exact]")
{
    CHECK_THROWS_AS(exact<cart_gate_clk_lyt>(blueprints::maj1_network<mockturtle::mig_network>(),
                                             twoddwave(configuration<cart_gate_clk_lyt>())),
                    high_degree_fanin_exception);
    CHECK_THROWS_AS(exact<cart_gate_clk_lyt>(blueprints::maj1_network<mockturtle::mig_network>(),
                                             use(configuration<cart_gate_clk_lyt>())),
                    high_degree_fanin_exception);

    CHECK_NOTHROW(exact<cart_gate_clk_lyt>(blueprints::maj1_network<mockturtle::mig_network>(),
                                           res(configuration<cart_gate_clk_lyt>())));
}

// TEST_CASE("Timeout", "[exact]")
//{
//     using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<cartesian::ucoord_t>>>>;
//
//     exact_physical_design_params<gate_layout> timeout_config{};
//
//     timeout_config.scheme  = std::make_shared<clocking_scheme<coordinate<Lyt>>>(use_4_clocking);
//     timeout_config.timeout = 1u;  // allow only one second to find a solution; this will fail (and is tested for)
//
//     const auto half_adder = blueprints::half_adder_network<mockturtle::aig_network>();
//
//     const auto layout = exact<gate_layout>(half_adder, timeout_config);
//
//     // since a half adder cannot be placed and routed in just one second, layout should not have a value
//     CHECK(!layout.has_value());
// }

#endif  // FICTION_Z3_SOLVER
