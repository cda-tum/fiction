//
// created by simon on 14.04.23.
//

#ifndef FICTION_HEXAGONALIZATION_HPP
#define FICTION_HEXAGONALIZATION_HPP

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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction
{

/**
 * Exception thrown when an error occurs during moving inputs/outputs to top/bottom border and rerouting.
 */
class hexagonalization_io_pin_routing_error : public std::runtime_error
{
  public:
    /**
     * Constructs a `hexagonalization_io_pin_routing_error` object with the given error message.
     *
     * @param msg The error message describing the error.
     */
    explicit hexagonalization_io_pin_routing_error(const std::string_view& msg) noexcept :
            std::runtime_error(msg.data())
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
    enum io_pin_extension_mode : std::uint8_t
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
    io_pin_extension_mode input_pin_extension = NONE;
    /**
     * Output extension mode. Defaults to none
     */
    io_pin_extension_mode output_pin_extension = NONE;
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
 * Encapsulates a routing objective with fanin update information.
 *
 * This struct specifies a routing objective by defining the source and target coordinates, and it includes
 * a flag that indicates whether the primary input was the first fanin for the corresponding fanout and the fanout gate
 * is asymmetric (e.g., greater than). If the flag is set to true, the fanin signals need to be reordered.
 *
 * @tparam HexLyt The type of the hexagonal layout.
 */
template <typename HexLyt>
struct routing_objective_with_fanin_update_information : public routing_objective<HexLyt>
{
    /**
     * Constructs a routing objective with fanin update information.
     *
     * Initializes the base routing objective with the given source and target coordinates,
     * and sets the update flag based on the provided parameter.
     *
     * @param src The source coordinate of the routing objective.
     * @param tgt The target coordinate of the routing objective.
     * @param update (Optional) A flag that, if true, indicates that the primary input was the first fanin and the
     * fanout gate is asymmetric, which means that the fanin signals need to be reordered. Defaults to false.
     */
    routing_objective_with_fanin_update_information(const coordinate<HexLyt>& src, const coordinate<HexLyt>& tgt,
                                                    const bool update = false) :
            routing_objective<HexLyt>{src, tgt},
            update_first_fanin{update}
    {}
    /**
     * Flag indicating whether the primary input was the first fanin and the fanout gate is asymmetric.
     *
     * If this flag is true, the fanin signals need to be reordered.
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
    const auto adjustment = static_cast<int64_t>(std::ceil(std::floor(half_height) - (static_cast<double>(y) / 2.0)));

    // compute new x coordinate by adding adjustment
    const auto x = cartesian_tile.x + adjustment;

    // return the new hexagonal tile
    return tile<HexLyt>{x, y, cartesian_tile.z};
}

/**
 * This function iterates over all primary inputs in the given Cartesian layout and counts those whose tile is at the
 * western border. Such inputs are considered to be positioned left of the middle primary input when the layout is
 * converted to a hexagonal format.
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
        [&lyt, &num_inputs_left_to_middle_pi](const auto& gate)
        {
            // if the tile is at the western border, it is placed left of the middle PI in the hex layout
            if (const auto coord = lyt.get_tile(gate); coord.x == 0 && coord.y != 0)
            {
                ++num_inputs_left_to_middle_pi;
            }
        });

    return num_inputs_left_to_middle_pi;
}

/**
 * This function iterates over all primary outputs in the given Cartesian layout and counts those whose tiles are at the
 * southern border. Such outputs are considered to be positioned left of the middle primary output when the layout is
 * converted to a hexagonal format.
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
        [&lyt, &num_outputs_left_to_middle_po](const auto& gate)
        {
            // if the tile is at the southern border, it is placed left of the middle PO in the hex layout
            if (const auto coord = lyt.get_tile(lyt.get_node(gate)); coord.x != lyt.x() && coord.y == lyt.y())
            {
                ++num_outputs_left_to_middle_po;
            }
        });

    return num_outputs_left_to_middle_po;
}

/**
 * This function iterates over all primary inputs in the given Cartesian layout and counts those whose tiles are at the
 * northern border. Such inputs are considered to be positioned right of the middle primary input when the layout is
 * converted to a hexagonal format.
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
        [&lyt, &num_inputs_right_to_middle_pi](const auto& gate)
        {
            // if the tile is at the northern border, it is placed right of the middle PI in the hex layout
            if (const auto coord = lyt.get_tile(gate); coord.x != 0 && coord.y == 0)
            {
                ++num_inputs_right_to_middle_pi;
            }
        });

    return num_inputs_right_to_middle_pi;
}

/**
 * This function iterates over all primary outputs in the given Cartesian layout and counts those whose tile is at the
 * eastern border. Such outputs are considered to be positioned right of the middle primary output when the layout is
 * converted to a hexagonal format.
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
        [&lyt, &num_outputs_right_to_middle_po](const auto& gate)
        {
            // if the tile is at the eastern border, it is placed right of the middle PO in the hex layout
            if (const auto coord = lyt.get_tile(lyt.get_node(gate)); coord.x == lyt.x() && coord.y != lyt.y())
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
 * until it has traversed all diagonals. In each iteration, it checks tiles along the diagonal line. If it finds a
 * non-empty tile, it calculates an offset value, which is the number of columns on the left side of the hexagonal
 * layout that will be empty. This offset is based on the leftmost tile that will appear in the hexagonal layout.
 *
 * @tparam HexLyt Even-row hexagonal gate-level layout return type.
 * @tparam CartLyt Input Cartesian gate-level layout type.
 * @param lyt 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
 * @param cartesian_layout_width Width of the Cartesian layout.
 * @param cartesian_layout_height Height of the Cartesian layout.
 * @param input_mode Adjust offset based on PIs relocated to the top row.
 * @param output_mode Adjust offset based on POs relocated to the bottom row.
 * @return positive and negative offset.
 */
template <typename HexLyt, typename CartLyt>
[[nodiscard]] std::pair<uint64_t, uint64_t>
get_offset(const CartLyt& lyt, uint64_t cartesian_layout_width, uint64_t cartesian_layout_height,
           hexagonalization_params::io_pin_extension_mode input_mode,
           hexagonalization_params::io_pin_extension_mode output_mode) noexcept
{
    static_assert(is_cartesian_layout_v<CartLyt>, "CartLyt is not a Cartesian layout");
    static_assert(is_hexagonal_layout_v<HexLyt>, "HexLyt is not a hexagonal layout");

    int64_t    offset               = 0;
    bool       found_non_empty_tile = false;
    const auto total_diagonals      = cartesian_layout_height + cartesian_layout_width - 1;

    // 1) Find the first diagonal that contains at least one non-empty tile
    //    and track the maximum X-coordinate among those tiles (in hex coords).
    for (uint64_t diagonal = 0; diagonal < total_diagonals && !found_non_empty_tile; ++diagonal)
    {
        for (uint64_t row = 0; row < cartesian_layout_height; ++row)
        {
            // 'col' is derived from current diagonal index minus the row.
            if (diagonal >= row)
            {
                const auto col = diagonal - row;
                if (col < cartesian_layout_width)
                {
                    const tile<CartLyt> current_tile{col, cartesian_layout_height - 1 - row};

                    if (!lyt.is_empty_tile(current_tile))
                    {
                        const auto hex_coord = to_hex<CartLyt, HexLyt>(current_tile, cartesian_layout_height);
                        offset               = std::max(offset, static_cast<int64_t>(hex_coord.x));
                        found_non_empty_tile = true;  // We only need the first diagonal that has a non-empty tile
                    }
                }
            }
        }
    }

    // 2) Adjust offset to accommodate primary inputs if required.
    if (input_mode != hexagonalization_params::io_pin_extension_mode::NONE)
    {
        const auto middle_pi                 = detail::to_hex<CartLyt, HexLyt>({0, 0}, cartesian_layout_height);
        const auto num_inputs_left_to_middle = compute_num_inputs_left_to_middle_pi(lyt);
        const auto needed_offset = static_cast<int64_t>(middle_pi.x) - static_cast<int64_t>(num_inputs_left_to_middle);

        offset = std::min(needed_offset, offset);
    }

    // 3) Adjust offset to accommodate primary outputs if required.
    if (output_mode != hexagonalization_params::io_pin_extension_mode::NONE)
    {
        const auto middle_po = detail::to_hex<CartLyt, HexLyt>({lyt.x(), lyt.y()}, cartesian_layout_height);
        const auto num_outputs_left_to_middle = compute_num_outputs_left_to_middle_po(lyt);
        const auto needed_offset = static_cast<int64_t>(middle_po.x) - static_cast<int64_t>(num_outputs_left_to_middle);

        offset = std::min(needed_offset, offset);
    }

    // 4) Convert the signed offset into a pair of positive values indicating how much to add or subtract.
    uint64_t offset_to_add      = 0;
    uint64_t offset_to_subtract = 0;

    if (offset < 0)
    {
        offset_to_add = static_cast<uint64_t>(-offset);
    }
    else
    {
        offset_to_subtract = static_cast<uint64_t>(offset);
    }

    return {offset_to_add, offset_to_subtract};
}

template <typename HexLyt, typename CartLyt>
class hexagonalization_impl
{
  public:
    hexagonalization_impl(const CartLyt& lyt, const hexagonalization_params& p, hexagonalization_stats* st = nullptr) :
            layout(lyt),
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
        assert(layout.is_clocking_scheme(clock_name::TWODDWAVE));

        // get Cartesian layout dimensions
        const auto layout_width  = layout.x() + 1;
        const auto layout_height = layout.y() + 1;
        const auto layout_depth  = layout.z();

        // compute hexagonal layout dimensions based on Cartesian dimensions
        const auto hex_height =
            detail::to_hex<CartLyt, HexLyt>({layout_width - 1, layout_height - 1, 0}, layout_height).y;
        const auto hex_width = detail::to_hex<CartLyt, HexLyt>({layout_width - 1, 0, 0}, layout_height).x;
        auto       hex_depth = layout_depth;

        // if input and/or input pin will get extended and crossings are allowed, add crossing layer
        if (hex_depth == 0 && (ps.input_pin_extension == hexagonalization_params::io_pin_extension_mode::EXTEND ||
                               ps.output_pin_extension == hexagonalization_params::io_pin_extension_mode::EXTEND))
        {
            hex_depth = 1;
        }

        // create the initial hexagonal layout
        HexLyt hex_layout{{hex_width, hex_height, hex_depth}, row_clocking<HexLyt>()};

        // initialize statistics for hexagonalization
        hexagonalization_stats stats{};

        // artificial scope to restrict the lifetime of the stopwatch
        {
            // start timing the hexagonalization process
            const mockturtle::stopwatch stop{stats.time_total};

            // calculate horizontal offset for hexagonal layout
            auto offset_info        = detail::get_offset<HexLyt, CartLyt>(layout, layout_width, layout_height,
                                                                          ps.input_pin_extension, ps.output_pin_extension);
            auto offset_to_add      = offset_info.first;
            auto offset_to_subtract = offset_info.second;

            // determine the top primary input coordinate
            auto middle_pi = detail::to_hex<CartLyt, HexLyt>({0, 0}, layout_height);

            // determine the bottom primary output coordinate
            auto middle_po = detail::to_hex<CartLyt, HexLyt>({layout.x(), layout.y()}, layout_height);

            // vectors to store primary inputs
            std::vector<tile<HexLyt>> left_pis{};
            std::vector<tile<HexLyt>> right_pis{};
            left_pis.reserve(layout.num_pis());
            right_pis.reserve(layout.num_pis());

            // vectors to store primary outputs
            std::vector<tile<HexLyt>> left_pos{};
            std::vector<tile<HexLyt>> right_pos{};
            left_pos.reserve(layout.num_pos());
            right_pos.reserve(layout.num_pos());

            // map primary inputs to the hexagonal layout
            layout.foreach_pi(
                [this, &hex_layout, &left_pis, &right_pis, layout_height, offset_to_add,
                 offset_to_subtract](const auto& gate)
                {
                    const auto old_coord = layout.get_tile(gate);
                    // convert Cartesian coordinate to hex coordinate
                    auto hex_coord = detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height);
                    hex_coord.x += offset_to_add - offset_to_subtract;
                    // create primary input in hex layout
                    hex_layout.create_pi(layout.get_name(layout.get_node(old_coord)), hex_coord);

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
            if (ps.input_pin_extension != hexagonalization_params::io_pin_extension_mode::NONE)
            {
                // adjust offset based on primary inputs in the first row
                const auto num_inputs_right_to_middle_pi = compute_num_inputs_right_to_middle_pi(layout);

                const auto min_width =
                    middle_pi.x + num_inputs_right_to_middle_pi + 2 + offset_to_add - offset_to_subtract;
                if (hex_width < min_width)
                {
                    hex_layout.resize({min_width, hex_height, hex_depth});
                }
            }

            // adjust hex layout width if necessary (only if all outputs placed in bottom row)
            if (ps.output_pin_extension != hexagonalization_params::io_pin_extension_mode::NONE)
            {
                // adjust offset based on primary outputs in the last row
                const auto num_outputs_right_to_middle_po = compute_num_outputs_right_to_middle_po(layout);

                const auto min_width =
                    middle_po.x + num_outputs_right_to_middle_po + 2 + offset_to_add - offset_to_subtract;
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
                            hex_tile.x += offset_to_add - offset_to_subtract;

                            // skip processing if tile is empty
                            if (layout.is_empty_tile(old_tile))
                            {
                                continue;
                            }

                            // retrieve node associated with the current tile
                            const auto node = layout.get_node(old_tile);

                            // skip if node is a primary input
                            if (layout.is_pi(node))
                            {
                                continue;
                            }

                            // get incoming data flow signals for the tile
                            const auto signals = layout.incoming_data_flow(old_tile);

                            // process single input signals (buffer or inverter)
                            if (signals.size() == 1)
                            {
                                const auto hex_source = [&signals, layout_height, offset_to_add, offset_to_subtract]
                                {
                                    auto t = detail::to_hex<CartLyt, HexLyt>(signals[0], layout_height);
                                    t.x += offset_to_add - offset_to_subtract;
                                    return t;
                                }();

                                // create a hex signal from the source
                                const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_source));

                                // create appropriate gate in hex layout based on node type
                                if (!layout.is_po(node))
                                {
                                    [[maybe_unused]] const auto s =
                                        place(hex_layout, hex_tile, layout, node, hex_signal);
                                }
                            }
                            else if (signals.size() == 2)
                            {
                                // process two-input gates
                                auto hex_tile_a = detail::to_hex<CartLyt, HexLyt>(signals[0], layout_height);
                                auto hex_tile_b = detail::to_hex<CartLyt, HexLyt>(signals[1], layout_height);

                                // adjust coordinates for offset
                                hex_tile_a.x += offset_to_add - offset_to_subtract;
                                hex_tile_b.x += offset_to_add - offset_to_subtract;

                                // create signals for both inputs
                                const auto hex_signal_a = hex_layout.make_signal(hex_layout.get_node(hex_tile_a));
                                const auto hex_signal_b = hex_layout.make_signal(hex_layout.get_node(hex_tile_b));

                                [[maybe_unused]] const auto s =
                                    place(hex_layout, hex_tile, layout, node, hex_signal_a, hex_signal_b);
                            }
                        }
                    }
                }
            }

            // map primary outputs to hex layout
            layout.foreach_po(
                [this, &hex_layout, &left_pos, &right_pos, layout_height, offset_to_add,
                 offset_to_subtract](const auto& gate)
                {
                    // get the original Cartesian tile for the output
                    const auto old_coord = layout.get_tile(layout.get_node(gate));

                    // get the output signal
                    const auto signal = layout.incoming_data_flow(old_coord)[0];

                    // convert coordinates to hex format and adjust x-coordinate
                    auto hex_coord = detail::to_hex<CartLyt, HexLyt>(old_coord, layout_height);
                    hex_coord.x += offset_to_add - offset_to_subtract;
                    auto hex_tile = detail::to_hex<CartLyt, HexLyt>(signal, layout_height);
                    hex_tile.x += offset_to_add - offset_to_subtract;

                    // create the primary output in the hex layout
                    const auto hex_signal = hex_layout.make_signal(hex_layout.get_node(hex_tile));
                    hex_layout.create_po(hex_signal, layout.get_name(layout.get_node(old_coord)), hex_coord);

                    // collect POs to the left and to the right of the middle PO
                    if (old_coord.x != layout.x() && old_coord.y == layout.y())
                    {
                        left_pos.push_back(hex_coord);
                    }
                    else if (old_coord.x == layout.x() && old_coord.y != layout.y())
                    {
                        right_pos.push_back(hex_coord);
                    }
                });

            std::sort(left_pos.begin(), left_pos.end(), [](const auto& lhs, const auto& rhs) { return lhs.x > rhs.x; });
            std::sort(right_pos.begin(), right_pos.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs.y > rhs.y; });

            if (ps.input_pin_extension != hexagonalization_params::io_pin_extension_mode::NONE)
            {
                // adjust positions and prepare for routing
                middle_pi.x += offset_to_add - offset_to_subtract;
                std::vector<routing_objective_with_fanin_update_information<HexLyt>> objectives{};
                objectives.reserve(hex_layout.num_pis());

                // process PIs from left column of the Cartesian layout
                for (const auto& c : left_pis)
                {
                    tile<HexLyt> fanout{};
                    hex_layout.foreach_fanout(hex_layout.get_node(c), [&fanout, &hex_layout](const auto& fout)
                                              { fanout = hex_layout.get_tile(fout); });

                    // shift left primary input position
                    middle_pi.x -= 1;

                    routing_objective_with_fanin_update_information<HexLyt> obj(middle_pi, fanout, false);

                    // collect fan-in signals for the fanout node
                    std::vector<mockturtle::signal<HexLyt>> fins{};
                    fins.reserve(2);

                    bool first_fanin_is_c = false;

                    hex_layout.foreach_fanin(
                        hex_layout.get_node(fanout),
                        [&first_fanin_is_c, &fins, &hex_layout, &c, first = true](const auto& i) mutable
                        {
                            const auto fout = static_cast<tile<HexLyt>>(i);

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

                    if (const auto target_node = hex_layout.get_node(fanout);
                        hex_layout.is_gt(target_node) || hex_layout.is_ge(target_node) ||
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
                    tile<HexLyt> fanout{};
                    hex_layout.foreach_fanout(hex_layout.get_node(c), [&fanout, &hex_layout](const auto& fout)
                                              { fanout = hex_layout.get_tile(fout); });

                    // shift top primary input position
                    middle_pi.x += 1;
                    routing_objective_with_fanin_update_information<HexLyt> obj(middle_pi, fanout, false);

                    // collect fan-in signals for the fanout node
                    std::vector<mockturtle::signal<HexLyt>> fins{};
                    fins.reserve(2);

                    bool first_fanin_is_c = false;

                    hex_layout.foreach_fanin(
                        hex_layout.get_node(fanout),
                        [&first_fanin_is_c, &c, &fins, &hex_layout, first = true](const auto& i) mutable
                        {
                            const auto fout = static_cast<tile<HexLyt>>(i);

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

                    if (const auto target_node = hex_layout.get_node(fanout);
                        hex_layout.is_gt(target_node) || hex_layout.is_ge(target_node) ||
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
                const auto crossings = ps.input_pin_extension == hexagonalization_params::io_pin_extension_mode::EXTEND;
                const a_star_params params_astar{crossings};
                using dist = manhattan_distance_functor<decltype(layout_obstruct), uint64_t>;
                using cost = unit_cost_functor<decltype(layout_obstruct), uint8_t>;

                // for each routing objective, find a path and route it
                for (const auto& obj : objectives)
                {
                    auto target        = obj.target;
                    auto update_target = false;

                    // for planar extension, check if target is in the crossing layer
                    if (!crossings && obj.target.z == 1)
                    {
                        target.z      = 0;
                        update_target = true;
                    }

                    if (auto new_path =
                            a_star<path>(layout_obstruct, {obj.source, target}, dist(), cost(), params_astar);
                        !new_path.empty())
                    {
                        // for planar extension, if target is in the crossing layer, update path
                        if (update_target)
                        {
                            new_path.back().z = 1;
                        }

                        route_path(hex_layout, new_path);

                        for (const auto& t : new_path)
                        {
                            layout_obstruct.obstruct_coordinate(t);
                        }
                        // if the flag is set, re-collect and update fanins
                        if (obj.update_first_fanin)
                        {
                            std::vector<mockturtle::signal<HexLyt>> fins{};
                            fins.reserve(2);

                            hex_layout.foreach_fanin(hex_layout.get_node(obj.target),
                                                     [&fins, &hex_layout](const auto& i)
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
                        throw hexagonalization_io_pin_routing_error(
                            fmt::format("After extending PI to tile {} at the top border, rerouting to its fanout at "
                                        "tile {} was not possible with crossings {}",
                                        obj.source, obj.target, crossings ? "enabled" : "disabled"));
                    }
                }
            }

            if (ps.output_pin_extension != hexagonalization_params::io_pin_extension_mode::NONE)
            {
                // adjust positions and prepare for routing
                middle_po.x += offset_to_add - offset_to_subtract;
                std::vector<routing_objective_with_fanin_update_information<HexLyt>> objectives{};
                objectives.reserve(hex_layout.num_pos());

                // process POs from left column of the Cartesian layout
                for (const auto& c : left_pos)
                {
                    tile<HexLyt> fanin{};
                    hex_layout.foreach_fanin(hex_layout.get_node(c),
                                             [&fanin](const auto& fin) { fanin = static_cast<tile<HexLyt>>(fin); });

                    // shift left primary output position
                    middle_po.x -= 1;

                    routing_objective_with_fanin_update_information<HexLyt> obj{fanin, middle_po, false};

                    hex_layout.move_node(hex_layout.get_node(c), middle_po);
                    objectives.push_back(obj);
                }

                // move back to middle POs original position
                middle_po.x += left_pos.size();

                // process POs from bottom row of the Cartesian layout (similar to before)
                for (const auto& c : right_pos)
                {
                    tile<HexLyt> fanin{};
                    hex_layout.foreach_fanin(hex_layout.get_node(c),
                                             [&fanin](const auto& fin) { fanin = static_cast<tile<HexLyt>>(fin); });

                    // shift bottom primary output position
                    middle_po.x += 1;
                    routing_objective_with_fanin_update_information<HexLyt> obj{fanin, middle_po, false};

                    hex_layout.move_node(hex_layout.get_node(c), middle_po);
                    objectives.push_back(obj);
                }

                // perform routing using A*
                auto layout_obstruct = obstruction_layout<HexLyt>(hex_layout);
                using path           = layout_coordinate_path<decltype(layout_obstruct)>;
                const auto crossings =
                    ps.output_pin_extension == hexagonalization_params::io_pin_extension_mode::EXTEND;
                const a_star_params params_astar{crossings};
                using dist = manhattan_distance_functor<decltype(layout_obstruct), uint64_t>;
                using cost = unit_cost_functor<decltype(layout_obstruct), uint8_t>;

                // for each routing objective, find a path and route it
                for (const auto& obj : objectives)
                {
                    auto source        = obj.source;
                    auto update_source = false;

                    // for planar extension, check if source is in the crossing layer
                    if (!crossings && obj.source.z == 1)
                    {
                        source.z      = 0;
                        update_source = true;
                    }

                    if (auto new_path =
                            a_star<path>(layout_obstruct, {source, obj.target}, dist(), cost(), params_astar);
                        !new_path.empty())
                    {
                        // for planar extension, if source or target are in the crossing layer, update path
                        if (update_source)
                        {
                            new_path.front().z = 1;
                        }

                        route_path(hex_layout, new_path);

                        for (const auto& t : new_path)
                        {
                            layout_obstruct.obstruct_coordinate(t);
                        }
                    }
                    else
                    {
                        throw hexagonalization_io_pin_routing_error(
                            fmt::format("After extending PO to tile {} at the bottom border, rerouting from its fanin "
                                        "at tile {} was not possible with crossings {}",
                                        obj.target, obj.source, crossings ? "enabled" : "disabled"));
                    }
                }
            }

            // adjust the layout size to its bounding box
            const auto bbox       = bounding_box_2d(hex_layout);
            const auto layout_max = bbox.get_max();
            hex_layout.resize({layout_max.x, layout_max.y, hex_layout.z()});

            // restore original names from the Cartesian layout
            restore_names<CartLyt, HexLyt>(layout, hex_layout);
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
    const CartLyt& layout;
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
