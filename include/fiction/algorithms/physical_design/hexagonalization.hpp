//
// Created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

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
[[nodiscard]] coordinate<HexLyt> to_hex(coordinate<CartLyt> cartesian_coordinate,
                                        int64_t             cartesian_layout_height) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "Old coordinate is not Cartesian");
    static_assert(is_hexagonal_layout_v<HexLyt>, "New coordinate is not hexagonal");

    const auto y = cartesian_coordinate.x + cartesian_coordinate.y;
    const auto x = static_cast<int64_t>(
        cartesian_coordinate.x +
        static_cast<int64_t>(
            std::ceil(std::floor(static_cast<double>(cartesian_layout_height) / 2) - static_cast<double>(y) / 2)));
    const auto z = cartesian_coordinate.z;

    return coordinate<HexLyt>{x, y, z};
}
}  // namespace detail

/**
 * Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even row clocked layout suitable for SiDBs by
 * remapping all gates and wires as originally proposed in \"Scalable Physical Design for Silicon Dangling Bond Logic:
 * How a 45° Turn Prevents the Reinvention of the Wheel\" by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2023
 * (https://ieeexplore.ieee.org/document/10231278).
 *
 * @tparam HexLyt Even-row hexagonal gate-level layout return type.
 * @tparam CartLyt Input Cartesian gate-level layout type.
 * @param lyt 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
 * @return Hexagonal representation of the Cartesian layout.
 */
template <typename HexLyt, typename CartLyt>
[[nodiscard]] HexLyt hexagonalization(const CartLyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<HexLyt>, "HexLyt is not a gate-level layout");
    static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");
    static_assert(has_even_row_hex_arrangement_v<HexLyt>, "HexLyt does not have an even row hexagon arrangement");

    static_assert(is_gate_level_layout_v<CartLyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<CartLyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE));

    // get width, height and depth of Cartesian layout
    const auto layout_width  = static_cast<int64_t>(lyt.x()) + 1;
    const auto layout_height = static_cast<int64_t>(lyt.y()) + 1;
    const auto layout_depth  = static_cast<int64_t>(lyt.z());

    // calculate max width, height and depth of hexagonal layout
    const auto hex_height =
        detail::to_hex<CartLyt, HexLyt>(coordinate<CartLyt>(layout_width - 1, layout_height - 1, 0), layout_height).y;
    const auto hex_width =
        detail::to_hex<CartLyt, HexLyt>(coordinate<CartLyt>(layout_width - 1, 0, 0), layout_height).x;
    const auto hex_depth = layout_depth;

    // instantiate hexagonal layout
    HexLyt hex_layout{{hex_width, hex_height, hex_depth}, row_clocking<HexLyt>()};

    // iterate through cartesian layout diagonally
    for (int64_t k = 0; k < layout_width + layout_height - 1; ++k)
    {
        for (int64_t x = 0; x < k + 1; ++x)
        {
            const auto y = k - x;
            if (y < layout_height && x < layout_width)
            {
                for (int64_t z = 0; z <= hex_depth; ++z)
                {
                    // old coordinate
                    const coordinate<CartLyt> old_coord{x, y, z};
                    // new coordinate
                    const coordinate<CartLyt> hex{detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height)};

                    if (lyt.is_empty_tile(old_coord))
                    {
                        continue;
                    }
                    const auto node = lyt.get_node(old_coord);

                    if (lyt.is_pi(node))
                    {
                        hex_layout.create_pi(lyt.get_name(lyt.get_node(old_coord)), hex);
                    }

                    if (const auto signals = lyt.incoming_data_flow(old_coord); signals.size() == 1)
                    {
                        const auto signal = signals[0];

                        const auto hex_signal = hex_layout.make_signal(
                            hex_layout.get_node(detail::to_hex<CartLyt, HexLyt>(signal, layout_height)));

                        if (lyt.is_po(node))
                        {
                            hex_layout.create_po(hex_signal, lyt.get_name(lyt.get_node(old_coord)), hex);
                        }
                        else if (lyt.is_wire(node))
                        {
                            hex_layout.create_buf(hex_signal, hex);
                        }
                        else if (lyt.is_inv(node))
                        {
                            hex_layout.create_not(hex_signal, hex);
                        }
                    }

                    else if (signals.size() == 2)
                    {
                        const auto signal_a = signals[0];
                        const auto signal_b = signals[1];

                        const auto hex_signal_a = hex_layout.make_signal(
                            hex_layout.get_node(detail::to_hex<CartLyt, HexLyt>(signal_a, layout_height)));
                        const auto hex_signal_b = hex_layout.make_signal(
                            hex_layout.get_node(detail::to_hex<CartLyt, HexLyt>(signal_b, layout_height)));

                        if (lyt.is_and(node))
                        {
                            hex_layout.create_and(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_nand(node))
                        {
                            hex_layout.create_nand(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_or(node))
                        {
                            hex_layout.create_or(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_nor(node))
                        {
                            hex_layout.create_nor(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_xor(node))
                        {
                            hex_layout.create_xor(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_xnor(node))
                        {
                            hex_layout.create_xnor(hex_signal_a, hex_signal_b, hex);
                        }
                        else if (lyt.is_function(node))
                        {
                            const auto node_fun = lyt.node_function(node);

                            hex_layout.create_node({hex_signal_a, hex_signal_b}, node_fun, hex);
                        }
                    }
                }
            }
        }
    }

    restore_names<CartLyt, HexLyt>(lyt, hex_layout);

    return hex_layout;
}

}  // namespace fiction

#endif  // FICTION_HEXAGONALIZATION_HPP
