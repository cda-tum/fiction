//
// Created by benjamin on 15.07.25.
//

#ifndef FICTION_MOLECULAR_QCA_LIBRARY_HPP
#define FICTION_MOLECULAR_QCA_LIBRARY_HPP

#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <phmap.h>

#include <stdexcept>

namespace fiction
{

/**
 * @brief SIM(7)-MolPDK molecular QCA gate library.
 *
 * This MolQCA gate library corresponds to the physically simulated standard-cell library SIM(7)-MolPDK, introduced
 * in \"Bridging the Gap Between Molecular FCN and Design Automation with SIM(7)-MolPDK: A Physically Simulated
 * Standard-Cell Library\" by B. Hien, D. Quinci, Y. Ardesi, G. Beretta, F. Ravera, M. Walter, and R. Wille, published
 * at IEEE LANANO 2025 in Cusco, Peru. It is based on detailed physical simulations using the SCERPA tool, and tiles
 * represent uniform \f$10 \times 10\f$ MolQCA cell blocks.
 *
 * More information and the open-source implementation are available at
 * https://github.com/vlsi-nanocomputing/The-OpenSource-MolPDK.
 */
class molecular_qca_library : public fcn_gate_library<mol_qca_technology, 10, 10>
{
  public:
    /**
     * Deleted constructor to prevent instantiation.
     */
    explicit molecular_qca_library() = delete;
    /**
     * @brief Maps a gate-level tile to its MolQCA standard-cell implementation.
     *
     * Overrides the corresponding function in fcn_gate_library. Given a tile `t`, this function takes all necessary
     * information from the stored grid into account to choose the correct fcn_gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @tparam GateLyt Cartesian gate-level layout type.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile to be realized as a molQCA gate.
     * @return molQCA gate representation of `t` including I/Os, rotation, const cells, etc.
     */
    template <typename GateLyt>
    [[nodiscard]] static fcn_gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate-level layout");

        const auto n = lyt.get_node(t);
        auto       p = determine_port_routing(lyt, t);

        try
        {
            if constexpr (fiction::has_is_fanout_v<GateLyt>)
            {
                if (lyt.is_fanout(n))
                {
                    // Fan-out orientation is determined from all physically connected fan-out branches. Clocking-aware
                    // routing can hide valid 1-to-3 branches because clocking schemes usually expose only a subset of
                    // outgoing directions from a tile.
                    const auto fanout_size = lyt.template fanout_size<false>(n);
                    p                      = determine_port_routing<GateLyt, false>(lyt, t);

                    if (fanout_size == 3u)
                    {
                        return set_up_1_to_3_fanout(p);
                    }

                    return FANOUT_MAP.at(p);
                }
            }
            if constexpr (fiction::has_is_buf_v<GateLyt>)
            {
                if (lyt.is_buf(n))
                {
                    return WIRE_MAP.at(p);
                }
            }
            if constexpr (fiction::has_is_inv_v<GateLyt>)
            {
                if (lyt.is_inv(n))
                {
                    return INVERTER_MAP.at(p);
                }
            }
            if constexpr (mockturtle::has_is_and_v<GateLyt>)
            {
                if (lyt.is_and(n))
                {
                    return CONJUNCTION_MAP.at(p);
                }
            }
            if constexpr (mockturtle::has_is_or_v<GateLyt>)
            {
                if (lyt.is_or(n))
                {
                    return DISJUNCTION_MAP.at(p);
                }
            }
            if constexpr (mockturtle::has_is_maj_v<GateLyt>)
            {
                if (lyt.is_maj(n))
                {
                    return MAJORITY_MAP.at(p);
                }
            }
        }
        catch (const std::out_of_range&)
        {
            throw unsupported_gate_orientation_exception(t, p);
        }

        throw unsupported_gate_type_exception(t);
    }

