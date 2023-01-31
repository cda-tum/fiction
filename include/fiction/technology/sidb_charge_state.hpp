//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

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
        case sidb_charge_state::POSITIVE:
            return +1;
        case sidb_charge_state::NEGATIVE:
            return -1;
        default:
            return 0;
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
    switch (sg) {
        case 1:
            return sidb_charge_state::POSITIVE;
        case -1:
            return sidb_charge_state::NEGATIVE;
        case 0:
            return sidb_charge_state::NEUTRAL;
        default:
            return sidb_charge_state::NONE;
    }
}

} // namespace fiction

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
