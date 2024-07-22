//
// Created by marcel on 24.07.23.
//

#ifndef FICTION_BDL_INPUT_ITERATOR_HPP
#define FICTION_BDL_INPUT_ITERATOR_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
{
/**
 * Iterator that iterates over all possible input states of a BDL layout. There are \f$2^n\f$ possible input states
 * for an \f$n\f$-input BDL layout, each with a unique input index. The input index is interpreted as a binary number,
 * where the \f$i\f$-th bit represents the input state of the \f$i\f$-th input BDL pair. If the bit is `1`, the lower
 * BDL dot is set and the upper BDL dot removed. If the bit is `0`, the upper BDL dot is removed and the lower BDL dot
 * set. The iterator creates and stores a deep-copy of the given layout. The state enumeration wraps around, i.e., after
 * the last possible input state, the first input state is set again.
 *
 * The iterator satisfies the requirements of `LegacyRandomAccessIterator` and can be used in iterator-based `for`
 * loops.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
class bdl_input_iterator
{
  public:
    /**
     * Standard constructor. It alters the layout to set the first input state, which assigns binary `0` to all input
     * BDL pairs.
     *
     * @param lyt The SiDB BDL layout to iterate over.
     * @param params Parameters for the BDL wire detection.
     */
    explicit bdl_input_iterator(const Lyt& lyt, const detect_bdl_wires_params& params = {}) noexcept :
            layout{lyt.clone()},
            input_pairs{detect_bdl_pairs<Lyt>(lyt, sidb_technology::cell_type::INPUT, params.params_bdl_pairs)},
            input_bdl_wires{detect_bdl_wires<Lyt>(lyt, params, bdl_wire_selection::INPUT)},
            num_inputs{static_cast<uint8_t>(input_pairs.size())},
            input_wire_directions{determine_input_bdl_wire_directions()}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        set_all_inputs();
    }

    /**
     * Constructor with pre-detected input wires and directions. It alters the layout to set the first input state,
     * which assigns binary `0` to all input BDL pairs.
     *
     * @param lyt The SiDB BDL layout to iterate over.
     * @param params Parameters for the BDL wire detection.
     * @param input_wires Pre-detected input BDL wires.
     * @param input_directions Pre-determined directions for the input BDL wires.
     */
    explicit bdl_input_iterator(const Lyt& lyt, const detect_bdl_wires_params& params,
                                const std::vector<bdl_wire<Lyt>>&      input_wires,
                                const std::vector<bdl_wire_direction>& input_directions) noexcept :
            layout{lyt.clone()},
            input_pairs{detect_bdl_pairs<Lyt>(lyt, sidb_technology::cell_type::INPUT, params.params_bdl_pairs)},
            input_bdl_wires{input_wires},
            num_inputs{static_cast<uint8_t>(input_pairs.size())},
            input_wire_directions{input_directions}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        set_all_inputs();
    }
    /**
     * Dereference operator. Returns a reference to the layout with the current input state.
     *
     * @return Reference to the current layout.
     */
    [[nodiscard]] const Lyt& operator*() const noexcept
    {
        return layout;
    }
    /**
     * Prefix increment operator. Sets the next input state.
     *
     * @return Reference to `this`.
     */
    bdl_input_iterator& operator++() noexcept
    {
        ++current_input_index;

        set_all_inputs();

        return *this;
    }
    /**
     * Postfix increment operator. Sets the next input state.
     *
     * @return Copy of `this` before incrementing.
     */
    bdl_input_iterator operator++(int) noexcept
    {
        auto result{*this};

        ++(*this);

        return result;
    }
    /**
     * Addition operator. Computes the input state of the current iterator plus the given integer.
     *
     * @param m The amount of input states to skip.
     * @return The input state of the current iterator plus the given integer.
     */
    [[nodiscard]] bdl_input_iterator operator+(const int m) const noexcept
    {
        auto result{*this};

        result += m;

        return result;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    /**
     * Addition assignment operator. Sets a next input state.
     *
     * @param m The amount of input states to skip.
     * @return Reference to `this`.
     */
    bdl_input_iterator& operator+=(const int m) noexcept
    {
        current_input_index += m;

        set_all_inputs();

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Subtraction operator. Computes the input state of the current iterator minus the given integer.
     *
     * @param m The amount of input states to skip.
     * @return The input state of the current iterator minus the given integer.
     */
    [[nodiscard]] bdl_input_iterator operator-(const int m) const noexcept
    {
        auto result{*this};

        result -= m;

        return result;
    }
    /**
     * Prefix decrement operator. Sets the previous input state.
     *
     * @return Reference to `this`.
     */
    bdl_input_iterator& operator--() noexcept
    {
        --current_input_index;

        set_all_inputs();

        return *this;
    }
    /**
     * Postfix decrement operator. Sets the previous input state.
     *
     * @return Copy of `this` before decrementing.
     */
    bdl_input_iterator operator--(int) noexcept
    {
        auto result{*this};

        --(*this);

        return result;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    /**
     * Subtraction assignment operator. Sets a previous input state.
     *
     * @param m The amount of input states to skip.
     * @return Reference to `this`.
     */
    bdl_input_iterator& operator-=(const int m) noexcept
    {
        current_input_index -= m;

        set_all_inputs();

        return *this;
    }
#pragma GCC diagnostic pop
    /**
     * Assignment operator. Sets the input state to the given integer.
     *
     * @param m The input state to set.
     */
    bdl_input_iterator& operator=(const uint64_t m) noexcept
    {
        current_input_index = m;

        set_all_inputs();

        return *this;
    }
    /**
     * Subscript operator. Computes the input state of the current iterator plus the given integer.
     *
     * @param m The amount of input states to skip.
     * @return The input state of the current iterator plus the given integer.
     */
    [[nodiscard]] bdl_input_iterator operator[](const int m) const noexcept
    {
        return (*this + m);
    }
    /**
     * Subtraction operator. Computes the difference between the current input index and the given iterator ones.
     *
     * @param other Iterator to compute the difference with.
     * @return The difference between the current input index and the given iterator ones.
     */
    [[nodiscard]] int64_t operator-(const bdl_input_iterator& other) const noexcept
    {
        return static_cast<int64_t>(current_input_index) - static_cast<int64_t>(other.current_input_index);
    }
    /**
     * Equality operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator==(const uint64_t m) const noexcept
    {
        return current_input_index == m;
    }
    /**
     * Inequality operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is not equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator!=(const uint64_t m) const noexcept
    {
        return current_input_index != m;
    }
    /**
     * Less-than operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is less than `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator<(const uint64_t m) const noexcept
    {
        return current_input_index < m;
    }
    /**
     * Less-or-equal-than operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is less than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator<=(const uint64_t m) const noexcept
    {
        return current_input_index <= m;
    }
    /**
     * Greater-than operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is greater than `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator>(const uint64_t m) const noexcept
    {
        return current_input_index > m;
    }
    /**
     * Greater-or-equal-than operator. Compares the current input index with the given integer.
     *
     * @param m Integer to compare with.
     * @return `true` if the current input index is greater than or equal to `m`, `false` otherwise.
     */
    [[nodiscard]] bool operator>=(const uint64_t m) const noexcept
    {
        return current_input_index >= m;
    }
    /**
     * Returns the total number of input BDL pairs of the given SiDB gate layout.
     *
     * @return The number of input BDL pairs.
     */
    [[nodiscard]] uint64_t get_number_of_inputs() const noexcept
    {
        return num_inputs;
    }

  private:
    /**
     * The layout to iterate over.
     */
    Lyt layout;
    /**
     * The detected input BDL pairs.
     */
    const std::vector<bdl_pair<cell<Lyt>>> input_pairs;
    /**
     * The detected input BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * The amount of input BDL pairs.
     */
    const uint8_t num_inputs;
    /**
     * Directions of the input BDL wires.
     */
    const std::vector<bdl_wire_direction> input_wire_directions;
    /**
     * The current input index. There are \f$2^n\f$ possible input states for an \f$n\f$-input BDL layout.
     */
    uint64_t current_input_index{0ull};
    /**
     * This function determines the directions of the input bdl wires.
     *
     * @return A vector of `bdl_wire_direction` representing the directions of the input BDL wires.
     */
    [[nodiscard]] std::vector<bdl_wire_direction> determine_input_bdl_wire_directions() const noexcept
    {
        std::vector<bdl_wire_direction> directions{};
        // Reserve space for the directions
        directions.reserve(input_bdl_wires.size());

        std::transform(input_bdl_wires.cbegin(), input_bdl_wires.cend(), std::back_inserter(directions),
                       [](const auto& wire) { return determine_wire_direction<Lyt>(wire); });

        return directions;
    }

    /**
     * Sets all input cells of the layout according to the current input index. The input index is interpreted as a
     * binary number, where the \f$i\f$-th bit represents the input state of the \f$i\f$-th input BDL pair. If the bit
     * is `1`, the lower BDL dot is set and the upper BDL dot removed. If the bit is `0`, the upper BDL dot is removed
     * and the lower BDL dot set.
     */
    void set_all_inputs() noexcept
    {
        for (uint8_t i = 0; i < num_inputs; ++i)
        {
            const auto& input_i = input_pairs[num_inputs - 1 - i];

            if (input_wire_directions[num_inputs - 1 - i] == bdl_wire_direction::NORTH_SOUTH)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    // set input i to 1
                    layout.assign_cell_type(input_i.upper, technology<Lyt>::cell_type::EMPTY);
                    layout.assign_cell_type(input_i.lower, technology<Lyt>::cell_type::INPUT);
                }
                else
                {
                    // set input i to 0
                    layout.assign_cell_type(input_i.upper, technology<Lyt>::cell_type::INPUT);
                    layout.assign_cell_type(input_i.lower, technology<Lyt>::cell_type::EMPTY);
                }
            }
            else
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    // set input i to 1
                    layout.assign_cell_type(input_i.upper, technology<Lyt>::cell_type::INPUT);
                    layout.assign_cell_type(input_i.lower, technology<Lyt>::cell_type::EMPTY);
                }
                else
                {
                    // set input i to 0
                    layout.assign_cell_type(input_i.upper, technology<Lyt>::cell_type::EMPTY);
                    layout.assign_cell_type(input_i.lower, technology<Lyt>::cell_type::INPUT);
                }
            }
        }
    }
};

}  // namespace fiction

// make `bdl_input_iterator` compatible with STL iterator categories
namespace std
{
template <typename Lyt>
struct iterator_traits<fiction::bdl_input_iterator<Lyt>>
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = int64_t;
    using value_type        = Lyt;
};
}  // namespace std

#endif  // FICTION_BDL_INPUT_ITERATOR_HPP
