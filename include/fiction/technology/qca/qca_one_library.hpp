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
 * @brief QCA gate library based on the QCA ONE standard cell design.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/fcn/gate_library.hpp"
#include "fiction/technology/qca/layout.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <phmap.h>

#include <stdexcept>
#include <vector>

namespace fiction::qca
{

/**
 * A concrete FCN gate library based on QCA ONE proposed in \"A Methodology for Standard Cell Design for QCA\" by Dayane
 * Alfenas Reis, Caio Araújo T. Campos, Thiago Rodrigues B. S. Soares, Omar Paranaiba V. Neto, and Frank Sill Torres in
 * IEEE International Symposium on Circuits and Systems, 2016. QCA ONE was originally proposed for the USE clocking
 * scheme. The version used here is an extension to the original QCA ONE by also theoretically allowing multiple wires
 * in the same tile. Furthermore, it can be used for a range of clocking schemes. Tiles in QCA ONE are \f$5 \times 5\f$
 * QCA cells.
 */
class qca_one_library : public fcn::gate_library<qca::layout, 5, 5>
{
  public:
    explicit qca_one_library() = delete;
    /**
     * Overrides the corresponding function in gate_library. Given a tile `t`, this function takes all necessary
     * information from the stored grid into account to choose the correct gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @tparam GateLyt Cartesian gate-level layout type.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile to be realized as a QCA ONE gate.
     * @return QCA ONE gate representation of `t` including I/Os, rotation, const cells, etc.
     */
    template <typename GateLyt>
    [[nodiscard]] static gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate-level layout");

        const auto object = lyt.find_object(t);
        if (!object)
        {
            throw fcn::unsupported_gate_type_exception(t);
        }
        const auto n = *object;
        const auto p = determine_port_routing(lyt, t);

        try
        {
            if (lyt.is_fanout(n))
            {
                if (p.out.size() == 2)
                {
                    return FANOUT_MAP.at(p);
                }
                if (p.out.size() == 3)
                {
                    return FAN_OUT_1_3;
                }
            }

            if (lyt.is_buf(n))
            {
                return WIRE_MAP.at(p);
            }

            if (lyt.is_inv(n))
            {
                return INVERTER_MAP.at(p);
            }

            if (lyt.is_and(n))
            {
                return CONJUNCTION_MAP.at(p);
            }

            if (lyt.is_or(n))
            {
                return DISJUNCTION_MAP.at(p);
            }

            if (lyt.is_maj(n))
            {
                return MAJORITY;
            }
        }
        catch (const std::out_of_range&)
        {
            throw fcn::unsupported_gate_orientation_exception(t, p);
        }

        throw fcn::unsupported_gate_type_exception(t);
    }
    /**
     * Post-layout optimization that turns the ends of crossing wires into vias: a crossing-layer cell with at most one
     * neighbor gets the via mode, and a via cell is added below it on the ground layer. The optimization visits
     * occupied cells inside the frame and ignores empty positions.
     *
     * @param lyt The QCA layout that has been created via application of `set_up_gate`.
     */
    static void post_layout_optimization(qca::layout& lyt)
    {
        /** Occupied crossing cells inside the frame, stable while ground vias are inserted. */
        std::vector<qca::layout::cell> crossing_cells{};
        lyt.foreach_cell(
            [&lyt, &crossing_cells](const auto& c)
            {
                if (lyt.contains_coordinate(c) && lyt.is_crossing_layer(c))
                {
                    crossing_cells.push_back(c);
                }
            });
        for (const auto& c : crossing_cells)
        {
            /** Occupied neighbors in the crossing layer. */
            auto adjacent_cells = lyt.adjacent_coordinates(c);
            std::erase_if(adjacent_cells, [&lyt](const auto& ac) { return lyt.is_empty_cell(ac); });
            if (adjacent_cells.size() <= 1)
            {
                lyt.assign_cell_mode(c, qca::cell_mode::VERTICAL);
                /** Ground coordinate beneath the crossing endpoint. */
                const qca::layout::cell ground_via_cell{c.x, c.y, 0};
                lyt.assign_cell_type(ground_via_cell, qca::cell_type::NORMAL);
                lyt.assign_cell_mode(ground_via_cell, qca::cell_mode::VERTICAL);
            }
        }
    }

