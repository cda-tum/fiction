//
// Created by Jan Drewniok on 25.07.23.
//

#ifndef FICTION_PHYSICAL_CONSTANTS_HPP
#define FICTION_PHYSICAL_CONSTANTS_HPP

namespace fiction::physical_constants
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
/**
 * The pop stability error is used for physical simulations to avoid floating-point errors.
 */
inline constexpr double POP_STABILITY_ERR = 1E-6;
/**
 * A value of \f$\pi\f$ that is accurate to 11 decimal places.
 */
inline constexpr double PI = 3.14159265359;

}  // namespace fiction::physical_constants

#endif  // FICTION_PHYSICAL_CONSTANTS_HPP
