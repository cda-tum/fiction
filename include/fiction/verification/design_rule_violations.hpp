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
 * @brief Design rule violation checking for gate-level layouts.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/traits.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <nlohmann/json.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

namespace fiction::verification
{

/**
 * Parameters for design rule violation checking that specify the checks that are to be executed.
 */
struct gate_level_drv_params
{
    // Topology

    /** @brief Check that every live object lies within the zero-origin extent. */
    bool outside_extent = true;

    /**
     * Check for nodes that are connected to non-adjacent ones.
     */
    bool non_adjacent_connections = true;
    /**
     * Check for nodes without connections.
     */
    bool missing_connections = true;
    /**
     * Check for wires that are crossing gates.
     */
    bool crossing_gates = true;

    // Clocking

    /**
     * Check if all node connections obey the clocking scheme data flow.
     */
    bool clocked_data_flow = true;

    // I/O

    /**
     * Check if the layout has I/Os.
     */
    bool has_io = true;
    /**
     * Check if the I/Os are located at the layout's border.
     */
    bool border_io = true;

    /**
     * Stream to write the report into.
     */
    std::ostream* out = &std::cout;
    /** @brief Reports completed work in each bounded phase. */
    utils::progress_callback on_progress{};
};

/** @brief Design rule report and issue counts. */
struct gate_level_drv_stats
{
    /**
     * Report.
     */
    nlohmann::json report{};
    /**
     * Number of design rule violations.
     */
    std::size_t drvs = 0;
    /**
     * Number of warnings.
     */
    std::size_t warnings = 0;
};

namespace detail
{

/** @brief Checks live object placement, connections, clocking, and interfaces. @tparam Lyt Gate layout type. */
template <typename Lyt>
class gate_level_drvs_impl
{
  public:
    /**
     * @brief Stores the layout, parameters, and statistics.
     *
     * @param src Gate layout to check for design rule flaws.
     * @param p Parameters.
     * @param st Statistics.
     */
    explicit gate_level_drvs_impl(const Lyt& src, gate_level_drv_params p, gate_level_drv_stats& st) :
            lyt{src},
            ps{std::move(p)},
            pst{st}
    {}
    /**
     * Performs design rule checks on the stored gate layout. The following properties are checked.
     *
     *  Design breaking:
     *   - Non-adjacent connections
     *   - Missing connections
     *   - Wires crossing operations
     *   - Non-consecutive clocking of connected tiles
     *   - Objects outside the extent
     *
     *  Warning:
     *   - Non-border I/O
     */
    void run()
    {
        utils::progress_reporter progress{
            ps.on_progress, "design rule checks",
            static_cast<std::size_t>(ps.outside_extent) + static_cast<std::size_t>(ps.non_adjacent_connections) +
                static_cast<std::size_t>(ps.missing_connections) + static_cast<std::size_t>(ps.crossing_gates) +
                static_cast<std::size_t>(ps.clocked_data_flow) + static_cast<std::size_t>(ps.has_io) +
                static_cast<std::size_t>(ps.border_io)};
        *ps.out << "[i] Topology:\n";
        if (ps.outside_extent)
        {
            *ps.out << "[i]" << outside_extent_check() << '\n';
            progress.advance();
        }
        if (ps.non_adjacent_connections)
        {
            *ps.out << "[i]" << non_adjacent_connections_check() << '\n';
            progress.advance();
        }
        if (ps.missing_connections)
        {
            *ps.out << "[i]" << missing_connections_check() << '\n';
            progress.advance();
        }
        if (ps.crossing_gates)
        {
            *ps.out << "[i]" << crossing_gates_check() << '\n';
            progress.advance();
        }
        *ps.out << '\n';

        *ps.out << "[i] Clocking:\n";
        if (ps.clocked_data_flow)
        {
            *ps.out << "[i]" << clocked_data_flow_check() << '\n';
            progress.advance();
        }
        *ps.out << '\n';

        *ps.out << "[i] I/O ports:\n";
        if (ps.has_io)
        {
            *ps.out << "[i]" << has_io_check() << '\n';
            progress.advance();
        }
        if (ps.border_io)
        {
            *ps.out << "[i]" << border_io_check() << '\n';
            progress.advance();
        }

        *ps.out << fmt::format(
                       "\n[i] DRVs: {}, Warnings: {}",
                       (pst.drvs != 0u ? fmt::format(fmt::fg(fmt::color::red), fmt::runtime(std::to_string(pst.drvs))) :
                                         ZERO_ISSUES),
                       (pst.warnings != 0u ?
                            fmt::format(fmt::fg(fmt::color::yellow), fmt::runtime(std::to_string(pst.warnings))) :
                            ZERO_ISSUES))
                << "\n";

        pst.report["DRVs"]     = pst.drvs;
        pst.report["Warnings"] = pst.warnings;
    }

