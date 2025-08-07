//
// Created by benjamin on 7/21/25.
//

#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/array_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <phmap.h>

#include <vector>

namespace fiction
{

/**
 * A concrete FCN gate library based on QCA ONE proposed in \"A Methodology for Standard Cell Design for QCA\" by Dayane
 * Alfenas Reis, Caio Araújo T. Campos, Thiago Rodrigues B. S. Soares, Omar Paranaiba V. Neto, and Frank Sill Torres in
 * IEEE International Symposium on Circuits and Systems, 2016. QCA ONE was originally proposed for the USE clocking
 * scheme. The version used here is an extension to the original QCA ONE by also theoretically allowing multiple wires
 * in the same tile. Furthermore, it can be used for a range of clocking schemes. Tiles in QCA ONE are \f$5 \times 5\f$
 * QCA cells.
 */
class seven_library : public fcn_gate_library<qca_technology, 7, 7>
{
  public:
    explicit seven_library() = delete;
    /**
     * Overrides the corresponding function in fcn_gate_library. Given a tile `t`, this function takes all necessary
     * information from the stored grid into account to choose the correct fcn_gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @tparam GateLyt Cartesian gate-level layout type.
     * @param lyt Layout that hosts tile `t`.
     * @param t Tile to be realized as a QCA ONE gate.
     * @return QCA ONE gate representation of `t` including I/Os, rotation, const cells, etc.
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
                    if (lyt.fanout_size(n) == 3)
                    {
                        return FAN_OUT_1_3;
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
                    return MAJORITY;
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
                            lyt.assign_cell_mode(c, qca_technology::cell_mode::VERTICAL);
                            // create a corresponding via ground cell
                            const cell<CellLyt> ground_via_cell{c.x, c.y, 0};
                            lyt.assign_cell_type(ground_via_cell, qca_technology::cell_type::NORMAL);
                            lyt.assign_cell_mode(ground_via_cell, qca_technology::cell_mode::VERTICAL);
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

        // determine incoming connector ports
        if (lyt.has_northern_incoming_signal(t))
        {
            p.inp.emplace(3u, 0u);
        }
        if (lyt.has_eastern_incoming_signal(t))
        {
            p.inp.emplace(6u, 3u);
        }
        if (lyt.has_southern_incoming_signal(t))
        {
            p.inp.emplace(3u, 6u);
        }
        if (lyt.has_western_incoming_signal(t))
        {
            p.inp.emplace(0u, 3u);
        }

        // determine outgoing connector ports
        if (lyt.has_northern_outgoing_signal(t))
        {
            p.out.emplace(3u, 0u);
        }
        if (lyt.has_eastern_outgoing_signal(t))
        {
            p.out.emplace(6u, 3u);
        }
        if (lyt.has_southern_outgoing_signal(t))
        {
            p.out.emplace(3u, 6u);
        }
        if (lyt.has_western_outgoing_signal(t))
        {
            p.out.emplace(0u, 3u);
        }

        // has no connector ports (fallbacks)
        if (const auto n = lyt.get_node(t); !lyt.is_wire(n) && !lyt.is_inv(n))
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(0u, 3u);
            }

            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(6u, 3u);
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
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', 'x', 'x', 'x', ' ', ' '},
{' ', ' ', 'x', ' ', 'x', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};


    static constexpr const fcn_gate BENT_INVERTER{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', 'x', 'x', 'x'},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '}
}})};

   static constexpr const fcn_gate CONJUNCTION{cell_list_to_gate<char>(
{{
 {' ', ' ', ' ', '0', ' ', ' ', ' '},
 {' ', ' ', ' ', 'x', ' ', ' ', ' '},
 {' ', ' ', ' ', 'x', ' ', ' ', ' '},
 {'x', 'x', 'x', 'x', 'x', 'x', 'x'},
 {' ', ' ', ' ', 'x', ' ', ' ', ' '},
 {' ', ' ', ' ', 'x', ' ', ' ', ' '},
 {' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};

    static constexpr const fcn_gate DISJUNCTION{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', '1', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{'x', 'x', 'x', 'x', 'x', 'x', 'x'},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};

    static constexpr const fcn_gate MAJORITY{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{'x', 'x', 'x', 'x', 'x', 'x', 'x'},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};

    static constexpr const fcn_gate FAN_OUT_1_2{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{'x', 'x', 'x', 'x', 'x', 'x', 'x'},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};

    static constexpr const fcn_gate FAN_OUT_1_3{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{'x', 'x', 'x', 'x', 'x', 'x', 'x'},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}})};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    static constexpr const fcn_gate PRIMARY_INPUT_PORT = cell_list_to_gate<char>(
{{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'i', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '}
}});

static constexpr const fcn_gate PRIMARY_OUTPUT_PORT = cell_list_to_gate<char>(
{{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'o', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '}
}});

    static constexpr const fcn_gate CENTER_WIRE = cell_list_to_gate<char>(
{{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '}
}});

    static constexpr const fcn_gate CENTER_BENT_WIRE{cell_list_to_gate<char>(
    {{
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', ' ', ' ', ' '},
{' ', ' ', ' ', 'x', 'x', 'x', 'x'},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '},
{' ', ' ', ' ', ' ', ' ', ' ', ' '}
}})};

    // clang-format on

    using port_gate_map = phmap::flat_hash_map<port_list<port_position>, fcn_gate>;
    /**
     * Lookup table for wire rotations. Maps ports to corresponding wires.
     */
    static inline const port_gate_map WIRE_MAP = {
        // primary inputs
        {{{}, {port_position(3, 0)}}, PRIMARY_INPUT_PORT},
        {{{}, {port_position(6, 3)}}, rotate_90(PRIMARY_INPUT_PORT)},
        {{{}, {port_position(3, 6)}}, rotate_180(PRIMARY_INPUT_PORT)},
        {{{}, {port_position(0, 3)}}, rotate_270(PRIMARY_INPUT_PORT)},
        // primary outputs
        {{{port_position(3, 0)}, {}}, PRIMARY_OUTPUT_PORT},
        {{{port_position(6, 3)}, {}}, rotate_90(PRIMARY_OUTPUT_PORT)},
        {{{port_position(3, 6)}, {}}, rotate_180(PRIMARY_OUTPUT_PORT)},
        {{{port_position(0, 3)}, {}}, rotate_270(PRIMARY_OUTPUT_PORT)},
        // center wire
        {{{port_position(3, 0)}, {port_position(3, 6)}}, CENTER_WIRE},
        {{{port_position(3, 6)}, {port_position(3, 0)}}, CENTER_WIRE},
        {{{port_position(0, 3)}, {port_position(6, 3)}}, rotate_90(CENTER_WIRE)},
        {{{port_position(6, 3)}, {port_position(0, 3)}}, rotate_90(CENTER_WIRE)},
        // center bent wire
        {{{port_position(3, 0)}, {port_position(6, 3)}}, CENTER_BENT_WIRE},
        {{{port_position(6, 3)}, {port_position(3, 0)}}, CENTER_BENT_WIRE},
        {{{port_position(6, 3)}, {port_position(3, 6)}}, rotate_90(CENTER_BENT_WIRE)},
        {{{port_position(3, 6)}, {port_position(6, 3)}}, rotate_90(CENTER_BENT_WIRE)},
        {{{port_position(0, 3)}, {port_position(3, 6)}}, rotate_180(CENTER_BENT_WIRE)},
        {{{port_position(3, 6)}, {port_position(0, 3)}}, rotate_180(CENTER_BENT_WIRE)},
        {{{port_position(3, 0)}, {port_position(0, 3)}}, rotate_270(CENTER_BENT_WIRE)},
        {{{port_position(0, 3)}, {port_position(3, 0)}}, rotate_270(CENTER_BENT_WIRE)}
        // TODO more wires go here!
    };
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map INVERTER_MAP = {
        // straight inverters
        {{{port_position(3, 0)}, {port_position(3, 6)}}, STRAIGHT_INVERTER},
        {{{port_position(6, 3)}, {port_position(0, 3)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{port_position(3, 6)}, {port_position(3, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{port_position(0, 3)}, {port_position(6, 3)}}, rotate_270(STRAIGHT_INVERTER)},
        // without outputs
        {{{port_position(3, 0)}, {}}, STRAIGHT_INVERTER},
        {{{port_position(6, 3)}, {}}, rotate_90(STRAIGHT_INVERTER)},
        {{{port_position(3, 6)}, {}}, rotate_180(STRAIGHT_INVERTER)},
        {{{port_position(0, 3)}, {}}, rotate_270(STRAIGHT_INVERTER)},
        // without inputs
        {{{}, {port_position(3, 6)}}, STRAIGHT_INVERTER},
        {{{}, {port_position(0, 3)}}, rotate_90(STRAIGHT_INVERTER)},
        {{{}, {port_position(3, 0)}}, rotate_180(STRAIGHT_INVERTER)},
        {{{}, {port_position(6, 3)}}, rotate_270(STRAIGHT_INVERTER)},
        // bent inverters
        {{{port_position(3, 0)}, {port_position(6, 3)}}, BENT_INVERTER},
        {{{port_position(6, 3)}, {port_position(3, 0)}}, BENT_INVERTER},
        {{{port_position(6, 3)}, {port_position(3, 6)}}, rotate_90(BENT_INVERTER)},
        {{{port_position(3, 6)}, {port_position(6, 3)}}, rotate_90(BENT_INVERTER)},
        {{{port_position(0, 3)}, {port_position(3, 6)}}, rotate_180(BENT_INVERTER)},
        {{{port_position(3, 6)}, {port_position(0, 3)}}, rotate_180(BENT_INVERTER)},
        {{{port_position(3, 0)}, {port_position(0, 3)}}, rotate_270(BENT_INVERTER)},
        {{{port_position(0, 3)}, {port_position(3, 0)}}, rotate_270(BENT_INVERTER)}
    };

    /**
     * Lookup table for conjunction rotations. Maps ports to corresponding AND gates.
     */
    static inline const port_gate_map CONJUNCTION_MAP = {
        // 0° rotation
        {{{port_position(0, 3), port_position(3, 6)}, {port_position(6, 3)}}, CONJUNCTION},
        {{{port_position(0, 3), port_position(6, 3)}, {port_position(3, 6)}}, CONJUNCTION},
        {{{port_position(3, 6), port_position(6, 3)}, {port_position(0, 3)}}, CONJUNCTION},

        // 90° rotation
        {{{port_position(0, 3), port_position(3, 6)}, {port_position(3, 0)}}, rotate_90(CONJUNCTION)},
        {{{port_position(0, 3), port_position(3, 0)}, {port_position(3, 6)}}, rotate_90(CONJUNCTION)},
        {{{port_position(3, 6), port_position(3, 0)}, {port_position(0, 3)}}, rotate_90(CONJUNCTION)},

        // 180° rotation
        {{{port_position(0, 3), port_position(3, 0)}, {port_position(6, 3)}}, rotate_180(CONJUNCTION)},
        {{{port_position(0, 3), port_position(6, 3)}, {port_position(3, 0)}}, rotate_180(CONJUNCTION)},
        {{{port_position(6, 3), port_position(3, 0)}, {port_position(0, 3)}}, rotate_180(CONJUNCTION)},

        // 270° rotation
        {{{port_position(3, 6), port_position(6, 3)}, {port_position(3, 0)}}, rotate_270(CONJUNCTION)},
        {{{port_position(3, 6), port_position(3, 0)}, {port_position(6, 3)}}, rotate_270(CONJUNCTION)},
        {{{port_position(6, 3), port_position(3, 0)}, {port_position(3, 6)}}, rotate_270(CONJUNCTION)}
    };

    /**
     * Lookup table for disjunction rotations. Maps ports to corresponding OR gates.
     */
    static inline const port_gate_map DISJUNCTION_MAP = {
        // 0° rotation
        {{{port_position(0, 3), port_position(3, 6)}, {port_position(6, 3)}}, DISJUNCTION},
        {{{port_position(0, 3), port_position(6, 3)}, {port_position(3, 6)}}, DISJUNCTION},
        {{{port_position(3, 6), port_position(6, 3)}, {port_position(0, 3)}}, DISJUNCTION},

        // 90° rotation
        {{{port_position(0, 3), port_position(3, 6)}, {port_position(3, 0)}}, rotate_90(DISJUNCTION)},
        {{{port_position(0, 3), port_position(3, 0)}, {port_position(3, 6)}}, rotate_90(DISJUNCTION)},
        {{{port_position(3, 6), port_position(3, 0)}, {port_position(0, 3)}}, rotate_90(DISJUNCTION)},

        // 180° rotation
        {{{port_position(0, 3), port_position(3, 0)}, {port_position(6, 3)}}, rotate_180(DISJUNCTION)},
        {{{port_position(0, 3), port_position(6, 3)}, {port_position(3, 0)}}, rotate_180(DISJUNCTION)},
        {{{port_position(6, 3), port_position(3, 0)}, {port_position(0, 3)}}, rotate_180(DISJUNCTION)},

        // 270° rotation
        {{{port_position(3, 6), port_position(6, 3)}, {port_position(3, 0)}}, rotate_270(DISJUNCTION)},
        {{{port_position(3, 6), port_position(3, 0)}, {port_position(6, 3)}}, rotate_270(DISJUNCTION)},
        {{{port_position(6, 3), port_position(3, 0)}, {port_position(3, 6)}}, rotate_270(DISJUNCTION)}
    };

    /**
     * Lookup table for fan-out rotations. Maps ports to corresponding fan-out gates.
     */
    static inline const port_gate_map FANOUT_MAP = {
        // 0° (no rotation)
        {{{port_position(6, 3)}, {port_position(0, 3), port_position(3, 6)}}, FAN_OUT_1_2},
        {{{port_position(3, 6)}, {port_position(0, 3), port_position(6, 3)}}, FAN_OUT_1_2},
        {{{port_position(0, 3)}, {port_position(3, 6), port_position(6, 3)}}, FAN_OUT_1_2},

        // 90° rotation
        {{{port_position(3, 0)}, {port_position(0, 3), port_position(3, 6)}}, rotate_90(FAN_OUT_1_2)},
        {{{port_position(3, 6)}, {port_position(0, 3), port_position(3, 0)}}, rotate_90(FAN_OUT_1_2)},
        {{{port_position(0, 3)}, {port_position(3, 6), port_position(3, 0)}}, rotate_90(FAN_OUT_1_2)},

        // 180° rotation
        {{{port_position(3, 0)}, {port_position(0, 3), port_position(6, 3)}}, rotate_180(FAN_OUT_1_2)},
        {{{port_position(6, 3)}, {port_position(0, 3), port_position(3, 0)}}, rotate_180(FAN_OUT_1_2)},
        {{{port_position(0, 3)}, {port_position(6, 3), port_position(3, 0)}}, rotate_180(FAN_OUT_1_2)},

        // 270° rotation
        {{{port_position(3, 0)}, {port_position(3, 6), port_position(6, 3)}}, rotate_270(FAN_OUT_1_2)},
        {{{port_position(6, 3)}, {port_position(3, 6), port_position(3, 0)}}, rotate_270(FAN_OUT_1_2)},
        {{{port_position(3, 6)}, {port_position(6, 3), port_position(3, 0)}}, rotate_270(FAN_OUT_1_2)}
    };

};

}  // namespace fiction