  private:
    /**
     * @brief Determines the 10x10 MolQCA connector positions used by the gate tile at `t`.
     *
     * Connector coordinates are placed at the center of the respective tile borders: north `(4, 0)`, east `(9, 4)`,
     * south `(5, 9)`, and west `(0, 5)`. Primary inputs and outputs without explicit incoming or outgoing signals are
     * assigned to the west and east borders, respectively.
     *
     * @tparam Lyt Gate-level layout type.
     * @tparam RespectClocking Whether to respect the layout clocking while tracing incoming and outgoing signals.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile whose port routing is determined.
     * @return Incoming and outgoing molQCA connector positions for `t`.
     */
    template <typename Lyt, bool RespectClocking = true>
    [[nodiscard]] static port_list<port_position> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t) noexcept
    {
        port_list<port_position> p{};

        // determine incoming connector ports for 10×10 tiles
        if (lyt.template has_northern_incoming_signal<RespectClocking>(t))
        {
            p.inp.emplace(4u, 0u);
        }
        if (lyt.template has_eastern_incoming_signal<RespectClocking>(t))
        {
            p.inp.emplace(9u, 4u);
        }
        if (lyt.template has_southern_incoming_signal<RespectClocking>(t))
        {
            p.inp.emplace(5u, 9u);
        }
        if (lyt.template has_western_incoming_signal<RespectClocking>(t))
        {
            p.inp.emplace(0u, 5u);
        }

        // determine outgoing connector ports (same regions)
        if (lyt.template has_northern_outgoing_signal<RespectClocking>(t))
        {
            p.out.emplace(4u, 0u);
        }
        if (lyt.template has_eastern_outgoing_signal<RespectClocking>(t))
        {
            p.out.emplace(9u, 4u);
        }
        if (lyt.template has_southern_outgoing_signal<RespectClocking>(t))
        {
            p.out.emplace(5u, 9u);
        }
        if (lyt.template has_western_outgoing_signal<RespectClocking>(t))
        {
            p.out.emplace(0u, 5u);
        }

        bool is_wire_or_inverter = false;

        const auto n = lyt.get_node(t);
        if constexpr (fiction::has_is_buf_v<Lyt>)
        {
            is_wire_or_inverter = is_wire_or_inverter || lyt.is_buf(n);
        }
        if constexpr (fiction::has_is_inv_v<Lyt>)
        {
            is_wire_or_inverter = is_wire_or_inverter || lyt.is_inv(n);
        }

        // fallback for tiles with no connectors (e.g., primary inputs/outputs on one side)
        if (!is_wire_or_inverter)
        {
            if (lyt.template has_no_incoming_signal<RespectClocking>(t))
            {
                // place on left edge
                p.inp.emplace(0u, 5u);
            }
            if (lyt.template has_no_outgoing_signal<RespectClocking>(t))
            {
                // place on right edge
                p.out.emplace(9u, 4u);
            }
        }

        return p;
    }

    /**
     * @brief Selects the MolQCA 1-to-3 fan-out implementation for a routed gate tile.
     *
     * 1-to-3 fan-outs always drive all three non-input sides. Their orientation is therefore determined by the missing
     * outgoing connector side, which is the incoming side of the physical fan-out cell.
     *
     * @param p Incoming and outgoing connector positions for the fan-out tile.
     * @return 1-to-3 fan-out gate matching `p`'s missing outgoing connector.
     * @throws std::out_of_range If `p` does not describe a supported 1-to-3 fan-out orientation.
     */
    [[nodiscard]] static fcn_gate set_up_1_to_3_fanout(const port_list<port_position>& p)
    {
        if (p.inp.size() != 1u || p.out.size() != 3u)
        {
            throw std::out_of_range{"unsupported MolQCA 1-to-3 fan-out port count"};
        }

        const auto north = port_position(4u, 0u);
        const auto east  = port_position(9u, 4u);
        const auto south = port_position(5u, 9u);
        const auto west  = port_position(0u, 5u);

        if (p.inp.contains(north) && !p.out.contains(north))
        {
            return FAN_OUT_1_3;
        }
        if (p.inp.contains(east) && !p.out.contains(east))
        {
            return rotate_90(FAN_OUT_1_3);
        }
        if (p.inp.contains(south) && !p.out.contains(south))
        {
            return rotate_180(FAN_OUT_1_3);
        }
        if (p.inp.contains(west) && !p.out.contains(west))
        {
            return rotate_270(FAN_OUT_1_3);
        }

        throw std::out_of_range{"unsupported MolQCA 1-to-3 fan-out output ports"};
    }

    // clang-format off

    // ************************************************************
    // ************************** Gates ***************************
    // ************************************************************

    static constexpr const fcn_gate STRAIGHT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', 'b', 'b', 'a', 'a', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', 'a', 'a', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', ' ', ' ', 'b', 'b', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'c', 'c', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'd', 'd', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate BENT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', 'b', ' ', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', 'c', 'c', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', 'c', 'c', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', ' ', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate BENT_INVERTER_R{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'd', 'd', 'd', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'c', 'c', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', 'b', 'b', ' ', ' ', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a', 'a', 'a'},
        {' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CONJUNCTION{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '0', '0', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CONJUNCTION_D{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '0', '0', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CONJUNCTION_R{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '0', '0', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate DISJUNCTION{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate DISJUNCTION_D{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate DISJUNCTION_R{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate MAJORITY{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate FAN_OUT_1_2{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'd', 'd'},
        {'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate FAN_OUT_1_2_R{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'d', 'd', 'd', 'd', 'c', 'c', 'b', 'b', 'a', 'a'},
        {'d', 'd', 'd', 'd', 'c', 'c', 'b', 'b', 'a', 'a'},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate FAN_OUT_1_2_D{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'d', 'd', 'd', 'd', 'c', 'c', 'd', 'd', 'd', 'd'},
        {'d', 'd', 'd', 'd', 'c', 'c', 'd', 'd', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate FAN_OUT_1_3{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    static constexpr const fcn_gate PRIMARY_INPUT_PORT{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'i', 'i', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate PRIMARY_OUTPUT_PORT{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'o', 'o', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CENTER_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CENTER_BENT_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', 'c', 'c', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'c', 'c', 'd', 'd'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate CENTER_BENT_WIRE_R{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    using port_gate_map = phmap::flat_hash_map<port_list<port_position>, fcn_gate>;
    /**
     * Lookup table for wire rotations. Maps ports to corresponding wires.
     */
    static inline const port_gate_map WIRE_MAP = {
        // primary inputs
        {{{}, {port_position(4, 0)}}, PRIMARY_INPUT_PORT},
        {{{}, {port_position(9, 4)}}, rotate_90(PRIMARY_INPUT_PORT)},
        {{{}, {port_position(5, 9)}}, rotate_180(PRIMARY_INPUT_PORT)},
        {{{}, {port_position(0, 5)}}, rotate_270(PRIMARY_INPUT_PORT)},
        // primary outputs
        {{{port_position(4, 0)}, {}}, PRIMARY_OUTPUT_PORT},
        {{{port_position(9, 4)}, {}}, rotate_90(PRIMARY_OUTPUT_PORT)},
        {{{port_position(5, 9)}, {}}, rotate_180(PRIMARY_OUTPUT_PORT)},
        {{{port_position(0, 5)}, {}}, rotate_270(PRIMARY_OUTPUT_PORT)},
        // center wire
        {{{port_position(4, 0)}, {port_position(5, 9)}}, CENTER_WIRE},
        {{{port_position(5, 9)}, {port_position(4, 0)}}, rotate_180(CENTER_WIRE)},
        {{{port_position(9, 4)}, {port_position(0, 5)}}, rotate_90(CENTER_WIRE)},
        {{{port_position(0, 5)}, {port_position(9, 4)}}, rotate_270(CENTER_WIRE)},
        // center bent wire
        {{{port_position(4, 0)}, {port_position(9, 4)}}, CENTER_BENT_WIRE},
        {{{port_position(9, 4)}, {port_position(4, 0)}}, CENTER_BENT_WIRE_R},
        {{{port_position(9, 4)}, {port_position(5, 9)}}, rotate_90(CENTER_BENT_WIRE)},
        {{{port_position(5, 9)}, {port_position(9, 4)}}, rotate_90(CENTER_BENT_WIRE_R)},
        {{{port_position(0, 5)}, {port_position(5, 9)}}, rotate_180(CENTER_BENT_WIRE_R)},
        {{{port_position(5, 9)}, {port_position(0, 5)}}, rotate_180(CENTER_BENT_WIRE)},
        {{{port_position(4, 0)}, {port_position(0, 5)}}, rotate_270(CENTER_BENT_WIRE_R)},
        {{{port_position(0, 5)}, {port_position(4, 0)}}, rotate_270(CENTER_BENT_WIRE)}};
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map INVERTER_MAP = {
        // straight inverters
        {{{port_position(4, 0)}, {port_position(5, 9)}}, STRAIGHT_INVERTER},
        {{{port_position(9, 4)}, {port_position(0, 5)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{port_position(5, 9)}, {port_position(4, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{port_position(0, 5)}, {port_position(9, 4)}}, rotate_270(STRAIGHT_INVERTER)},
        // without outputs
        {{{port_position(4, 0)}, {}}, STRAIGHT_INVERTER},
        {{{port_position(9, 4)}, {}}, rotate_90(STRAIGHT_INVERTER)},
        {{{port_position(5, 9)}, {}}, rotate_180(STRAIGHT_INVERTER)},
        {{{port_position(0, 5)}, {}}, rotate_270(STRAIGHT_INVERTER)},
        // without inputs
        {{{}, {port_position(5, 9)}}, STRAIGHT_INVERTER},
        {{{}, {port_position(0, 5)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{}, {port_position(4, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{}, {port_position(9, 4)}}, rotate_270(STRAIGHT_INVERTER)},
        // bent inverter
        {{{port_position(4, 0)}, {port_position(9, 4)}}, BENT_INVERTER},
        {{{port_position(9, 4)}, {port_position(4, 0)}}, BENT_INVERTER_R},
        {{{port_position(9, 4)}, {port_position(5, 9)}}, rotate_90(BENT_INVERTER)},
        {{{port_position(5, 9)}, {port_position(9, 4)}}, rotate_90(BENT_INVERTER_R)},
        {{{port_position(0, 5)}, {port_position(5, 9)}}, rotate_180(BENT_INVERTER_R)},
        {{{port_position(5, 9)}, {port_position(0, 5)}}, rotate_180(BENT_INVERTER)},
        {{{port_position(4, 0)}, {port_position(0, 5)}}, rotate_270(BENT_INVERTER_R)},
        {{{port_position(0, 5)}, {port_position(4, 0)}}, rotate_270(BENT_INVERTER)}};
    /**
     * Lookup table for conjunction rotations. Maps ports to corresponding AND gates.
     */
    static inline const port_gate_map CONJUNCTION_MAP = {
        // identity orientation
        {{{port_position(5, 9), port_position(9, 4)}, {port_position(0, 5)}}, CONJUNCTION},
        {{{port_position(0, 5), port_position(9, 4)}, {port_position(5, 9)}}, CONJUNCTION_D},
        {{{port_position(0, 5), port_position(5, 9)}, {port_position(9, 4)}}, CONJUNCTION_R},

        // rotated 90°
        {{{port_position(0, 5), port_position(5, 9)}, {port_position(4, 0)}}, rotate_90(CONJUNCTION)},
        {{{port_position(5, 9), port_position(4, 0)}, {port_position(0, 5)}}, rotate_90(CONJUNCTION_D)},
        {{{port_position(0, 5), port_position(4, 0)}, {port_position(5, 9)}}, rotate_90(CONJUNCTION_R)},

        // rotated 180°
        {{{port_position(0, 5), port_position(4, 0)}, {port_position(9, 4)}}, rotate_180(CONJUNCTION)},
        {{{port_position(0, 5), port_position(9, 4)}, {port_position(4, 0)}}, rotate_180(CONJUNCTION_D)},
        {{{port_position(9, 4), port_position(4, 0)}, {port_position(0, 5)}}, rotate_180(CONJUNCTION_R)},

        // rotated 270°
        {{{port_position(9, 4), port_position(4, 0)}, {port_position(5, 9)}}, rotate_270(CONJUNCTION)},
        {{{port_position(5, 9), port_position(4, 0)}, {port_position(9, 4)}}, rotate_270(CONJUNCTION_D)},
        {{{port_position(5, 9), port_position(9, 4)}, {port_position(4, 0)}}, rotate_270(CONJUNCTION_R)}};

    /**
     * Lookup table for disjunction rotations. Maps ports to corresponding OR gates.
     */
    static inline const port_gate_map DISJUNCTION_MAP = {
        // identity orientation
        {{{port_position(5, 9), port_position(9, 4)}, {port_position(0, 5)}}, DISJUNCTION},
        {{{port_position(0, 5), port_position(9, 4)}, {port_position(5, 9)}}, DISJUNCTION_D},
        {{{port_position(0, 5), port_position(5, 9)}, {port_position(9, 4)}}, DISJUNCTION_R},

        // rotated 90°
        {{{port_position(0, 5), port_position(5, 9)}, {port_position(4, 0)}}, rotate_90(DISJUNCTION)},
        {{{port_position(5, 9), port_position(4, 0)}, {port_position(0, 5)}}, rotate_90(DISJUNCTION_D)},
        {{{port_position(0, 5), port_position(4, 0)}, {port_position(5, 9)}}, rotate_90(DISJUNCTION_R)},

        // rotated 180°
        {{{port_position(0, 5), port_position(4, 0)}, {port_position(9, 4)}}, rotate_180(DISJUNCTION)},
        {{{port_position(0, 5), port_position(9, 4)}, {port_position(4, 0)}}, rotate_180(DISJUNCTION_D)},
        {{{port_position(9, 4), port_position(4, 0)}, {port_position(0, 5)}}, rotate_180(DISJUNCTION_R)},

        // rotated 270°
        {{{port_position(9, 4), port_position(4, 0)}, {port_position(5, 9)}}, rotate_270(DISJUNCTION)},
        {{{port_position(5, 9), port_position(4, 0)}, {port_position(9, 4)}}, rotate_270(DISJUNCTION_D)},
        {{{port_position(5, 9), port_position(9, 4)}, {port_position(4, 0)}}, rotate_270(DISJUNCTION_R)}};
    /**
     * Lookup table for 1-to-2 fan-out rotations. Maps ports to corresponding fan-out gates.
     */
    static inline const port_gate_map FANOUT_MAP = {
        // fanout 2
        // identity orientation
        {{{port_position(0, 5)}, {port_position(5, 9), port_position(9, 4)}}, FAN_OUT_1_2},
        {{{port_position(9, 4)}, {port_position(0, 5), port_position(5, 9)}}, FAN_OUT_1_2_R},
        {{{port_position(5, 9)}, {port_position(0, 5), port_position(9, 4)}}, FAN_OUT_1_2_D},

        // rotated 90°
        {{{port_position(4, 0)}, {port_position(0, 5), port_position(5, 9)}}, rotate_90(FAN_OUT_1_2)},
        {{{port_position(5, 9)}, {port_position(0, 5), port_position(4, 0)}}, rotate_90(FAN_OUT_1_2_R)},
        {{{port_position(0, 5)}, {port_position(5, 9), port_position(4, 0)}}, rotate_90(FAN_OUT_1_2_D)},

        // rotated 180°
        {{{port_position(9, 4)}, {port_position(0, 5), port_position(4, 0)}}, rotate_180(FAN_OUT_1_2)},
        {{{port_position(0, 5)}, {port_position(9, 4), port_position(4, 0)}}, rotate_180(FAN_OUT_1_2_R)},
        {{{port_position(4, 0)}, {port_position(0, 5), port_position(9, 4)}}, rotate_180(FAN_OUT_1_2_D)},

        // rotated 270°
        {{{port_position(5, 9)}, {port_position(9, 4), port_position(4, 0)}}, rotate_270(FAN_OUT_1_2)},
        {{{port_position(4, 0)}, {port_position(5, 9), port_position(9, 4)}}, rotate_270(FAN_OUT_1_2_R)},
        {{{port_position(9, 4)}, {port_position(5, 9), port_position(4, 0)}}, rotate_270(FAN_OUT_1_2_D)},
    };

    /**
     * Lookup table for majority rotations. Maps ports to corresponding majority gates.
     */
    static inline const port_gate_map MAJORITY_MAP = {
        // identity orientation
        {{{port_position(0, 5), port_position(4, 0), port_position(9, 4)}, {port_position(5, 9)}}, MAJORITY},
        {{{port_position(4, 0), port_position(9, 4), port_position(5, 9)}, {port_position(0, 5)}}, rotate_90(MAJORITY)},
        {{{port_position(9, 4), port_position(5, 9), port_position(0, 5)}, {port_position(4, 0)}},
         rotate_180(MAJORITY)},
        {{{port_position(5, 9), port_position(0, 5), port_position(4, 0)}, {port_position(9, 4)}},
         rotate_270(MAJORITY)}};
};

}  // namespace fiction

#endif  // FICTION_MOLECULAR_QCA_LIBRARY_HPP
