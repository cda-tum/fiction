//
// Created by simon on 14.04.23.
//

#ifndef FICTION_CARTESIAN_TO_HEXAGONAL_HPP
#define FICTION_CARTESIAN_TO_HEXAGONAL_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction
{

/**
 * A 2D bounding box object that computes a minimum-sized box around all non-empty coordinates in a given layout.
 * Layouts can be of arbitrary size and, thus, may be larger than their contained elements. Sometimes, it might be
 * necessary to know exactly which space the associated layout internals occupy. A bounding box computes coordinates
 * that span a minimum-sized rectangle that encloses all non-empty layout coordinates.
 *
 * The bounding box does not automatically updated when the layout changes. Call `update_bounding_box()` to recompute
 * it.
 *
 * @tparam Lyt Gate-level or cell-level layout type.
 */
template <typename Lyt>
class cartesian_to_hexagonal
{
  public:
    /**
     * Standard constructor that computes an initial bounding box.
     *
     * @param lyt Gate-level or cell-level layout whose bounding box is desired.
     */
    explicit cartesian_to_hexagonal(const Lyt& lyt) noexcept : layout{lyt}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
        update_bounding_box();
    }
    /**
     * The bounding box is not automatically updated when the layout changes. This function recomputes the bounding box.
     */
    hex_even_row_gate_clk_lyt cartesian_to_hexagonal(const Lyt& lyt)
    {
        using gate_lyt = Lyt;
        using hex_lyt  = hex_even_row_gate_clk_lyt;
        using cell_lyt = sidb_cell_clk_lyt;

        // get width & height of cartesian layout
        const auto layout_width  = gate_level_layout.x() + 1;
        const auto layout_height = gate_level_layout.y() + 1;

        // calculate max width & height of hexagonal layout
        const auto hex_height = to_hex<gate_lyt>(layout_width - 1, layout_height - 1, layout_height, 0).y;
        const auto hex_width  = to_hex<gate_lyt>(layout_width - 1, 0, layout_height, 0).x;

        // instantiate hexagonal layout
        hex_lyt hex_layout{{hex_width, hex_height, 1}, fiction::row_clocking<hex_lyt>()};

        // iterate through cartesian layout diagonally
        for (int k = 0; k < layout_width + layout_height - 1; k++)
        {
            for (int x = 0; x < k + 1; x++)
            {
                const auto y = k - x;
                if (y < layout_height && x < layout_width)
                {
                    for (int z = 0; z < 2; z++)
                    {
                        // old coordinate
                        const coordinate<gate_lyt> old_coord{x, y, z};
                        // new coordinate
                        const coordinate<gate_lyt> hex{to_hex<gate_lyt>(x, y, layout_height, z)};

                        if (gate_level_layout.is_empty_tile(old_coord))
                        {
                            continue;
                        }

                        if (const auto node = gate_level_layout.get_node(old_coord); gate_level_layout.is_pi(node))
                        {
                            hex_layout.create_pi(
                                gate_level_layout.get_name(gate_level_layout.get_node({x, y, z})), hex);
                        }
                        else if (gate_level_layout.is_po(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_po(
                                hex_layout.make_signal(
                                    hex_layout.get_node(to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                gate_level_layout.get_name(gate_level_layout.get_node(old_coord)), hex);
                        }
                        else if (gate_level_layout.is_wire(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_buf(hex_layout.make_signal(hex_layout.get_node(
                                                      to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_inv(node))
                        {
                            const auto signal = gate_level_layout.incoming_data_flow(old_coord)[0];

                            hex_layout.create_not(hex_layout.make_signal(hex_layout.get_node(
                                                      to_hex<gate_lyt>(signal.x, signal.y, layout_height, signal.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_and(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_and(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_nand(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_nand(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                   hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                   hex);
                        }
                        else if (gate_level_layout.is_or(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_or(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                     signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                 hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                     signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                 hex);
                        }
                        else if (gate_level_layout.is_nor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_nor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_xor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_xor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                  hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                      signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                  hex);
                        }
                        else if (gate_level_layout.is_xnor(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            hex_layout.create_xnor(hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                   hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                       signal_b.x, signal_b.y, layout_height, signal_b.z))),
                                                   hex);
                        }
                        else if (gate_level_layout.is_function(node))
                        {
                            const auto signal_a = gate_level_layout.incoming_data_flow(old_coord)[0];
                            const auto signal_b = gate_level_layout.incoming_data_flow(old_coord)[1];

                            const auto node_fun = gate_level_layout.node_function(node);

                            hex_layout.create_node({hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                        signal_a.x, signal_a.y, layout_height, signal_a.z))),
                                                    hex_layout.make_signal(hex_layout.get_node(to_hex<gate_lyt>(
                                                        signal_b.x, signal_b.y, layout_height, signal_b.z)))},
                                                   node_fun, hex);
                        }
                    }
                }
            }
        }
        return hex_lyt;
    }


  private:
    [[nodiscard]] template <typename Lyt>fiction::coordinate<Lyt> to_hex(int old_x, int old_y, int height, int z)
    {
        const auto y = old_x + old_y;
        const auto x = old_x + std::ceil(std::floor(height / 2) - y / 2);

        fiction::coordinate<Lyt> hex{x, y, z};

        return hex;
    }
};

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_CARTESIAN_TO_HEXAGONAL_HPP
