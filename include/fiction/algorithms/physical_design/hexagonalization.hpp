//
// created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace fiction
{

/**
 * Exception thrown when an error occurs during moving inputs to top border and rerouting.
 */
class hexagonalization_route_inputs_error : public std::runtime_error
{
  public:
    /**
     * Constructs a `hexagonalization_route_inputs_error` object with the given error message.
     *
     * @param msg The error message describing the error.
     */
    explicit hexagonalization_route_inputs_error(const std::string_view& msg) noexcept : std::runtime_error(msg.data())
    {}
};

/**
 * This structure encapsulates settings that determine how primary inputs (PIs) and primary outputs (POs) are handled
 * during the conversion from a Cartesian to a hexagonal layout.
 */
struct hexagonalization_params
{
    /**
     * Specifies how primary inputs/outputs should be handled in the hexagonalization process.
     */
    enum hexagonalization_input_output_mode : std::uint8_t
    {
        /**
         * Do not extend primary inputs/outputs to the top/bottom row (default).
         */
        NONE,
        /**
         * Extend primary inputs/outputs to the top/bottom row.
         */
        EXTEND,
        /**
         * Extend primary inputs/outputs to the top/bottom row with planar rerouting (i.e., without crossings).
         */
        EXTEND_PLANAR
    };
    /**
     * Input extension mode. Defaults to none
     */
    hexagonalization_input_output_mode input_mode = NONE;
    /**
     * Output extension mode. Defaults to none
     */
    hexagonalization_input_output_mode output_mode = NONE;
};

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
 * This struct encapsulates a routing objective that specifies the source and target coordinates,
 * along with a flag indicating whether the primary input was the first fanin for the corresponding fanout.
 * @tparam HexLyt type of the hexagonal layout.
 */
template <typename HexLyt>
struct extended_routing_objective
{
    /**
     * The coordinate of the moved primary input.
     */
    tile<HexLyt> source;
    /**
     * The coordinate of the fanout node.
     */
    tile<HexLyt> target;
    /**
     * Flag that is set to true if the primary input was the first fanin; this indicates that the
     * fanin signals need to be reordered.
     */
    bool update_first_fanin = false;
};

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

    // compute new y coordinate as sum of x and y
    const auto y = cartesian_tile.x + cartesian_tile.y;

    // compute half of layout height for adjustment calculation
    const auto half_height = static_cast<double>(cartesian_layout_height) / 2.0;

    // calculate adjustment based on layout height and y coordinate
    const auto adjustment = static_cast<int64_t>(std::ceil(std::floor(half_height) - static_cast<double>(y) / 2.0));

    // compute new x coordinate by adding adjustment
    const auto x = cartesian_tile.x + adjustment;

    // return the new hexagonal tile
    return tile<HexLyt>{x, y, cartesian_tile.z};
}

/**
 * This function iterates over all primary inputs in the given Cartesian layout and counts those
 * whose tile is at the western border. Such inputs are considered to be positioned left of the middle
 * primary input when the layout is converted to a hexagonal format.
 *
 * @tparam CartLyt Type of the Cartesian layout.
 * @param lyt The Cartesian gate-level layout containing primary inputs.
 * @return The number of primary inputs that are placed to the left of the middle primary input.
 */
template <typename CartLyt>
[[nodiscard]] uint64_t compute_num_inputs_left_to_middle_pi(const CartLyt& lyt) noexcept
{
    uint64_t num_inputs_left_to_middle_pi = 0;

    // iterate over all primary inputs
    lyt.foreach_pi(
        [&](const auto& gate)
        {
            const auto coord = lyt.get_tile(gate);

            // if the tile is at the western border, it is placed left of the middle PI in the hex layout
            if (coord.x == 0 && coord.y != 0)
            {
                ++num_inputs_left_to_middle_pi;
            }
        });

    return num_inputs_left_to_middle_pi;
}

/**
 * This function iterates over all primary outputs in the given Cartesian layout and counts those
 * whose tile is at the southern border. Such outputs are considered to be positioned left of the middle
 * primary output when the layout is converted to a hexagonal format.
 *
 * @tparam CartLyt Type of the Cartesian layout.
 * @param lyt The Cartesian gate-level layout containing primary outputs.
 * @return The number of primary outputs that are placed to the left of the middle primary output.
 */
template <typename CartLyt>
[[nodiscard]] uint64_t compute_num_outputs_left_to_middle_po(const CartLyt& lyt) noexcept
{
    uint64_t num_outputs_left_to_middle_po = 0;

    // iterate over all primary inputs
    lyt.foreach_po(
        [&](const auto& gate)
        {
            const auto coord = lyt.get_tile(lyt.get_node(gate));

            // if the tile is at the southern border, it is placed left of the middle PO in the hex layout
            if (coord.x != 0 && coord.y == lyt.y())
            {
                ++num_outputs_left_to_middle_po;
            }
        });

    return num_outputs_left_to_middle_po;
}

/**
 * This function iterates over all primary inputs in the given Cartesian layout and counts those
 * whose tile is at the northern border. Such inputs are considered to be positioned right of the middle
 * primary input when the layout is converted to a hexagonal format.
 *
 * @tparam CartLyt Type of the Cartesian layout.
 * @param lyt The Cartesian gate-level layout containing primary inputs.
 * @return The number of primary inputs that are placed to the right of the middle primary input.
 */
template <typename CartLyt>
[[nodiscard]] uint64_t compute_num_inputs_right_to_middle_pi(const CartLyt& lyt) noexcept
{
    uint64_t num_inputs_right_to_middle_pi = 0;

    // iterate over all primary inputs
    lyt.foreach_pi(
        [&](const auto& gate)
        {
            const auto coord = lyt.get_tile(gate);

            // if the tile is at the northern border, it is placed right of the middle PI in the hex layout
            if (coord.x != 0 && coord.y == 0)
            {
                ++num_inputs_right_to_middle_pi;
            }
        });

    return num_inputs_right_to_middle_pi;
}

/**
 * This function iterates over all primary outputs in the given Cartesian layout and counts those
 * whose tile is at the eastern border. Such outputs are considered to be positioned right of the middle
 * primary output when the layout is converted to a hexagonal format.
 *
 * @tparam CartLyt Type of the Cartesian layout.
 * @param lyt The Cartesian gate-level layout containing primary outputs.
 * @return The number of primary outputs that are placed to the right of the middle primary output.
 */
