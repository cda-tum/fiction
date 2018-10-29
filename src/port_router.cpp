//
// Created by marcel on 29.09.18.
//

#include "port_router.h"

port_router::port_router(fcn_gate_layout_ptr fgl, const fcn::technology t, const fcn::tile_size s)
        :
        layout{fgl},
        tech{t},
        size{s}
{}

void port_router::compute_ports()
{
    switch (tech)
    {
        case fcn::technology::QCA:
        {
            switch (size)
            {
                case fcn::tile_size::FIVE_X_FIVE:
                {
                    compute_qca_5x5_ports();
                    return;
                }
            }
        }
    }
}

port_router::port_list port_router::get_ports(const fcn_gate_layout::tile t, const logic_network::vertex v) const
{
    return g_ports.at(std::make_pair(t, v));
}

port_router::port_list port_router::get_ports(const fcn_gate_layout::tile t, const logic_network::edge& e) const
{
    return w_ports.at(std::make_pair(t, e));
}

void port_router::compute_qca_5x5_ports()
{
    // for all non-free (i.e. occupied) tiles
    for (auto&& t : layout->tiles() |
                    iter::filterfalse([&](const fcn_gate_layout::tile& _t){return layout->is_free_tile(_t);}))
    {
        // distinguish between gates and wires
        if (layout->is_gate_tile(t))
        {
            port_list p{};

            // determine incoming connector ports
            if (layout->is_tile_inp_dir(t, layout::DIR_N))
                p.inp.emplace(2u, 0u);
            if (layout->is_tile_inp_dir(t, layout::DIR_E))
                p.inp.emplace(4u, 2u);
            if (layout->is_tile_inp_dir(t, layout::DIR_S))
                p.inp.emplace(2u, 4u);
            if (layout->is_tile_inp_dir(t, layout::DIR_W))
                p.inp.emplace(0u, 2u);

            // determine outgoing connector ports
            if (layout->is_tile_out_dir(t, layout::DIR_N))
                p.out.emplace(2u, 0u);
            if (layout->is_tile_out_dir(t, layout::DIR_E))
                p.out.emplace(4u, 2u);
            if (layout->is_tile_out_dir(t, layout::DIR_S))
                p.out.emplace(2u, 4u);
            if (layout->is_tile_out_dir(t, layout::DIR_W))
                p.out.emplace(0u, 2u);

            // assign connector ports
            g_ports[std::make_pair(t, *layout->get_logic_vertex(t))] = p;
        }
        else if (layout->is_wire_tile(t))
        {
            for (auto& e : layout->get_logic_edges(t))
            {
                port_list p{};

                // numeral representation of direction bitset
                auto inp_dir_num = layout->get_wire_inp_dirs(t, e).to_ulong();
                switch (inp_dir_num)
                {
                    case 0b0001: { p.inp.emplace(0u, 2u); break; }  // WEST
                    case 0b0010: { p.inp.emplace(2u, 4u); break; }  // SOUTH
                    case 0b0100: { p.inp.emplace(4u, 2u); break; }  // EAST
                    case 0b1000: { p.inp.emplace(2u, 0u); break; }  // NORTH
                    default:
                    {
                        throw std::invalid_argument{"Unsupported input wire directions in tile"};
                    }
                }
                // numeral representation of direction bitset
                auto out_dir_num = layout->get_wire_out_dirs(t, e).to_ulong();
                switch (out_dir_num)
                {
                    case 0b0001: { p.out.emplace(0u, 2u); break; }  // WEST
                    case 0b0010: { p.out.emplace(2u, 4u); break; }  // SOUTH
                    case 0b0100: { p.out.emplace(4u, 2u); break; }  // EAST
                    case 0b1000: { p.out.emplace(2u, 0u); break; }  // NORTH
                    default:
                    {
                        throw std::invalid_argument{"Unsupported output wire directions in tile"};
                    }
                }

                // assign ports
                w_ports[std::make_pair(t, e)] = p;
            }
        }
    }
}
