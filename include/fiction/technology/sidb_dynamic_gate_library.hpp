//
// Created by marcel on 15.09.23.
//

#ifndef FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP
#define FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/traits.hpp"

namespace fiction
{

class sidb_dynamic_gate_library : fcn_gate_library<sidb_technology, 60, 46>  // width and height of a hexagon
{
  public:
    explicit sidb_dynamic_gate_library() = delete;

    template <typename GateLyt>
    [[nodiscard]] fcn_gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate level layout.");

        const auto n = lyt.get_node(t);
        const auto f = lyt.node_function(n);
        const auto p = determine_port_routing(lyt, n);

        // TODO implement on-the-fly gate design here and return an fcn_gate object

        // if the gate type is unknown, throw an exception
        throw unsupported_gate_type_exception(t);
    }

  private:
    template <typename Lyt>
    [[nodiscard]] static port_list<port_direction> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t) noexcept
    {
        port_list<port_direction> p{};

        // determine incoming connector ports
        if (lyt.has_north_eastern_incoming_signal(t))
        {
            p.inp.emplace(port_direction::cardinal::NORTH_EAST);
        }
        if (lyt.has_north_western_incoming_signal(t))
        {
            p.inp.emplace(port_direction::cardinal::NORTH_WEST);
        }

        // determine outgoing connector ports
        if (lyt.has_south_eastern_outgoing_signal(t))
        {
            p.out.emplace(port_direction::cardinal::SOUTH_EAST);
        }
        if (lyt.has_south_western_outgoing_signal(t))
        {
            p.out.emplace(port_direction::cardinal::SOUTH_WEST);
        }

        // gates without connector ports

        // 1-input functions
        if (const auto n = lyt.get_node(t); lyt.is_pi(n) || lyt.is_po(n) || lyt.is_buf(n) || lyt.is_inv(n))
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(port_direction::cardinal::NORTH_WEST);
            }
            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(port_direction::cardinal::SOUTH_EAST);
            }
        }
        else  // 2-input functions
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(port_direction::cardinal::NORTH_WEST);
                p.inp.emplace(port_direction::cardinal::NORTH_EAST);
            }
            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(port_direction::cardinal::SOUTH_EAST);
            }
        }

        return p;
    }
};

}  // namespace fiction

#endif  // FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP
