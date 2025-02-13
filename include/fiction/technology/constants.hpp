//
// Created by Jan Drewniok on 25.07.23.
//

#ifndef FICTION_CONSTANTS_HPP
#define FICTION_CONSTANTS_HPP

namespace fiction::constants
{

/**
 * A value of \f$\pi\f$ that is accurate to 11 decimal places.
 */
inline constexpr double PI = 3.14159265359;

/**
 * This error margin is used for the comparison of floating-point numbers.
 */
inline constexpr double ERROR_MARGIN = 1E-6;

namespace physical
{
/**
 * The vacuum permittivity \f$\epsilon_0\f$ in \f$F \cdot m^{-1}\f$.
 */
constexpr double EPSILON = 8.8541878 * 1E-12;
/**
 * The elementary charge \f$e\f$ in \f$C\f$.
 */
constexpr double ELEMENTARY_CHARGE = 1.6021766 * 1E-19;
/**
 * The Coulomb constant \f$k\f$ in \f$N \cdot m^{2} \cdot C^{-2}\f$.
 */
constexpr double K_E = 8.987552 * 1E9;

}  // namespace physical

}  // namespace fiction::constants

#endif  // FICTION_CONSTANTS_HPP
