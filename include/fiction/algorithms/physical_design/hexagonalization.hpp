//
// Created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <cmath>
#include <cstdint>

namespace fiction
{
/**
 * Utility function to transform a Cartesian coordinate into a hexagonal one.
 *
 * @param cartesian_x X-coordinate on the Cartesian grid.
 * @param cartesian_y Y-coordinate on the Cartesian grid.
 * @param cartesian_layout_height Height of the Cartesian layout.
 * @param cartesian_z Z-coordinate on the Cartesian grid.
 *
 * @return coordinate on the hexagonal grid.
 */
template <typename Lyt>
[[nodiscard]] coordinate<Lyt> to_hex(int64_t cartesian_x, int64_t cartesian_y, int64_t cartesian_layout_height, int64_t cartesian_z) noexcept
{
    const auto y = cartesian_x + cartesian_y;
    const auto x =
        static_cast<int64_t>(cartesian_x + static_cast<int64_t>(std::ceil(std::floor(static_cast<double>(cartesian_layout_height) / 2) -
                                                 static_cast<double>(y) / 2)));
    const auto z = cartesian_z;

    return {x, y, z};
}

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
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE));

    using hex_lyt = hex_even_row_gate_clk_lyt;

    // get width, height and depth of Cartesian layout
    const auto layout_width  = static_cast<int64_t>(lyt.x()) + 1;
    const auto layout_height = static_cast<int64_t>(lyt.y()) + 1;
    const auto layout_depth = static_cast<int64_t>(lyt.z()) + 1;

    // calculate max width, height and depth of hexagonal layout
    const auto hex_height = to_hex<Lyt>(layout_width - 1, layout_height - 1, layout_height, 0).y;
    const auto hex_width  = to_hex<Lyt>(layout_width - 1, 0, layout_height, 0).x;
    const auto hex_depth = layout_depth;

    // instantiate hexagonal layout
    hex_lyt hex_layout{{hex_width, hex_height, hex_depth}, fiction::row_clocking<hex_lyt>()};

    // iterate through cartesian layout diagonally
    for (int64_t k = 0; k < layout_width + layout_height - 1; ++k)
    {
        for (int64_t x = 0; x < k + 1; ++x)
        {
            const auto y = k - x;
            if (y < layout_height && x < layout_width)
            {
                for (int64_t z = 0; z <= layout_depth; ++z)
                {
                    // old coordinate
                    const coordinate<Lyt> old_coord{x, y, z};
                    // new coordinate
                    const coordinate<Lyt> hex{to_hex<Lyt>(x, y, layout_height, z)};

                    if (lyt.is_empty_tile(old_coord))
                    {
                        continue;
                    }

                    if (const auto node = lyt.get_node(old_coord); lyt.is_pi(node))
                    {
                        hex_layout.create_pi(lyt.get_name(lyt.get_node({x, y, z})), hex);
                    }
                    else if (lyt.is_po(node))
                    {
                        const auto signal = lyt.incoming_data_flow(old_coord)[0];

                        hex_layout.create_po(hex_layout.make_signal(hex_layout.get_node(
                                                 to_hex<Lyt>(signal.x, signal.y, layout_height, signal.z))),
                                             lyt.get_name(lyt.get_node(old_coord)), hex);
                    }
                    else if (lyt.is_wire(node))
                    {
                        const auto signal = lyt.incoming_data_flow(old_coord)[0];

                        hex_layout.create_buf(hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal.x, signal.y, layout_height, signal.z))),
                                              hex);
                    }
                    else if (lyt.is_inv(node))
                    {
                        const auto signal = lyt.incoming_data_flow(old_coord)[0];

                        hex_layout.create_not(hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal.x, signal.y, layout_height, signal.z))),
                                              hex);
                    }
                    else if (lyt.is_and(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_and(hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                              hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                              hex);
                    }
                    else if (lyt.is_nand(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_nand(hex_layout.make_signal(hex_layout.get_node(
                                                   to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                               hex_layout.make_signal(hex_layout.get_node(
                                                   to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                               hex);
                    }
                    else if (lyt.is_or(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_or(hex_layout.make_signal(hex_layout.get_node(
                                                 to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                             hex_layout.make_signal(hex_layout.get_node(
                                                 to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                             hex);
                    }
                    else if (lyt.is_nor(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_nor(hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                              hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                              hex);
                    }
                    else if (lyt.is_xor(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_xor(hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                              hex_layout.make_signal(hex_layout.get_node(
                                                  to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                              hex);
                    }
                    else if (lyt.is_xnor(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        hex_layout.create_xnor(hex_layout.make_signal(hex_layout.get_node(
                                                   to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                               hex_layout.make_signal(hex_layout.get_node(
                                                   to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                               hex);
                    }
                    else if (lyt.is_function(node))
                    {
                        const auto signal_a = lyt.incoming_data_flow(old_coord)[0];
                        const auto signal_b = lyt.incoming_data_flow(old_coord)[1];

                        const auto node_fun = lyt.node_function(node);

                        hex_layout.create_node({hex_layout.make_signal(hex_layout.get_node(
                                                    to_hex<Lyt>(signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                hex_layout.make_signal(hex_layout.get_node(
                                                    to_hex<Lyt>(signal_b.x, signal_b.y, layout_height, signal_b.z)))},
                                               node_fun, hex);
                    }
                }
            }
        }
    }
    return hex_layout;
}

}  // namespace fiction

#endif  // FICTION_HEXAGONALIZATION_HPP
