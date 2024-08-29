//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_STORES_HPP
#define FICTION_STORES_HPP

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>
#include <fiction/io/dot_drawers.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/write_svg_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace alice
{

/**
 * Truth tables.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/stores/tt.hpp.
 */
ALICE_ADD_STORE(fiction::truth_table_t, "truth_table", "t", "truth table", "truth tables")

ALICE_DESCRIBE_STORE(fiction::truth_table_t, tt)
{
    if (tt->num_vars() <= 6)
    {
        return fmt::format("{} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    }
    return fmt::format("{} vars, (description omitted due to truth table size)", tt->num_vars());
}

ALICE_PRINT_STORE_STATISTICS(fiction::truth_table_t, os, tt)
{
    if (tt->num_vars() <= 6)
    {
        os << fmt::format("[i] {} vars, hex: {}, bin: {}\n", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    }
    else
    {
        os << fmt::format("[i] {} vars, (description omitted due to truth table size)\n", tt->num_vars());
    }
}

ALICE_LOG_STORE_STATISTICS(fiction::truth_table_t, tt)
{
    return {{"vars", tt->num_vars()}, {"hex", kitty::to_hex(*tt)}, {"binary", kitty::to_binary(*tt)}};
}

ALICE_PRINT_STORE(fiction::truth_table_t, os, tt)
{
    os << fmt::format("[i] {} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
}

/**
 * Logic networks.
 */
ALICE_ADD_STORE(fiction::logic_network_t, "network", "n", "logic network", "logic networks")

ALICE_DESCRIBE_STORE(fiction::logic_network_t, ln)
{
    const auto describe = [](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        const mockturtle::depth_view depth_ntk{*ntk_ptr};

        return fmt::format("{} ({}) - I/O: {}/{}, gates: {}, level: {}", ntk_ptr->get_network_name(),
                           fiction::ntk_type_name<Ntk>, ntk_ptr->num_pis(), ntk_ptr->num_pos(), ntk_ptr->num_gates(),
                           depth_ntk.depth());
    };

    return std::visit(describe, ln);
}

ALICE_PRINT_STORE_STATISTICS(fiction::logic_network_t, os, ln)
{
    const auto print_statistics = [&os](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        const mockturtle::depth_view depth_ntk{*ntk_ptr};

        os << fmt::format("[i] {} ({}) - I/O: {}/{}, gates: {}, level: {}\n", ntk_ptr->get_network_name(),
                          fiction::ntk_type_name<Ntk>, ntk_ptr->num_pis(), ntk_ptr->num_pos(), ntk_ptr->num_gates(),
                          depth_ntk.depth());
    };

    std::visit(print_statistics, ln);
}

ALICE_LOG_STORE_STATISTICS(fiction::logic_network_t, ln)
{
    const auto log_statistics = [](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        const mockturtle::depth_view depth_ntk{*ntk_ptr};

        return nlohmann::json{{"name", ntk_ptr->get_network_name()}, {"type", fiction::ntk_type_name<Ntk>},
                              {"inputs", ntk_ptr->num_pis()},        {"outputs", ntk_ptr->num_pos()},
                              {"gates", ntk_ptr->num_gates()},       {"level", depth_ntk.depth()}};
    };

    return std::visit(log_statistics, ln);
}

template <>
bool can_show<fiction::logic_network_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // indexes for both logic networks and gate layouts
    cmd.add_flag("--indexes,-i", "Show node indexes")->group("logic_network (-n) / gate_layout (-g)");

    return true;
}

template <>
// NOLINTNEXTLINE
void show<fiction::logic_network_t>(std::ostream& os, const fiction::logic_network_t& element, const command& cmd)
{
    const auto show_ntk = [&os, &cmd](auto&& ntk_ptr)
    {
        try
        {
            const mockturtle::depth_view depth_ntk{*ntk_ptr};

            using Ntk = typename std::decay_t<decltype(depth_ntk)>;

            if (cmd.is_set("indexes"))
            {
                mockturtle::write_dot(depth_ntk, os, fiction::technology_dot_drawer<Ntk, true>());
            }
            else
            {
                mockturtle::write_dot(depth_ntk, os, fiction::technology_dot_drawer<Ntk, false>());
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    };

    std::visit(show_ntk, element);
}

/**
 * FCN gate-level layouts.
 */
ALICE_ADD_STORE(fiction::gate_layout_t, "gate_layout", "g", "gate layout", "gate layouts")

ALICE_PRINT_STORE(fiction::gate_layout_t, os, layout)
{
    const auto print = [&os](auto&& lyt_ptr) { fiction::print_layout(*lyt_ptr, os); };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::gate_layout_t, layout)
{
    const auto describe = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        uint32_t num_se{0};

        if constexpr (fiction::has_synchronization_elements_v<Lyt>)
        {
            num_se = lyt_ptr->num_se();
        }

        const auto cp_tp = fiction::critical_path_length_and_throughput(*lyt_ptr);

        return fmt::format(
            "{} ({}) - {} × {}, I/O: {}/{}, gates: {}, wires: {}, crossings: {}, CP: {}, TP: 1/{}, sync. elems.: {}",
            lyt_ptr->get_layout_name(), lyt_ptr->get_clocking_scheme().name, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
            lyt_ptr->num_pis(), lyt_ptr->num_pos(), lyt_ptr->num_gates(), lyt_ptr->num_wires(),
            lyt_ptr->num_crossings(), cp_tp.critical_path_length, cp_tp.throughput, num_se);
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::gate_layout_t, os, layout)
{
    const auto print_statistics = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        uint32_t num_se{0};

        if constexpr (fiction::has_synchronization_elements_v<Lyt>)
        {
            num_se = lyt_ptr->num_se();
        }

        const auto cp_tp = fiction::critical_path_length_and_throughput(*lyt_ptr);

        os << fmt::format("[i] {} ({}) - {} × {}, I/O: {}/{}, gates: {}, wires: {}, crossings: {}, CP: {}, TP: 1/{}, "
                          "sync. elems.: {}\n",
                          lyt_ptr->get_layout_name(), lyt_ptr->get_clocking_scheme().name, lyt_ptr->x() + 1,
                          lyt_ptr->y() + 1, lyt_ptr->num_pis(), lyt_ptr->num_pos(), lyt_ptr->num_gates(),
                          lyt_ptr->num_wires(), lyt_ptr->num_crossings(), cp_tp.critical_path_length, cp_tp.throughput,
                          num_se);
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::gate_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        uint32_t num_se{0};

        if constexpr (fiction::has_synchronization_elements_v<Lyt>)
        {
            num_se = lyt_ptr->num_se();
        }

        const auto cp_tp = fiction::critical_path_length_and_throughput(*lyt_ptr);

        return nlohmann::json{
            {"name", lyt_ptr->get_layout_name()},
            {"clocking scheme", lyt_ptr->get_clocking_scheme().name},
            {"inputs", lyt_ptr->num_pis()},
            {"outputs", lyt_ptr->num_pos()},
            {"gates", lyt_ptr->num_gates()},
            {"wires", lyt_ptr->num_wires()},
            {"crossings", lyt_ptr->num_crossings()},
            {"layout", {{"x-size", lyt_ptr->x() + 1}, {"y-size", lyt_ptr->y() + 1}, {"area", lyt_ptr->area()}}},
            // {"free tiles", area - (gate_tiles + wire_tiles - crossings)},  // free tiles in ground layer
            {"synchronization elements", num_se},
            {"critical path", cp_tp.critical_path_length},
            {"throughput", fmt::format("1/{}", cp_tp.throughput)}};
    };

    return std::visit(log_statistics, layout);
}

template <>
bool can_show<fiction::gate_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // already added for logic network; alice doesn't allow for both
    // cmd.add_flag("--indexes,-i", "Show node indexes")->group("gate_layout (-g)");

    cmd.add_flag("--clk_colors,-k", "Show clock colors")->group("gate_layout (-g)");

    return true;
}

template <>
// NOLINTNEXTLINE
void show<fiction::gate_layout_t>(std::ostream& os, const fiction::gate_layout_t& element, const command& cmd)
{
    const auto show_lyt = [&os, &cmd](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        // Cartesian layout
        if constexpr (fiction::is_cartesian_layout_v<Lyt>)
        {
            if (cmd.is_set("indexes"))
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, false, false>());
                }
            }
        }
        // shifted Cartesian layout
        else if constexpr (fiction::is_shifted_cartesian_layout_v<Lyt>)
        {
            if (cmd.is_set("indexes"))
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os,
                                              fiction::gate_layout_shifted_cartesian_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os,
                                              fiction::gate_layout_shifted_cartesian_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os,
                                              fiction::gate_layout_shifted_cartesian_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os,
                                              fiction::gate_layout_shifted_cartesian_drawer<Lyt, false, false>());
                }
            }
        }
        // hexagonal layout
        else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
        {
            if (cmd.is_set("indexes"))
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_hexagonal_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_hexagonal_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_hexagonal_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_hexagonal_drawer<Lyt, false, false>());
                }
            }
        }
        else
        {
            cmd.env->out() << "[e] unsupported layout topology" << std::endl;
        }
    };

    std::visit(show_lyt, element);
}

