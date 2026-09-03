/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief The three SiDB charge states and the sets used in base-2 and base-3 simulation.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace fiction::sidb::model
{

/**
 * Charge states of SiDBs.
 */
enum class charge_state : int8_t
{
    NEGATIVE = -1,
    NEUTRAL  = 0,
    POSITIVE = 1,
    NONE     = 127  // assigned when layout cell is empty
};
/**
 * Charge states of SiDBs for the context of base 2 simulation.
 */
static inline const std::vector<charge_state> CHARGE_STATES_BASE_2{charge_state::NEGATIVE, charge_state::NEUTRAL};
/**
 * Charge states of SiDBs for the context of base-3 (full base) simulation.
 */
static inline const std::vector<charge_state> CHARGE_STATES_BASE_3{charge_state::NEGATIVE, charge_state::NEUTRAL,
                                                                   charge_state::POSITIVE};
/**
 * Charge states of SiDBs for a given simulation base number. The full base states are returned for an invalid
 * simulation base.
 *
 * @param base The simulation base number to get the associated SiDB charge states for.
 * @return NEG, NEUT, POS for base 3 (full base), and NEG, NEUT otherwise, associated with base 2 simulation.
 */
[[nodiscard]] inline std::vector<charge_state> charge_states_for_base_number(const uint8_t base) noexcept
{
    return base == 2 ? CHARGE_STATES_BASE_2 : CHARGE_STATES_BASE_3;
}
/**
 * Converts the charge state into an integer (`-1`, `0`, `1`).
 *
 * @param cs SiDB charge state.
 * @return Integer representing the SiDB's charge state.
 */
[[nodiscard]] constexpr int8_t charge_state_to_sign(const charge_state& cs) noexcept
{
    switch (cs)
    {
        case charge_state::NEGATIVE:
        {
            return -1;
        }
        case charge_state::POSITIVE:
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
 * @return charge_state representation of `sg`.
 */
[[nodiscard]] constexpr charge_state sign_to_charge_state(const int8_t sg) noexcept
{
    switch (sg)
    {
        case -1:
        {
            return charge_state::NEGATIVE;
        }
        case 0:
        {
            return charge_state::NEUTRAL;
        }
        case +1:
        {
            return charge_state::POSITIVE;
        }
        default:
        {
            return charge_state::NONE;
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
charge_configuration_to_string(const std::vector<charge_state>& charge_distribution) noexcept
{
    std::stringstream config_str{};

    for (const auto& cs : charge_distribution)
    {
        if (cs == charge_state::NONE)
        {
            continue;
        }

        switch (cs)
        {
            case charge_state::NEGATIVE:
            {
                config_str << '-';

                break;
            }
            case charge_state::NEUTRAL:
            {
                config_str << '0';

                break;
            }
            case charge_state::POSITIVE:
            {
                config_str << '+';

                break;
            }
            case charge_state::NONE:
            {
                break;
            }
        }
    }

    return config_str.str();
}

}  // namespace fiction::sidb::model
