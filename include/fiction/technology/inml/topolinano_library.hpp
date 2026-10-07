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
 * @brief iNML gate library as used by ToPoliNano.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/fcn/gate_library.hpp"
#include "fiction/technology/inml/layout.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <phmap.h>

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace fiction::inml
{
/**
 * A concrete FCN gate library as used in \"ToPoliNano\" (https://topolinano.polito.it/) for the iNML technology. In
 * fiction, this is emulated by using vertically shifted layouts and implementing the ToPoliNano library with \f$4
 * \times 4\f$ magnet positions with one empty row in most tiles (except for MAJ which needs to be handled differently
 * as this library is not uniform otherwise). Theoretically, it allows for multiple wires in the same tile.
 */
class topolinano_library : public fcn::gate_library<inml::layout, 4, 4>
{
  public:
    explicit topolinano_library() = delete;
    /**
     * Overrides the corresponding function in gate_library. Given a tile `t`, this function takes all necessary
     * information from the stored grid into account to choose the correct gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @tparam GateLyt Shifted Cartesian gate-level layout type.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile to be realized as a ToPoliNano gate.
     * @return ToPoliNano gate representation of `t` including I/Os, rotation, etc.
     * @throws std::invalid_argument If `lyt` shifts rows instead of columns.
     */
    template <typename GateLyt>
    [[nodiscard]] static gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate-level layout");
        static_assert(is_shifted_cartesian_layout_v<GateLyt>, "GateLyt must be a shifted Cartesian layout");

        if (layouts::is_row_arrangement(lyt.get_arrangement()))
        {
            throw std::invalid_argument("GateLyt must be a column-shifted Cartesian layout");
        }

        // crossing magnets are handled only in the ground layer
        if (lyt.is_crossing_layer(t))
        {
            return EMPTY_GATE;
        }

        const auto object = lyt.find_object(t);
        if (!object)
        {
            throw fcn::unsupported_gate_type_exception(t);
        }
        const auto n = *object;

        if (lyt.is_fanout(n))
        {
            return COUPLER;
        }

        if (lyt.is_and(n))
        {
            return CONJUNCTION;
        }

        if (lyt.is_or(n))
        {
            return DISJUNCTION;
        }

        if (lyt.is_maj(n))
        {
            return MAJORITY;
        }

        const auto p = determine_port_routing(lyt, t);

        try
        {

            if (lyt.is_inv(n))
            {
                return INVERTER_MAP.at(p);
            }

            if (lyt.is_buf(n))
            {
                // crossing case
                if (const auto a = lyt.above(t); a && lyt.is_wire_tile(*a))
                {
                    return CROSSWIRE;
                }

                auto wire = WIRE_MAP.at(p);

                if (lyt.is_pi(n))
                {
                    const auto inp_mark_pos = p.inp.empty() ? opposite(*p.out.begin()) : *p.inp.begin();

                    wire = mark_cell(wire, inp_mark_pos, inml::magnet_type::INPUT);
                }
                if (lyt.is_po(n))
                {
                    const auto out_mark_pos = p.out.empty() ? opposite(*p.inp.begin()) : *p.out.begin();

                    wire = mark_cell(wire, out_mark_pos, inml::magnet_type::OUTPUT);
                }

                return wire;
            }
        }
        catch (const std::out_of_range&)
        {
            throw fcn::unsupported_gate_orientation_exception(t, p);
        }

        throw fcn::unsupported_gate_type_exception(t);
    }

    /**
     * Post-layout optimization that straightens the wire segments to save cells.
     *
     * @param lyt The iNML layout that has been created via application of `set_up_gate`.
     */
    static void post_layout_optimization(inml::layout& lyt)
    {
        enum class status : std::uint8_t
        {
            SEARCH,
            COLLECT,
            SKIP
        };

        bool improvement_found{false};

        const auto handle = [&lyt, &improvement_found](const auto& hump)
        {
            // single cell collections are no humps, do not merge them.
            if (hump.size() > 1)
            {
                // if sequence starts with a PI or there are normal cells south of first and last hump cell, this is an
                // upper hump
                const auto inp = lyt.get_cell_type(hump.front()), out = lyt.get_cell_type(hump.back());
                if (const auto front = lyt.south(hump.front()), back = lyt.south(hump.back());
                    front && back &&
                    (inp == inml::magnet_type::INPUT || lyt.get_cell_type(*front) == inml::magnet_type::NORMAL) &&
                    (lyt.get_cell_type(*back) == inml::magnet_type::NORMAL || out == inml::magnet_type::OUTPUT ||
                     lyt.get_cell_type(*back) == inml::magnet_type::INVERTER_MAGNET))
                {
                    // hump found, check if there is enough space below for merging
                    if (std::all_of(hump.begin() + 1, hump.end() - 1,
                                    [&lyt](const auto hc)
                                    {
                                        const auto neighbor = lyt.south(hc);
                                        const auto beyond   = neighbor ? lyt.south(*neighbor) : std::nullopt;
                                        return beyond && lyt.is_empty_cell(*beyond);
                                    }))
                    {
                        // merge it down
                        for (const auto& hc : hump)
                        {
                            const auto s = *lyt.south(hc);
                            if (lyt.get_cell_type(s) != inml::magnet_type::INVERTER_MAGNET)
                            {
                                lyt.assign_cell_type(s, lyt.get_cell_type(hc));
                                lyt.assign_cell_name(s, lyt.get_cell_name(hc));
                            }
                            lyt.assign_cell_type(hc, inml::magnet_type::EMPTY);
                            lyt.assign_cell_name(hc, "");
                        }

                        improvement_found = true;
                    }
                }
                // if there are normal cells north of first and last hump cell, this is a lower hump
                else if (const auto north_front = lyt.north(hump.front()), north_back = lyt.north(hump.back());
                         north_front && north_back &&
                         (inp == inml::magnet_type::INPUT ||
                          lyt.get_cell_type(*north_front) == inml::magnet_type::NORMAL) &&
                         (lyt.get_cell_type(*north_back) == inml::magnet_type::NORMAL ||
                          out == inml::magnet_type::OUTPUT))
                {
                    // hump found, check if there is enough space above for merging
                    if (std::all_of(hump.begin() + 1, hump.end() - 1,
                                    [&lyt](const auto hc)
                                    {
                                        const auto neighbor = lyt.north(hc);
                                        const auto beyond   = neighbor ? lyt.north(*neighbor) : std::nullopt;
                                        return beyond && lyt.is_empty_cell(*beyond);
                                    }))
                    {
                        // merge it up
                        for (const auto& hc : hump)
                        {
                            const auto n = *lyt.north(hc);
                            lyt.assign_cell_type(n, lyt.get_cell_type(hc));
                            lyt.assign_cell_name(n, lyt.get_cell_name(hc));
                            lyt.assign_cell_type(hc, inml::magnet_type::EMPTY);
                            lyt.assign_cell_name(hc, "");
                        }

                        improvement_found = true;
                    }
                }
            }
        };

        do  // NOLINT(cppcoreguidelines-avoid-do-while): the exit condition (`improvement_found`) is
            // computed by the body itself, so a `while` would need to duplicate the body or introduce
            // a flag variable just to seed the first iteration
        {
            status st = status::SEARCH;

            improvement_found = false;

            for (uint32_t row = 0; row < lyt.height(); ++row)
            {
                std::vector<inml::layout::cell> hump{};

                for (uint32_t column = 0; column < lyt.width(); ++column)
                {
                    // simple state machine for identifying humps and removing them
                    switch (const auto c = inml::layout::cell{column, row}; st)
                    {
                        case status::SEARCH:
                        {
                            switch (const auto t = lyt.get_cell_type(c); t)
                            {
                                // encountering a normal, input, or inverter magnet triggers collecting hump cells
                                case inml::magnet_type::NORMAL:
                                case inml::magnet_type::INPUT:
                                case inml::magnet_type::INVERTER_MAGNET:
                                {
                                    st = status::COLLECT;
                                    hump.push_back(c);
                                    break;
                                }
                                // remain searching
                                case inml::magnet_type::EMPTY:
                                {
                                    break;
                                }
                                // everything else leads to skipping
                                default:
                                {
                                    st = status::SKIP;
                                    break;
                                }
                            }
                            break;
                        }
                        case status::COLLECT:
                        {
                            switch (const auto t = lyt.get_cell_type(c); t)
                            {
                                // collect cells
                                case inml::magnet_type::NORMAL:
                                case inml::magnet_type::INVERTER_MAGNET:
                                {
                                    hump.push_back(c);
                                    break;
                                }
                                // interesting branch: could be a hump
                                case inml::magnet_type::EMPTY:
                                case inml::magnet_type::OUTPUT:
                                {
                                    handle(hump);
                                    // discard hump cells and start searching again
                                    hump.clear();
                                    st = status::SEARCH;
                                    break;
                                }
                                // encountered anything else: cannot be a hump
                                default:
                                {
                                    hump.clear();
                                    st = status::SKIP;
                                    break;
                                }
                            }
                            break;
                        }
                        case status::SKIP:
                        {
                            if (const auto t = lyt.get_cell_type(c); t == inml::magnet_type::EMPTY)
                            {
                                // skipping over, return to searching
                                st = status::SEARCH;
                            }
                            break;
                        }
                        break;  // silence compiler warning
                    }
                }
            }
        } while (improvement_found);
    }

  private:
    /**
     * Checks whether the given node has an AND, OR, or MAJ fanin node.
     *
     * @tparam Lyt Gate-level layout type.
     * @param lyt The layout to check.
     * @param n Node whose fanins are to be considered.
     * @return `true` iff `n` has an AND, OR, or MAJ fanin node.
     */
    template <typename Lyt>
    [[nodiscard]] static bool has_and_or_maj_fanin(const Lyt& lyt, const typename Lyt::object_id n) noexcept
    {
        auto pre_and_or_maj = false;

        lyt.foreach_fanin(n,
                          [&lyt, &pre_and_or_maj](const auto& fi)
                          {
                              const auto fin = fi.object;

                              if (lyt.is_and(fin))
                              {
                                  pre_and_or_maj = true;
                                  return false;  // exit function
                              }

                              if (lyt.is_or(fin))
                              {
                                  pre_and_or_maj = true;
                                  return false;  // exit function
                              }

                              if (lyt.is_maj(fin))
                              {
                                  pre_and_or_maj = true;
                                  return false;  // exit function
                              }

                              return true;  // continue iteration
                          });

        return pre_and_or_maj;
    }
    /**
     * Checks whether the given node has an fanout node as fanout.
     *
     * @tparam Lyt Gate-level layout type.
     * @param lyt The layout to check.
     * @param n Node whose fanouts are to be considered.
     * @return `true` iff `n` has a fanout node as fanout.
     */
    template <typename Lyt>
    [[nodiscard]] static bool has_fanout_fanout(const Lyt& lyt, const typename Lyt::object_id n) noexcept
    {
        auto fanout_fanout = false;

        lyt.foreach_fanout(n,
                           [&lyt, &fanout_fanout](const auto& fon)
                           {
                               if (lyt.is_fanout(fon))
                               {
                                   fanout_fanout = true;
                                   return false;  // exit function
                               }

                               return true;  // continue iteration
                           });

        return fanout_fanout;
    }

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

        const auto object = lyt.find_object(t);
        if (!object)
        {
            throw fcn::unsupported_gate_type_exception(t);
        }
        const auto n = *object;

        // Different physical directions share connector positions in the ToPoliNano gate blocks.
        // NOLINTBEGIN(bugprone-branch-clone)

        // wires within the circuit
        if (lyt.is_buf(n) && !lyt.is_pi(n) && !lyt.is_po(n))
        {
            // inputs
            if (lyt.has_northern_incoming_signal(t))
            {
                p.inp.emplace(0u, 0u);
            }
            else if (lyt.has_southern_incoming_signal(t))
            {
                p.inp.emplace(0u, 3u);
            }
            else if (lyt.has_north_western_incoming_signal(t))
            {
                p.inp.emplace(0u, 0u);
            }
            else if (lyt.has_south_western_incoming_signal(t))
            {
                // special case: if predecessor is AND, OR, MAJ, input port is at (0,3)
                if (has_and_or_maj_fanin(lyt, n))
                {
                    p.inp.emplace(0u, 3u);
                }
                else
                {
                    p.inp.emplace(0u, 2u);
                }
            }
            else
            {
                p.inp.emplace(0u, 0u);
            }

            // outputs
            if (lyt.has_northern_outgoing_signal(t))
            {
                // special case: if northern tile is MAJ, output port is at (1,0)
                const auto north           = lyt.north(t);
                const auto northern_object = north ? lyt.find_object(*north) : std::nullopt;
                if (northern_object && lyt.is_maj(*northern_object))
                {
                    p.out.emplace(1u, 0u);
                }
                else
                {
                    p.out.emplace(3u, 0u);
                }
            }
            else if (lyt.has_southern_outgoing_signal(t))
            {
                p.out.emplace(3u, 3u);
            }
            else if (lyt.has_north_eastern_outgoing_signal(t))
            {
                p.out.emplace(3u, 0u);
            }
            else if (lyt.has_south_eastern_outgoing_signal(t))
            {
                // special case: if successor is a fanout, output port is at (3,3)
                if (has_fanout_fanout(lyt, n))
                {
                    p.out.emplace(3u, 3u);
                }
                else
                {
                    p.out.emplace(3u, 2u);
                }
            }
            else
            {
                p.out.emplace(3u, 2u);
            }
        }
        else  // PI, PO, etc.
        {
            // special case: PO determines output according to its predecessor
            if (lyt.is_po(n))
            {
                // if predecessor is an AND/OR/MAJ gate, input port is at (0,3) and output port at (3,3)
                if (has_and_or_maj_fanin(lyt, n))
                {
                    // but only if that gate is SW of the PO
                    if (lyt.has_south_western_incoming_signal(t))
                    {
                        p.inp.emplace(0u, 3u);
                        p.out.emplace(3u, 3u);

                        return p;
                    }
                }
                // if input is north-western, output port is at (3,0)
                else if (lyt.has_north_western_incoming_signal(t))
                {
                    p.out.emplace(3u, 0u);
                }
                // if input is south-western, output port is at (3,2)
                else if (lyt.has_south_western_incoming_signal(t))
                {
                    p.out.emplace(3u, 2u);
                }
            }

            if (lyt.has_north_western_incoming_signal(t))
            {
                p.inp.emplace(0u, 0u);
            }
            if (lyt.has_south_western_incoming_signal(t))
            {
                if (lyt.is_po(n) || lyt.is_inv(n))
                {
                    // special case: if predecessor is AND, OR, MAJ, input port is at (0,3)
                    if (has_and_or_maj_fanin(lyt, n))
                    {
                        p.inp.emplace(0u, 3u);
                    }
                    else
                    {
                        p.inp.emplace(0u, 2u);
                    }
                }
                else
                {
                    p.inp.emplace(0u, 2u);
                }
            }
            // special case: PI determines input according to successor
            if (lyt.is_pi(n))
            {
                // if successor is a fan-out, input port is at (0,3) and output port at (3,3)
                if (has_fanout_fanout(lyt, n))
                {
                    p.inp.emplace(0u, 3u);
                    p.out.emplace(3u, 3u);

                    return p;
                }
                // if output is north-eastern, input port is at (0,0)
                if (lyt.has_north_eastern_outgoing_signal(t))
                {
                    p.inp.emplace(0u, 0u);
                }
                // if output is south-eastern, input port is at (0,2)
                else if (lyt.has_south_eastern_outgoing_signal(t))
                {
                    p.inp.emplace(0u, 2u);
                }
            }
            // determine outgoing connector ports
            if (lyt.has_north_eastern_outgoing_signal(t))
            {
                // special case: output port of AND, OR, MAJ is fixed at pos (3,1)
                if (lyt.is_and(n) || lyt.is_or(n) || lyt.is_maj(n))
                {
                    p.out.emplace(3u, 1u);
                }
                else
                {
                    p.out.emplace(3u, 0u);
                }
            }
            if (lyt.has_south_eastern_outgoing_signal(t))
            {
                p.out.emplace(3u, 2u);
            }
        }

        // NOLINTEND(bugprone-branch-clone)

        return p;
    }

    static fcn::port_position opposite(const fcn::port_position& p)
    {
        using port_port_map = phmap::flat_hash_map<fcn::port_position, fcn::port_position>;

        static const port_port_map pp_map = {
            {fcn::port_position(0, 0), fcn::port_position(3, 0)}, {fcn::port_position(0, 1), fcn::port_position(3, 1)},
            {fcn::port_position(0, 2), fcn::port_position(3, 2)}, {fcn::port_position(0, 3), fcn::port_position(3, 3)},
            {fcn::port_position(3, 0), fcn::port_position(0, 0)}, {fcn::port_position(3, 1), fcn::port_position(0, 1)},
            {fcn::port_position(3, 2), fcn::port_position(0, 2)}, {fcn::port_position(3, 3), fcn::port_position(0, 3)},
        };

        return pp_map.at(p);
    }

    // clang-format off

    // ************************************************************
    // ************************** Gates ***************************
    // ************************************************************

    static constexpr const gate CONJUNCTION{cell_list_to_gate<char>(
    {{
        {'d', ' ', ' ', ' '},
        {'d', 'x', 'x', 'x'},
        {'d', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate DISJUNCTION{cell_list_to_gate<char>(
    {{
        {'u', ' ', ' ', ' '},
        {'u', 'x', 'x', 'x'},
        {'u', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate MAJORITY{cell_list_to_gate<char>(
    {{
        {'x', 'x', ' ', ' '},
        {' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {' ', 'x', ' ', ' '}
    }})};

    static constexpr const gate LOWER_STRAIGHT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate BOTTOM_LOWER_STRAIGHT_INVERTER{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const gate UPPER_STRAIGHT_INVERTER{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate TOP_DOWN_BENT_INVERTER{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate BOTTOM_UP_BENT_INVERTER{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate BOTTOM_LOWER_UP_BENT_INVERTER{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '}
    }})};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    static constexpr const gate CROSSWIRE{cell_list_to_gate<char>(
    {{
        {'c', ' ', 'c', 'x'},
        {' ', 'c', ' ', ' '},
        {'c', ' ', 'c', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate COUPLER{cell_list_to_gate<char>(
    {{
        {'f', 'f', 'x', 'x'},
        {'f', ' ', ' ', ' '},
        {'f', 'f', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate LOWER_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate UPPER_WIRE{cell_list_to_gate<char>(
    {{
        {'x', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate TOP_DOWN_BENT_WIRE{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate BOTTOM_UP_BENT_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'x'},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate TOP_DOWN_STAIRCASE_WIRE{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', ' '},
        {' ', ' ', 'x', 'x'}
    }})};

    static constexpr const gate BOTTOM_UP_STAIRCASE_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', 'x'},
        {' ', ' ', 'x', ' '},
        {'x', 'x', 'x', ' '},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const gate BOTTOM_DOWN_BENT_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', 'x'}
    }})};

    static constexpr const gate MAJORITY_WIRE{cell_list_to_gate<char>(
    {{
        {'x', 'x', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const gate COUPLER_WIRE{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '}
    }})};

    // clang-format on

    using port_gate_map = phmap::flat_hash_map<fcn::port_list<fcn::port_position>, gate>;

    static inline const port_gate_map WIRE_MAP = {
        // straight wires
        {{{fcn::port_position(0, 2)}, {fcn::port_position(3, 2)}}, LOWER_WIRE},
        {{{}, {fcn::port_position(3, 2)}}, LOWER_WIRE},
        {{{fcn::port_position(0, 2)}, {}}, LOWER_WIRE},
        {{{fcn::port_position(0, 0)}, {fcn::port_position(3, 0)}}, UPPER_WIRE},
        {{{}, {fcn::port_position(3, 0)}}, UPPER_WIRE},
        {{{fcn::port_position(0, 0)}, {}}, UPPER_WIRE},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(3, 3)}}, rotate_180(UPPER_WIRE)},
        {{{}, {fcn::port_position(3, 3)}}, rotate_180(UPPER_WIRE)},
        {{{fcn::port_position(0, 3)}, {}}, rotate_180(UPPER_WIRE)},
        // bent wires
        {{{fcn::port_position(0, 0)}, {fcn::port_position(3, 2)}}, TOP_DOWN_BENT_WIRE},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(3, 0)}}, BOTTOM_UP_BENT_WIRE},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(3, 3)}}, BOTTOM_DOWN_BENT_WIRE},
        // staircase wires
        {{{fcn::port_position(0, 0)}, {fcn::port_position(3, 3)}}, TOP_DOWN_STAIRCASE_WIRE},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(3, 0)}}, BOTTOM_UP_STAIRCASE_WIRE},
        // special wires
        {{{fcn::port_position(0, 0)}, {fcn::port_position(1, 0)}}, MAJORITY_WIRE},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(3, 2)}}, COUPLER_WIRE}
        // NOTE more wires go here!
    };
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map INVERTER_MAP = {
        // straight inverters
        {{{fcn::port_position(0, 2)}, {fcn::port_position(3, 2)}}, LOWER_STRAIGHT_INVERTER},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(3, 2)}}, BOTTOM_LOWER_STRAIGHT_INVERTER},
        {{{fcn::port_position(0, 0)}, {fcn::port_position(3, 0)}}, UPPER_STRAIGHT_INVERTER},
        // without outputs
        {{{fcn::port_position(0, 2)}, {}}, LOWER_STRAIGHT_INVERTER},
        {{{fcn::port_position(0, 0)}, {}}, UPPER_STRAIGHT_INVERTER},
        // without inputs
        {{{}, {fcn::port_position(3, 2)}}, LOWER_STRAIGHT_INVERTER},
        {{{}, {fcn::port_position(3, 0)}}, UPPER_STRAIGHT_INVERTER},
        // bent inverters
        {{{fcn::port_position(0, 0)}, {fcn::port_position(3, 2)}}, TOP_DOWN_BENT_INVERTER},
        {{{fcn::port_position(0, 2)}, {fcn::port_position(3, 0)}}, BOTTOM_UP_BENT_INVERTER},
        {{{fcn::port_position(0, 3)}, {fcn::port_position(3, 0)}}, BOTTOM_LOWER_UP_BENT_INVERTER}};
};
}  // namespace fiction::inml
