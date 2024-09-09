//
// Created by Jan Drewniok on 15.07.24.
//

#ifndef FICTION_DETECT_BDL_WIRES_HPP
#define FICTION_DETECT_BDL_WIRES_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <set>
#include <vector>

namespace fiction
{

/**
 * Possible directions of a BDL wire.
 */
enum class bdl_wire_direction
{
    /**
     * Defines the direction of the wire from north to south.
     * The starting point is defined by the position of input cells, or the ending point is defined by the
     * position of output cells.
     *
     * @note A wire running from west to east is interpreted as north-south direction.
     */
    NORTH_SOUTH,
    /**
     * Defines the direction of the wire from south to north.
     * The starting point is defined by the position of input cells, or the ending point is defined by the
     * position of output cells.
     *
     * @note A wire running from east to west is interpreted as south-north direction.
     */
    SOUTH_NORTH,
    /**
     * A wire without input or output cells does not exhibit a direction.
     */
    NO_DIRECTION
};

/**
 * An enumeration of the selection of different types of wires.
 */
enum class bdl_wire_selection
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
 * This struct encapsulates a vector of `bdl_pair` objects, representing the pairs of SiDBs in the BDL wire.
 *
 * @tparam CellType Cell type.
 */
template <typename CellType>
struct bdl_wire
{
    /**
     * Vector of BDL pairs representing the wire.
     */
    std::vector<bdl_pair<CellType>> pairs{};

    /**
     * Direction of the BDL wire.
     */
    bdl_wire_direction direction{bdl_wire_direction::NO_DIRECTION};

    /**
     * Default constructor for an empty BDL wire.
     */
    bdl_wire() noexcept
    {
        update_direction();
    }

    /**
     * Constructor to initialize the BDL wire with a given vector of BDL pairs.
     *
     * @param p The vector of BDL pairs to initialize the wire with.
     */
    explicit bdl_wire(const std::vector<bdl_pair<CellType>>& p) noexcept : pairs(p)
    {
        update_direction();
    }
    /**
     * Copy constructor.
     *
     * Creates a new `bdl_wire` object as a copy of another `bdl_wire` object.
     *
     * @param other The `bdl_wire` object to copy from.
     */
    bdl_wire(const bdl_wire& other) noexcept : pairs(other.pairs), direction(other.direction) {}
    /**
     * Move constructor.
     *
     * Transfers ownership of the BDL pairs and direction from another `bdl_wire` object.
     *
     * @param other The `bdl_wire` object to move from.
     */
    bdl_wire(bdl_wire&& other) noexcept : pairs(std::move(other.pairs)), direction(other.direction)
    {
        other.direction = bdl_wire_direction::NO_DIRECTION;  // Reset the direction of the moved-from object
    }
    /**
     * Move assignment operator.
     *
     * Transfers ownership of the BDL pairs and direction from another `bdl_wire` object.
     *
     * @param other The `bdl_wire` object to move from.
     * @return A reference to the updated object.
     */
    bdl_wire& operator=(bdl_wire&& other) noexcept
    {
        if (this != &other)
        {
            pairs           = std::move(other.pairs);
            direction       = other.direction;
            other.direction = bdl_wire_direction::NO_DIRECTION;  // Reset the direction of the moved-from object
        }
        return *this;
    }

    /**
     * Copy assignment operator.
     *
     * Copies the content of another `bdl_wire` object.
     *
     * @param other The `bdl_wire` object to copy from.
     * @return A reference to the updated object.
     */
    bdl_wire& operator=(const bdl_wire& other) noexcept
    {
        if (this != &other)
        {
            pairs     = other.pairs;
            direction = other.direction;
        }
        return *this;
    }
    /**
     * Add a BDL pair to the wire.
     *
     * @param pair The BDL pair to add.
     */
    [[maybe_unused]] void add_bdl_pair(const bdl_pair<CellType>& pair) noexcept
    {
        pairs.push_back(pair);
        update_direction();
    }
    /**
     * Erase a specific BDL pair from the wire.
     *
     * @param pair The BDL pair to remove. The pair is compared using the equality operator (operator==).
     */
    void erase_bdl_pair(const bdl_pair<CellType>& pair)
    {
        // Find the position of the pair to be removed
        const auto it = std::remove(pairs.cbegin(), pairs.cend(), pair);
        // If the pair was found, erase it
        if (it != pairs.cend())
        {
            pairs.erase(it, pairs.cend());
            update_direction();
        }
    }
    /**
     * Find the first Binary-dot Logic (BDL) pair of a specified type in the wire.
     *
     * @param t Type of BDL pair to search for (`sidb_technology::cell_type::INPUT`,
     * `sidb_technology::cell_type::OUTPUT`, etc.).
     * @return Optional containing the first BDL pair with the specified type `t`,
     *         or `std::nullopt` if no such BDL pair is found.
     */
    [[nodiscard]] std::optional<bdl_pair<CellType>>
    find_bdl_pair_by_type(const sidb_technology::cell_type& t) const noexcept
    {
        const auto it = std::find_if(pairs.cbegin(), pairs.cend(), [&t](const auto& bdl) { return bdl.type == t; });

        if (it != pairs.cend())
        {
            return *it;  // Return the first BDL pair of the specified type found
        }

        return std::nullopt;  // Return std::nullopt if no BDL pair of the specified type is found
    }

