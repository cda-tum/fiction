//
// Created by marcel on 27.07.18.
//

#ifndef FICTION_QCA_ONE_LIBRARY_H
#define FICTION_QCA_ONE_LIBRARY_H

#include "fcn_gate_library.h"


/**
 * A concrete FCN gate library based on QCA-ONE proposed in "A Methodology for Standard Cell Design for QCA" by Dayane
 * Alfenas Reis, Caio Araújo T. Campos, Thiago Rodrigues B. S. Soares, Omar Paranaiba V. Neto, and Frank Sill Torres in
 * IEEE International Symposium on Circuits and Systems, 2016. QCA-ONE was originally proposed for the USE clocking
 * scheme. The version used here is an extension to the original QCA-ONE by also theoretically allowing multiple wires
 * in the same tile. Furthermore, it can be used for a range of clocking schemes. Tiles in QCA-ONE are 5 x 5 QCA cells.
 */
class qca_one_library : public fcn_gate_library
{
public:
    /**
     * Default constructor. Initializes the library from a gate layout pointer. In the constructor, also a port_router
     * will be initialized. The port_router immediately tries to route all ports and throws an exception if it fails
     * doing so. That means, construction of a qca_one_library object can fail with an exception. This is intended!
     * See commands.h for details on usage.
     */
    qca_one_library(fcn_gate_layout_ptr fgl);
    /**
     * Default copy constructor.
     */
    qca_one_library(const qca_one_library& qol) noexcept = default;
    /**
     * Default move constructor.
     */
    qca_one_library(qca_one_library&& qol) noexcept = default;
    /**
     * Default destructor.
     */
    ~qca_one_library() override = default;
    /**
     * Default assignment operator.
     */
    qca_one_library& operator=(const qca_one_library& rhs) noexcept = default;
    /**
     * Default move operator.
     */
    qca_one_library& operator=(qca_one_library&& rhs) noexcept = default;
    /**
     * Overrides the corresponding function in fcn_gate_library. Given a tile t, this function takes all necessary
     * information from the stored grid into accout to choose the correct fcn_gate representation for that tile. May it
     * be a gate or wires. Rotation and special marks like input and output, const cells etc. are computed additionally.
     *
     * @param t Tile to be realized in QCA-ONE.
     * @return QCA-ONE gate representation of t including I/Os, rotation, cost cells, etc.
     */
    fcn_gate set_up_gate(const fcn_gate_layout::tile& t) override;

private:

    // ************************************************************
    // ************************** Gates ***************************
    // ************************************************************

