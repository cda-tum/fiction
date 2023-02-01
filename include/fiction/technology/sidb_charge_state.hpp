//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

#include <sstream>
namespace fiction
{
/**
 * Possible SiDB charges.
 */
enum class sidb_charge_state
{
    NONE,  // assigned when layout cell is empty
    POSITIVE,
    NEUTRAL,
    NEGATIVE
};

/**
 * Converts the charge state into int (-1,0,1).
 *
 * @param cs sidb charge state.
 * @return int representing the SiDB's charge state.
 */
[[nodiscard]] constexpr int charge_state_to_sign(const sidb_charge_state& cs) noexcept
{
    switch (cs)
    {
        case sidb_charge_state::POSITIVE: return +1;
        case sidb_charge_state::NEGATIVE: return -1;
        default: return 0;
    }
}

/**
 * Converts the charge state (-1,0,1) into enum.
 *
 * @param sg charge state as integer (-1,0,1).
 * @return sidb_charge_state.
 */
[[nodiscard]] constexpr sidb_charge_state sign_to_charge_state(const int& sg) noexcept
{
    switch (sg)
    {
        case 1: return sidb_charge_state::POSITIVE;
        case -1: return sidb_charge_state::NEGATIVE;
        case 0: return sidb_charge_state::NEUTRAL;
        default: return sidb_charge_state::NONE;
    }
}

/**
 * Converts the vector of charges (enums) to a string (-101...).
 *
 * @param charge_distribution vector of the SiDBs' charge states.
 * @return string of the charge states.
 */
[[nodiscard]] std::string
charge_configuration_to_string(const std::vector<sidb_charge_state>& charge_distribution) noexcept
{
    std::ostringstream config_str;
    for (const auto& cs : charge_distribution)
    {
        if (cs == sidb_charge_state::NONE)
        {
            continue;
        }
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: config_str << "-"; break;
            case sidb_charge_state::NEUTRAL: config_str << "0"; break;
            case sidb_charge_state::POSITIVE: config_str << "+"; break;
            case sidb_charge_state::NONE:; break;
        }
    }
    return config_str.str();
}

}  // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