  private:
    /**
     * Routes the physical connector ports of an occupied tile.
     * @tparam Lyt Gate-level layout type.
     * @param lyt Layout.
     * @param t Occupied tile.
     * @return Physical connector ports.
     */
    template <typename Lyt>
    [[nodiscard]] static fcn::port_list<fcn::port_position> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t)
    {
        fcn::port_list<fcn::port_position> p{};

        // determine incoming connector ports
        if (lyt.has_northern_incoming_signal(t))
        {
            p.inp.emplace(2u, 0u);
        }
        if (lyt.has_eastern_incoming_signal(t))
        {
            p.inp.emplace(4u, 2u);
        }
        if (lyt.has_southern_incoming_signal(t))
        {
            p.inp.emplace(2u, 4u);
        }
        if (lyt.has_western_incoming_signal(t))
        {
            p.inp.emplace(0u, 2u);
        }

        // determine outgoing connector ports
        if (lyt.has_northern_outgoing_signal(t))
        {
            p.out.emplace(2u, 0u);
        }
        if (lyt.has_eastern_outgoing_signal(t))
        {
            p.out.emplace(4u, 2u);
        }
        if (lyt.has_southern_outgoing_signal(t))
        {
            p.out.emplace(2u, 4u);
        }
        if (lyt.has_western_outgoing_signal(t))
        {
            p.out.emplace(0u, 2u);
        }

        // has no connector ports
        if (const auto n = lyt.find_object(t); n && (!lyt.is_wire(*n) && !lyt.is_inv(*n)))
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(0u, 2u);
            }

            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(4u, 2u);
            }
        }

        return p;
    }

    // clang-format off

    // ************************************************************
    // ************************** Gates ***************************
    // ************************************************************

    static constexpr const gate STRAIGHT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', 'x', 'x', 'x', ' '},
        {' ', 'x', ' ', 'x', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate BENT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', ' ', 'x', 'x'},
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate CONJUNCTION{cell_list_to_gate<char>(
    {{
        {' ', ' ', '0', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x', 'x'},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate DISJUNCTION{cell_list_to_gate<char>(
    {{
        {' ', ' ', '1', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x', 'x'},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate MAJORITY{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x', 'x'},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate FAN_OUT_1_2{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x', 'x'},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate FAN_OUT_1_3{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x', 'x'},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    static constexpr const gate PRIMARY_INPUT_PORT{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'i', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate PRIMARY_OUTPUT_PORT{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'o', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate CENTER_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate INNER_SIDE_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'x', ' '},
        {' ', ' ', ' ', 'x', ' '},
        {' ', ' ', ' ', 'x', ' '},
        {' ', ' ', ' ', 'x', ' '},
        {' ', ' ', ' ', 'x', ' '}
    }})};

    static constexpr const gate OUTER_SIDE_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'x'},
        {' ', ' ', ' ', ' ', 'x'},
        {' ', ' ', ' ', ' ', 'x'},
        {' ', ' ', ' ', ' ', 'x'},
        {' ', ' ', ' ', ' ', 'x'}
    }})};

    static constexpr const gate CENTER_BENT_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', ' ', ' '},
        {' ', ' ', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate INNER_CENTER_TO_INNER_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate INNER_CENTER_TO_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate INNER_CENTER_TO_OUTER_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate INNER_CENTER_TO_OUTER_SIDE_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', ' '},
                                  {' ', ' ', ' ', 'x', 'x'}}})};

    static constexpr const gate CENTER_TO_INNER_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                  {' ', ' ', 'x', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate OUTER_CENTER_TO_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', 'x', ' ', ' ', ' '},
                                  {' ', 'x', ' ', ' ', ' '},
                                  {' ', 'x', 'x', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate OUTER_CENTER_TO_OUTER_CENTER_BENT_WIRE{
        cell_list_to_gate<char>({{{' ', 'x', ' ', ' ', ' '},
                                  {' ', 'x', ' ', ' ', ' '},
                                  {' ', 'x', ' ', ' ', ' '},
                                  {' ', 'x', 'x', 'x', 'x'},
                                  {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const gate OUTER_SIDE_TO_OUTER_SIDE_BENT_WIRE{
        cell_list_to_gate<char>({{{'x', ' ', ' ', ' ', ' '},
                                  {'x', ' ', ' ', ' ', ' '},
                                  {'x', ' ', ' ', ' ', ' '},
                                  {'x', ' ', ' ', ' ', ' '},
                                  {'x', 'x', 'x', 'x', 'x'}}})};

    // clang-format on

    using port_gate_map = phmap::flat_hash_map<fcn::port_list<fcn::port_position>, gate>;
    /**
     * Lookup table for wire rotations. Maps ports to corresponding wires.
     */
    static inline const port_gate_map WIRE_MAP = {
        // primary inputs
        {{{}, {fcn::port_position(2, 0)}}, PRIMARY_INPUT_PORT},
        {{{}, {fcn::port_position(4, 2)}}, rotate_90(PRIMARY_INPUT_PORT)},
        {{{}, {fcn::port_position(2, 4)}}, rotate_180(PRIMARY_INPUT_PORT)},
        {{{}, {fcn::port_position(0, 2)}}, rotate_270(PRIMARY_INPUT_PORT)},
        // primary outputs
        {{{fcn::port_position(2, 0)}, {}}, PRIMARY_OUTPUT_PORT},
        {{{fcn::port_position(4, 2)}, {}}, rotate_90(PRIMARY_OUTPUT_PORT)},
        {{{fcn::port_position(2, 4)}, {}}, rotate_180(PRIMARY_OUTPUT_PORT)},
        {{{fcn::port_position(0, 2)}, {}}, rotate_270(PRIMARY_OUTPUT_PORT)},
        // center wire
        {{{fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, CENTER_WIRE},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(2, 0)}}, CENTER_WIRE},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(4, 2)}}, rotate_90(CENTER_WIRE)},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(0, 2)}}, rotate_90(CENTER_WIRE)},
        // inner side wire
        {{{fcn::port_position(3, 0)}, {fcn::port_position(3, 4)}}, INNER_SIDE_WIRE},
        {{{fcn::port_position(3, 4)}, {fcn::port_position(3, 0)}}, INNER_SIDE_WIRE},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(4, 3)}}, rotate_90(INNER_SIDE_WIRE)},
        {{{fcn::port_position(4, 3)}, {fcn::port_position(0, 3)}}, rotate_90(INNER_SIDE_WIRE)},
        {{{fcn::port_position(1, 0)}, {fcn::port_position(1, 4)}}, rotate_180(INNER_SIDE_WIRE)},
        {{{fcn::port_position(1, 4)}, {fcn::port_position(1, 0)}}, rotate_180(INNER_SIDE_WIRE)},
        {{{fcn::port_position(0, 1)}, {fcn::port_position(4, 1)}}, rotate_270(INNER_SIDE_WIRE)},
        {{{fcn::port_position(4, 1)}, {fcn::port_position(0, 1)}}, rotate_270(INNER_SIDE_WIRE)},
        // outer side wire
        {{{fcn::port_position(4, 0)}, {fcn::port_position(4, 4)}}, OUTER_SIDE_WIRE},
        {{{fcn::port_position(4, 4)}, {fcn::port_position(4, 0)}}, OUTER_SIDE_WIRE},
        {{{fcn::port_position(0, 4)}, {fcn::port_position(4, 4)}}, rotate_90(OUTER_SIDE_WIRE)},
        {{{fcn::port_position(4, 4)}, {fcn::port_position(0, 4)}}, rotate_90(OUTER_SIDE_WIRE)},
        {{{fcn::port_position(0, 0)}, {fcn::port_position(0, 4)}}, rotate_180(OUTER_SIDE_WIRE)},
        {{{fcn::port_position(0, 4)}, {fcn::port_position(0, 0)}}, rotate_180(OUTER_SIDE_WIRE)},
        {{{fcn::port_position(0, 0)}, {fcn::port_position(4, 0)}}, rotate_270(OUTER_SIDE_WIRE)},
        {{{fcn::port_position(4, 0)}, {fcn::port_position(0, 0)}}, rotate_270(OUTER_SIDE_WIRE)},
        // center bent wire
        {{{fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, CENTER_BENT_WIRE},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, CENTER_BENT_WIRE},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(2, 4)}}, rotate_90(CENTER_BENT_WIRE)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(4, 2)}}, rotate_90(CENTER_BENT_WIRE)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 4)}}, rotate_180(CENTER_BENT_WIRE)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(0, 2)}}, rotate_180(CENTER_BENT_WIRE)},
        {{{fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_270(CENTER_BENT_WIRE)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 0)}}, rotate_270(CENTER_BENT_WIRE)}
        // TODO more wires go here!
    };
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map INVERTER_MAP = {
        // straight inverters
        {{{fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, STRAIGHT_INVERTER},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(0, 2)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(2, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(4, 2)}}, rotate_270(STRAIGHT_INVERTER)},
        // without outputs
        {{{fcn::port_position(2, 0)}, {}}, STRAIGHT_INVERTER},
        {{{fcn::port_position(4, 2)}, {}}, rotate_90(STRAIGHT_INVERTER)},
        {{{fcn::port_position(2, 4)}, {}}, rotate_180(STRAIGHT_INVERTER)},
        {{{fcn::port_position(0, 2)}, {}}, rotate_270(STRAIGHT_INVERTER)},
        // without inputs
        {{{}, {fcn::port_position(2, 4)}}, STRAIGHT_INVERTER},
        {{{}, {fcn::port_position(0, 2)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{}, {fcn::port_position(2, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{}, {fcn::port_position(4, 2)}}, rotate_270(STRAIGHT_INVERTER)},
        // bent inverters
        {{{fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, BENT_INVERTER},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, BENT_INVERTER},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(2, 4)}}, rotate_90(BENT_INVERTER)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(4, 2)}}, rotate_90(BENT_INVERTER)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 4)}}, rotate_180(BENT_INVERTER)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(0, 2)}}, rotate_180(BENT_INVERTER)},
        {{{fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_270(BENT_INVERTER)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 0)}}, rotate_270(BENT_INVERTER)}};
    /**
     * Lookup table for conjunction rotations. Maps ports to corresponding AND gates.
     */
    static inline const port_gate_map CONJUNCTION_MAP = {
        {{{fcn::port_position(0, 2), fcn::port_position(2, 4)}, {fcn::port_position(4, 2)}}, CONJUNCTION},
        {{{fcn::port_position(0, 2), fcn::port_position(4, 2)}, {fcn::port_position(2, 4)}}, CONJUNCTION},
        {{{fcn::port_position(2, 4), fcn::port_position(4, 2)}, {fcn::port_position(0, 2)}}, CONJUNCTION},

        {{{fcn::port_position(0, 2), fcn::port_position(2, 4)}, {fcn::port_position(2, 0)}}, rotate_90(CONJUNCTION)},
        {{{fcn::port_position(0, 2), fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, rotate_90(CONJUNCTION)},
        {{{fcn::port_position(2, 4), fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_90(CONJUNCTION)},

        {{{fcn::port_position(0, 2), fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, rotate_180(CONJUNCTION)},
        {{{fcn::port_position(0, 2), fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, rotate_180(CONJUNCTION)},
        {{{fcn::port_position(4, 2), fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_180(CONJUNCTION)},

        {{{fcn::port_position(2, 4), fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, rotate_270(CONJUNCTION)},
        {{{fcn::port_position(2, 4), fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, rotate_270(CONJUNCTION)},
        {{{fcn::port_position(4, 2), fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, rotate_270(CONJUNCTION)}};
    /**
     * Lookup table for disjunction rotations. Maps ports to corresponding OR gates.
     */
    static inline const port_gate_map DISJUNCTION_MAP = {
        {{{fcn::port_position(0, 2), fcn::port_position(2, 4)}, {fcn::port_position(4, 2)}}, DISJUNCTION},
        {{{fcn::port_position(0, 2), fcn::port_position(4, 2)}, {fcn::port_position(2, 4)}}, DISJUNCTION},
        {{{fcn::port_position(2, 4), fcn::port_position(4, 2)}, {fcn::port_position(0, 2)}}, DISJUNCTION},

        {{{fcn::port_position(0, 2), fcn::port_position(2, 4)}, {fcn::port_position(2, 0)}}, rotate_90(DISJUNCTION)},
        {{{fcn::port_position(0, 2), fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, rotate_90(DISJUNCTION)},
        {{{fcn::port_position(2, 4), fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_90(DISJUNCTION)},

        {{{fcn::port_position(0, 2), fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, rotate_180(DISJUNCTION)},
        {{{fcn::port_position(0, 2), fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, rotate_180(DISJUNCTION)},
        {{{fcn::port_position(4, 2), fcn::port_position(2, 0)}, {fcn::port_position(0, 2)}}, rotate_180(DISJUNCTION)},

        {{{fcn::port_position(2, 4), fcn::port_position(4, 2)}, {fcn::port_position(2, 0)}}, rotate_270(DISJUNCTION)},
        {{{fcn::port_position(2, 4), fcn::port_position(2, 0)}, {fcn::port_position(4, 2)}}, rotate_270(DISJUNCTION)},
        {{{fcn::port_position(4, 2), fcn::port_position(2, 0)}, {fcn::port_position(2, 4)}}, rotate_270(DISJUNCTION)}};
    /**
     * Lookup table for fan-out rotations. Maps ports to corresponding fan-out gates.
     */
    static inline const port_gate_map FANOUT_MAP = {
        {{{fcn::port_position(4, 2)}, {fcn::port_position(0, 2), fcn::port_position(2, 4)}}, FAN_OUT_1_2},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(0, 2), fcn::port_position(4, 2)}}, FAN_OUT_1_2},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 4), fcn::port_position(4, 2)}}, FAN_OUT_1_2},

        {{{fcn::port_position(2, 0)}, {fcn::port_position(0, 2), fcn::port_position(2, 4)}}, rotate_90(FAN_OUT_1_2)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(0, 2), fcn::port_position(2, 0)}}, rotate_90(FAN_OUT_1_2)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(2, 4), fcn::port_position(2, 0)}}, rotate_90(FAN_OUT_1_2)},

        {{{fcn::port_position(2, 0)}, {fcn::port_position(0, 2), fcn::port_position(4, 2)}}, rotate_180(FAN_OUT_1_2)},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(0, 2), fcn::port_position(2, 0)}}, rotate_180(FAN_OUT_1_2)},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(4, 2), fcn::port_position(2, 0)}}, rotate_180(FAN_OUT_1_2)},

        {{{fcn::port_position(2, 0)}, {fcn::port_position(2, 4), fcn::port_position(4, 2)}}, rotate_270(FAN_OUT_1_2)},
        {{{fcn::port_position(4, 2)}, {fcn::port_position(2, 4), fcn::port_position(2, 0)}}, rotate_270(FAN_OUT_1_2)},
        {{{fcn::port_position(2, 4)}, {fcn::port_position(4, 2), fcn::port_position(2, 0)}}, rotate_270(FAN_OUT_1_2)}};
};

}  // namespace fiction::qca