  private:
    /**
     * Layout to perform design rule checks on.
     */
    const Lyt& lyt;
    /**
     * Parameters.
     */
    gate_level_drv_params ps;
    /**
     * Statistics.
     */
    gate_level_drv_stats& pst;

    /**
     * Escape color sequence for passed checks followed by a check mark.
     */
    inline static const auto CHECK_PASSED = fmt::format(fmt::fg(fmt::color::green), "✓");
    /**
     * Escape color sequence for failed checks followed by an x mark.
     */
    inline static const auto CHECK_FAILED = fmt::format(fmt::fg(fmt::color::red), "✗");
    /**
     * Escape color sequence for warnings followed by an exclamation point.
     */
    inline static const auto WARNING = fmt::format(fmt::fg(fmt::color::yellow), "!");
    /**
     * Escape color sequence for no issues followed by the number 0.
     */
    inline static const auto ZERO_ISSUES = fmt::format(fmt::fg(fmt::color::green), "0");

    /**
     * Logs information about the given tile in the given report. Nodes are logged in this process under the tile
     * position.
     *
     * @param t Tile whose attributes are to be logged.
     * @param report Report to log into.
     */
    void log_tile(const tile<Lyt> t, nlohmann::json& report) const
    {
        std::stringstream s{};

        if (lyt.is_empty_tile(t))
        {
            s << "empty";
        }
        else
        {
            s << "node: " << lyt.find_object(t)->index;  // log node
        }

        auto clk = lyt.get_clock_number(t);

        const auto se = lyt.get_synchronization_element(t);

        const std::array<const char*, 4> inp{
            {lyt.has_northern_incoming_signal(t) ? "N" : "", lyt.has_eastern_incoming_signal(t) ? "E" : "",
             lyt.has_southern_incoming_signal(t) ? "S" : "", lyt.has_western_incoming_signal(t) ? "W" : ""}};
        const std::array<const char*, 4> out{
            {lyt.has_northern_outgoing_signal(t) ? "N" : "", lyt.has_eastern_outgoing_signal(t) ? "E" : "",
             lyt.has_southern_outgoing_signal(t) ? "S" : "", lyt.has_western_outgoing_signal(t) ? "W" : ""}};

        s << fmt::format(", clk: {}, se: {}, inp: {}, out: {}{}{}", clk, se, fmt::join(inp, ""), fmt::join(out, ""),
                         (lyt.is_pi_tile(t) ? ", PI" : ""), (lyt.is_po_tile(t) ? ", PO" : ""));

        report[t.str()] = s.str();
    }

