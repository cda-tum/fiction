//
// Created by marcel on 08.10.21.
//

#if (FICTION_Z3_SOLVER)

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/apply_gate_library.hpp>
#include <fiction/algorithms/exact.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/debug/network_writer.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <chrono>
#include <memory>
#include <type_traits>
#include <vector>

using namespace fiction;

template <typename Lyt>
std::vector<exact_physical_design_params<Lyt>> configurations() noexcept
{
    exact_physical_design_params<Lyt> twoddwave_config{};

    twoddwave_config.scheme    = std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_4_clocking<Lyt>());
    twoddwave_config.crossings = true;

    exact_physical_design_params<Lyt> use_config{};

    use_config.scheme    = std::make_shared<clocking_scheme<coordinate<Lyt>>>(use_4_clocking<Lyt>());
    use_config.crossings = true;

    exact_physical_design_params<Lyt> res_config{};

    res_config.scheme    = std::make_shared<clocking_scheme<coordinate<Lyt>>>(res_4_clocking<Lyt>());
    res_config.crossings = true;

    exact_physical_design_params<Lyt> async_config{};

    async_config.scheme      = std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_4_clocking<Lyt>());
    async_config.crossings   = true;
    async_config.num_threads = 2ul;

    return {{twoddwave_config, use_config, res_config, async_config}};
}

void check_stats(const exact_physical_design_stats& st) noexcept
{
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(st.time_total).count() > 0);
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Lyt, typename Ntk>
Lyt generate_layout(const Ntk& ntk, const exact_physical_design_params<Lyt>& ps)
{
    exact_physical_design_stats stats{};

    const auto layout = exact<Lyt>(ntk, ps, &stats);

    REQUIRE(layout.has_value());
    check_stats(stats);

    print_gate_level_layout(std::cout, *layout);

    debug::write_dot_layout(*layout);

    return *layout;
}

template <typename Lyt>
void apply_gate_library(const Lyt& lyt)
{
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
    CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(lyt));
}

template <typename Ntk>
void check_all(const Ntk& ntk)
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<cartesian::ucoord_t>>>>;

    for (const auto& ps : configurations<gate_layout>())
    {
        const auto layout = generate_layout<gate_layout>(ntk, ps);

        check_eq(ntk, layout);
        apply_gate_library(layout);
    }
}

TEST_CASE("Exact physical design", "[exact]")
{
    check_all(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_all(blueprints::maj1_network<mockturtle::mig_network>());
    check_all(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>());
    check_all(blueprints::multi_output_and_network<mockturtle::aig_network>());
    //    check_all(blueprints::half_adder_network<mockturtle::aig_network>());
    check_all(blueprints::se_coloring_corner_case_network<mockturtle::aig_network>());
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
