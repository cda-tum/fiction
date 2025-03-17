//
// Created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <cmath>
#include <cstdint>

namespace fiction
{

/**
 * This struct stores statistics about the hexagonalization process.
 */
struct hexagonalization_stats
{
    /**
     * Runtime of the hexagonalization process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

/**
 * Utility function to transform a Cartesian tile into a hexagonal one.
 *
 * @param cartesian_tile Tile on the Cartesian grid.
 * @param cartesian_layout_height Height of the Cartesian layout.
 * @return corresponding tile on the hexagonal grid.
 */
template <typename CartLyt, typename HexLyt>
[[nodiscard]] tile<HexLyt> to_hex(tile<CartLyt> cartesian_tile, uint64_t cartesian_layout_height) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "Old tile is not Cartesian");
    static_assert(is_hexagonal_layout_v<HexLyt>, "New tile is not hexagonal");

    const auto y = cartesian_tile.x + cartesian_tile.y;
    const auto x = static_cast<int64_t>(
        cartesian_tile.x + static_cast<int64_t>(std::ceil(std::floor(static_cast<double>(cartesian_layout_height) / 2) -
                                                          (static_cast<double>(y) / 2))));
    const auto z = cartesian_tile.z;

    return tile<HexLyt>{x, y, z};
}

/**
 * Utility function to calculate the offset that has to be subtracted from any x-coordinate on the hexagonal layout.
 *
 * This function iterates through diagonals starting from the bottom left corner until it finds a non-empty tile or
 * until it has traversed all diagonals.
 * In each iteration, it checks tiles along the diagonal line.
 * If it finds a non-empty tile, it calculates an offset value, which is the number of columns on the left side of the
 * hexagonal layout that will be empty.
 * This offset is based on the leftmost tile that will appear in the hexagonal layout.
 *
 * @tparam HexLyt Even-row hexagonal gate-level layout return type.
 * @tparam CartLyt Input Cartesian gate-level layout type.
 * @param lyt 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
 * @param cartesian_layout_width Width of the Cartesian layout.
 * @param cartesian_layout_height Height of the Cartesian layout.
 * @return offset.
 */