template <typename CartLyt>
[[nodiscard]] uint64_t compute_num_outputs_right_to_middle_po(const CartLyt& lyt) noexcept
{
    uint64_t num_outputs_right_to_middle_po = 0;

    // iterate over all primary inputs
    lyt.foreach_po(
        [&](const auto& gate)
        {
            const auto coord = lyt.get_tile(lyt.get_node(gate));

            // if the tile is at the eastern border, it is placed right of the middle PO in the hex layout
            if (coord.x == lyt.x() && coord.y != 0)
            {
                ++num_outputs_right_to_middle_po;
            }
        });

    return num_outputs_right_to_middle_po;
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
 * @param input_mode Adjust offset based on PIs relocated to the top row.
 * @param output_mode Adjust offset based on POs relocated to the bottom row.
 * @return offset.
 */
template <typename HexLyt, typename CartLyt>
[[nodiscard]] uint64_t get_offset(const CartLyt& lyt, uint64_t cartesian_layout_width, uint64_t cartesian_layout_height,
                                  hexagonalization_params::hexagonalization_input_output_mode input_mode, hexagonalization_params::hexagonalization_input_output_mode output_mode) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "CartLyt is not a Cartesian layout");
    static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");

    bool     non_empty_tile_found = false;
    uint64_t offset               = 0;

    // iterate diagonally over the Cartesian layout
    for (uint64_t diag = 0; diag < cartesian_layout_height + cartesian_layout_width - 1 && !non_empty_tile_found;
         ++diag)
    {
        for (uint64_t idx = 0; idx < cartesian_layout_height; ++idx)
        {
            if (diag - idx < cartesian_layout_width)
            {
                // create current tile on the diagonal
                const tile<CartLyt> current_tile{diag - idx, cartesian_layout_height - 1 - idx};

                // if tile is not empty, update offset accordingly
                if (!lyt.is_empty_tile(current_tile))
                {
                    non_empty_tile_found = true;
                    const auto hex_coord = to_hex<CartLyt, HexLyt>(current_tile, cartesian_layout_height);
                    offset               = std::max(offset, static_cast<uint64_t>(hex_coord.x));
                }
            }
        }
    }

    if (input_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
    {
        const auto middle_pi = detail::to_hex<CartLyt, HexLyt>({0, 0}, cartesian_layout_height);

        // adjust offset based on primary inputs in the first column
        auto num_inputs_left_to_middle_pi = compute_num_inputs_left_to_middle_pi(lyt);

        // if necessary, adjust the offset to account for primary inputs
        if (middle_pi.x < offset + num_inputs_left_to_middle_pi)
        {
            offset = middle_pi.x - num_inputs_left_to_middle_pi;
        }
    }

    if (output_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
    {
        const auto middle_po = detail::to_hex<CartLyt, HexLyt>({lyt.x(), lyt.y()}, cartesian_layout_height);

        // adjust offset based on primary inputs in the first column
        auto num_outputs_left_to_middle_po = compute_num_outputs_left_to_middle_po(lyt);

        // if necessary, adjust the offset to account for primary inputs
        if (middle_po.x < offset + num_outputs_left_to_middle_po)
        {
            offset = middle_po.x - num_outputs_left_to_middle_po;
        }
    }

    return offset;
}

template <typename HexLyt, typename CartLyt>
class hexagonalization_impl
{
  public:
    hexagonalization_impl(const CartLyt& lyt, const hexagonalization_params& p, hexagonalization_stats* st = nullptr) :
            plyt(lyt),
            ps(p),
            pst(st)
    {}

    [[nodiscard]] HexLyt run()
    {
        // static assertions for layout types
        static_assert(is_gate_level_layout_v<HexLyt>, "HexLyt is not a gate-level layout");
        static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");
        static_assert(has_even_row_hex_arrangement_v<HexLyt>, "HexLyt does not have an even row hexagon arrangement");
        static_assert(is_gate_level_layout_v<CartLyt>, "CartLyt is not a gate-level layout");
        static_assert(is_cartesian_layout_v<CartLyt>, "CartLyt is not a Cartesian layout");

        // ensure the layout uses the correct clocking scheme
        assert(plyt.is_clocking_scheme(clock_name::TWODDWAVE));

        // get Cartesian layout dimensions
        const auto layout_width  = plyt.x() + 1;
        const auto layout_height = plyt.y() + 1;
        const auto layout_depth  = plyt.z();

        // compute hexagonal layout dimensions based on Cartesian dimensions
        const auto hex_height =
            detail::to_hex<CartLyt, HexLyt>({layout_width - 1, layout_height - 1, 0}, layout_height).y;
        const auto hex_width = detail::to_hex<CartLyt, HexLyt>({layout_width - 1, 0, 0}, layout_height).x;
        const auto hex_depth = layout_depth;

        // create the initial hexagonal layout
        HexLyt hex_layout{{hex_width, hex_height, hex_depth}, row_clocking<HexLyt>()};

        // initialize statistics for hexagonalization
        hexagonalization_stats stats{};

        {  // start timing the hexagonalization process
            const mockturtle::stopwatch stop{stats.time_total};

            // calculate horizontal offset for hexagonal layout
            const auto offset = detail::get_offset<HexLyt, CartLyt>(plyt, layout_width, layout_height, ps.input_mode, ps.output_mode);

            // determine the top primary input coordinate
            auto middle_pi = detail::to_hex<CartLyt, HexLyt>({0, 0}, layout_height);

            // determine the bottom primary output coordinate
            auto middle_po = detail::to_hex<CartLyt, HexLyt>({plyt.x(), plyt.y()}, layout_height);

            // vectors to store primary inputs
            std::vector<tile<HexLyt>> left_pis;
            std::vector<tile<HexLyt>> right_pis;

            // vectors to store primary outputs
            std::vector<tile<HexLyt>> left_pos;
            std::vector<tile<HexLyt>> right_pos;

            // map primary inputs to the hexagonal layout
            plyt.foreach_pi(
                [&](const auto& gate)
                {
                    const auto old_coord = plyt.get_tile(gate);
                    // convert Cartesian coordinate to hex coordinate
                    auto hex_coord = detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height);
                    hex_coord.x -= static_cast<decltype(hex_coord.x)>(offset);

                    // create primary input in hex layout
                    hex_layout.create_pi(plyt.get_name(plyt.get_node(old_coord)), hex_coord);

                    // collect PIs to the left and to the right of the middle PI
                    if (old_coord.x == 0 && old_coord.y != 0)
                    {
                        left_pis.push_back(hex_coord);
                    }
                    else if (old_coord.x != 0 && old_coord.y == 0)
                    {
                        right_pis.push_back(hex_coord);
                    }
                });

            // sort primary inputs by y-coordinate for consistency
            std::sort(left_pis.begin(), left_pis.end(), [](const auto& lhs, const auto& rhs) { return lhs.y < rhs.y; });
            std::sort(right_pis.begin(), right_pis.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs.x < rhs.x; });

            // adjust hex layout width if necessary (only if all inputs placed in top row)
            if (ps.input_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
            {
                // adjust offset based on primary inputs in the first row
                const auto num_inputs_right_to_middle_pi = compute_num_inputs_right_to_middle_pi(plyt);

                const auto min_width = middle_pi.x - offset + num_inputs_right_to_middle_pi + 1;
                if (hex_width < min_width)
                {
                    hex_layout.resize({min_width, hex_height, hex_depth});
                }
            }

            // adjust hex layout width if necessary (only if all outputs placed in bottom row)
            if (ps.output_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
            {
                // adjust offset based on primary outputs in the last row
                const auto num_outputs_right_to_middle_po = compute_num_outputs_right_to_middle_po(plyt);

                const auto min_width = middle_po.x - offset + num_outputs_right_to_middle_po + 1;
                if (hex_width < min_width)
                {
                    hex_layout.resize({min_width, hex_height, hex_depth});
                }
            }

            // process internal nodes by iterating diagonally over the Cartesian layout
            for (uint64_t k = 0; k < layout_width + layout_height - 1; ++k)
            {
                for (uint64_t x = 0; x <= k; ++x)
                {
                    const auto y = k - x;

                    // ensure coordinate is within layout bounds
                    if (y < layout_height && x < layout_width)
                    {
                        // iterate through all layers
                        for (uint64_t z = 0; z <= hex_depth; ++z)
                        {
                            // define the current Cartesian tile
                            tile<CartLyt> old_tile{x, y, z};

                            // convert the Cartesian tile to a hexagonal tile and adjust x-coordinate
                            auto hex_tile = detail::to_hex<CartLyt, HexLyt>(old_tile, layout_height);
                            hex_tile.x -= static_cast<decltype(hex_tile.x)>(offset);

                            // skip processing if tile is empty
                            if (plyt.is_empty_tile(old_tile))
                            {
                                continue;
                            }

                            // retrieve node associated with the current tile
                            const auto node = plyt.get_node(old_tile);

                            // skip if node is a primary input
                            if (plyt.is_pi(node))
                            {
                                continue;
                            }

                            // get incoming data flow signals for the tile
                            const auto signals = plyt.incoming_data_flow(old_tile);

                            // process single input signals (buffer or inverter)
                            if (signals.size() == 1)
                            {
                                const auto hex_source = [&]()
                                {
                                    auto t = detail::to_hex<CartLyt, HexLyt>(signals[0], layout_height);
                                    t.x -= static_cast<decltype(t.x)>(offset);
                                    return t;
                                }();

                                // create a hex signal from the source
                                const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_source));

                                // create appropriate gate in hex layout based on node type
                                if (!plyt.is_po(node) && plyt.is_wire(node))
                                {
                                    hex_layout.create_buf(hex_signal, hex_tile);
                                }
                                else if (plyt.is_inv(node))
                                {
                                    hex_layout.create_not(hex_signal, hex_tile);
                                }
                            }
                            else if (signals.size() == 2)
                            {
                                // process two-input gates
                                auto hex_tile_a = detail::to_hex<CartLyt, HexLyt>(signals[0], layout_height);
                                auto hex_tile_b = detail::to_hex<CartLyt, HexLyt>(signals[1], layout_height);

                                // adjust coordinates for offset
                                hex_tile_a.x -= static_cast<decltype(hex_tile_a.x)>(offset);
                                hex_tile_b.x -= static_cast<decltype(hex_tile_b.x)>(offset);

                                // create signals for both inputs
                                const auto hex_signal_a = hex_layout.make_signal(hex_layout.get_node(hex_tile_a));
                                const auto hex_signal_b = hex_layout.make_signal(hex_layout.get_node(hex_tile_b));

                                // create the corresponding gate based on the node's function
                                if (plyt.is_and(node))
                                {
                                    hex_layout.create_and(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_nand(node))
                                {
                                    hex_layout.create_nand(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_or(node))
                                {
                                    hex_layout.create_or(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_nor(node))
                                {
                                    hex_layout.create_nor(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_xor(node))
                                {
                                    hex_layout.create_xor(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_xnor(node))
                                {
                                    hex_layout.create_xnor(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_lt(node))
                                {
                                    hex_layout.create_lt(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_le(node))
                                {
                                    hex_layout.create_le(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_gt(node))
                                {
                                    hex_layout.create_gt(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_ge(node))
                                {
                                    hex_layout.create_ge(hex_signal_a, hex_signal_b, hex_tile);
                                }
                                else if (plyt.is_function(node))
                                {
                                    const auto node_fun = plyt.node_function(node);
                                    hex_layout.create_node({hex_signal_a, hex_signal_b}, node_fun, hex_tile);
                                }
                            }
                        }
                    }
                }
            }

            // map primary outputs to hex layout
            plyt.foreach_po(
                [&](const auto& gate)
                {
                    // get the original Cartesian tile for the output
                    const auto old_coord = plyt.get_tile(plyt.get_node(gate));

                    // get the output signal
                    const auto signal = plyt.incoming_data_flow(old_coord)[0];

                    // convert coordinates to hex format and adjust x-coordinate
                    auto hex_coord = detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height);
                    hex_coord.x -= static_cast<decltype(hex_coord.x)>(offset);
                    auto hex_tile = detail::to_hex<CartLyt, HexLyt>(signal, layout_height);
                    hex_tile.x -= static_cast<decltype(hex_tile.x)>(offset);

                    // create the primary output in the hex layout
                    const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_tile));
                    hex_layout.create_po(hex_signal, plyt.get_name(plyt.get_node(old_coord)), hex_coord);

                    // collect POs to the left and to the right of the middle PO
                    if (old_coord.x != 0 && old_coord.y == plyt.y())
                    {
                        left_pos.push_back(hex_coord);
                    }
                    else if (old_coord.x == plyt.x() && old_coord.y != 0)
                    {
                        right_pos.push_back(hex_coord);
                    }
                });

            std::sort(left_pos.begin(), left_pos.end(), [](const auto& lhs, const auto& rhs) { return lhs.x < rhs.x; });
            std::sort(right_pos.begin(), right_pos.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs.y > rhs.y; });

            if (ps.input_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
            {
                // adjust positions and prepare for routing
                middle_pi.x -= static_cast<decltype(middle_pi.x)>(offset);
                std::vector<extended_routing_objective<HexLyt>> objectives;

                // process PIs from left column of the Cartesian layout
                for (const auto& c : left_pis)
                {
                    tile<HexLyt> fanout;
                    hex_layout.foreach_fanout(hex_layout.get_node(c),
                                              [&](const auto& fout) { fanout = hex_layout.get_tile(fout); });

                    // shift left primary input position
                    middle_pi.x -= 1;

                    extended_routing_objective<HexLyt> obj;
                    obj.source             = middle_pi;
                    obj.target             = fanout;
                    obj.update_first_fanin = false;

                    // collect fan-in signals for the fanout node
                    std::vector<mockturtle::signal<HexLyt>> fins;
                    fins.reserve(2);
                    bool first            = true;
                    bool first_fanin_is_c = false;
                    hex_layout.foreach_fanin(hex_layout.get_node(fanout),
                                             [&](const auto& i)
                                             {
                                                 auto fout = static_cast<tile<HexLyt>>(i);
                                                 if (first)
                                                 {
                                                     first = false;
                                                     if (fout == c)
                                                     {
                                                         first_fanin_is_c = true;
                                                     }
                                                 }
                                                 if (fout != c)
                                                 {
                                                     fins.push_back(hex_layout.make_signal(hex_layout.get_node(fout)));
                                                 }
                                             });

                    hex_layout.move_node(hex_layout.get_node(c), middle_pi);

                    const auto target_node = hex_layout.get_node(fanout);
                    if (hex_layout.is_gt(target_node) || hex_layout.is_ge(target_node) ||
                        hex_layout.is_lt(target_node) || hex_layout.is_le(target_node))
                    {
                        obj.update_first_fanin = first_fanin_is_c;
                    }

                    hex_layout.move_node(hex_layout.get_node(fanout), fanout, fins);

                    objectives.push_back(obj);
                }

                // move back to middle PIs original position
                middle_pi.x += left_pis.size();

                // process PIs from top row of the Cartesian layout (similar to before)
                for (const auto& c : right_pis)
                {
                    tile<HexLyt> fanout;
                    hex_layout.foreach_fanout(hex_layout.get_node(c),
                                              [&](const auto& fout) { fanout = hex_layout.get_tile(fout); });

                    // shift top primary input position
                    middle_pi.x += 1;
                    extended_routing_objective<HexLyt> obj;
                    obj.source             = middle_pi;
                    obj.target             = fanout;
                    obj.update_first_fanin = false;

                    // collect fan-in signals for the fanout node
                    std::vector<mockturtle::signal<HexLyt>> fins;
                    fins.reserve(2);
                    bool first            = true;
                    bool first_fanin_is_c = false;
                    hex_layout.foreach_fanin(hex_layout.get_node(fanout),
                                             [&](const auto& i)
                                             {
                                                 auto fout = static_cast<tile<HexLyt>>(i);
                                                 if (first)
                                                 {
                                                     first = false;
                                                     if (fout == c)
                                                     {
                                                         first_fanin_is_c = true;
                                                     }
                                                 }
                                                 if (fout != c)
                                                 {
                                                     fins.push_back(hex_layout.make_signal(hex_layout.get_node(fout)));
                                                 }
                                             });

                    hex_layout.move_node(hex_layout.get_node(c), middle_pi);

                    const auto target_node = hex_layout.get_node(fanout);
                    if (hex_layout.is_gt(target_node) || hex_layout.is_ge(target_node) ||
                        hex_layout.is_lt(target_node) || hex_layout.is_le(target_node))
                    {
                        obj.update_first_fanin = first_fanin_is_c;
                    }

                    hex_layout.move_node(hex_layout.get_node(fanout), fanout, fins);

                    objectives.push_back(obj);
                }

                // perform routing using A*
                auto layout_obstruct = obstruction_layout<HexLyt>(hex_layout);
                using path           = layout_coordinate_path<decltype(layout_obstruct)>;
                const auto crossings =
                    (hex_depth != 0) &&
                    !(ps.input_mode != hexagonalization_params::hexagonalization_input_output_mode::EXTEND_PLANAR);
                const a_star_params params_astar{crossings};
                using dist = manhattan_distance_functor<decltype(layout_obstruct), uint64_t>;
                using cost = unit_cost_functor<decltype(layout_obstruct), uint8_t>;

                // for each routing objective, find a path and route it
                for (const auto& obj : objectives)
                {
                    const auto new_path =
                        a_star<path>(layout_obstruct, {obj.source, obj.target}, dist(), cost(), params_astar);
                    if (!new_path.empty())
                    {
                        route_path(hex_layout, new_path);
                        for (const auto& t : new_path)
                        {
                            layout_obstruct.obstruct_coordinate(t);
                        }
                        // if the flag is set, re-collect and update fanins
                        if (obj.update_first_fanin)
                        {
                            std::vector<mockturtle::signal<HexLyt>> fins;
                            fins.reserve(2);
                            hex_layout.foreach_fanin(hex_layout.get_node(obj.target),
                                                     [&](const auto& i)
                                                     {
                                                         auto fout = static_cast<tile<HexLyt>>(i);
                                                         fins.push_back(
                                                             hex_layout.make_signal(hex_layout.get_node(fout)));
                                                     });
                            std::reverse(fins.begin(), fins.end());
                            hex_layout.move_node(hex_layout.get_node(obj.target), obj.target, fins);
                        }
                    }
                    else
                    {
                        throw hexagonalization_route_inputs_error("Extending PIs to the top border failed.");
                    }
                }
            }

            if (ps.output_mode != hexagonalization_params::hexagonalization_input_output_mode::NONE)
            {
                // adjust positions and prepare for routing
                middle_po.x -= static_cast<decltype(middle_po.x)>(offset);
                std::vector<extended_routing_objective<HexLyt>> objectives;

                // process PIs from left column of the Cartesian layout
                for (const auto& c : left_pos)
                {
                    tile<HexLyt> fanin;
                    hex_layout.foreach_fanin(hex_layout.get_node(c),
                                              [&](const auto& fin) {
                                                 fanin = static_cast<tile<HexLyt>>(fin); });

                    // shift left primary output position
                    middle_po.x -= 1;

                    extended_routing_objective<HexLyt> obj;
                    obj.source             = fanin;
                    obj.target             = middle_po;
                    obj.update_first_fanin = false;

                    hex_layout.move_node(hex_layout.get_node(c), middle_po);
                    objectives.push_back(obj);
                }

                // move back to middle POs original position
                middle_po.x += left_pos.size();

                // process POs from bottom row of the Cartesian layout (similar to before)
                for (const auto& c : right_pos)
                {
                    tile<HexLyt> fanin;
                    hex_layout.foreach_fanin(hex_layout.get_node(c),
                                             [&](const auto& fin) {
                                                 fanin = static_cast<tile<HexLyt>>(fin); });

                    // shift bottom primary output position
                    middle_po.x += 1;
                    extended_routing_objective<HexLyt> obj;
                    obj.source             = fanin;
                    obj.target             = middle_po;
                    obj.update_first_fanin = false;

                    hex_layout.move_node(hex_layout.get_node(c), middle_po);
                    objectives.push_back(obj);
                }

                // perform routing using A*
                auto layout_obstruct = obstruction_layout<HexLyt>(hex_layout);
                using path           = layout_coordinate_path<decltype(layout_obstruct)>;
                const auto crossings =
                    (hex_depth != 0) &&
                    !(ps.output_mode != hexagonalization_params::hexagonalization_input_output_mode::EXTEND_PLANAR);
                const a_star_params params_astar{crossings};
                using dist = manhattan_distance_functor<decltype(layout_obstruct), uint64_t>;
                using cost = unit_cost_functor<decltype(layout_obstruct), uint8_t>;

                // for each routing objective, find a path and route it
                for (const auto& obj : objectives)
                {
                    const auto new_path =
                        a_star<path>(layout_obstruct, {obj.source, obj.target}, dist(), cost(), params_astar);
                    if (!new_path.empty())
                    {
                        route_path(hex_layout, new_path);
                        for (const auto& t : new_path)
                        {
                            layout_obstruct.obstruct_coordinate(t);
                        }
                    }
                    else
                    {
                        throw hexagonalization_route_inputs_error("Extending POs to the bottom border failed.");
                    }
                }
            }

            // adjust the layout size to its bounding box
            const auto bbox       = bounding_box_2d(hex_layout);
            const auto layout_max = bbox.get_max();
            hex_layout.resize({layout_max.x, layout_max.y, hex_layout.z()});

            // restore original names from the Cartesian layout
            restore_names<CartLyt, HexLyt>(plyt, hex_layout);
        }

        // update statistics if provided
        if (pst != nullptr)
        {
            *pst = stats;
        }

        return hex_layout;
    }

  private:
    /**
     * The 2DDWave-clocked layout to hexagonalize.
     */
    const CartLyt& plyt;
    /**
     * Hexagonalization parameters.
     */
    const hexagonalization_params ps;
    /**
     * Hexagonalization statistics.
     */
    hexagonalization_stats* pst;
};

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
 * @param params Parameters.
 * @param stats Statistics.
 * @return Hexagonal representation of the Cartesian layout.
 */
template <typename HexLyt, typename CartLyt>
[[nodiscard]] HexLyt hexagonalization(const CartLyt& lyt, const hexagonalization_params& params = {},
                                      hexagonalization_stats* stats = nullptr)
{
    detail::hexagonalization_impl<HexLyt, CartLyt> impl(lyt, params, stats);

    return impl.run();
}

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_HEXAGONALIZATION_HPP