/**
 * FCN cell-level layouts.
 */
ALICE_ADD_STORE(fiction::cell_layout_t, "cell_layout", "c", "cell layout", "cell layouts")

ALICE_PRINT_STORE(fiction::cell_layout_t, os, layout)
{
    const auto print = [&os](auto&& lyt_ptr) { fiction::print_layout(*lyt_ptr, os); };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::cell_layout_t, layout)
{
    const auto describe = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        // print z dimension only if layout uses cube coordinates
        decltype(lyt_ptr->z()) z{};
        if constexpr (std::is_same_v<fiction::coordinate<Lyt>, fiction::cube::coord_t>)
        {
            z = lyt_ptr->z() + 1;
        }

        return fmt::format("{} ({}) - {} × {}{}, I/O: {}/{}, {}: {}", lyt_ptr->get_layout_name(),
                           fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                           (z ? fmt::format(" × {}", z) : ""), lyt_ptr->num_pis(), lyt_ptr->num_pos(),
                           fiction::tech_cell_name<fiction::technology<Lyt>>, lyt_ptr->num_cells());
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::cell_layout_t, os, layout)
{
    const auto print_statistics = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        // print z dimension only if layout uses cube coordinates
        decltype(lyt_ptr->z()) z{};
        if constexpr (std::is_same_v<fiction::coordinate<Lyt>, fiction::cube::coord_t>)
        {
            z = lyt_ptr->z() + 1;
        }

        os << fmt::format("[i] {} ({}) - {} × {}{}, I/O: {}/{}, {}: {}\n", lyt_ptr->get_layout_name(),
                          fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                          (z ? fmt::format(" × {}", z) : ""), lyt_ptr->num_pis(), lyt_ptr->num_pos(),
                          fiction::tech_cell_name<fiction::technology<Lyt>>, lyt_ptr->num_cells());
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::cell_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        return nlohmann::json{{"name", lyt_ptr->get_layout_name()},
                              {"technology", fiction::tech_impl_name<fiction::technology<Lyt>>},
                              {"inputs", lyt_ptr->num_pis()},
                              {"outputs", lyt_ptr->num_pos()},
                              {fiction::tech_cell_name<fiction::technology<Lyt>>, lyt_ptr->num_cells()},
                              {"layout",
                               {{"x-size", lyt_ptr->x() + 1},
                                {"y-size", lyt_ptr->y() + 1},
                                {"z-size", lyt_ptr->z() + 1},
                                {"area", lyt_ptr->area()}}}};
    };

    return std::visit(log_statistics, layout);
}