    /**
     * Returns the check icon corresponding to a check's outcome.
     *
     * @param chk Result of the check.
     * @param brk Flag to indicate that a failure is design breaking. If it's not, a warning icon is returned.
     * @return Escape color sequence for the given outcome.
     */
    static const std::string& check_icon(const bool chk, const bool brk) noexcept
    {
        if (chk)
        {
            return CHECK_PASSED;
        }
        if (brk)
        {
            return CHECK_FAILED;
        }

        return WARNING;
    }
    /**
     * Generates a summarizing one liner for a design rule check.
     *
     * @param msg Message to output in success case. For failure, a "not" will be added as a prefix.
     * @param chk Result of the check.
     * @param brk Flag to indicate that a failure is design breaking. If it's not, msg is printed as a warning.
     * @return Formatted summary message.
     */
    std::string summary(std::string&& msg, const bool chk, const bool brk) const
    {
        return fmt::format(" [{}] {}{}", check_icon(chk, brk), chk ? "" : "not ", std::move(msg));
    }
    /**
     * @brief Checks containment of all live object placements.
     * @return Check summary.
     */
    std::string outside_extent_check()
    {
        nlohmann::json           report{};
        bool                     contained = true;
        utils::progress_reporter traversal{ps.on_progress, "outside extent", lyt.size()};
        lyt.foreach_object(
            [&](const auto id)
            {
                const auto t = lyt.get_tile(id);
                if (!lyt.contains_coordinate(t))
                {
                    contained = false;
                    log_tile(t, report);
                    ++pst.drvs;
                }
                traversal.advance();
            });
        pst.report["Objects outside extent"] = report;
        return summary("all objects lie within the layout extent", contained, true);
    }
    /**
     * Checks for proper clocking of connected tiles based on their assigned nodes.
     *
     * @return Check summary as a one liner.
     */
    std::string non_adjacent_connections_check()
    {
        nlohmann::json non_adjacency_report{};

        auto adjacencies_respected = true;

        if (!lyt.is_empty())
        {
            utils::progress_reporter traversal{ps.on_progress, "non adjacent connections", lyt.size()};
            lyt.foreach_object(
                [this, &non_adjacency_report, &adjacencies_respected, &traversal](const auto id)
                {
                    const auto t = lyt.get_tile(id);

                    lyt.foreach_fanin(id,
                                      [&](const auto child)
                                      {
                                          const auto ct = lyt.get_tile(child);
                                          if (!lyt.is_adjacent_elevation_of(t, ct))
                                          {
                                              adjacencies_respected = false;
                                              log_tile(ct, non_adjacency_report);
                                              log_tile(t, non_adjacency_report);
                                              ++pst.drvs;
                                          }
                                      });
                    traversal.advance();
                });
        }

        pst.report["Non adjacent connections"] = non_adjacency_report;

        return summary("all tiles are adjacently connected", adjacencies_respected, true);
    }
    /**
     * Checks for non-PO tiles with successors and non-PI tiles without predecessors.
     *
     * @return Check summary as a one liner.
     */
    std::string missing_connections_check()
    {
        nlohmann::json connections_report{};

        auto all_connected = true;

        if (!lyt.is_empty())
        {
            utils::progress_reporter traversal{ps.on_progress, "missing connections", lyt.size()};
            lyt.foreach_object(
                [this, &connections_report, &all_connected, &traversal](const auto id)
                {
                    const auto t = lyt.get_tile(id);

                    bool dangling_inp_connection = false;
                    for (uint32_t input{}; input < lyt.input_count(id); ++input)
                    {
                        dangling_inp_connection |= !lyt.source({id, input}).has_value();
                    }
                    const bool dangling_out_connection = lyt.fanout_size(id) == 0 && !lyt.is_po_tile(t);

                    if (dangling_out_connection || dangling_inp_connection)
                    {
                        all_connected = false;
                        log_tile(t, connections_report);
                        ++pst.drvs;
                    }
                    traversal.advance();
                });
        }

        pst.report["Missing connections"] = connections_report;

        return summary("all occupied tiles are properly connected", all_connected, true);
    }
    /**
     * Check for wires crossing gates.
     *
     * @return Check summary as a one liner.
     */
    std::string crossing_gates_check()
    {
        nlohmann::json crossing_report{};

        auto all_wire_crossings = true;

        if (!lyt.is_empty())
        {
            utils::progress_reporter traversal{ps.on_progress, "crossing gates", lyt.num_wires()};
            lyt.foreach_wire(
                [this, &crossing_report, &all_wire_crossings, &traversal](const auto& w)
                {
                    if (const auto t = lyt.get_tile(w); lyt.is_crossing_layer(t))
                    {
                        if (const auto lower = lyt.below(t); !lower || !lyt.is_wire_tile(*lower))
                        {
                            all_wire_crossings = false;
                            log_tile(t, crossing_report);
                            ++pst.drvs;
                        }
                    }
                    traversal.advance();
                });
        }

        pst.report["Wires crossing gates"] = crossing_report;

        return summary("all wire crossings cross over other wires only", all_wire_crossings, true);
    }
    /**
     * Checks for proper clocking of connected tiles based on their assigned nodes.
     *
     * @return Check summary as a one liner.
     */
    std::string clocked_data_flow_check()
    {
        nlohmann::json data_flow_report{};

        auto data_flow_respected = true;

        if (!lyt.is_empty())
        {
            utils::progress_reporter traversal{ps.on_progress, "clocked data flow", lyt.size()};
            lyt.foreach_object(
                [this, &data_flow_report, &data_flow_respected, &traversal](const auto id)
                {
                    const auto t = lyt.get_tile(id);

                    lyt.foreach_fanin(id,
                                      [&](const auto child)
                                      {
                                          const auto ct = lyt.get_tile(child);
                                          if (!lyt.is_incoming_clocked(t, ct))
                                          {
                                              data_flow_respected = false;
                                              log_tile(ct, data_flow_report);
                                              log_tile(t, data_flow_report);
                                              ++pst.drvs;
                                          }
                                      });
                    traversal.advance();
                });
        }

        pst.report["Improperly clocked tiles"] = data_flow_report;

        return summary("all connected tiles are properly clocked", data_flow_respected, true);
    }
    /**
     * Checks if PI/PO assignments are present.
     *
     * @return Check summary as a one liner.
     */
    std::string has_io_check()
    {
        nlohmann::json has_io_report{};

        auto ios_present = true;

        if (!lyt.is_empty())
        {
            has_io_report["Specified PIs"] = lyt.num_pis();
            has_io_report["Counted PIs"]   = lyt.num_pis();
            has_io_report["Specified POs"] = lyt.num_pos();
            has_io_report["Counted POs"]   = lyt.num_pos();
            if (lyt.num_pis() == 0)
            {
                ios_present = false;
                ++pst.drvs;
            }
            if (lyt.num_pos() == 0)
            {
                ios_present = false;
                ++pst.drvs;
            }
        }

        pst.report["I/O counts"] = has_io_report;

        return summary("all I/O are properly specified", ios_present, true);
    }
    /**
     * Checks if all PI/POs are located at the layout's borders.
     *
     * @return Check summary as a one liner.
     */
    std::string border_io_check()
    {
        nlohmann::json border_report{};

        auto all_border = true;

        if (!lyt.is_empty())
        {
            const auto check_io = [this, &border_report, &all_border](const auto io)
            {
                if (const auto iot = lyt.get_tile(io); !lyt.is_at_any_border(iot))
                {
                    all_border = false;
                    log_tile(iot, border_report);
                    ++pst.warnings;
                }
            };

            lyt.foreach_pi(check_io);
            lyt.foreach_po(check_io);
        }

        pst.report["Border I/O ports"] = border_report;

        return summary("all I/O ports are located at the layout's borders", all_border, false);
    }
};

}  // namespace detail

/**
 * Performs design rule violation (DRV) checking on the given gate-level layout. The implementation of gate_level_layout
 * allows for layouts with structural defects like the connection of non-adjacent tiles or connections that defy the
 * clocking scheme. This function checks for such violations and documents them in the statistics. A brief report can be
 * printed and more in-depth information including with error sites can be obtained from a generated json object.
 *
 * Furthermore, this function does not only find and log DRVs but can also warn for instances that are not per se errors
 * but defy best practices of layout generation, e.g., I/Os not being placed at the layout borders.
 *
 * The checker inspects every live object through public ordered ports, including placements outside the extent.
 * Unplaced objects, placed dead objects, empty terminals, and gate terminals cannot occur in the placed-object API
 * and have no corresponding checks.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout that is to be examined for DRVs and warnings.
 * @param ps Parameters.
 * @param pst Statistics.
 */
template <typename Lyt>
void gate_level_drvs(const Lyt& lyt, const gate_level_drv_params& ps = {}, gate_level_drv_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    gate_level_drv_stats              st{};
    detail::gate_level_drvs_impl<Lyt> p{lyt, ps, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction::verification
