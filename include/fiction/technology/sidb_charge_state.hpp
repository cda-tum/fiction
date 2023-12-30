//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

#include <cstdint>
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

/*
 * Iterator for SiDB charge states. Iterates -1 -> 0 -> +1 -> None by default, and -1 -> 0 -> +1 -> None when reversed.
 */
class sidb_charge_state_iterator : public std::iterator<std::input_iterator_tag, sidb_charge_state>
{
  public:
    explicit sidb_charge_state_iterator(sidb_charge_state charge_state = sidb_charge_state::NEGATIVE,
                                        const bool        reversed     = false) :
            cs{charge_state},
            reverse{reversed}
    {}

    // Check for inequality
    bool operator!=(const sidb_charge_state_iterator& other) const noexcept
    {
        return cs != other.cs;
    }

    // Check for equality
    bool operator==(const sidb_charge_state_iterator& other) const noexcept
    {
        return cs == other.cs;
    }

    sidb_charge_state_iterator begin() const noexcept
    {
        return sidb_charge_state_iterator(cs, reverse);
    }

    sidb_charge_state_iterator end() const noexcept
    {
        return sidb_charge_state_iterator(sidb_charge_state::NONE, reverse);
    }

    // Dereference operator
    sidb_charge_state operator*() const noexcept
    {
        return cs;
    }

    // Pre-increment operator
    sidb_charge_state_iterator& operator++() noexcept
    {
        cs = sign_to_charge_state(charge_state_to_sign(cs) + (reverse ? -1 : 1));
        return *this;
    }

    // Post-increment operator
    sidb_charge_state_iterator operator++(int) noexcept
    {
        sidb_charge_state_iterator temp(*this);
        ++(*this);
        return temp;
    }

  private:
    sidb_charge_state cs;
    const bool        reverse;
};

}  // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
