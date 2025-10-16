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
 * This concrete molQCA gate library was proposed by the Politecnico di Torino and is based on physical simulations
 * using the SCERPA tool. Tiles in the molQCA library are \f$10 \times 10\f$ molQCA cells.
 */
class molecular_qca_library : public fcn_gate_library<mol_qca_technology, 10, 10>
{
  public:
    explicit molecular_qca_library() = delete;
    /**
     * Overrides the corresponding function in fcn_gate_library. Given a tile `t`, this function takes all necessary
     * information from the stored grid into account to choose the correct fcn_gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @tparam GateLyt Cartesian gate-level layout type.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile to be realized as a QCA ONE gate.
     * @return MolQCA gate representation of `t` including I/Os, rotation, const cells, etc.
     */
    template <typename GateLyt>
    [[nodiscard]] static fcn_gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate-level layout");

        const auto n = lyt.get_node(t);
        const auto p = determine_port_routing(lyt, t);

        try
        {
            if constexpr (fiction::has_is_fanout_v<GateLyt>)
            {
                if (lyt.is_fanout(n))
                {
                    if (lyt.fanout_size(n) == 2)
                    {
                        return FANOUT_MAP.at(p);
                    }
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
    /**
     * Post-layout optimization that assigns via cell mode to wire crossings.
     *
     * @tparam CellLyt Cell-level layout type.
     * @param lyt The cell-level layout that has been created via application of `set_up_gate`.
     */
    template <typename CellLyt>
    static void post_layout_optimization(CellLyt& lyt) noexcept
    {
        lyt.foreach_cell_position(
            [&lyt](const auto& c)
            {
                if (lyt.is_crossing_layer(c))
                {
                    if (!lyt.is_empty_cell(c))
                    {
                        // gather adjacent cell positions
                        auto adjacent_cells = lyt.adjacent_coordinates(c);
                        // remove all empty cells
                        adjacent_cells.erase(std::remove_if(adjacent_cells.begin(), adjacent_cells.end(),
                                                            [&lyt](const auto& ac) { return lyt.is_empty_cell(ac); }),
                                             adjacent_cells.end());
                        // if there is at most one neighbor left
                        if (std::distance(adjacent_cells.cbegin(), adjacent_cells.cend()) <= 1)
                        {
                            // change cell mode to via
                            lyt.assign_cell_mode(c, mol_qca_technology::cell_mode::VERTICAL);
                            // create a corresponding via ground cell
                            const cell<CellLyt> ground_via_cell{c.x, c.y, 0};
                            lyt.assign_cell_type(ground_via_cell, mol_qca_technology::cell_type::NORMAL);
                            lyt.assign_cell_mode(ground_via_cell, mol_qca_technology::cell_mode::VERTICAL);
                        }
                    }
                }
            });
    }

  private:
    template <typename Lyt>
    [[nodiscard]] static port_list<port_position> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t) noexcept
    {
        port_list<port_position> p{};

        // determine incoming connector ports for 10×10 tiles
        if (lyt.has_northern_incoming_signal(t))
        {
            p.inp.emplace(4u, 0u);
        }
        if (lyt.has_eastern_incoming_signal(t))
        {
            p.inp.emplace(9u, 4u);
        }
        if (lyt.has_southern_incoming_signal(t))
        {
            p.inp.emplace(5u, 9u);
        }
        if (lyt.has_western_incoming_signal(t))
        {
            p.inp.emplace(0u, 5u);
        }

        // determine outgoing connector ports (same regions)
        if (lyt.has_northern_outgoing_signal(t))
        {
            p.out.emplace(4u, 0u);
        }
        if (lyt.has_eastern_outgoing_signal(t))
        {
            p.out.emplace(9u, 4u);
        }
        if (lyt.has_southern_outgoing_signal(t))
        {
            p.out.emplace(5u, 9u);
        }
        if (lyt.has_western_outgoing_signal(t))
        {
            p.out.emplace(0u, 5u);
        }

        // fallback for tiles with no connectors (e.g., primary inputs/outputs on one side)
        if (const auto n = lyt.get_node(t); !lyt.is_wire(n) && !lyt.is_inv(n))
        {
            if (lyt.has_no_incoming_signal(t))
            {
                // place on left edge
                p.inp.emplace(0u, 5u);
            }
            if (lyt.has_no_outgoing_signal(t))
            {
                // place on right edge
                p.out.emplace(9u, 4u);
            }
        }

        return p;
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
     * Lookup table for fan-out rotations. Maps ports to corresponding fan-out gates.
     */
    static inline const port_gate_map FANOUT_MAP = {
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
        {{{port_position(9, 4)}, {port_position(5, 9), port_position(4, 0)}}, rotate_270(FAN_OUT_1_2_D)}};

    /**
     * Lookup table for majority rotations. Maps ports to corresponding fan-out gates.
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
