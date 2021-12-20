//
// Created by marcel on 08.10.21.
//

#if (FICTION_Z3_SOLVER)

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/apply_gate_library.hpp>
#include <fiction/algorithms/critical_path_length_and_throughput.hpp>
#include <fiction/algorithms/design_rule_violations.hpp>
#include <fiction/algorithms/exact.hpp>
#include <fiction/algorithms/network_utils.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/networks/topology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

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
exact_physical_design_params<Lyt>&& open(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(open_clocking<Lyt>());

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& columnar(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(columnar_clocking<Lyt>());

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& row(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.scheme = std::make_shared<clocking_scheme<coordinate<Lyt>>>(row_clocking<Lyt>());

    return std::move(ps);
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
exact_physical_design_params<Lyt>&& desynchronize(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.desynchronize = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& straight_inverter(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.straight_inverters = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& sync_elems(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.synchronization_elements = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& topolinano(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.technology_specifics = technology_constraints::TOPOLINANO;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& async(const std::size_t t, exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.num_threads = t;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& minimize_wires(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.minimize_wires = true;

    return std::move(ps);
}

template <typename Lyt>
exact_physical_design_params<Lyt>&& minimize_crossings(exact_physical_design_params<Lyt>&& ps) noexcept
{
    ps.minimize_crossings = true;

    return std::move(ps);
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
void check_tp(const Lyt& lyt, const uint64_t tp)
{
    critical_path_length_and_throughput_stats st{};
    critical_path_length_and_throughput(lyt, &st);

    if (st.throughput != tp)
    {
        print_gate_level_layout(std::cout, lyt);
        std::cout << lyt.get_clocking_scheme().name << std::endl;
    }

    CHECK(st.throughput == tp);
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

    check_drvs(*layout);
    check_stats(stats);

    return *layout;
}

template <typename CellLyt, typename Lib, typename GateLyt>
void check_apply_lib(const GateLyt& lyt)
{
    CHECK_NOTHROW(apply_gate_library<CellLyt, Lib>(lyt));
}

template <typename CellLyt, typename Lib, typename Ntk, typename GateLyt>
void check_with_gate_library(const Ntk& ntk, const exact_physical_design_params<GateLyt>& ps)
{
    const auto layout = generate_layout<GateLyt>(ntk, ps);

    check_eq(ntk, layout);
    check_tp(layout, 1);
    check_apply_lib<CellLyt, Lib>(layout);
}

template <typename Ntk, typename Lyt>
void check_without_gate_library(const Ntk& ntk, const exact_physical_design_params<Lyt>& ps)
{
    const auto layout = generate_layout<Lyt>(ntk, ps);

    check_eq(ntk, layout);
    check_tp(layout, 1);
}

template <typename Lyt>
bool has_straight_inverters(const Lyt& lyt) noexcept
{
    bool only_straight_inverters = true;
    lyt.foreach_gate(
        [&lyt, &only_straight_inverters](const auto& g)
        {
            if constexpr (has_is_inv_v<Lyt>)
            {
                if (lyt.is_inv(g))
                {
                    if (!lyt.has_opposite_incoming_and_outgoing_signals(lyt.get_tile(g)))
                    {
                        only_straight_inverters = false;
                        return false;  // break loop
                    }
                }
            }

            return true;  // continue
        });

    return only_straight_inverters;
}

TEST_CASE("Exact Cartesian physical design", "[exact]")
{
    SECTION("Open clocking")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::and_or_network<mockturtle::mig_network>(), open(crossings(configuration<cart_gate_clk_lyt>())));
    }
    SECTION("2DDWave clocking")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            twoddwave(crossings(configuration<cart_gate_clk_lyt>())));
    }
    SECTION("USE clocking")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::and_or_network<mockturtle::mig_network>(), use(crossings(configuration<cart_gate_clk_lyt>())));
    }
    SECTION("RES clocking")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::and_or_network<mockturtle::mig_network>(), res(crossings(configuration<cart_gate_clk_lyt>())));
    }
    SECTION("Border I/O")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            twoddwave(crossings(border_io(configuration<cart_gate_clk_lyt>()))));
    }
    SECTION("Planar")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
            twoddwave(configuration<cart_gate_clk_lyt>()));
    }
    SECTION("Straight inverters")
    {
        CHECK(has_straight_inverters(
            generate_layout<cart_gate_clk_lyt>(blueprints::inverter_network<topology_network>(),
                                               use(straight_inverter(configuration<cart_gate_clk_lyt>())))));
        CHECK(has_straight_inverters(
            generate_layout<cart_gate_clk_lyt>(blueprints::inverter_network<topology_network>(),
                                               open(straight_inverter(configuration<cart_gate_clk_lyt>())))));
    }
    SECTION("Global synchronization")
    {
        SECTION("enabled")
        {
            check_tp(
                generate_layout<cart_gate_clk_lyt>(blueprints::one_to_five_path_difference_network<topology_network>(),
                                                   use(configuration<cart_gate_clk_lyt>())),
                1);
        }
        SECTION("disabled")
        {
            check_tp(
                generate_layout<cart_gate_clk_lyt>(blueprints::one_to_five_path_difference_network<topology_network>(),
                                                   use(desynchronize(configuration<cart_gate_clk_lyt>()))),
                2);
        }
    }
    //    SECTION("Asynchronicity")
    //    {
    //        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
    //            blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
    //            twoddwave(crossings(border_io(async(2, configuration<cart_gate_clk_lyt>())))));
    //    }
    SECTION("Synchronization elements")
    {
        //            CHECK(generate_layout<cart_gate_clk_lyt>(blueprints::one_to_five_path_difference_network<topology_network>(),
        //                                                     twoddwave(border_io(sync_elems(configuration<cart_gate_clk_lyt>()))))
        //                      .num_se() > 0);

        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
            use(border_io(sync_elems(configuration<cart_gate_clk_lyt>()))));
    }
    SECTION("Minimize wires")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
            res(crossings(minimize_wires(configuration<cart_gate_clk_lyt>()))));
    }
    SECTION("Minimize crossings")
    {
        check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
            blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
            res(crossings(minimize_crossings(configuration<cart_gate_clk_lyt>()))));
    }
}

