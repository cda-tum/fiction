//
// Created by Jan Drewniok on 08.12.22.
//

#ifndef FICTION_SIDB_CHARGE_STATE_HPP
#define FICTION_SIDB_CHARGE_STATE_HPP

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

#endif  // FICTION_SIDB_CHARGE_STATE_HPP