template <>
bool can_show<fiction::cell_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    cmd.add_flag("--simple,-s", "Simplified depiction abstracting from details")->group("cell_layout (-c)");

    extension = "svg";

    return true;
}

template <>
// NOLINTNEXTLINE
void show<fiction::cell_layout_t>(std::ostream& os, const fiction::cell_layout_t& element,
                                  const command& cmd)  // const & for pointer because alice says so...
{
    const auto show_lyt = [&os, &cmd](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (!fiction::has_qca_technology_v<Lyt>)
        {
            cmd.env->out() << fmt::format("[e] {} is not a QCA layout", lyt_ptr->get_layout_name()) << std::endl;
        }
        else if constexpr (!std::is_same_v<fiction::coordinate<Lyt>, fiction::offset::ucoord_t>)
        {
            cmd.env->out() << fmt::format("[e] {} is not a Cartesian layout", lyt_ptr->get_layout_name()) << std::endl;
        }
        else
        {
            try
            {
                fiction::write_qca_layout_svg(*lyt_ptr, os, {cmd.is_set("simple")});
            }
            catch (const fiction::unsupported_cell_type_exception<fiction::coordinate<Lyt>>& e)
            {
                cmd.env->out() << fmt::format("[e] unsupported cell type at cell position {}", e.where()) << std::endl;
            }
            catch (...)
            {
                cmd.env->out() << "[e] an error occurred while writing the SVG description to the output stream"
                               << std::endl;
            }
        }
    };

    std::visit(show_lyt, element);
}

}  // namespace alice

#endif  // FICTION_STORES_HPP
