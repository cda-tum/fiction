//
// Created by marcel on 29.07.19.
//

#ifndef FICTION_INML_TOPOLINANO_LIBRARY_HPP
#define FICTION_INML_TOPOLINANO_LIBRARY_HPP

#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/array_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>

#include <unordered_map>
#include <vector>

namespace fiction
{
/**
 * A concrete FCN gate library as used in "ToPoliNano" (https://topolinano.polito.it/) for the iNML technology. In
 * fiction, this is emulated by using vertically shifted layouts and implementing the ToPoliNano library with 4 x 4
 * magnet positions with one empty row in most tiles (except for MAJ which needs to be handled differently as this
 * library is not uniform otherwise. Theoretically, it allows for multiple wires in the same tile.
 */
class inml_topolinano_library : public fcn_gate_library<inml_technology, 4, 4>
{
  public:
    explicit inml_topolinano_library() = delete;
    /**
     * Overrides the corresponding function in fcn_gate_library. Given a tile t, this function takes all
     * necessary information from the stored grid into account to choose the correct fcn_gate representation for
     * that tile. May it be a gate or wires. Rotation and special marks like input and output, const cells etc.
     * are computed additionally.
     *
     * @tparam Lyt Gate-level layout type.
     * @param lyt Gate-level layout that hosts tile t.
     * @param t Tile to be realized in the ToPoliNano library.
     * @return ToPoliNano gate representation of t including I/Os, rotation, etc.
     */
    template <typename Lyt>
    [[nodiscard]] static fcn_gate set_up_gate(const Lyt& lyt, const tile<Lyt>& t)
    {
        static_assert(is_gate_level_layout_v<Lyt>, "Lyt must be a gate-level layout");

        const auto n = lyt.get_node(t);

        if constexpr (fiction::has_is_fanout_v<Lyt>)
        {
            if (lyt.is_fanout(n))
            {
                return coupler;
            }
        }
        if constexpr (mockturtle::has_is_and_v<Lyt>)
        {
            if (lyt.is_and(n))
            {
                return conjunction;
            }
        }
        if constexpr (mockturtle::has_is_or_v<Lyt>)
        {
            if (lyt.is_or(n))
            {
                return disjunction;
            }
        }
        if constexpr (mockturtle::has_is_maj_v<Lyt>)
        {
            if (lyt.is_maj(n))
            {
                return majority;
            }
        }

        const auto p = determine_port_routing(lyt, t);

        try
        {
            if constexpr (fiction::has_is_inv_v<Lyt>)
            {
                if (lyt.is_inv(n))
                {
                    return inverter_map.at(p);
                }
            }
            if constexpr (fiction::has_is_buf_v<Lyt>)
            {
                if (lyt.is_buf(n))
                {
                    // crossing case
                    if (lyt.is_wire_tile(lyt.above(t)))
                    {
                        return crosswire;
                    }

                    auto wire = wire_map.at(p);

                    if (lyt.is_pi(n))
                    {
                        const auto inp_mark_pos = p.inp.empty() ? opposite(*p.out.begin()) : *p.inp.begin();
                        wire = mark_cell(wire, inp_mark_pos, inml_technology::cell_mark::INPUT);
                    }
                    if (lyt.is_po(n))
                    {
                        const auto out_mark_pos = p.out.empty() ? opposite(*p.inp.begin()) : *p.out.begin();
                        wire = mark_cell(wire, out_mark_pos, inml_technology::cell_mark::OUTPUT);
                    }

                    return wire;
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
     * Checks whether the given node has an AND, OR, or MAJ fanin node.
     *
     * @tparam Lyt Gate-level layout type.
     * @param lyt The layout to check.
     * @param n Node whose fanins are to be considered.
     * @return True iff n has an AND, OR, or MAJ fanin node.
     */
    template <typename Lyt>
    [[nodiscard]] static bool has_and_or_maj_fanin(const Lyt& lyt, const mockturtle::node<Lyt>& n) noexcept
    {
        auto pre_and_or_maj = false;

        lyt.foreach_fanin(n,
                          [&lyt, &pre_and_or_maj](const auto& fi)
                          {
                              const auto fin = lyt.get_node(fi);

                              if constexpr (mockturtle::has_is_and_v<Lyt>)
                              {
                                  if (lyt.is_and(fin))
                                  {
                                      pre_and_or_maj = true;
                                      return false;  // exit function
                                  }
                              }
                              if constexpr (mockturtle::has_is_or_v<Lyt>)
                              {
                                  if (lyt.is_or(fin))
                                  {
                                      pre_and_or_maj = true;
                                      return false;  // exit function
                                  }
                              }
                              if constexpr (mockturtle::has_is_maj_v<Lyt>)
                              {
                                  if (lyt.is_maj(fin))
                                  {
                                      pre_and_or_maj = true;
                                      return false;  // exit function
                                  }
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
     * @return True iff n has a fanout node as fanout.
     */
    template <typename Lyt>
    [[nodiscard]] static bool has_fanout_fanout(const Lyt& lyt, const mockturtle::node<Lyt>& n) noexcept
    {
        auto fanout_fanout = false;

        lyt.foreach_fanout(n,
                           [&lyt, &fanout_fanout](const auto& fon)
                           {
                               if constexpr (fiction::has_is_fanout_v<Lyt>)
                               {
                                   if (lyt.is_fanout(fon))
                                   {
                                       fanout_fanout = true;
                                       return false;  // exit function
                                   }
                               }
                               return true;  // continue iteration
                           });

        return fanout_fanout;
    }

    template <typename Lyt>
    [[nodiscard]] static port_list<port_position> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t) noexcept
    {
        static_assert(fiction::has_is_inv_v<Lyt>, "Lyt must implement the is_inv function");
        static_assert(fiction::has_is_po_v<Lyt>, "Lyt must implement the is_po function");
        static_assert(mockturtle::has_is_pi_v<Lyt>, "Lyt must implement the is_pi function");
        static_assert(mockturtle::has_is_and_v<Lyt>, "Lyt must implement the is_and function");
        static_assert(mockturtle::has_is_or_v<Lyt>, "Lyt must implement the is_or function");
        static_assert(mockturtle::has_is_maj_v<Lyt>, "Lyt must implement the is_maj function");

        port_list<port_position> p{};

        const auto n = lyt.get_node(t);

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
                if (lyt.is_maj(lyt.get_node(lyt.north(t))))
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
            // TODO this might not be necessary since MAJ does not need to access the port map
            // special case: MAJ needs to have an input at the bottom
            if constexpr (mockturtle::has_is_maj_v<Lyt>)
            {
                if (lyt.is_maj(n) && lyt.has_northern_incoming_signal(t))
                {
                    p.inp.emplace(1u, 3u);
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
                else if (lyt.has_north_eastern_outgoing_signal(t))
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

        return p;
    }

    static port_position opposite(const port_position& p)
    {
        using port_port_map = std::unordered_map<port_position, port_position>;

        static const port_port_map pp_map = {
            {port_position(0, 0), port_position(3, 0)}, {port_position(0, 1), port_position(3, 1)},
            {port_position(0, 2), port_position(3, 2)}, {port_position(0, 3), port_position(3, 3)},
            {port_position(3, 0), port_position(0, 0)}, {port_position(3, 1), port_position(0, 1)},
            {port_position(3, 2), port_position(0, 2)}, {port_position(3, 3), port_position(0, 3)},
        };

        return pp_map.at(p);
    }

    // clang-format off

    // ************************************************************
    // ************************** Gates ***************************
    // ************************************************************

    static constexpr const fcn_gate conjunction{cell_list_to_gate<char>(
    {{
        {'d', ' ', ' ', ' '},
        {'d', 'x', 'x', 'x'},
        {'d', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate disjunction{cell_list_to_gate<char>(
    {{
        {'u', ' ', ' ', ' '},
        {'u', 'x', 'x', 'x'},
        {'u', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate majority{cell_list_to_gate<char>(
    {{
        {'x', 'x', ' ', ' '},
        {' ', 'x', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {' ', 'x', ' ', ' '}
    }})};

    static constexpr const fcn_gate lower_straight_inverter{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate bottom_lower_straight_inverter{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate upper_straight_inverter{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate top_down_bent_inverter{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {'n', 'n', 'n', 'n'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate bottom_up_bent_inverter{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate bottom_lower_up_bent_inverter{cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '}
    }})};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    static constexpr const fcn_gate crosswire{cell_list_to_gate<char>(
    {{
        {'c', ' ', 'c', 'x'},
        {' ', 'c', ' ', ' '},
        {'c', ' ', 'c', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate coupler{cell_list_to_gate<char>(
    {{
        {'f', 'f', 'x', 'x'},
        {'f', ' ', ' ', ' '},
        {'f', 'f', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate lower_wire{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate upper_wire{cell_list_to_gate<char>(
    {{
        {'x', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate top_down_bent_wire{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate bottom_up_bent_wire{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'x'},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate top_down_staircase_wire{cell_list_to_gate<char>(
    {{
        {'x', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', ' '},
        {' ', ' ', 'x', 'x'}
    }})};

    static constexpr const fcn_gate bottom_up_staircase_wire{cell_list_to_gate<char>(
    {{
        {' ', ' ', 'x', 'x'},
        {' ', ' ', 'x', ' '},
        {'x', 'x', 'x', ' '},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate bottom_down_bent_wire{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', ' '},
        {' ', ' ', 'x', 'x'}
    }})};

    static constexpr const fcn_gate majority_wire{cell_list_to_gate<char>(
    {{
        {'x', 'x', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const fcn_gate coupler_wire{cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '}
    }})};

    // clang-format on

    using port_gate_map = std::unordered_map<port_list<port_position>, fcn_gate>;

    static inline const port_gate_map wire_map = {
        // straight wires
        {{{port_position(0, 2)}, {port_position(3, 2)}}, lower_wire},
        {{{}, {port_position(3, 2)}}, lower_wire},
        {{{port_position(0, 2)}, {}}, lower_wire},
        {{{port_position(0, 0)}, {port_position(3, 0)}}, upper_wire},
        {{{}, {port_position(3, 0)}}, upper_wire},
        {{{port_position(0, 0)}, {}}, upper_wire},
        {{{port_position(0, 3)}, {port_position(3, 3)}}, rotate_180(upper_wire)},
        {{{}, {port_position(3, 3)}}, rotate_180(upper_wire)},
        {{{port_position(0, 3)}, {}}, rotate_180(upper_wire)},
        // bent wires
        {{{port_position(0, 0)}, {port_position(3, 2)}}, top_down_bent_wire},
        {{{port_position(0, 2)}, {port_position(3, 0)}}, bottom_up_bent_wire},
        {{{port_position(0, 2)}, {port_position(3, 3)}}, bottom_down_bent_wire},
        // staircase wires
        {{{port_position(0, 0)}, {port_position(3, 3)}}, top_down_staircase_wire},
        {{{port_position(0, 3)}, {port_position(3, 0)}}, bottom_up_staircase_wire},
        // special wires
        {{{port_position(0, 0)}, {port_position(1, 0)}}, majority_wire},
        {{{port_position(0, 3)}, {port_position(3, 2)}}, coupler_wire}
        // TODO more wires go here!
    };
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map inverter_map = {
        // straight inverters
        {{{port_position(0, 2)}, {port_position(3, 2)}}, lower_straight_inverter},
        {{{port_position(0, 3)}, {port_position(3, 2)}}, bottom_lower_straight_inverter},
        {{{port_position(0, 0)}, {port_position(3, 0)}}, upper_straight_inverter},
        // without outputs
        {{{port_position(0, 2)}, {}}, lower_straight_inverter},
        {{{port_position(0, 0)}, {}}, upper_straight_inverter},
        // without inputs
        {{{}, {port_position(3, 2)}}, lower_straight_inverter},
        {{{}, {port_position(3, 0)}}, upper_straight_inverter},
        // bent inverters
        {{{port_position(0, 0)}, {port_position(3, 2)}}, top_down_bent_inverter},
        {{{port_position(0, 2)}, {port_position(3, 0)}}, bottom_up_bent_inverter},
        {{{port_position(0, 3)}, {port_position(3, 0)}}, bottom_lower_up_bent_inverter}};
};
}  // namespace fiction

#endif  // FICTION_INML_TOPOLINANO_LIBRARY_HPP