  private:
    /**
     * Update the direction of the wire based on the current BDL pairs.
     */
    void update_direction() noexcept
    {
        if (pairs.size() < 2)
        {
            direction = bdl_wire_direction::NO_DIRECTION;
            return;
        }

        if (std::all_of(pairs.cbegin(), pairs.cend(),
                        [](const auto& bdl) { return bdl.type == sidb_technology::cell_type::NORMAL; }))
        {
            direction = bdl_wire_direction::NO_DIRECTION;
            return;
        }

        const auto input_exists = std::any_of(pairs.cbegin(), pairs.cend(), [](const auto& bdl)
                                              { return bdl.type == sidb_technology::cell_type::INPUT; });

        const auto output_exists = std::any_of(pairs.cbegin(), pairs.cend(), [](const auto& bdl)
                                               { return bdl.type == sidb_technology::cell_type::OUTPUT; });

        if (input_exists && output_exists)
        {
            const auto input_bdl  = find_bdl_pair_by_type(sidb_technology::cell_type::INPUT).value();
            const auto output_bdl = find_bdl_pair_by_type(sidb_technology::cell_type::OUTPUT).value();

            if (input_bdl < output_bdl)
            {
                direction = bdl_wire_direction::NORTH_SOUTH;
            }
            else if (input_bdl.equal_ignore_type(output_bdl))
            {
                assert(false && "input and output BDL pairs are at the same position");
                direction = bdl_wire_direction::NO_DIRECTION;
            }
            else
            {
                direction = bdl_wire_direction::SOUTH_NORTH;
            }
        }
        else if (input_exists)
        {
            const auto input_bdl_pair = find_bdl_pair_by_type(sidb_technology::cell_type::INPUT);

            const auto bdl_pair_above_found = std::any_of(
                pairs.cbegin(), pairs.cend(), [&input_bdl_pair](const auto& bdl) { return bdl > input_bdl_pair; });

            direction = bdl_pair_above_found ? bdl_wire_direction::NORTH_SOUTH : bdl_wire_direction::SOUTH_NORTH;
        }
        else
        {
            const auto output_bdl_pair = find_bdl_pair_by_type(sidb_technology::cell_type::OUTPUT);

            const auto bdl_pair_above_found = std::any_of(
                pairs.cbegin(), pairs.cend(), [&output_bdl_pair](const auto& bdl) { return bdl > output_bdl_pair; });

            direction = bdl_pair_above_found ? bdl_wire_direction::SOUTH_NORTH : bdl_wire_direction::NORTH_SOUTH;
        }
    }
};

namespace detail
{

template <typename Lyt>
class detect_bdl_wires_impl
{
  public:
    detect_bdl_wires_impl(const Lyt& lyt, const detect_bdl_wires_params& params, const bdl_wire_selection& selection) :
            lyt{lyt},
            params{params},
            selection{selection}
    {
        detect_bdl_wires();
    }

