//
// Created by Jan Drewniok on 25.07.23.
//

#ifndef FICTION_TECHNOLOGY_FCN_CONSTANTS_HPP
#define FICTION_TECHNOLOGY_FCN_CONSTANTS_HPP

namespace fiction::fcn::constants
{

/**
 * This error margin is used for the comparison of floating-point numbers.
 */
inline constexpr double ERROR_MARGIN = 1E-6;

namespace physical
{

/**
 * The elementary charge \f$e\f$ in \f$C\f$.
 */
inline constexpr double ELEMENTARY_CHARGE = 1.6021766 * 1E-19;
/**
 * The Coulomb constant \f$k\f$ in \f$N \cdot m^{2} \cdot C^{-2}\f$.
 */
inline constexpr double K_E = 8.987552 * 1E9;
/**
 * The Boltzmann constant \f$k_B\f$ in \f$J \cdot K^{-1}\f$.
 */
inline constexpr double BOLTZMANN_CONSTANT = 1.38064852 * 1E-23;
/**
 * The conversion factor from electronvolts to joules.
 */
inline constexpr double EV_TO_JOULE = ELEMENTARY_CHARGE;

}  // namespace physical

}  // namespace fiction::fcn::constants

#endif  // FICTION_TECHNOLOGY_FCN_CONSTANTS_HPP