    const fcn_gate inverter_straight =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', 'x', 'x', 'x', ' '},
         {' ', 'x', ' ', 'x', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate inverter_bent =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', ' ', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate conjunction =
    {{
         {' ', ' ', '0', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate disjunction =
    {{
         {' ', ' ', '1', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate majority =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate fan_out_1_2 =
    {{
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate fan_out_1_3 =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    // ************************************************************
    // ************************** Wires ***************************
    // ************************************************************

    const fcn_gate center_wire =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '}
    }};

    const fcn_gate inner_side_wire =
    {{
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '}
    }};

    const fcn_gate outer_side_wire =
    {{
         {' ', ' ', ' ', ' ', 'x'},
         {' ', ' ', ' ', ' ', 'x'},
         {' ', ' ', ' ', ' ', 'x'},
         {' ', ' ', ' ', ' ', 'x'},
         {' ', ' ', ' ', ' ', 'x'}
    }};

    const fcn_gate center_bent_wire =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate inner_center_to_inner_center_bent_wire =
    {{
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate inner_center_to_center_bent_wire =
    {{
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate inner_center_to_outer_center_bent_wire =
    {{
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate inner_center_to_outer_side_bent_wire =
    {{
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', ' '},
         {' ', ' ', ' ', 'x', 'x'}
    }};

    const fcn_gate center_to_inner_center_bent_wire =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate outer_center_to_center_bent_wire =
    {{
         {' ', 'x', ' ', ' ', ' '},
         {' ', 'x', ' ', ' ', ' '},
         {' ', 'x', 'x', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate outer_center_to_outer_center_bent_wire =
    {{
         {' ', 'x', ' ', ' ', ' '},
         {' ', 'x', ' ', ' ', ' '},
         {' ', 'x', ' ', ' ', ' '},
         {' ', 'x', 'x', 'x', 'x'},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate outer_side_to_outer_side_bent_wire =
    {{
         {'x', ' ', ' ', ' ', ' '},
         {'x', ' ', ' ', ' ', ' '},
         {'x', ' ', ' ', ' ', ' '},
         {'x', ' ', ' ', ' ', ' '},
         {'x', 'x', 'x', 'x', 'x'}
    }};

    const fcn_gate primary_input_port =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'i', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    const fcn_gate primary_output_port =
    {{
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'x', ' ', ' '},
         {' ', ' ', 'o', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '},
         {' ', ' ', ' ', ' ', ' '}
    }};

    /**
     * Shortcut for a list of ports.
     */
    using ports = port_router::port_list;
    /**
     * Shortcut for a single port.
     */
    using port = port_router::port;
    /**
     * Alias for a map of ports to fcn_gates. Used to identify rotation quickly.
     */
    using port_gate_map = std::unordered_map<ports, fcn_gate, boost::hash<ports>>;
    /**
     * Lookup table for wire rotations. Maps ports to corresponding wires.
     */
    port_gate_map wire_map =
    {
        // center wire
        {{{port(2, 0)}, {port(2, 4)}}, center_wire},
        {{{port(2, 4)}, {port(2, 0)}}, center_wire},
        {{{port(0, 2)}, {port(4, 2)}}, rotate_90(center_wire)},
        {{{port(4, 2)}, {port(0, 2)}}, rotate_90(center_wire)},
        // inner side wire
        {{{port(3, 0)}, {port(3, 4)}}, inner_side_wire},
        {{{port(3, 4)}, {port(3, 0)}}, inner_side_wire},
        {{{port(0, 3)}, {port(4, 3)}}, rotate_90(inner_side_wire)},
        {{{port(4, 3)}, {port(0, 3)}}, rotate_90(inner_side_wire)},
        {{{port(1, 0)}, {port(1, 4)}}, rotate_180(inner_side_wire)},
        {{{port(1, 4)}, {port(1, 0)}}, rotate_180(inner_side_wire)},
        {{{port(0, 1)}, {port(4, 1)}}, rotate_270(inner_side_wire)},
        {{{port(4, 1)}, {port(0, 1)}}, rotate_270(inner_side_wire)},
        // outer side wire
        {{{port(4, 0)}, {port(4, 4)}}, outer_side_wire},
        {{{port(4, 4)}, {port(4, 0)}}, outer_side_wire},
        {{{port(0, 4)}, {port(4, 4)}}, rotate_90(outer_side_wire)},
        {{{port(4, 4)}, {port(0, 4)}}, rotate_90(outer_side_wire)},
        {{{port(0, 0)}, {port(0, 4)}}, rotate_180(outer_side_wire)},
        {{{port(0, 4)}, {port(0, 0)}}, rotate_180(outer_side_wire)},
        {{{port(0, 0)}, {port(4, 0)}}, rotate_270(outer_side_wire)},
        {{{port(4, 0)}, {port(0, 0)}}, rotate_270(outer_side_wire)},
        // center bent wire
        {{{port(2, 0)}, {port(4, 2)}}, center_bent_wire},
        {{{port(4, 2)}, {port(2, 0)}}, center_bent_wire},
        {{{port(4, 2)}, {port(2, 4)}}, rotate_90(center_bent_wire)},
        {{{port(2, 4)}, {port(4, 2)}}, rotate_90(center_bent_wire)},
        {{{port(0, 2)}, {port(2, 4)}}, rotate_180(center_bent_wire)},
        {{{port(2, 4)}, {port(0, 2)}}, rotate_180(center_bent_wire)},
        {{{port(2, 0)}, {port(0, 2)}}, rotate_270(center_bent_wire)},
        {{{port(0, 2)}, {port(2, 0)}}, rotate_270(center_bent_wire)}
        // TODO more wires go here!
    };
    /**
     * Lookup table for inverter rotations. Maps ports to corresponding inverters.
     */
    port_gate_map inverter_map =
    {
        // straight inverters
        {{{port(2, 0)}, {port(2, 4)}}, inverter_straight},
        {{{port(4, 2)}, {port(0, 2)}}, rotate_90(inverter_straight)},
        {{{port(2, 4)}, {port(2, 0)}}, rotate_180(inverter_straight)},
        {{{port(0, 2)}, {port(4, 2)}}, rotate_270(inverter_straight)},
        // without outputs
        {{{port(2, 0)}, {}}, inverter_straight},
        {{{port(4, 2)}, {}}, rotate_90(inverter_straight)},
        {{{port(2, 4)}, {}}, rotate_180(inverter_straight)},
        {{{port(0, 2)}, {}}, rotate_270(inverter_straight)},
        // without inputs
        {{{}, {port(2, 4)}}, inverter_straight},
        {{{}, {port(0, 2)}}, rotate_90(inverter_straight)},
        {{{}, {port(2, 0)}}, rotate_180(inverter_straight)},
        {{{}, {port(4, 2)}}, rotate_270(inverter_straight)},
        // bent inverters
        {{{port(2, 0)}, {port(4, 2)}}, inverter_bent},
        {{{port(4, 2)}, {port(2, 0)}}, inverter_bent},
        {{{port(4, 2)}, {port(2, 4)}}, rotate_90(inverter_bent)},
        {{{port(2, 4)}, {port(4, 2)}}, rotate_90(inverter_bent)},
        {{{port(0, 2)}, {port(2, 4)}}, rotate_180(inverter_bent)},
        {{{port(2, 4)}, {port(0, 2)}}, rotate_180(inverter_bent)},
        {{{port(2, 0)}, {port(0, 2)}}, rotate_270(inverter_bent)},
        {{{port(0, 2)}, {port(2, 0)}}, rotate_270(inverter_bent)}
    };
    /**
     * Returns the port equivalent of the given direction. Only single directions are accepted.
     *
     * @param d Direction whose port is desired.
     * @return Port of direction d.
     */
    port dir_to_port(const layout::directions d) const;
    /**
     * Returns the opposite of a port. Only border ports are accepted.
     *
     * @param p Port whose opposite counterpart is desired.
     * @return Opposite port to p.
     */
    port opposite(const port& p) const;
    /**
     * Applies given mark to given fcn_gate g at given port p.
     *
     * @param g Gate to apply mark to.
     * @param p Port specifying where to apply the mark.
     * @param mark Mark to be applied
     * @return Marked fcn_gate.
     */
    fcn_gate mark_cell(const fcn_gate& g, const port& p, const fcn::cell_mark mark) const noexcept;
};


#endif //FICTION_QCA_ONE_LIBRARY_H