template <typename HexLyt, typename CartLyt>
[[nodiscard]] uint64_t get_offset(const CartLyt& lyt, uint64_t cartesian_layout_width,
                                  uint64_t cartesian_layout_height) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "CartLyt is not a Cartesian layout");
    static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");

    auto     found_non_empty_tile = false;
    uint64_t offset               = 0;

    // Iterate through diagonals
    for (uint64_t diagonal_idx = 0;
         (diagonal_idx < cartesian_layout_height + cartesian_layout_width - 1) && !found_non_empty_tile; ++diagonal_idx)
    {
        for (uint64_t idx = 0; idx < cartesian_layout_height; ++idx)
        {
            if (diagonal_idx - idx < cartesian_layout_width)
            {
                const tile<CartLyt> current_tile = {diagonal_idx - idx, cartesian_layout_height - 1 - idx};
                if (!lyt.is_empty_tile(current_tile))
                {
                    found_non_empty_tile = true;
                    const auto coord     = detail::to_hex<CartLyt, HexLyt>(current_tile, cartesian_layout_height);
                    if (coord.x > offset)
                    {
                        offset = coord.x;
                    }
                }
            }
        }
    }

    return offset;
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
[[nodiscard]] HexLyt hexagonalization(const CartLyt& lyt, hexagonalization_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<HexLyt>, "HexLyt is not a gate-level layout");
    static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");
    static_assert(has_even_row_hex_arrangement_v<HexLyt>, "HexLyt does not have an even row hexagon arrangement");

    static_assert(is_gate_level_layout_v<CartLyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<CartLyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE));

    // get width, height and depth of Cartesian layout
    const auto layout_width  = lyt.x() + 1;
    const auto layout_height = lyt.y() + 1;
    const auto layout_depth  = lyt.z();

    // calculate max width, height and depth of hexagonal layout
    const auto hex_height =
        detail::to_hex<CartLyt, HexLyt>(tile<CartLyt>(layout_width - 1, layout_height - 1, 0), layout_height).y;
    const auto hex_width = detail::to_hex<CartLyt, HexLyt>(tile<CartLyt>(layout_width - 1, 0, 0), layout_height).x;
    const auto hex_depth = layout_depth;

    // instantiate hexagonal layout
    HexLyt hex_layout{{hex_width, hex_height, hex_depth}, row_clocking<HexLyt>()};

    hexagonalization_stats stats{};
    // measure run time
    {
        const mockturtle::stopwatch stop{stats.time_total};

        // calculate offset
        const auto offset = detail::get_offset<HexLyt, CartLyt>(lyt, layout_width, layout_height);

        // inputs
        lyt.foreach_pi(
            [&lyt, &hex_layout, &offset, &layout_height](const auto& gate)
            {
                const auto    old_coord = lyt.get_tile(gate);
                tile<CartLyt> hex{detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height)};
                hex.x -= static_cast<decltype(hex.x)>(offset);
                hex_layout.create_pi(lyt.get_name(lyt.get_node(old_coord)), hex);
            });

        // iterate through cartesian layout diagonally
        for (uint64_t k = 0; k < layout_width + layout_height - 1; ++k)
        {
            for (uint64_t x = 0; x < k + 1; ++x)
            {
                const auto y = k - x;
                if (y < layout_height && x < layout_width)
                {
                    for (uint64_t z = 0; z <= hex_depth; ++z)
                    {
                        // old tile
                        const tile<CartLyt> old_coord{x, y, z};
                        // new tile
                        tile<CartLyt> hex{detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height)};
                        hex.x -= static_cast<decltype(hex.x)>(offset);

                        if (lyt.is_empty_tile(old_coord))
                        {
                            continue;
                        }
                        const auto node = lyt.get_node(old_coord);

                        if (lyt.is_pi(node))
                        {
                            continue;
                        }

                        if (const auto signals = lyt.incoming_data_flow(old_coord); signals.size() == 1)
                        {
                            const auto signal = signals[0];

                            auto hex_tile = detail::to_hex<CartLyt, HexLyt>(signal, layout_height);

                            hex_tile.x -= static_cast<decltype(hex_tile.x)>(offset);

                            const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_tile));

                            if (!lyt.is_po(node) and lyt.is_wire(node))
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
                            const auto signal_a   = signals[0];
                            const auto signal_b   = signals[1];
                            auto       hex_tile_a = detail::to_hex<CartLyt, HexLyt>(signal_a, layout_height);
                            auto       hex_tile_b = detail::to_hex<CartLyt, HexLyt>(signal_b, layout_height);

                            hex_tile_a.x -= static_cast<decltype(hex_tile_a.x)>(offset);
                            hex_tile_b.x -= static_cast<decltype(hex_tile_b.x)>(offset);

                            const auto hex_signal_a = hex_layout.make_signal(hex_layout.get_node(hex_tile_a));
                            const auto hex_signal_b = hex_layout.make_signal(hex_layout.get_node(hex_tile_b));

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

        // outputs
        lyt.foreach_po(
            [&lyt, &hex_layout, &offset, &layout_height](const auto& gate)
            {
                const auto old_coord = lyt.get_tile(lyt.get_node(gate));
                const auto signal    = lyt.incoming_data_flow(old_coord)[0];

                tile<CartLyt> hex{detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height)};
                auto          hex_tile = detail::to_hex<CartLyt, HexLyt>(signal, layout_height);
                hex.x -= static_cast<decltype(hex.x)>(offset);
                hex_tile.x -= static_cast<decltype(hex_tile.x)>(offset);

                const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_tile));
                hex_layout.create_po(hex_signal, lyt.get_name(lyt.get_node(old_coord)), hex);
            });

        // calculate bounding box
        const auto bounding_box         = bounding_box_2d(hex_layout);
        const auto layout_max           = bounding_box.get_max();
        const auto mapped_layout_width  = layout_max.x;
        const auto mapped_layout_height = layout_max.y;

        hex_layout.resize(aspect_ratio_type_t<CartLyt>{mapped_layout_width, mapped_layout_height, hex_layout.z()});

        restore_names<CartLyt, HexLyt>(lyt, hex_layout);
    }

    if (pst != nullptr)
    {
        *pst = stats;
    }

    return hex_layout;
}

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_HEXAGONALIZATION_HPP