    /**
     * Function to detect BDL wires.
     */
    void detect_bdl_wires() noexcept
    {
        auto bdl_pairs = aggregate_bdl_pairs();

        std::vector<bdl_wire<cell<Lyt>>> wires{};

        while (!bdl_pairs.empty())
        {
            bdl_wire<cell<Lyt>> wire{};

            bool neighbor_bdl_found = true;

            // choose the current bdl pair
            auto current_bdl_pair = *bdl_pairs.cbegin();
            // store the front bdl pair as an initial bdl pair
            const auto initial_bdl_pair = current_bdl_pair;

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
                    current_bdl_pair = initial_bdl_pair;

                    if (find_bdl_neighbor_above(current_bdl_pair, bdl_pairs, params.threshold_bdl_interdistance)
                            .has_value())
                    {
                        wire.add_bdl_pair(neighbor_below.value());
                        bdl_pairs.erase(neighbor_below.value());
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
     * @return A vector of filtered `bdl_wire` objects based on the current selection. If no wires match the selection
     * criteria, an empty vector is returned.
     */
    [[nodiscard]] std::vector<bdl_wire<cell<Lyt>>> filter_wires() const noexcept
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
     * SiDB cell-level layout.
     */
    const Lyt& lyt;
    /**
     * Parameters for detecting BDL wires.
     */
    const detect_bdl_wires_params& params;
    /**
     * Selection which BDl wires are detected.
     */
    const bdl_wire_selection& selection;
    /**
     * All detected BDL wires.
     */
    std::vector<bdl_wire<cell<Lyt>>> bdl_wires;
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
        const auto it =
            std::find_if(bdl_pairs.cbegin(), bdl_pairs.cend(),
                         [&given_bdl, inter_bdl_distance](const bdl_pair<cell<Lyt>>& bdl)
                         {
                             return sidb_nm_distance<Lyt>(Lyt{}, given_bdl.upper, bdl.lower) < inter_bdl_distance &&
                                    !given_bdl.equal_ignore_type(bdl) && given_bdl > bdl;
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
        const auto it =
            std::find_if(bdl_pairs.cbegin(), bdl_pairs.cend(),
                         [&given_bdl, inter_bdl_distance](const bdl_pair<cell<Lyt>>& bdl)
                         {
                             return sidb_nm_distance<Lyt>(Lyt{}, given_bdl.lower, bdl.upper) < inter_bdl_distance &&
                                    given_bdl.not_equal_ignore_type(bdl) && given_bdl < bdl;
                         });

        if (it != bdl_pairs.cend())
        {
            return std::optional<bdl_pair<cell<Lyt>>>(*it);
        }

        return std::nullopt;
    }
    /**
     * This function scans through the `bdl_wires` and selects those containing a cell of the specified type.
     * It also checks that all selected wires have the same length and triggers an assertion if wires of different
     * lengths are found.
     *
     * @param type The type of the cell to filter by.
     * @return A vector of `bdl_wire` objects containing cells of the specified type. If no such wires are found,
     *         an empty vector is returned.
     */
    [[nodiscard]] std::vector<bdl_wire<cell<Lyt>>>
    filter_wires_by_type(const typename technology<Lyt>::cell_type& type) const noexcept
    {
        std::vector<bdl_wire<cell<Lyt>>> filtered_wires{};
        std::set<uint64_t>               lengths_of_filtered_wires{};

        for (const auto& wire : bdl_wires)
        {
            if (std::any_of(wire.pairs.cbegin(), wire.pairs.cend(),
                            [&type](const auto& bdl) { return bdl.type == type; }))
            {
                filtered_wires.push_back(wire);
                lengths_of_filtered_wires.insert(wire.pairs.size());
            }
        }

        if (!filtered_wires.empty())
        {
            assert(lengths_of_filtered_wires.size() < 2 && "input or output wires have different lengths");
        }

        return filtered_wires;
    }
    /**
     * Aggregates BDL pairs of specified types into a set.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param lyt Layout type used for detection.
     * @param params Parameters containing BDL pairs.
     * @return A set of BDL pairs containing all pairs of the specified types.
     */
    [[nodiscard]] std::set<bdl_pair<cell<Lyt>>> aggregate_bdl_pairs() const noexcept
    {
        // Collect BDL pairs of each type
        const auto all_input_bdls  = detect_bdl_pairs(lyt, Lyt::cell_type::INPUT, params.bdl_pairs_params);
        const auto all_output_bdls = detect_bdl_pairs(lyt, Lyt::cell_type::OUTPUT, params.bdl_pairs_params);
        const auto all_normal_bdls = detect_bdl_pairs(lyt, Lyt::cell_type::NORMAL, params.bdl_pairs_params);

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
[[nodiscard]] std::vector<bdl_wire<cell<Lyt>>>
detect_bdl_wires(const Lyt& lyt, const detect_bdl_wires_params& params = {},
                 const bdl_wire_selection wire_selection = bdl_wire_selection::ALL) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::detect_bdl_wires_impl<Lyt> p{lyt, params, wire_selection};

    return p.filter_wires();
}

}  // namespace fiction

#endif  // FICTION_DETECT_BDL_WIRES_HPP
