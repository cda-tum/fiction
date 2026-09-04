/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief alice store definitions for truth tables, logic networks, and layouts.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/io/layout_drawers.hpp>
#include <fiction/layouts/io/print_layout.hpp>
#include <fiction/networks/io/dot_drawers.hpp>
#include <fiction/networks/name_utils.hpp>
#include <fiction/technology/fcn/area.hpp>
#include <fiction/technology/qca/io/write_qca_layout_svg.hpp>
#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/print_sidb_layout.hpp>
#include <fiction/technology/sidb/io/write_sidb_layout_svg.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/critical_path_length_and_throughput.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

namespace fiction::cli
{

/**
 * Whether a cell-layout store element is one of the SiDB types: the lattice-based `sidb::layout` or an SiDB simulation
 * result, which carries the simulated layout.
 *
 * @tparam T Element type of a `cell_layout_t` pointer.
 */
template <typename T>
inline constexpr bool is_sidb_store_v = std::is_same_v<T, sidb::layout> || std::is_same_v<T, sidb::simulation::result>;
/**
 * The cell technology of a cell-layout store element: `fiction::technology<T>` for cell-level layouts and
 * `sidb_technology` for the SiDB types.
 *
 * @tparam T Element type of a `cell_layout_t` pointer.
 */
template <typename T>
struct store_technology
{
    using type = fiction::technology<T>;
};
template <>
struct store_technology<sidb::layout>
{
    using type = sidb::sidb_technology;
};
template <>
struct store_technology<sidb::simulation::result>
{
    using type = sidb::sidb_technology;
};
/**
 * The SiDB layout behind a store element.
 *
 * @param lyt An SiDB layout.
 * @return The layout itself.
 */
[[nodiscard]] inline const sidb::layout& sidb_layout_of(const sidb::layout& lyt) noexcept
{
    return lyt;
}
/**
 * The SiDB layout behind a store element.
 *
 * @param res An SiDB simulation result.
 * @return The simulated layout.
 */
[[nodiscard]] inline const sidb::layout& sidb_layout_of(const sidb::simulation::result& res) noexcept
{
    return res.lyt;
}
/**
 * The ground state of a store element, if it has been simulated.
 *
 * @return `std::nullopt` for a plain layout.
 */
[[nodiscard]] inline std::optional<sidb::charge_distribution> ground_state_of(const sidb::layout&) noexcept
{
    return std::nullopt;
}
/**
 * The ground state of a store element, if it has been simulated.
 *
 * @param res An SiDB simulation result.
 * @return The lowest-energy charge distribution of the result, or `std::nullopt` if it has none.
 */
[[nodiscard]] inline std::optional<sidb::charge_distribution> ground_state_of(const sidb::simulation::result& res)
{
    if (const auto gs = res.groundstates(); !gs.empty())
    {
        return gs.front();
    }

    return std::nullopt;
}
/**
 * Name of a store element: the network or layout name, or the simulated layout's name for an SiDB simulation result.
 *
 * @tparam T Element type.
 * @param element The element.
 * @return Its name.
 */
template <typename T>
[[nodiscard]] std::string name_of(const T& element)
{
    if constexpr (std::is_same_v<T, sidb::simulation::result>)
    {
        return element.lyt.get_layout_name();
    }
    else
    {
        return networks::get_name(element);
    }
}

/**
 * One-line description of an SiDB store element: its extent in columns and single-SiDB rows, I/O SiDBs, SiDB count,
 * defect count, and, for a simulation result, the number of stable states and the ground state energy.
 *
 * @tparam T `sidb::layout` or `sidb::simulation::result`.
 * @param element The element.
 * @return The description.
 */
template <typename T>
[[nodiscard]] std::string describe_sidb(const T& element)
{
    const auto& lyt     = sidb_layout_of(element);
    const auto [nw, se] = lyt.bounding_box();
    const auto  lattice = lyt.get_lattice().name;
    std::string simulation{};

    if constexpr (std::is_same_v<T, sidb::simulation::result>)
    {
        simulation = fmt::format(", stable states: {}", element.charge_distributions.size());

        if (const auto gs = ground_state_of(element); gs.has_value())
        {
            simulation += fmt::format(", ground state energy: {} eV", gs->energy());
        }
    }

    return fmt::format("{} (SiDB on {}) - {} × {}, I/O: {}/{}, SiDBs: {}, defects: {}{}", lyt.get_layout_name(),
                       lattice, se.x - nw.x + 1, sidb::row_of(se) - sidb::row_of(nw) + 1, lyt.num_pis(), lyt.num_pos(),
                       lyt.num_cells(), lyt.num_defects(), simulation);
}
/**
 * JSON statistics of an SiDB store element, mirroring `describe_sidb`.
 *
 * @tparam T `sidb::layout` or `sidb::simulation::result`.
 * @param element The element.
 * @return The statistics.
 */
template <typename T>
[[nodiscard]] nlohmann::json log_sidb(const T& element)
{
    const auto& lyt     = sidb_layout_of(element);
    const auto [nw, se] = lyt.bounding_box();

    nlohmann::json j{
        {"name", lyt.get_layout_name()},
        {"technology", tech_impl_name<sidb::sidb_technology>},
        {"lattice", lyt.get_lattice().name},
        {"inputs", lyt.num_pis()},
        {"outputs", lyt.num_pos()},
        {tech_cell_name<sidb::sidb_technology>, lyt.num_cells()},
        {"defects", lyt.num_defects()},
        {"layout",
         {{"x-size", se.x - nw.x + 1}, {"y-size", sidb::row_of(se) - sidb::row_of(nw) + 1}, {"area", fcn::area(lyt)}}}};

    if constexpr (std::is_same_v<T, sidb::simulation::result>)
    {
        j["stable states"] = element.charge_distributions.size();

        if (const auto gs = ground_state_of(element); gs.has_value())
        {
            j["ground state energy (eV)"] = gs->energy();
        }
    }

    return j;
}

}  // namespace fiction::cli

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
inline bool can_show<fiction::logic_network_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // indexes for both logic networks and gate layouts
    cmd.add_flag("--indexes,-i", "Show node indexes")->group("logic_network (-n) / gate_layout (-g)");

    return true;
}

