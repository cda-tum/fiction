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

#include "fiction/physical_design/path_finding/distance.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"

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
     * Select only BDL wires that start with input cells.
     */
    INPUT,
    /**
     * Select only BDL wires that end with output cells.
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
 * A BDL wire is a chain of BDL pairs. Its port direction follows from the positions of its input and output pairs;
 * a wire with fewer than two pairs or without input and output pairs has no port. The first and last pairs are
 * the input and output pairs where present, and otherwise the pairs at the ends of the chain.
 */
struct bdl_wire
{
    /**
     * The BDL pairs of the wire, sorted.
     */
    std::vector<bdl_pair<lattice_site>> pairs{};
    /**
     * Port direction of the wire.
     */
    fcn::port_direction port{fcn::port_direction::NONE};
    /**
     * First BDL pair of the wire.
     */
    std::optional<bdl_pair<lattice_site>> first_bdl_pair{};
    /**
     * Last BDL pair of the wire.
     */
    std::optional<bdl_pair<lattice_site>> last_bdl_pair{};
    /**
     * Constructs an empty wire.
     */
    bdl_wire() noexcept = default;
    /**
     * Constructs a wire from BDL pairs and determines its direction.
     *
     * @param p The BDL pairs of the wire.
     */
    explicit bdl_wire(std::vector<bdl_pair<lattice_site>> p) noexcept : pairs(std::move(p))
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
    void add_bdl_pair(const bdl_pair<lattice_site>& pair) noexcept
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
    void erase_bdl_pair(const bdl_pair<lattice_site>& pair) noexcept
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
    [[nodiscard]] std::optional<bdl_pair<lattice_site>>
    find_bdl_pair_by_type(const sidb_technology::cell_type t) const noexcept
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

        // a wire without input or output cells does not have a port
        if (std::ranges::all_of(pairs, [](const auto& bdl) { return bdl.type == sidb_technology::cell_type::NORMAL; }))
        {
            port.dir = fcn::port_direction::NONE;
            return;
        }

        const auto input_exists =
            std::ranges::any_of(pairs, [](const auto& bdl) { return bdl.type == sidb_technology::cell_type::INPUT; });
        const auto output_exists =
            std::ranges::any_of(pairs, [](const auto& bdl) { return bdl.type == sidb_technology::cell_type::OUTPUT; });

