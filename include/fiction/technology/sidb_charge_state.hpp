//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace fiction
{

/**
 * Charge states of SiDBs.
 */
enum class sidb_charge_state : int8_t
{
    NEGATIVE = -1,
    NEUTRAL  = 0,
    POSITIVE = 1,
    NONE     = 127  // assigned when layout cell is empty
};
/**
 * Converts the charge state into an integer (`-1`, `0`, `1`).
 *
 * @param cs SiDB charge state.
 * @return Integer representing the SiDB's charge state.
 */
[[nodiscard]] inline constexpr int8_t charge_state_to_sign(const sidb_charge_state& cs) noexcept
{
    switch (cs)
    {
        case sidb_charge_state::NEGATIVE:
        {
            return -1;
        }
        case sidb_charge_state::POSITIVE:
        {
            return +1;
        }
        default:
        {
            return 0;
        }
    }
}
/**
 * Converts an integer (`-1`, `0`, `1`) into a charge state.
 *
 * @param sg Integer (`-1`, `0`, `1`) representing a charge state.
 * @return sidb_charge_state representation of `sg`.
 */
[[nodiscard]] inline constexpr sidb_charge_state sign_to_charge_state(const int8_t sg) noexcept
{
    switch (sg)
    {
        case -1:
        {
            return sidb_charge_state::NEGATIVE;
        }
        case 0:
        {
            return sidb_charge_state::NEUTRAL;
        }
        case +1:
        {
            return sidb_charge_state::POSITIVE;
        }
        default:
        {
            return sidb_charge_state::NONE;
        }
    }
}
/**
 * Converts a vector of charge states to a string representation (`"-101..."`).
 *
 * @param charge_distribution A vector of SiDBs charge states.
 * @return A string representation of the charge states.
 */
[[nodiscard]] inline std::string
charge_configuration_to_string(const std::vector<sidb_charge_state>& charge_distribution) noexcept
{
    std::stringstream config_str{};

    for (const auto& cs : charge_distribution)
    {
        if (cs == sidb_charge_state::NONE)
        {
            continue;
        }

        switch (cs)
        {
            case sidb_charge_state::NEGATIVE:
            {
                config_str << '-';

                break;
            }
            case sidb_charge_state::NEUTRAL:
            {
                config_str << '0';

                break;
            }
            case sidb_charge_state::POSITIVE:
            {
                config_str << '+';

                break;
            }
            case sidb_charge_state::NONE:
            {
                break;
            }
        }
    }

    return config_str.str();
}
/**
 * The iteration direction over the SiDB states.
 */
enum class sidb_state_iter_dir
{
    /**
     * To positive iterates "\f$-\to 0\to +\f$".
     */
    TO_POSITIVE,
    /**
     * To negative iterates "\f$+\to 0\to -\f$".
     */
    TO_NEGATIVE
};
/**
 * Iterator for SiDB charge states. Iterates `-1 -> 0 -> +1 -> NONE`.
 */
template <sidb_state_iter_dir dir = sidb_state_iter_dir::TO_POSITIVE>
class sidb_charge_state_iterator
{
  public:
    // Iterator typedefs
    using iterator_category = std::input_iterator_tag;
    using value_type        = sidb_charge_state;
    using difference_type   = std::ptrdiff_t;
    using pointer           = sidb_charge_state*;
    using reference         = sidb_charge_state&;
    /**
     * Constructor of the non-reversed charge state iterator.
     *
     * @param charge_state The charge state to start iterating at.
     */
    explicit sidb_charge_state_iterator(const sidb_charge_state charge_state = sidb_charge_state::NEGATIVE) :
            cs{charge_state}
    {}
    /**
     * Check for equality.
     *
     * @param other Other iterator to compare with.
     * @return `true` if and only if the current charge state is equal to the charge state of the other iterator.
     */
    bool operator==(const sidb_charge_state_iterator& other) const noexcept
    {
        return cs == other.cs;
    }
    /**
     * Check for inequality.
     *
     * @param other Other iterator to compare with.
     * @return `true` if and only if the current charge state differs from the charge state of the other iterator.
     */
    bool operator!=(const sidb_charge_state_iterator& other) const noexcept
    {
        return cs != other.cs;
    }
    /**
     * Iterator to the initial charge state for the non-reversed charge state iteration.
     *
     * @return An iterator to the charge state that this iterator was initialized with.
     */
    [[nodiscard]] sidb_charge_state_iterator begin() const noexcept
    {
        return sidb_charge_state_iterator(cs);
    }
    /**
     * Iterator to the final charge state for the non-reversed charge state iteration.
     *
     * @return An iterator to the `NONE` charge state.
     */
    [[nodiscard]] static sidb_charge_state_iterator end() noexcept
    {
        return sidb_charge_state_iterator{sidb_charge_state::NONE};
    }
    /**
     * Dereference operator.
     *
     * @return The current charge state at the iterator.
     */
    sidb_charge_state operator*() const noexcept
    {
        return cs;
    }
    /**
     * Pre-increment operator. Advances in the order `-1 -> 0 -> +1 -> NONE`.
     *
     * @return The resulting iterator.
     */
    sidb_charge_state_iterator& operator++() noexcept
    {
        if constexpr (dir == sidb_state_iter_dir::TO_POSITIVE)
        {
            cs = sign_to_charge_state(charge_state_to_sign(cs) + int8_t{1});
        }
        else if constexpr (dir == sidb_state_iter_dir::TO_NEGATIVE)
        {
            cs = sign_to_charge_state(charge_state_to_sign(cs) - int8_t{1});
        }
        return *this;
    }
    /**
     * Post-increment operator. Advances in the order `-1 -> 0 -> +1 -> NONE`.
     *
     * @return A copy of `this` before incrementing.
     */
    sidb_charge_state_iterator operator++(int) noexcept
    {
        sidb_charge_state_iterator temp(*this);
        ++(*this);
        return temp;
    }

  private:
    /**
     * Current charge state.
     */
    sidb_charge_state cs;
};

}  // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
