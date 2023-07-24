//
// Created by marcel on 24.07.23.
//

#ifndef FICTION_BDL_INPUT_ITERATOR_HPP
#define FICTION_BDL_INPUT_ITERATOR_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/types.hpp"

#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
{

/**
 * Iterator that iterates over all possible input states of a BDL layout. There are \f$ 2^n \f$ possible input states
 * for an \f$n\f$-input BDL layout, each with a unique input index. The input index is interpreted as a binary number,
 * where the \f$i\f$-th bit represents the input state of the \f$i\f$-th input BDL pair. If the bit is `1`, the lower
 * BDL dot is set and the upper BDL dot removed. If the bit is `0`, the upper BDL dot is removed and the lower BDL dot
 * set. To this end, the iterator alters the given layout. The state enumeration wraps around, i.e. after the last
 * possible input state, the first input state is set again.
 *
 * The iterator is bidirectional and can be used in iterator-based `for` loops.
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
     * @param params Parameters for the BDL pair detection.
     */
    explicit bdl_input_iterator(Lyt& lyt, const detect_bdl_pairs_params& params = {}) noexcept :
            layout{lyt},
            input_pairs{detect_bdl_pairs(lyt, sidb_technology::cell_type::INPUT, params)},
            num_inputs{static_cast<uint8_t>(input_pairs.size())}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

        set_all_inputs();
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
    /**
     * Dereference operator. Returns a reference to the layout with the current input state.
     *
     * @return Reference to the current layout.
     */
    [[nodiscard]] Lyt& operator*() const noexcept
    {
        return layout;
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

  private:
    /**
     * The layout to iterate over.
     */
    Lyt& layout;
    /**
     * The detected input BDL pairs.
     */
    const std::vector<bdl_pair<Lyt>> input_pairs;
    /**
     * The amount of input BDL pairs.
     */
    const uint8_t num_inputs;
    /**
     * The current input index. There are \f$ 2^n \f$ possible input states for an \f$n\f$-input BDL layout.
     */
    uint64_t current_input_index{0ull};

    /**
     * Sets all input cells of the layout according to the current input index. The input index is interpreted as a
     * binary number, where the \f$i\f$-th bit represents the input state of the \f$i\f$-th input BDL pair. If the bit
     * is `1`, the lower BDL dot is set and the upper BDL dot removed. If the bit is `0`, the upper BDL dot is removed
     * and the lower BDL dot set.
     */
    void set_all_inputs()
    {
        for (uint8_t i = 0; i < num_inputs; ++i)
        {
            const auto& input_i = input_pairs[i];

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
    }
};

}  // namespace fiction

// make `bdl_input_iterator` compatible with STL iterator categories
namespace std
{
template <typename Lyt>
struct iterator_traits<fiction::bdl_input_iterator<Lyt>>
{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Lyt;
};
}  // namespace std

#endif  // FICTION_BDL_INPUT_ITERATOR_HPP
