//
// Created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/views/topo_view.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <cmath>
#include <cstdint>

namespace fiction
{

namespace detail
{

/**
 * Utility function to transform a Cartesian coordinate into a hexagonal one.
 *
 * @param cartesian_coordinate Coordinate on the Cartesian grid.
 * @param cartesian_layout_height Height of the Cartesian layout.
 *
 * @return coordinate on the hexagonal grid.
 */
template <typename CartLyt, typename HexLyt>
[[nodiscard]] fiction::coordinate<HexLyt> to_hex(fiction::coordinate<CartLyt> cartesian_coordinate, int64_t cartesian_layout_height) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "Old coordinate is not Cartesian");
    static_assert(is_hexagonal_layout_v<HexLyt>, "New coordinate is not hexagonal");
    const auto y = cartesian_coordinate.x + cartesian_coordinate.y;
    const auto x = static_cast<int64_t>(
        cartesian_coordinate.x + static_cast<int64_t>(std::ceil(std::floor(static_cast<double>(cartesian_layout_height) / 2) -
                                                     static_cast<double>(y) / 2)));
    const auto z = cartesian_coordinate.z;

    return fiction::coordinate<HexLyt>{x, y, z};
}
}  // namespace detail

/**
 * Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even row clocked layout suitable for SiDBs by
 * remapping all gates and wires as originally proposed in \"Scalable Physical Design for Silicon Dangling Bond Logic:
 * How a 45° Turn Prevents the Reinvention of the Wheel\" by S. Hofmann, M. Walter, and R. Wille in IEEE Nano 2023.
 *
 * @param Lyt Gate-level layout that is 2DDWave-clocked.
 *
 * @return Hexagonal representation of the Cartesian layout.
 */
template <typename Lyt>
[[nodiscard]] hex_even_row_gate_clk_lyt hexagonalization(const Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE));

    using hex_lyt = hex_even_row_gate_clk_lyt;

    // get width, height and depth of Cartesian layout
    const auto layout_width  = static_cast<int64_t>(lyt.x()) + 1;
    const auto layout_height = static_cast<int64_t>(lyt.y()) + 1;
    const auto layout_depth = static_cast<int64_t>(lyt.z());

    // calculate max width, height and depth of hexagonal layout
    const auto hex_height = detail::to_hex<Lyt, hex_lyt>(coordinate<Lyt>(layout_width - 1, layout_height - 1, 0), layout_height).y;
    const auto hex_width  = detail::to_hex<Lyt, hex_lyt>(coordinate<Lyt>(layout_width - 1, 0, 0), layout_height).x;
    const auto hex_depth = layout_depth;

    // instantiate hexagonal layout
    hex_lyt hex_layout{{hex_width, hex_height, hex_depth}, fiction::row_clocking<hex_lyt>()};

    // create a topological view on the network
    mockturtle::topo_view cartesian_topo{lyt};

    // create a node map
    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(cartesian_topo)> node2pos{cartesian_topo};

    // reserve PI nodes without positions
    auto pi2node = reserve_input_nodes(hex_layout, cartesian_topo);

    cartesian_topo.foreach_pi(
        [&](const auto& node)
        {
        // old coordinate
        const coordinate<Lyt> old_coord = lyt.get_tile(node);
        // new coordinate
        const coordinate<Lyt> hex{detail::to_hex<Lyt, hex_lyt>(old_coord, layout_height)};

        if (lyt.is_pi(node))
        {
            node2pos[node] = hex_layout.move_node(pi2node[node], hex);
        }
        });

    cartesian_topo.foreach_gate(
        [&](const auto& node)
        {
        // old coordinate
        const coordinate<Lyt> old_coord = lyt.get_tile(node);
        // new coordinate
        const coordinate<Lyt> hex{detail::to_hex<Lyt, hex_lyt>(old_coord, layout_height)};

        if (const auto fc = fanins(cartesian_topo, node); fc.fanin_nodes.size() == 1)
        {
            const auto fanin_signal = node2pos[cartesian_topo.make_signal(fc.fanin_nodes[0])];

            if (lyt.is_po(node))
            {
                node2pos[node] = hex_layout.create_po(fanin_signal,lyt.get_name(node), hex);
            }
            else if (lyt.is_wire(node))
            {
                node2pos[node] = hex_layout.create_buf(fanin_signal,hex);
            }
            else if (lyt.is_inv(node))
            {
                node2pos[node] = hex_layout.create_not(fanin_signal,hex);
            }
        }
        else
        {
            const auto fanin_signal_a = node2pos[cartesian_topo.make_signal(fc.fanin_nodes[0])];
            const auto fanin_signal_b = node2pos[cartesian_topo.make_signal(fc.fanin_nodes[1])];

            if (lyt.is_and(node))
            {
                node2pos[node] = hex_layout.create_and(fanin_signal_a, fanin_signal_b,hex);
            }
            else if (lyt.is_nand(node))
            {
                node2pos[node] = hex_layout.create_nand(fanin_signal_a, fanin_signal_b, hex);
            }
            else if (lyt.is_or(node))
            {
                node2pos[node] = hex_layout.create_or(fanin_signal_a, fanin_signal_b, hex);
            }
            else if (lyt.is_nor(node))
            {
                node2pos[node] = hex_layout.create_nor(fanin_signal_a, fanin_signal_b, hex);
            }
            else if (lyt.is_xor(node))
            {
                node2pos[node] = hex_layout.create_xor(fanin_signal_a, fanin_signal_b,hex);
            }
            else if (lyt.is_xnor(node))
            {
                node2pos[node] = hex_layout.create_xnor(fanin_signal_a, fanin_signal_b, hex);
            }
            else if (lyt.is_function(node))
            {
                const auto node_fun = lyt.node_function(node);

                node2pos[node] = hex_layout.create_node({fanin_signal_a, fanin_signal_b},
                                       node_fun, hex);
            }
        }
    });

    // restore possibly set signal names
    restore_names(cartesian_topo, hex_layout, node2pos);
    return hex_layout;
}

}  // namespace fiction

#endif  // FICTION_HEXAGONALIZATION_HPP