        // input and output cells are present
        if (input_exists && output_exists)
        {
            first_bdl_pair = find_bdl_pair_by_type(sidb_technology::cell_type::INPUT);
            last_bdl_pair  = find_bdl_pair_by_type(sidb_technology::cell_type::OUTPUT);

            // determine the port of the wire based on the position of input and output BDL pairs
            if (first_bdl_pair.value() < last_bdl_pair)
            {
                port.dir = fcn::port_direction::SOUTH;

                if (first_bdl_pair.value().lower.y == last_bdl_pair.value().lower.y ||
                    first_bdl_pair.value().upper.y == last_bdl_pair.value().upper.y)
                {
                    port.dir = fcn::port_direction::EAST;
                }
            }
            // if the input BDL pair is at the same position as the output BDL pair, the wire has no port
            else if (first_bdl_pair.value().equal_ignore_type(last_bdl_pair.value()))
            {
                assert(false && "input and output BDL pairs are at the same position");
                port.dir = fcn::port_direction::NONE;
            }
            else
            {
                port.dir = fcn::port_direction::NORTH;

                if (first_bdl_pair.value().lower.y == last_bdl_pair.value().lower.y &&
                    first_bdl_pair.value().upper.y == last_bdl_pair.value().upper.y)
                {
                    port.dir = fcn::port_direction::WEST;
                }
            }
        }
        // only input cells are present
        else if (input_exists)
        {
            first_bdl_pair = find_bdl_pair_by_type(sidb_technology::cell_type::INPUT).value();

            auto max_distance = 0.0;

            for (const auto& pair : pairs)
            {
                const auto distance = index_distance(pair.lower, first_bdl_pair.value().lower);

                if (distance > max_distance)
                {
                    max_distance  = distance;
                    last_bdl_pair = pair;
                }
            }

            // the input and final BDL pairs share the y-coordinate and the input pair is to the left --> EAST
            if (first_bdl_pair.value().lower.x < last_bdl_pair.value().lower.x &&
                first_bdl_pair.value().has_same_y_coordinate(last_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::EAST;
            }
            // the lower cell of the input BDL pair is below the lower cell of the final BDL pair --> NORTH
            else if (first_bdl_pair.value().lower.y > last_bdl_pair.value().lower.y)
            {
                port.dir = fcn::port_direction::NORTH;
            }
            // the input BDL pair is to the right of the final BDL pair --> WEST
            else if (first_bdl_pair.value().lower.x > last_bdl_pair.value().lower.x &&
                     first_bdl_pair.value().has_same_y_coordinate(last_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::WEST;
            }
            else
            {
                port.dir = fcn::port_direction::SOUTH;
            }
        }
        // only output cells are present
        else
        {
            last_bdl_pair = find_bdl_pair_by_type(sidb_technology::cell_type::OUTPUT).value();

            auto max_distance = 0.0;

            for (const auto& pair : pairs)
            {
                const auto distance = index_distance(pair.lower, last_bdl_pair.value().upper);

                if (distance > max_distance)
                {
                    max_distance   = distance;
                    first_bdl_pair = pair;
                }
            }

            // the output and first BDL pairs share the y-coordinate and the output pair is to the left --> WEST
            if (last_bdl_pair.value().lower.x < first_bdl_pair.value().lower.x &&
                last_bdl_pair.value().has_same_y_coordinate(first_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::WEST;
            }
            // the lower cell of the output BDL pair is below the lower cell of the first BDL pair --> SOUTH
            else if (last_bdl_pair.value().lower.y > first_bdl_pair.value().lower.y)
            {
                port.dir = fcn::port_direction::SOUTH;
            }
            // the output BDL pair is to the right of the first BDL pair --> EAST
            else if (last_bdl_pair.value().lower.x > first_bdl_pair.value().lower.x &&
                     last_bdl_pair.value().has_same_y_coordinate(first_bdl_pair.value()))
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
                return filter_wires_by_type(sidb_technology::cell_type::INPUT);
            }
            case bdl_wire_selection::OUTPUT:
            {
                return filter_wires_by_type(sidb_technology::cell_type::OUTPUT);
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
    [[nodiscard]] std::optional<bdl_pair<lattice_site>>
    find_bdl_neighbor_above(const bdl_pair<lattice_site>&           given,
                            const std::set<bdl_pair<lattice_site>>& bdl_pairs) const noexcept
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
    [[nodiscard]] std::optional<bdl_pair<lattice_site>>
    find_bdl_neighbor_below(const bdl_pair<lattice_site>&           given,
                            const std::set<bdl_pair<lattice_site>>& bdl_pairs) const noexcept
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
    [[nodiscard]] std::vector<bdl_wire> filter_wires_by_type(const sidb_technology::cell_type type) const noexcept
    {
        std::vector<bdl_wire> filtered_wires{};

        std::optional<std::size_t> wire_length_of_the_first_wire{};

        std::optional<sidb_technology::cell_type> filtered_out_type{};

        if (type == sidb_technology::cell_type::INPUT)
        {
            filtered_out_type = sidb_technology::cell_type::OUTPUT;
        }
        else if (type == sidb_technology::cell_type::OUTPUT)
        {
            filtered_out_type = sidb_technology::cell_type::INPUT;
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
    [[nodiscard]] std::set<bdl_pair<lattice_site>> aggregate_bdl_pairs(const layout& lyt) const noexcept
    {
        std::set<bdl_pair<lattice_site>> bdl_pairs{};

        for (const auto type : {sidb_technology::cell_type::INPUT, sidb_technology::cell_type::OUTPUT,
                                sidb_technology::cell_type::NORMAL})
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
                 const bdl_wire_selection wire_selection = bdl_wire_selection::ALL) noexcept
{
    const detail::detect_bdl_wires_impl p{lyt, params};

    return p.filter_wires(wire_selection);
}

// ---------------------------------------------------------------------------------------------------------------
// Transitional: the template versions below serve the algorithms that still run on SiDB cell-level layouts. They
// disappear once every consumer takes `sidb::layout`.
// ---------------------------------------------------------------------------------------------------------------

/**
 * This struct encapsulates a vector of `bdl_pair` objects, representing the pairs of SiDBs in the BDL wire.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct legacy_bdl_wire
{
    /**
     * Vector of BDL pairs representing the wire.
     */
    std::vector<bdl_pair<cell<Lyt>>> pairs{};
    /**
     * Port of the BDL wire.
     */
    fcn::port_direction port{fcn::port_direction::NONE};
    /**
     * First BDL pair of the wire.
     */
    std::optional<bdl_pair<cell<Lyt>>> first_bdl_pair{};
    /**
     * Last BDL pair of the wire.
     */
    std::optional<bdl_pair<cell<Lyt>>> last_bdl_pair{};
    /**
     * Default constructor for an empty BDL wire.
     */
    legacy_bdl_wire() noexcept
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

        update_direction();
    }

    /**
     * Constructor to initialize the BDL wire with a given vector of BDL pairs.
     *
     * Also updates the start and end BDL pairs based on the given vector.
     *
     * @param p The vector of BDL pairs to initialize the wire with.
     */
    explicit legacy_bdl_wire(const std::vector<bdl_pair<cell<Lyt>>>& p) noexcept : pairs(p)
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

        if (!pairs.empty())
        {
            first_bdl_pair = pairs.front();
            last_bdl_pair  = pairs.back();
        }
        update_direction();
    }

    /**
     * Copy constructor.
     *
     * Creates a new `legacy_bdl_wire` object as a copy of another `legacy_bdl_wire` object.
     *
     * @param other The `legacy_bdl_wire` object to copy from.
     */
    legacy_bdl_wire(const legacy_bdl_wire& other) noexcept :
            pairs(other.pairs),
            port(other.port),
            first_bdl_pair(other.first_bdl_pair),
            last_bdl_pair(other.last_bdl_pair)
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    /**
     * Move constructor.
     *
     * Transfers ownership of the BDL pairs, port, and start/end pairs from another `legacy_bdl_wire` object.
     *
     * @param other The `legacy_bdl_wire` object to move from.
     */
    legacy_bdl_wire(legacy_bdl_wire&& other) noexcept :
            pairs(std::move(other.pairs)),
            port(other.port),
            first_bdl_pair(std::move(other.first_bdl_pair)),
            last_bdl_pair(std::move(other.last_bdl_pair))
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

        other.port.dir = fcn::port_direction::NONE;  // Reset the port of the moved-from object
    }

    /**
     * Move assignment operator.
     *
     * Transfers ownership of the BDL pairs, port, and start/end pairs from another `legacy_bdl_wire` object.
     *
     * @param other The `legacy_bdl_wire` object to move from.
     * @return A reference to the updated object.
     */
    legacy_bdl_wire& operator=(legacy_bdl_wire&& other) noexcept
    {
        if (this != &other)
        {
            pairs          = std::move(other.pairs);
            port           = other.port;
            first_bdl_pair = std::move(other.first_bdl_pair);
            last_bdl_pair  = std::move(other.last_bdl_pair);
            other.port.dir = fcn::port_direction::NONE;  // Reset the port of the moved-from object
        }

        return *this;
    }

    /**
     * Copy assignment operator.
     *
     * Copies the content of another `legacy_bdl_wire` object, including start and end pairs.
     *
     * @param other The `legacy_bdl_wire` object to copy from.
     * @return A reference to the updated object.
     */
    legacy_bdl_wire& operator=(const legacy_bdl_wire& other) noexcept
    {
        if (this != &other)
        {
            pairs          = other.pairs;
            port           = other.port;
            first_bdl_pair = other.first_bdl_pair;
            last_bdl_pair  = other.last_bdl_pair;
        }
        return *this;
    }
    /**
     * Destructor for legacy_bdl_wire.
     *
     */
    ~legacy_bdl_wire() noexcept = default;
    /**
     * Add a BDL pair to the wire.
     *
     * @param pair The BDL pair to add.
     */
    [[maybe_unused]] void add_bdl_pair(const bdl_pair<cell<Lyt>>& pair) noexcept
    {
        pairs.push_back(pair);

        // Sort the BDL pairs by the x-coordinate of the lower SiDB
        std::ranges::sort(pairs);
        update_direction();
    }
    /**
     * Erase a specific BDL pair from the wire.
     *
     * @param pair The BDL pair to remove. The pair is compared using the equality operator (operator==).
     */
    void erase_bdl_pair(const bdl_pair<cell<Lyt>>& pair) noexcept
    {
        // If the pair was found, erase it
        if (std::erase(pairs, pair) > 0)
        {
            update_direction();
        }
    }
    /**
     * Find the first Binary-dot Logic (BDL) pair of a specified type in the wire.
     *
     * @param t Type of BDL pair to search for (`sidb::sidb_technology::cell_type::INPUT`,
     * `sidb::sidb_technology::cell_type::OUTPUT`, etc.).
     * @return Optional containing the first BDL pair with the specified type `t`,
     *         or `std::nullopt` if no such BDL pair is found.
     */
    [[nodiscard]] std::optional<bdl_pair<cell<Lyt>>>
    find_bdl_pair_by_type(const sidb::sidb_technology::cell_type& t) const noexcept
    {
        const auto it = std::ranges::find_if(pairs, [&t](const auto& bdl) { return bdl.type == t; });

        if (it != pairs.cend())
        {
            return *it;  // Return the first BDL pair of the specified type found
        }

        return std::nullopt;  // Return std::nullopt if no BDL pair of the specified type is found
    }

  private:
    /**
     * Update the port of the wire based on the current BDL pairs.
     */
    void update_direction() noexcept
    {
        // a wire with fewer than 2 BDL pairs does not have a port
        if (pairs.size() < 2)
        {
            port.dir = fcn::port_direction::NONE;
            return;
        }

        // a wire without input or output cells does not have a port
        if (std::ranges::all_of(pairs,
                                [](const auto& bdl) { return bdl.type == sidb::sidb_technology::cell_type::NORMAL; }))
        {
            port.dir = fcn::port_direction::NONE;
            return;
        }

        const auto input_exists = std::ranges::any_of(pairs, [](const auto& bdl)
                                                      { return bdl.type == sidb::sidb_technology::cell_type::INPUT; });

        const auto output_exists = std::ranges::any_of(
            pairs, [](const auto& bdl) { return bdl.type == sidb::sidb_technology::cell_type::OUTPUT; });

        // input and output cells are present
        if (input_exists && output_exists)
        {
            first_bdl_pair = find_bdl_pair_by_type(sidb::sidb_technology::cell_type::INPUT);
            last_bdl_pair  = find_bdl_pair_by_type(sidb::sidb_technology::cell_type::OUTPUT);

            // determine the port of the wire based on the position of input and output BDL pairs
            if (first_bdl_pair.value() < last_bdl_pair)
            {
                port.dir = fcn::port_direction::SOUTH;

                if (first_bdl_pair.value().lower.y == last_bdl_pair.value().lower.y ||
                    first_bdl_pair.value().upper.y == last_bdl_pair.value().upper.y)
                {
                    port.dir = fcn::port_direction::EAST;
                }
            }
            // if the input BDL pair is at the same position as the output BDL pair, the wire has no port
            else if (first_bdl_pair.value().equal_ignore_type(last_bdl_pair.value()))
            {
                assert(false && "input and output BDL pairs are at the same position");
                port.dir = fcn::port_direction::NONE;
            }
            else
            {
                port.dir = fcn::port_direction::NORTH;

                if (first_bdl_pair.value().lower.y == last_bdl_pair.value().lower.y &&
                    first_bdl_pair.value().upper.y == last_bdl_pair.value().upper.y)
                {
                    port.dir = fcn::port_direction::WEST;
                }
            }
        }
        // only input cells are present
        else if (input_exists)
        {
            first_bdl_pair = find_bdl_pair_by_type(sidb::sidb_technology::cell_type::INPUT).value();

            auto max_distance = 0.0;

            for (const auto& pair : pairs)
            {
                const auto distance =
                    physical_design::path_finding::euclidean_distance(Lyt{}, pair.lower, first_bdl_pair.value().lower);
                if (distance > max_distance)
                {
                    max_distance  = distance;
                    last_bdl_pair = pair;
                };
            }

            // BDL pairs can be above or below the input and final BDL pairs, but the input and output BDL pairs are on
            // the same y-coordinate --> EAST
            if (first_bdl_pair.value().lower.x < last_bdl_pair.value().lower.x &&
                first_bdl_pair.value().has_same_y_coordinate(last_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::EAST;
            }
            // Lower cell of the input BDL pair is below the lower cell of the final BDL pair --> SOUTH
            else if (first_bdl_pair.value().lower.y > last_bdl_pair.value().lower.y)
            {
                port.dir = fcn::port_direction::NORTH;
            }
            // the input BDL pair is to the right of the final BDL pair --> EAST
            else if (first_bdl_pair.value().lower.x > last_bdl_pair.value().lower.x &&
                     first_bdl_pair.value().has_same_y_coordinate(last_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::WEST;
            }
            else
            {
                port.dir = fcn::port_direction::SOUTH;
            }
        }
        // only output cells are present
        else
        {
            last_bdl_pair = find_bdl_pair_by_type(sidb::sidb_technology::cell_type::OUTPUT).value();

            auto max_distance = 0.0;

            for (const auto& pair : pairs)
            {
                const auto distance =
                    physical_design::path_finding::euclidean_distance(Lyt{}, pair.lower, last_bdl_pair.value().upper);
                if (distance > max_distance)
                {
                    max_distance   = distance;
                    first_bdl_pair = pair;
                };
            }

            // BDL pairs can be above or below the input and final BDL pairs, but the input and output BDL pairs are on
            // the same y-coordinate --> EAST
            if (last_bdl_pair.value().lower.x < first_bdl_pair.value().lower.x &&
                last_bdl_pair.value().has_same_y_coordinate(first_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::WEST;
            }
            // Lower cell of the input BDL pair is below the lower cell of the final BDL pair --> SOUTH
            else if (last_bdl_pair.value().lower.y > first_bdl_pair.value().lower.y)
            {
                port.dir = fcn::port_direction::SOUTH;
            }
            // the input BDL pair is to the right of the final BDL pair --> EAST
            else if (last_bdl_pair.value().lower.x > first_bdl_pair.value().lower.x &&
                     last_bdl_pair.value().has_same_y_coordinate(first_bdl_pair.value()))
            {
                port.dir = fcn::port_direction::EAST;
            }
            else
            {
                port.dir = fcn::port_direction::NORTH;
            };
        }
    }
};

namespace detail
{

template <typename Lyt>
class legacy_detect_bdl_wires_impl
{
  public:
    legacy_detect_bdl_wires_impl(const Lyt& lyt, const detect_bdl_wires_params& ps) : layout{lyt}, params{ps}
    {
        detect_bdl_wires();
    }
    /**
     * This function operates in two main phases:
     *
     * 1. **Aggregation**: It first collects all BDL pairs of the specified types.
     *
     * 2. **Wire Detection**: The function then iterates through the collected BDL pairs:
     *    - For each initial BDL pair, it searches for neighboring BDL pairs located above or below.
     *    - If a neighboring BDL pair is found, it is added to the current wire.
     *    - This process repeats until no additional neighboring BDL pairs can be found.
     *
     * Once a wire is fully constructed, all BDL pairs belonging to that wire are removed from the initial collection.
     * The function then proceeds to detect the next wire using any remaining BDL pairs.
     */
    void detect_bdl_wires() noexcept
    {
        auto bdl_pairs = aggregate_bdl_pairs();

        std::vector<legacy_bdl_wire<Lyt>> wires{};

        while (!bdl_pairs.empty())
        {
            legacy_bdl_wire<Lyt> wire{};

            bool neighbor_bdl_found = true;

            // choose the current bdl pair
            auto current_bdl_pair = *bdl_pairs.cbegin();

            // add current bdl pair to wire
            wire.add_bdl_pair(current_bdl_pair);
            // delete bdl pair
            bdl_pairs.erase(current_bdl_pair);

            while (neighbor_bdl_found)
            {
                // determine a neighbor bdl pair below
                const auto neighbor_below =
                    find_bdl_neighbor_below(current_bdl_pair, bdl_pairs, params.threshold_bdl_interdistance);

                if (neighbor_below.has_value())
                {
                    // add a neighbor bdl pair to wire
                    wire.add_bdl_pair(neighbor_below.value());
                    // delete bdl pair from a set
                    bdl_pairs.erase(neighbor_below.value());
                    // update current bdl pair
                    current_bdl_pair = neighbor_below.value();
                }
                // If no neighbor bdl pair is found below, the existence of a neighbor bdl pair above is checked
                else
                {
                    const auto neighbor_above =
                        find_bdl_neighbor_above(current_bdl_pair, bdl_pairs, params.threshold_bdl_interdistance);

                    if (neighbor_above.has_value())
                    {
                        wire.add_bdl_pair(neighbor_above.value());
                        bdl_pairs.erase(neighbor_above.value());
                        current_bdl_pair = neighbor_above.value();
                    }
                    else
                    {
                        neighbor_bdl_found = false;
                        wires.push_back(wire);
                    }
                }
            }
        }

        bdl_wires = wires;
    }

    /**
     * This function filters the wires from the `bdl_wires` collection based on the current `selection`.
     * If `selection` is set to `bdl_wire_selection::INPUT`, it returns all wires containing an input cell.
     * If `selection` is set to `bdl_wire_selection::OUTPUT`, it returns all wires containing an output cell.
     * If `selection` is set to any other value, it returns all the wires.
     *
     * The function also ensures that all selected wires of the same type have the same length. If wires of different
     * lengths are found, an assertion is triggered.
     *
     * @param selection The type of wires to filter by, specified by the `bdl_wire_selection` enum.
     * @return A vector of filtered `legacy_bdl_wire` objects based on the current selection. If no wires match the
     * selection criteria, an empty vector is returned.
     */
    [[nodiscard]] std::vector<legacy_bdl_wire<Lyt>> filter_wires(const bdl_wire_selection& selection) const noexcept
    {
        switch (selection)
        {
            case bdl_wire_selection::INPUT:
            {
                return filter_wires_by_type(fiction::technology<Lyt>::cell_type::INPUT);
            }

            case bdl_wire_selection::OUTPUT:
            {
                return filter_wires_by_type(fiction::technology<Lyt>::cell_type::OUTPUT);
            }

            default:
            {
                return bdl_wires;
            }
        }
    }

  private:
    /**
     * SiDB cell-level layout.
     */
    const Lyt& layout;
    /**
     * Parameters for detecting BDL wires.
     */
    const detect_bdl_wires_params& params;
    /**
     * All detected BDL wires.
     */
    std::vector<legacy_bdl_wire<Lyt>> bdl_wires;
    /**
     * This function searches for the first Binary-dot Logic (BDL) pair in a given set of BDL pairs that is above a
     * specified BDL pair. The function returns the first BDL pair that meets the following criteria:
     * - The distance between the upper SiDB of the given BDL pair and the lower SiDB of the candidate BDL pair is less
     * than the specified inter-BDL distance.
     * - The candidate BDL pair is not equal to the given BDL pair (ignoring the type of SiDBs).
     * - The y-coordinate of the upper SiDB of the given BDL pair is less than the y-coordinate of the lower SiDB of the
     * candidate BDL pair.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param given_bdl The BDL pair to find a neighbor for.
     * @param bdl_pairs A set of BDL pairs to search within.
     * @param inter_bdl_distance The maximum allowable distance between the lower SiDB of the given BDL pair and the
     * upper SiDB of the potential neighbor BDL pair.
     * @return A std::optional containing the first BDL pair that meets the criteria, or std::nullopt if no such pair is
     * found.
     */
    [[nodiscard]] std::optional<bdl_pair<cell<Lyt>>>
    find_bdl_neighbor_above(const bdl_pair<cell<Lyt>>& given_bdl, const std::set<bdl_pair<cell<Lyt>>>& bdl_pairs,
                            const double inter_bdl_distance) const noexcept
    {
        const auto it = std::ranges::find_if(
            bdl_pairs,
            [&given_bdl, inter_bdl_distance](const bdl_pair<cell<Lyt>>& bdl)
            {
                return sidb::model::nm_distance<Lyt>(Lyt{}, given_bdl.lower, bdl.upper) < inter_bdl_distance ||
                       (sidb::model::nm_distance<Lyt>(Lyt{}, given_bdl.upper, bdl.lower) < inter_bdl_distance &&
                        !given_bdl.equal_ignore_type(bdl) && given_bdl > bdl);
            });

        if (it != bdl_pairs.cend())
        {
            return std::optional<bdl_pair<cell<Lyt>>>(*it);
        }

        return std::nullopt;
    }
    /**
     * This function searches for the first Binary-dot Logic (BDL) pair in a given set of BDL pairs that is below a
     * specified BDL pair. The function returns the first BDL pair that meets the following criteria:
     * - The distance between the lower SiDB of the given BDL pair and the upper SiDB of the candidate BDL pair is less
     * than the specified inter-BDL distance.
     * - The candidate BDL pair is not equal to the given BDL pair (ignoring the type of SiDBs).
     * - The y-coordinate of the lower SiDB of the given BDL pair is less than the y-coordinate of the upper SiDB of the
     * candidate BDL pair.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param given_bdl The BDL pair to find a neighbor for.
     * @param bdl_pairs A set of BDL pairs to search within.
     * @param inter_bdl_distance The maximum allowable distance between the lower SiDB of the given BDL pair and the
     * upper SiDB of the potential neighbor BDL pair.
     * @return A `std::optional` containing the first BDL pair that meets the criteria, or `std::nullopt` if no such
     * pair is found.
     */
    [[nodiscard]] std::optional<bdl_pair<cell<Lyt>>>
    find_bdl_neighbor_below(const bdl_pair<cell<Lyt>>& given_bdl, const std::set<bdl_pair<cell<Lyt>>>& bdl_pairs,
                            const double inter_bdl_distance) const noexcept
    {
        const auto it = std::ranges::find_if(
            bdl_pairs,
            [&given_bdl, inter_bdl_distance](const bdl_pair<cell<Lyt>>& bdl)
            {
                return sidb::model::nm_distance<Lyt>(Lyt{}, given_bdl.lower, bdl.upper) < inter_bdl_distance ||
                       (sidb::model::nm_distance<Lyt>(Lyt{}, given_bdl.upper, bdl.lower) < inter_bdl_distance &&
                        given_bdl.not_equal_ignore_type(bdl) && given_bdl < bdl);
            });

        if (it != bdl_pairs.cend())
        {
            return std::optional<bdl_pair<cell<Lyt>>>(*it);
        }

        return std::nullopt;
    }
    /**
     * This function scans through the `bdl_wires` and selects those containing BDL pair cells of the specified type.
     * It also checks that all selected wires have the same length and triggers an assertion if wires of different
     * lengths are found.
     *
     * @param type The type of the BDL pair cells to filter by.
     * @return A vector of `legacy_bdl_wire` objects containing cells of the specified type. If no such wires are found,
     *         an empty vector is returned.
     */
    [[nodiscard]] std::vector<legacy_bdl_wire<Lyt>>
    filter_wires_by_type(const typename fiction::technology<Lyt>::cell_type& type) const noexcept
    {
        std::vector<legacy_bdl_wire<Lyt>> filtered_wires{};
        std::optional<std::size_t>        wire_length_of_the_first_wire{};  // Track the length of the first wire

        // Filter out the BDL pairs of the opposite type. This is needed if a wire contains both input and output cells.
        // Example: If an input wire is selected, the output cells are removed and vice versa.
        std::optional<typename fiction::technology<Lyt>::cell_type> filtered_out_bdl_pair_type{};
        if (type == fiction::technology<Lyt>::cell_type::INPUT)
        {
            filtered_out_bdl_pair_type = fiction::technology<Lyt>::cell_type::OUTPUT;
        }
        else if (type == fiction::technology<Lyt>::cell_type::OUTPUT)
        {
            filtered_out_bdl_pair_type = fiction::technology<Lyt>::cell_type::INPUT;
        }

        for (const auto& wire : bdl_wires)
        {
            if (std::ranges::any_of(wire.pairs, [&type](const auto& bdl) { return bdl.type == type; }))
            {
                if (filtered_out_bdl_pair_type.has_value())
                {
                    if (std::ranges::any_of(wire.pairs, [&filtered_out_bdl_pair_type](const auto& bdl)
                                            { return bdl.type == filtered_out_bdl_pair_type.value(); }))
                    {
                        auto wire_copy = wire;
                        std::erase_if(wire_copy.pairs, [&filtered_out_bdl_pair_type](const auto& bdl)
                                      { return bdl.type == filtered_out_bdl_pair_type.value(); });
                        filtered_wires.push_back(wire_copy);
                    }
                    else
                    {
                        filtered_wires.push_back(wire);

                        std::size_t current_wire_length = wire.pairs.size();

                        // Store the length of the first wire
                        if (!wire_length_of_the_first_wire)
                        {
                            wire_length_of_the_first_wire = current_wire_length;
                        }
                        else
                        {
                            assert(*wire_length_of_the_first_wire == current_wire_length &&
                                   "input or output wires have different lengths");
                        }
                    }
                }

                else
                {
                    filtered_wires.push_back(wire);

                    std::size_t current_wire_length = wire.pairs.size();

                    // Store the length of the first wire
                    if (!wire_length_of_the_first_wire)
                    {
                        wire_length_of_the_first_wire = current_wire_length;
                    }
                    else
                    {
                        assert(*wire_length_of_the_first_wire == current_wire_length &&
                               "input or output wires have different lengths");
                    }
                }
            }
        }

        return filtered_wires;
    }
    /**
     * Aggregates BDL pairs of specified types into a set.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @return A set of BDL pairs containing all pairs of the specified types.
     */
    [[nodiscard]] std::set<bdl_pair<cell<Lyt>>> aggregate_bdl_pairs() const noexcept
    {
        // Collect BDL pairs of each type
        const auto all_input_bdls  = detect_bdl_pairs(layout, Lyt::cell_type::INPUT, params.bdl_pairs_params);
        const auto all_output_bdls = detect_bdl_pairs(layout, Lyt::cell_type::OUTPUT, params.bdl_pairs_params);
        const auto all_normal_bdls = detect_bdl_pairs(layout, Lyt::cell_type::NORMAL, params.bdl_pairs_params);

        std::set<bdl_pair<cell<Lyt>>> bdl_pairs{};

        // Insert all detected BDL pairs into the set
        bdl_pairs.insert(all_input_bdls.begin(), all_input_bdls.end());
        bdl_pairs.insert(all_output_bdls.begin(), all_output_bdls.end());
        bdl_pairs.insert(all_normal_bdls.begin(), all_normal_bdls.end());

        return bdl_pairs;
    }
};

}  // namespace detail

/**
 * This function identifies BDL wires in a given SiDB cell-level layout by detecting BDL pairs and linking them based on
 * their spatial relationships. The function supports selection of different types of wires, such as input wires, output
 * wires, or all wires.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The SiDB layout to detect BDL wires in.
 * @param params Parameters used for detecting BDL wires.
 * @param wire_selection The type of wires to detect, specified by the `bdl_wire_selection` enum. Default is
 * `bdl_wire_selection::ALL`.
 * @return A vector of BDL wires, where each wire is represented as a vector of BDL pairs.
 */
template <typename Lyt>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::vector<legacy_bdl_wire<Lyt>>
detect_bdl_wires(const Lyt& lyt, const detect_bdl_wires_params& params = {},
                 const bdl_wire_selection wire_selection = bdl_wire_selection::ALL) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::legacy_detect_bdl_wires_impl<Lyt> p{lyt, params};

    return p.filter_wires(wire_selection);
}

}  // namespace fiction::sidb::simulation::logic