template <>
// NOLINTNEXTLINE
inline void show<fiction::logic_network_t>(std::ostream& os, const fiction::logic_network_t& element,
                                           const command& cmd)
{
    const auto show_ntk = [&os, &cmd](auto&& ntk_ptr)
    {
        try
        {
            const mockturtle::depth_view depth_ntk{*ntk_ptr};

            using Ntk = typename std::decay_t<decltype(depth_ntk)>;

            if (cmd.is_set("indexes"))
            {
                mockturtle::write_dot(depth_ntk, os, fiction::networks::io::technology_dot_drawer<Ntk, true>());
            }
            else
            {
                mockturtle::write_dot(depth_ntk, os, fiction::networks::io::technology_dot_drawer<Ntk, false>());
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << fmt::format("[e] {}\n", e.what());
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
    const auto print = [&os](auto&& lyt_ptr) { fiction::layouts::io::print_layout(*lyt_ptr, os); };

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

        const auto cp_tp = fiction::verification::critical_path_length_and_throughput(*lyt_ptr);

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

        const auto cp_tp = fiction::verification::critical_path_length_and_throughput(*lyt_ptr);

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

        const auto cp_tp = fiction::verification::critical_path_length_and_throughput(*lyt_ptr);

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
inline bool can_show<fiction::gate_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // already added for logic network; alice doesn't allow for both
    // cmd.add_flag("--indexes,-i", "Show node indexes")->group("gate_layout (-g)");

    cmd.add_flag("--clk_colors,-k", "Show clock colors")->group("gate_layout (-g)");

    return true;
}

template <>
// NOLINTNEXTLINE
inline void show<fiction::gate_layout_t>(std::ostream& os, const fiction::gate_layout_t& element, const command& cmd)
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
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt, false, false>());
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
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_shifted_cartesian_drawer<Lyt, false, false>());
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
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, true, true>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, false, true>());
                }
            }
            else
            {
                if (cmd.is_set("clk_colors"))
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, true, false>());
                }
                else
                {
                    fiction::layouts::io::write_dot_layout(
                        *lyt_ptr, os, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt, false, false>());
                }
            }
        }
        else
        {
            cmd.env->out() << "[e] unsupported layout topology\n";
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
    const auto print = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            if (const auto gs = fiction::cli::ground_state_of(*lyt_ptr); gs.has_value())
            {
                fiction::sidb::io::print_sidb_layout(os, fiction::cli::sidb_layout_of(*lyt_ptr), *gs);
            }
            else
            {
                fiction::sidb::io::print_sidb_layout(os, fiction::cli::sidb_layout_of(*lyt_ptr));
            }
        }
        else
        {
            fiction::layouts::io::print_layout(*lyt_ptr, os);
        }
    };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::cell_layout_t, layout)
{
    const auto describe = [](auto&& lyt_ptr) -> std::string
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            return fiction::cli::describe_sidb(*lyt_ptr);
        }
        else
        {
            // print z dimension only if layout uses cube coordinates
            decltype(lyt_ptr->z()) z{};
            if constexpr (std::is_same_v<fiction::coordinate<Lyt>, fiction::layouts::coords::cube>)
            {
                z = lyt_ptr->z() + 1;
            }

            return fmt::format("{} ({}) - {} × {}{}, I/O: {}/{}, {}: {}", lyt_ptr->get_layout_name(),
                               fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                               (z ? fmt::format(" × {}", z) : ""), lyt_ptr->num_pis(), lyt_ptr->num_pos(),
                               fiction::tech_cell_name<fiction::technology<Lyt>>, lyt_ptr->num_cells());
        }
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::cell_layout_t, os, layout)
{
    const auto print_statistics = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            os << fmt::format("[i] {}\n", fiction::cli::describe_sidb(*lyt_ptr));
        }
        else
        {
            // print z dimension only if layout uses cube coordinates
            decltype(lyt_ptr->z()) z{};
            if constexpr (std::is_same_v<fiction::coordinate<Lyt>, fiction::layouts::coords::cube>)
            {
                z = lyt_ptr->z() + 1;
            }

            os << fmt::format("[i] {} ({}) - {} × {}{}, I/O: {}/{}, {}: {}\n", lyt_ptr->get_layout_name(),
                              fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                              (z ? fmt::format(" × {}", z) : ""), lyt_ptr->num_pis(), lyt_ptr->num_pos(),
                              fiction::tech_cell_name<fiction::technology<Lyt>>, lyt_ptr->num_cells());
        }
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::cell_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt_ptr) -> nlohmann::json
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            return fiction::cli::log_sidb(*lyt_ptr);
        }
        else
        {
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
        }
    };

    return std::visit(log_statistics, layout);
}

