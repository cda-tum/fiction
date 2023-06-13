//
// Created by Jan Drewniok on 24.11.22.
//

#ifndef FICTION_PHYSICAL_CONSTANTS_AND_EV_UNIT_HPP
#define FICTION_PHYSICAL_CONSTANTS_AND_EV_UNIT_HPP

#include <units.h>

namespace units
{
UNIT_ADD(energy, electron_volt, electron_volts, eV, units::unit<std::ratio<1, 6'242'000'000'000'000'000>, joule>);
}

namespace fiction::physical_constants
{
/**
 * The pop stability error is used for physical simulations to avoid floating-point errors.
 */
constexpr double POP_STABILITY_ERR = 1E-6;

}  // namespace fiction::physical_constants

#endif  // FICTION_PHYSICAL_CONSTANTS_AND_EV_UNIT_HPP
