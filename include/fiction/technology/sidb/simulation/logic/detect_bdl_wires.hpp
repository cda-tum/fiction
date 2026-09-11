/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Links BDL pairs into the input, output, and internal wires of a layout.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/technology.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::logic
{

/**
 * An enumeration of the selection of different types of wires.
 */
enum class bdl_wire_selection : std::uint8_t
{
    /**
     * Select all BDL wires.
     */
    ALL,
    /**
     * Select only BDL wires that start with input dots.
     */
    INPUT,
    /**
     * Select only BDL wires that end with output dots.
     */
    OUTPUT
};

/**
 * This struct encapsulates parameters used for detecting BDL wires.
 */
struct detect_bdl_wires_params
{
    /**
     * A distance threshold, which is used to determine if two pairs of BDLs are part of the same wire. (unit: nm).
     */
    double threshold_bdl_interdistance = 2.0;
    /**
     * Parameters for the `detect_bdl_pairs` algorithm.
     */
    detect_bdl_pairs_params bdl_pairs_params{};
};

/**
 * A BDL wire is a chain of BDL pairs. Wires with fewer than two pairs or only NORMAL pairs have no port;
 * direction updates leave their end pairs unchanged. The vector constructor initializes the end pairs from
 * the first and last supplied pairs. Adding only NORMAL pairs to a default-constructed wire leaves both unset.
 *
 * For other wires, input and output pairs form the endpoints where present. An input-only wire ends at the
 * farthest pair from its input; an output-only wire starts at the farthest pair from its output.
 */
struct bdl_wire
{
    /**
     * The BDL pairs of the wire.
     */
    std::vector<bdl_pair> pairs{};
    /**
     * Port direction of the wire.
     */
    fcn::port_direction port{fcn::port_direction::NONE};
    /**
     * First BDL pair of the wire.
     */
    std::optional<bdl_pair> first_bdl_pair{};
    /**
     * Last BDL pair of the wire.
     */
    std::optional<bdl_pair> last_bdl_pair{};
    /**
     * Constructs an empty wire.
     */
    bdl_wire() noexcept = default;
    /**
     * Constructs a wire from BDL pairs and determines its direction.
     *
     * @param p The BDL pairs of the wire.
     */
    explicit bdl_wire(std::vector<bdl_pair> p) noexcept : pairs(std::move(p))
    {
        if (!pairs.empty())
        {
            first_bdl_pair = pairs.front();
            last_bdl_pair  = pairs.back();
        }

        update_direction();
    }
    /**
     * Adds a BDL pair to the wire, keeps the pairs sorted, and updates the direction.
     *
     * @param pair The BDL pair to add.
     */
    void add_bdl_pair(const bdl_pair& pair) noexcept
    {
        pairs.push_back(pair);
        std::ranges::sort(pairs);
        update_direction();
    }
    /**
     * Removes a BDL pair from the wire and updates the direction if it was present.
     *
     * @param pair The BDL pair to remove.
     */
    void erase_bdl_pair(const bdl_pair& pair) noexcept
    {
        if (std::erase(pairs, pair) > 0)
        {
            update_direction();
        }
    }
    /**
     * Finds the first BDL pair of the given type.
     *
     * @param t The SiDB type to look for.
     * @return The first BDL pair of type `t`, or `std::nullopt` if the wire has none.
     */
    [[nodiscard]] std::optional<bdl_pair> find_bdl_pair_by_type(const dot_tag t) const noexcept
    {
        const auto it = std::ranges::find_if(pairs, [t](const auto& bdl) { return bdl.type == t; });

        if (it != pairs.cend())
        {
            return *it;
        }

        return std::nullopt;
    }
    /**
     * Equality operator. Also provides `operator!=` via `= default`.
     *
     * @param other The other wire to compare with.
     * @return `true` if both wires hold the same pairs, port, and end pairs.
     */
    [[nodiscard]] bool operator==(const bdl_wire& other) const noexcept = default;

  private:
    /**
     * Euclidean distance of two sites in units of lattice indices, ignoring the sublattice index. This is the
     * measure the direction heuristics below rank the pairs of a wire by.
     *
     * @param a First site.
     * @param b Second site.
     * @return The distance.
     */
    [[nodiscard]] static double index_distance(const lattice_site& a, const lattice_site& b) noexcept
    {
        const auto dx = static_cast<double>(a.x) - static_cast<double>(b.x);
        const auto dy = static_cast<double>(a.y) - static_cast<double>(b.y);

        return std::hypot(dx, dy);
    }
    /**
     * Determines the port direction of the wire from the positions of its input and output pairs.
     */
    void update_direction() noexcept
    {
        // a wire with fewer than 2 BDL pairs does not have a port
        if (pairs.size() < 2)
        {
            port.dir = fcn::port_direction::NONE;
            return;
        }

        // a wire without input or output dots does not have a port
        if (std::ranges::all_of(pairs, [](const auto& bdl) { return bdl.type == dot_tag::NORMAL; }))
        {
            port.dir = fcn::port_direction::NONE;
            return;
        }

        const auto input_exists =
            std::ranges::any_of(pairs, [](const auto& bdl) { return bdl.type == dot_tag::INPUT; });
        const auto output_exists =
            std::ranges::any_of(pairs, [](const auto& bdl) { return bdl.type == dot_tag::OUTPUT; });

        // input and output dots are present
        if (input_exists && output_exists)
        {
            const auto input_pair  = find_bdl_pair_by_type(dot_tag::INPUT);
            const auto output_pair = find_bdl_pair_by_type(dot_tag::OUTPUT);

            if (!input_pair.has_value() || !output_pair.has_value())
            {
                port.dir = fcn::port_direction::NONE;
                return;
            }

            first_bdl_pair = *input_pair;
            last_bdl_pair  = *output_pair;

            // determine the port of the wire based on the position of input and output BDL pairs
            if (*input_pair < *output_pair)
            {
                port.dir = fcn::port_direction::SOUTH;

                if (input_pair->lower.y == output_pair->lower.y || input_pair->upper.y == output_pair->upper.y)
                {
                    port.dir = fcn::port_direction::EAST;
                }
            }
            // if the input BDL pair is at the same position as the output BDL pair, the wire has no port
            else if (input_pair->equal_ignore_type(*output_pair))
            {
                assert(false && "input and output BDL pairs are at the same position");
                port.dir = fcn::port_direction::NONE;
            }
            else
            {
                port.dir = fcn::port_direction::NORTH;

                if (input_pair->lower.y == output_pair->lower.y && input_pair->upper.y == output_pair->upper.y)
                {
                    port.dir = fcn::port_direction::WEST;
                }
            }
        }
        // only input dots are present
        else if (input_exists)
        {
            const auto input_pair = find_bdl_pair_by_type(dot_tag::INPUT);

            if (!input_pair.has_value())
            {
                port.dir = fcn::port_direction::NONE;
                return;
            }

            const auto farthest_pair = std::ranges::max_element(
                pairs, {}, [&input_pair](const auto& pair) { return index_distance(pair.lower, input_pair->lower); });

            first_bdl_pair = *input_pair;
            last_bdl_pair  = *farthest_pair;

            // the input and final BDL pairs share the y-coordinate and the input pair is to the left --> EAST
            if (input_pair->lower.x < farthest_pair->lower.x && input_pair->has_same_y_coordinate(*farthest_pair))
            {
                port.dir = fcn::port_direction::EAST;
            }
            // the lower dot of the input BDL pair is below the lower dot of the final BDL pair --> NORTH
            else if (input_pair->lower.y > farthest_pair->lower.y)
            {
                port.dir = fcn::port_direction::NORTH;
            }
            // the input BDL pair is to the right of the final BDL pair --> WEST
            else if (input_pair->lower.x > farthest_pair->lower.x && input_pair->has_same_y_coordinate(*farthest_pair))
            {
                port.dir = fcn::port_direction::WEST;
            }
            else
            {
                port.dir = fcn::port_direction::SOUTH;
            }
        }
        // only output dots are present
        else
        {
            const auto output_pair = find_bdl_pair_by_type(dot_tag::OUTPUT);

            if (!output_pair.has_value())
            {
                port.dir = fcn::port_direction::NONE;
                return;
            }

            const auto farthest_pair = std::ranges::max_element(
                pairs, {}, [&output_pair](const auto& pair) { return index_distance(pair.lower, output_pair->upper); });

            first_bdl_pair = *farthest_pair;
            last_bdl_pair  = *output_pair;

            // the output and first BDL pairs share the y-coordinate and the output pair is to the left --> WEST
            if (output_pair->lower.x < farthest_pair->lower.x && output_pair->has_same_y_coordinate(*farthest_pair))
            {
                port.dir = fcn::port_direction::WEST;
            }
            // the lower dot of the output BDL pair is below the lower dot of the first BDL pair --> SOUTH
            else if (output_pair->lower.y > farthest_pair->lower.y)
            {
                port.dir = fcn::port_direction::SOUTH;
            }
            // the output BDL pair is to the right of the first BDL pair --> EAST
            else if (output_pair->lower.x > farthest_pair->lower.x &&
                     output_pair->has_same_y_coordinate(*farthest_pair))
            {
                port.dir = fcn::port_direction::EAST;
            }
            else
            {
                port.dir = fcn::port_direction::NORTH;
            }
        }
    }
};

namespace detail
{

/**
 * Chains the BDL pairs of a layout into wires.
 */
class detect_bdl_wires_impl
{
  public:
    /**
     * Detects the wires of `lyt`.
     *
     * @param lyt The layout.
     * @param ps Parameters.
     */
    detect_bdl_wires_impl(const layout& lyt, const detect_bdl_wires_params& ps) : lat{lyt.get_lattice()}, params{ps}
    {
        auto bdl_pairs = aggregate_bdl_pairs(lyt);

        while (!bdl_pairs.empty())
        {
            bdl_wire wire{};

            auto current = *bdl_pairs.cbegin();
            wire.add_bdl_pair(current);
            bdl_pairs.erase(current);

            while (true)
            {
                if (const auto below = find_bdl_neighbor_below(current, bdl_pairs); below.has_value())
                {
                    wire.add_bdl_pair(*below);
                    bdl_pairs.erase(*below);
                    current = *below;
                }
                else if (const auto above = find_bdl_neighbor_above(current, bdl_pairs); above.has_value())
                {
                    wire.add_bdl_pair(*above);
                    bdl_pairs.erase(*above);
                    current = *above;
                }
                else
                {
                    bdl_wires.push_back(std::move(wire));
                    break;
                }
            }
        }
    }
    /**
     * Returns the detected wires, optionally restricted to input or output wires.
     *
     * @param selection Which wires to return.
     * @return The selected wires.
     */
    [[nodiscard]] std::vector<bdl_wire> filter_wires(const bdl_wire_selection selection) const noexcept
    {
        switch (selection)
        {
            case bdl_wire_selection::INPUT:
            {
                return filter_wires_by_type(dot_tag::INPUT);
            }
            case bdl_wire_selection::OUTPUT:
            {
                return filter_wires_by_type(dot_tag::OUTPUT);
            }
            default:
            {
                return bdl_wires;
            }
        }
    }

  private:
    /**
     * The layout's lattice.
     */
    const lattice lat;
    /**
     * Parameters.
     */
    const detect_bdl_wires_params params;
    /**
     * The detected wires.
     */
    std::vector<bdl_wire> bdl_wires{};
    /**
     * Finds a neighboring BDL pair above the given one.
     *
     * @param given The given pair.
     * @param bdl_pairs The pairs not assigned to a wire yet.
     * @return A neighboring pair above `given`, if any.
     */
    [[nodiscard]] std::optional<bdl_pair> find_bdl_neighbor_above(const bdl_pair&           given,
                                                                  const std::set<bdl_pair>& bdl_pairs) const
    {
        const auto it = std::ranges::find_if(
            bdl_pairs,
            [this, &given](const auto& bdl)
            {
                return lat.nm_distance(given.lower, bdl.upper) < params.threshold_bdl_interdistance ||
                       (lat.nm_distance(given.upper, bdl.lower) < params.threshold_bdl_interdistance &&
                        !given.equal_ignore_type(bdl) && given > bdl);
            });

        if (it != bdl_pairs.cend())
        {
            return *it;
        }

        return std::nullopt;
    }
    /**
     * Finds a neighboring BDL pair below the given one.
     *
     * @param given The given pair.
     * @param bdl_pairs The pairs not assigned to a wire yet.
     * @return A neighboring pair below `given`, if any.
     */
    [[nodiscard]] std::optional<bdl_pair> find_bdl_neighbor_below(const bdl_pair&           given,
                                                                  const std::set<bdl_pair>& bdl_pairs) const
    {
        const auto it = std::ranges::find_if(
            bdl_pairs,
            [this, &given](const auto& bdl)
            {
                return lat.nm_distance(given.lower, bdl.upper) < params.threshold_bdl_interdistance ||
                       (lat.nm_distance(given.upper, bdl.lower) < params.threshold_bdl_interdistance &&
                        given.not_equal_ignore_type(bdl) && given < bdl);
            });

        if (it != bdl_pairs.cend())
        {
            return *it;
        }

        return std::nullopt;
    }
    /**
     * Returns the wires that contain a pair of the given type. If an input wire also holds output pairs, those
     * are removed from the returned copy, and vice versa. All returned unmixed wires have to be equally long.
     *
     * @param type The SiDB type to filter by.
     * @return The filtered wires.
     */
    [[nodiscard]] std::vector<bdl_wire> filter_wires_by_type(const dot_tag type) const noexcept
    {
        std::vector<bdl_wire> filtered_wires{};

        std::optional<std::size_t> wire_length_of_the_first_wire{};

        std::optional<dot_tag> filtered_out_type{};

        if (type == dot_tag::INPUT)
        {
            filtered_out_type = dot_tag::OUTPUT;
        }
        else if (type == dot_tag::OUTPUT)
        {
            filtered_out_type = dot_tag::INPUT;
        }

        for (const auto& wire : bdl_wires)
        {
            if (!std::ranges::any_of(wire.pairs, [type](const auto& bdl) { return bdl.type == type; }))
            {
                continue;
            }

            if (filtered_out_type.has_value() && std::ranges::any_of(wire.pairs, [&filtered_out_type](const auto& bdl)
                                                                     { return bdl.type == *filtered_out_type; }))
            {
                auto wire_copy = wire;
                std::erase_if(wire_copy.pairs,
                              [&filtered_out_type](const auto& bdl) { return bdl.type == *filtered_out_type; });
                filtered_wires.push_back(std::move(wire_copy));

                continue;
            }

            filtered_wires.push_back(wire);

            if (!wire_length_of_the_first_wire.has_value())
            {
                wire_length_of_the_first_wire = wire.pairs.size();
            }
            else
            {
                assert(*wire_length_of_the_first_wire == wire.pairs.size() &&
                       "input or output wires have different lengths");
            }
        }

        return filtered_wires;
    }
    /**
     * Collects the input, output, and normal BDL pairs of the layout.
     *
     * @param lyt The layout.
     * @return All BDL pairs.
     */
    [[nodiscard]] std::set<bdl_pair> aggregate_bdl_pairs(const layout& lyt) const
    {
        std::set<bdl_pair> bdl_pairs{};

        for (const auto type : {dot_tag::INPUT, dot_tag::OUTPUT, dot_tag::NORMAL})
        {
            const auto pairs = detect_bdl_pairs(lyt, type, params.bdl_pairs_params);
            bdl_pairs.insert(pairs.cbegin(), pairs.cend());
        }

        return bdl_pairs;
    }
};

}  // namespace detail

/**
 * Detects the BDL wires of an SiDB layout: the BDL pairs of all types are chained into wires along the layout's
 * lattice whenever two pairs lie within `params.threshold_bdl_interdistance` of each other.
 *
 * @param lyt The layout.
 * @param params Parameters for the BDL pair and wire detection.
 * @param wire_selection Which wires to return: all, input, or output wires.
 * @return The detected wires.
 */
[[nodiscard]] inline std::vector<bdl_wire>
detect_bdl_wires(const layout& lyt, const detect_bdl_wires_params& params = {},
                 const bdl_wire_selection wire_selection = bdl_wire_selection::ALL)
{
    const detail::detect_bdl_wires_impl p{lyt, params};

    return p.filter_wires(wire_selection);
}

}  // namespace fiction::sidb::simulation::logic
