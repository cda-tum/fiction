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
 * Charge states of SiDBs for the context of base 2 simulation.
 */
static inline const std::vector<sidb_charge_state> SIDB_CHARGE_STATES_BASE_2{sidb_charge_state::NEGATIVE,
                                                                             sidb_charge_state::NEUTRAL};
/**
 * Charge states of SiDBs for the context of base 3 (full base) simulation.
 */
static inline const std::vector<sidb_charge_state> SIDB_CHARGE_STATES_BASE_3{
    sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL, sidb_charge_state::POSITIVE};
/**
 * Charge states of SiDBs for a given simulation base. The full base states are returned for an invalid simulation base.
 */
static inline auto SIDB_CHARGE_STATES = [b2 = SIDB_CHARGE_STATES_BASE_2, b3 = SIDB_CHARGE_STATES_BASE_3](
                                            const uint8_t base) { return base == 2 ? b2 : b3; };
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

}  // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
