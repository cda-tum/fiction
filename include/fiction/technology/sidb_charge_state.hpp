//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

#include <cassert>
#include <cstdint>
#include <sstream>
#include <vector>

namespace fiction
{

/**
 * Charge states of SiDBs.
 */
enum class sidb_charge_state
{
    NONE,  // assigned when layout cell is empty
    POSITIVE,
    NEUTRAL,
    NEGATIVE
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
        case sidb_charge_state::POSITIVE:
        {
            return +1;
        }
        case sidb_charge_state::NEGATIVE:
        {
            return -1;
        }
        default:
        {
            return 0;
        }
    }
}
/**
 * Converts a charge state (-1,0,1) into an enum.
 *
 * @param sg Charge state as integer (-1,0,1).
 * @return sidb_charge_state representation of `sg`.
 */
[[nodiscard]] inline constexpr sidb_charge_state sign_to_charge_state(const int8_t sg) noexcept
{
    assert(((sg == -1) || (sg == 0) || (sg == 1)) && "Invalid charge state.");

    switch (sg)
    {
        case 1:
        {
            return sidb_charge_state::POSITIVE;
        }
        case -1:
        {
            return sidb_charge_state::NEGATIVE;
        }
        case 0:
        {
            return sidb_charge_state::NEUTRAL;
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
                config_str << "-";

                break;
            }
            case sidb_charge_state::NEUTRAL:
            {
                config_str << "0";

                break;
            }
            case sidb_charge_state::POSITIVE:
            {
                config_str << "+";

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

}  // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