TEST_CASE("Exact hexagonal physical design", "[exact]")
{
    SECTION("odd row")
    {
        using hex_lyt = hex_odd_row_gate_clk_lyt;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), open(crossings(configuration<hex_lyt>())));
        }
        SECTION("Row clocking")
        {
            check_with_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                row(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), twoddwave(crossings(configuration<hex_lyt>())));
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                twoddwave(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(), twoddwave(configuration<hex_lyt>()));
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  use(straight_inverter(configuration<hex_lyt>())))));
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  open(straight_inverter(configuration<hex_lyt>())))));
        }
    }
    SECTION("even row")
    {
        using hex_lyt = hex_even_row_gate_clk_lyt;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), open(crossings(configuration<hex_lyt>())));
        }
        SECTION("Row clocking")
        {
            check_with_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(
                blueprints::unbalanced_and_inv_network<mockturtle::mig_network>(),
                row(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), twoddwave(crossings(configuration<hex_lyt>())));
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                twoddwave(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(), twoddwave(configuration<hex_lyt>()));
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  use(straight_inverter(configuration<hex_lyt>())))));
        }
    }
    SECTION("odd column")
    {
        using hex_lyt = hex_odd_col_gate_clk_lyt;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), open(crossings(configuration<hex_lyt>())));
        }
        SECTION("Columnar clocking")
        {
            check_without_gate_library(blueprints::and_or_network<mockturtle::mig_network>(),
                                       columnar(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), twoddwave(crossings(configuration<hex_lyt>())));
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                twoddwave(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(), twoddwave(configuration<hex_lyt>()));
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  use(straight_inverter(configuration<hex_lyt>())))));
        }
        SECTION("Technology constraints: ToPoliNano")
        {
            check_without_gate_library(blueprints::topolinano_network<mockturtle::mig_network>(),
                                       columnar(crossings(border_io(topolinano(configuration<hex_lyt>())))));
        }
    }
    SECTION("even column")
    {
        using hex_lyt = hex_even_col_gate_clk_lyt;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), open(crossings(configuration<hex_lyt>())));
        }
        SECTION("Columnar clocking")
        {
            check_without_gate_library(blueprints::and_or_network<mockturtle::mig_network>(),
                                       columnar(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(), twoddwave(crossings(configuration<hex_lyt>())));
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                twoddwave(crossings(border_io(configuration<hex_lyt>()))));
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_cell_clk_lyt, qca_one_library>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(), twoddwave(configuration<hex_lyt>()));
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  use(straight_inverter(configuration<hex_lyt>())))));
            CHECK(has_straight_inverters(generate_layout<hex_lyt>(blueprints::inverter_network<topology_network>(),
                                                                  open(straight_inverter(configuration<hex_lyt>())))));
        }
    }
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

TEST_CASE("Timeout", "[exact]")
{
    auto timeout_config    = use(crossings(configuration<cart_gate_clk_lyt>()));
    timeout_config.timeout = 1u;  // allow only one second to find a solution; this will fail (and is tested for)

    const auto half_adder = blueprints::half_adder_network<mockturtle::aig_network>();
    const auto layout     = exact<cart_gate_clk_lyt>(half_adder, timeout_config);

    // since a half adder cannot be synthesized in just one second, layout should not have a value
    CHECK(!layout.has_value());
}

#endif  // FICTION_Z3_SOLVER
