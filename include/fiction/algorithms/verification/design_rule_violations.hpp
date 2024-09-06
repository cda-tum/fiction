//
// Created by marcel on 26.04.19.
//

#ifndef FICTION_DESIGN_RULE_VIOLATIONS_HPP
#define FICTION_DESIGN_RULE_VIOLATIONS_HPP

#include "fiction/traits.hpp"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <mockturtle/traits.hpp>
#include <nlohmann/json.hpp>

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace fiction
{

/**
 * Parameters for design rule violation checking that specify the checks that are to be executed.
 */
struct gate_level_drv_params
{
    // Topology

    /**
     * Check for nodes without locations.
     */
    bool unplaced_nodes = true;
    /**
     * Check for placed but dead nodes.
     */
    bool placed_dead_nodes = true;
    /*
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
     * Check if the I/Os are assigned to empty tiles.
     */
    bool empty_io = true;
    /**
     * Check if the I/Os are assigned to wire segments.
     */
    bool io_pins = true;
    /**
     * Check if the I/Os are located at the layout's border.
     */
    bool border_io = true;

    /**
     * Stream to write the report into.
     */
    std::ostream* out = &std::cout;
};

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

template <typename Lyt>
class gate_level_drvs_impl
{
  public:
    /**
     * Standard constructor.
     *
     * @param lyt Gate layout to check for design rule flaws.
     * @param p Parameters.
     * @param st Statistics.
     */
    explicit gate_level_drvs_impl(const Lyt& src, const gate_level_drv_params& p, gate_level_drv_stats& st) :
            lyt{src},
            ps{p},
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
     *   - I/O assigned to empty tiles
     *
     *  Warning:
     *   - Unplaced (alive) nodes
     *   - Non-wire I/O
     *   - Non-border I/O
     */
    void run()
    {
        *ps.out << "[i] Topology:\n";
        if (ps.unplaced_nodes)
        {
            *ps.out << "[i]" << unplaced_nodes_check() << '\n';
        }
        if (ps.placed_dead_nodes)
        {
            *ps.out << "[i]" << placed_dead_nodes_check() << '\n';
        }
        if (ps.non_adjacent_connections)
        {
            *ps.out << "[i]" << non_adjacent_connections_check() << '\n';
        }
        if (ps.missing_connections)
        {
            *ps.out << "[i]" << missing_connections_check() << '\n';
        }
        if (ps.crossing_gates)
        {
            *ps.out << "[i]" << crossing_gates_check() << '\n';
        }
        *ps.out << '\n';

        *ps.out << "[i] Clocking:\n";
        if (ps.clocked_data_flow)
        {
            *ps.out << "[i]" << clocked_data_flow_check() << '\n';
        }
        *ps.out << '\n';

        *ps.out << "[i] I/O ports:\n";
        if (ps.has_io)
        {
            *ps.out << "[i]" << has_io_check() << '\n';
        }
        if (ps.empty_io)
        {
            *ps.out << "[i]" << empty_io_check() << '\n';
        }
        if (ps.io_pins)
        {
            *ps.out << "[i]" << io_pin_check() << '\n';
        }
        if (ps.border_io)
        {
            *ps.out << "[i]" << border_io_check() << '\n';
        }

        *ps.out << fmt::format(
                       "\n[i] DRVs: {}, Warnings: {}",
                       (pst.drvs != 0u ? fmt::format(fmt::fg(fmt::color::red), std::to_string(pst.drvs)) : ZERO_ISSUES),
                       (pst.warnings != 0u ? fmt::format(fmt::fg(fmt::color::yellow), std::to_string(pst.warnings)) :
                                             ZERO_ISSUES))
                << std::endl;

        pst.report["DRVs"]     = pst.drvs;
        pst.report["Warnings"] = pst.warnings;
    }

  private:
    /**
     * Layout to perform design rule checks on.
     */
    Lyt lyt;
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
    void log_tile(const tile<Lyt> t, nlohmann::json& report) const noexcept
    {
        std::stringstream s{};

        if (lyt.is_empty_tile(t))
        {
            s << "empty";
        }
        else
        {
            s << "node: " << lyt.node_to_index(lyt.get_node(t));  // log node
        }

        auto clk = lyt.get_clock_number(t);

        uint32_t se{0};

        if constexpr (has_synchronization_elements_v<Lyt>)
        {
            se = lyt.get_synchronization_element(t);
        }

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

    void log_node(const mockturtle::node<Lyt>& n, nlohmann::json& report) const noexcept
    {
        report[n] = lyt.node_to_index(n);
    }
    /**
     * Generates a summarizing one liner for a design rule check.
     *
     * @param msg Message to output in success case. For failure, a "not" will be added as a prefix.
     * @param chk Result of the check.
     * @param brk Flag to indicate that a failure is design breaking. If it's not, msg is printed as a warning.
     * @return Formatted summary message.
     */
    std::string summary(std::string&& msg, const bool chk, const bool brk) const noexcept
    {
        return fmt::format(" [{}] {}{}", chk ? CHECK_PASSED : (brk ? CHECK_FAILED : WARNING), chk ? "" : "not ",
                           std::move(msg));
    }
    /**
     * Checks for nodes that are not placed but still alive.
     *
     * @return Check summary as a one liner.
     */
    std::string unplaced_nodes_check()
    {
        nlohmann::json unplaced_report{};

        auto all_placed = true;

        if (!lyt.is_empty())
        {
            lyt.foreach_node(
                [&unplaced_report, &all_placed, this](const auto& n)
                {
                    // skip constants
                    if (!lyt.is_constant(n))
                    {
                        // if a node is alive but placed on a dead tile (e.g. not placed at all)
                        if (lyt.get_tile(n).is_dead())
                        {
                            all_placed = false;
                            log_node(n, unplaced_report);
                            ++pst.warnings;
                        }
                    }
                });
        }

        pst.report["Unplaced nodes"] = unplaced_report;

        return summary("all nodes are properly placed", all_placed, false);
    }
    /**
     * Checks for nodes that are placed but dead.
     *
     * @return Check summary as a one liner.
     */
    std::string placed_dead_nodes_check()
    {
        nlohmann::json placed_dead_report{};

        auto all_alive = true;

        if (!lyt.is_empty())
        {
            lyt.foreach_tile(
                [&placed_dead_report, &all_alive, this](const auto& t)
                {
                    // skip empty tiles
                    if (!lyt.is_empty_tile(t))
                    {
                        const auto n = lyt.get_node(t);

                        // if the node is dead but placed
                        if (lyt.is_dead(n))
                        {
                            all_alive = false;
                            log_tile(t, placed_dead_report);
                            ++pst.warnings;
                        }
                    }
                });
        }

        pst.report["Dead placed nodes"] = placed_dead_report;

        return summary("all placed nodes are alive", all_alive, false);
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
            lyt.foreach_tile(
                [this, &non_adjacency_report, &adjacencies_respected](const auto& t)
                {
                    // skip empty tiles
                    if (lyt.is_empty_tile(t))
                    {
                        return;
                    }

                    const auto n = lyt.get_node(t);

                    for (const auto& child : lyt.strg->nodes[n].children)
                    {
                        const auto ct = lyt.get_tile(lyt.get_node(child.index));
                        if (!lyt.is_adjacent_elevation_of(t, ct))
                        {
                            adjacencies_respected = false;
                            log_tile(ct, non_adjacency_report);
                            log_tile(t, non_adjacency_report);
                            ++pst.drvs;
                        }
                    }
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
            lyt.foreach_tile(
                [this, &connections_report, &all_connected](const auto& t)
                {
                    if (lyt.is_empty_tile(t))
                    {
                        return;
                    }

                    const auto n = lyt.get_node(t);

                    const bool dangling_inp_connection = lyt.fanin_size(n) == 0 && !lyt.is_pi_tile(t);
                    const bool dangling_out_connection = lyt.fanout_size(n) == 0 && !lyt.is_po_tile(t);

                    if (dangling_out_connection || dangling_inp_connection)
                    {
                        all_connected = false;
                        log_tile(t, connections_report);
                        ++pst.drvs;
                    }
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
            lyt.foreach_wire(
                [this, &crossing_report, &all_wire_crossings](const auto& w)
                {
                    if (const auto t = lyt.get_tile(w); lyt.is_crossing_layer(t))
                    {
                        if (!lyt.is_wire_tile(lyt.below(t)))
                        {
                            all_wire_crossings = false;
                            log_tile(t, crossing_report);
                            ++pst.drvs;
                        }
                    }
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
            lyt.foreach_tile(
                [this, &data_flow_report, &data_flow_respected](const auto& t)
                {
                    if (lyt.is_empty_tile(t))
                    {
                        return;
                    }

                    const auto n = lyt.get_node(t);

                    for (const auto& child : lyt.strg->nodes[n].children)
                    {
                        const auto ct = lyt.get_tile(lyt.get_node(child.index));
                        if (!lyt.is_incoming_clocked(t, ct))
                        {
                            data_flow_respected = false;
                            log_tile(ct, data_flow_report);
                            log_tile(t, data_flow_report);
                            ++pst.drvs;
                        }
                    }
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
            uint32_t num_io{0ul};

            const auto count_io = [&num_io]([[maybe_unused]] const mockturtle::node<Lyt>& io) { ++num_io; };

            has_io_report["Specified PIs"] = lyt.num_pis();
            lyt.foreach_pi(count_io);
            has_io_report["Counted PIs"] = num_io;

            if (lyt.num_pis() != num_io || lyt.num_pis() == 0 || num_io == 0)
            {
                ios_present = false;
                ++pst.drvs;
            }

            num_io = 0ul;

            has_io_report["Specified POs"] = lyt.num_pos();
            lyt.foreach_po([this, &count_io](const auto& o) { count_io(lyt.get_node(o)); });
            has_io_report["Counted POs"] = num_io;

            if (lyt.num_pos() != num_io || lyt.num_pos() == 0 || num_io == 0)
            {
                ios_present = false;
                ++pst.drvs;
            }
        }

        pst.report["I/O counts"] = has_io_report;

        return summary("all I/O are properly specified", ios_present, true);
    }
    /**
     * Checks if no PI/PO is assigned to an empty tile.
     *
     * @return Check summary as a one liner.
     */
    std::string empty_io_check()
    {
        nlohmann::json empty_io_report{};

        auto all_non_empty = true;

        if (!lyt.is_empty())
        {
            const auto check_io = [this, &empty_io_report, &all_non_empty](const mockturtle::node<Lyt>& io)
            {
                if (const auto iot = lyt.get_tile(io); lyt.is_empty_tile(iot))
                {
                    all_non_empty = false;
                    log_tile(iot, empty_io_report);
                    ++pst.drvs;
                }
            };

            lyt.foreach_pi(check_io);
            lyt.foreach_po([this, &check_io](const auto& o) { check_io(lyt.get_node(o)); });
        }

        pst.report["Empty I/O ports"] = empty_io_report;

        return summary("all I/O ports are assigned to a non-empty tile", all_non_empty, true);
    }
    /**
     * Checks if all PI/POs are designated pins.
     *
     * @return Check summary as a one liner.
     */
    std::string io_pin_check()
    {
        nlohmann::json io_pin_report{};

        auto all_pin = true;

        if (!lyt.is_empty())
        {
            const auto check_io = [this, &io_pin_report, &all_pin](const mockturtle::node<Lyt>& io)
            {
                if (const auto iot = lyt.get_tile(io); !lyt.is_buf(io))
                {
                    all_pin = false;
                    log_tile(iot, io_pin_report);
                    ++pst.warnings;
                }
            };

            lyt.foreach_pi(check_io);
            lyt.foreach_po([this, &check_io](const auto& o) { check_io(lyt.get_node(o)); });
        }

        pst.report["Gate I/O ports"] = io_pin_report;

        return summary("all I/O ports are realized by designated pins", all_pin, false);
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
            const auto check_io = [this, &border_report, &all_border](const mockturtle::node<Lyt>& io)
            {
                if (const auto iot = lyt.get_tile(io); !lyt.is_at_any_border(iot))
                {
                    all_border = false;
                    log_tile(iot, border_report);
                    ++pst.warnings;
                }
            };

            lyt.foreach_pi(check_io);
            lyt.foreach_po([this, &check_io](const auto& o) { check_io(lyt.get_node(o)); });
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
 * For this function to work, `detail::gate_level_drvs_impl` need to be declared as a `friend class` to the layout type
 * that is going to be examined.
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
    static_assert(mockturtle::has_get_node_v<Lyt>, "Lyt does not implement the get_node function");
    static_assert(mockturtle::has_node_to_index_v<Lyt>, "Lyt does not implement the node_to_index function");
    static_assert(mockturtle::has_is_constant_v<Lyt>, "Lyt does not implement the is_constant function");
    static_assert(mockturtle::has_is_buf_v<Lyt>, "Lyt does not implement the is_buf function");
    static_assert(mockturtle::has_foreach_pi_v<Lyt>, "Lyt does not implement the foreach_pi function");
    static_assert(mockturtle::has_foreach_po_v<Lyt>, "Lyt does not implement the foreach_po function");

    gate_level_drv_stats              st{};
    detail::gate_level_drvs_impl<Lyt> p{lyt, ps, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_DESIGN_RULE_VIOLATIONS_HPP
