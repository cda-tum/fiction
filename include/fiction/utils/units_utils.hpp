//
// Created by Jan Drewniok on 24.11.22.
//

#ifndef FICTION_UNITS_UTILS_HPP
#define FICTION_UNITS_UTILS_HPP

#include <units.h>

namespace units
{
UNIT_ADD(energy, electron_volt, electron_volts, eV, units::unit<std::ratio<1, 6'242'000'000'000'000'000>, joule>);
UNIT_ADD(charge, elementary_charge, elementary_charges, e,
         units::unit<std::ratio<1, 6'242'000'000'000'000'000>, coulomb>);
}  // namespace units

namespace fiction
{
/**
 * Can be used in combination with a number to generate an Ångström unit (e.g. 40.3 Å can be defined as 40_angstrom (is
 * of type units::length::angstrom)).
 */
using units::literals::operator""_angstrom;
/**
 * Can be used in combination with an integer to generate a charge in units of the elementary charge (e.g. -1 e can be
 * defined as -1_e (is of type units::charge::elementary_charge)).
 */
using units::literals::operator""_e;
/**
 * Can be used in combination with a number to generate an electron volt unit (e.g. 40.1 eV can be defined as 40.1_eV
 * (is of type units::energy::electron_volt)).
 */
using units::literals::operator""_eV;
/**
 * Can be used in combination with a number to generate a Kelvin unit (e.g. 40.1 K can be defined as 40.1_K (is of type
 * units::temperature::kelvin)).
 */
using units::literals::operator""_K;
/**
 * Can be used in combination with a number to generate a nanometer unit (e.g. 40.1 nm can be defined as 40.1_nm (is of
 * type units::length::nanometer)).
 */
using units::literals::operator""_nm;
/**
 * Can be used in combination with a number to generate a second unit (e.g. 40.1 s can be defined as 40.1_s (is of type
 * units::time::second)).
 */
using units::literals::operator""_s;
/**
 * Can be used in combination with a number to generate a Voltage unit (e.g. 40.1 V can be defined as 40.1_V (is of type
 * units::voltage::volt)).
 */
using units::literals::operator""_V;
/**
 *  Unit of the Coulomb constant (\f$ N \cdot m^{2} \cdot C^{-2} \f$).
 */
using coulomb_constant_unit =
    units::unit_t<units::compound_unit<units::force::newtons, units::area::square_meter,
                                       units::inverse<units::squared<units::charge::coulomb>>>>;
/**
 * The pop stability error is used for physical simulations to avoid floating-point errors.
 */
inline constexpr double POP_STABILITY_ERR = 1E-6;

}  // namespace fiction

#endif  // FICTION_UNITS_UTILS_HPP