template <>
inline bool can_show<fiction::cell_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    cmd.add_flag("--simple,-s", "Simplified depiction abstracting from details")->group("cell_layout (-c)");

    extension = "svg";

    return true;
}

template <>
// NOLINTNEXTLINE
inline void show<fiction::cell_layout_t>(std::ostream& os, const fiction::cell_layout_t& element,
                                         const command& cmd)  // const & for pointer because alice says so...
{
    const auto show_lyt = [&os, &cmd](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            if (const auto gs = fiction::cli::ground_state_of(*lyt_ptr); gs.has_value())
            {
                fiction::sidb::io::write_sidb_layout_svg(fiction::cli::sidb_layout_of(*lyt_ptr), *gs, os, {});
            }
            else
            {
                fiction::sidb::io::write_sidb_layout_svg(fiction::cli::sidb_layout_of(*lyt_ptr), os, {});
            }
        }
        else if constexpr (!fiction::has_qca_technology_v<Lyt> && !fiction::has_mol_qca_technology_v<Lyt>)
        {
            cmd.env->out() << fmt::format("[e] {} has an unsupported cell technology\n", lyt_ptr->get_layout_name());
        }
        else if constexpr ((fiction::has_qca_technology_v<Lyt> || fiction::has_mol_qca_technology_v<Lyt>) &&
                           !fiction::has_offset_coord_v<Lyt>)
        {
            cmd.env->out() << fmt::format("[e] {} is not a Cartesian layout\n", lyt_ptr->get_layout_name());
        }
        else
        {
            try
            {
                if constexpr (fiction::has_qca_technology_v<Lyt>)
                {
                    fiction::qca::io::write_qca_layout_svg(*lyt_ptr, os, {cmd.is_set("simple")});
                }
                else if constexpr (fiction::has_mol_qca_technology_v<Lyt>)
                {
                    fiction::qca::io::write_mol_qca_layout_svg(*lyt_ptr, os, {cmd.is_set("simple")});
                }
            }
            catch (const fiction::qca::io::unsupported_cell_type_exception<fiction::coordinate<Lyt>>& e)
            {
                cmd.env->out() << fmt::format("[e] unsupported cell type at cell position {}\n", e.where());
            }
            catch (...)
            {
                cmd.env->out() << "[e] an error occurred while writing the SVG description to the output stream\n";
            }
        }
    };

    std::visit(show_lyt, element);
}

}  // namespace alice
