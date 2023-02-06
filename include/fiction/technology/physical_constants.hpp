//
// Created by Jan Drewniok on 24.11.22.
//

#ifndef FICTION_PHYSICAL_CONSTANTS_HPP
#define FICTION_PHYSICAL_CONSTANTS_HPP

namespace fiction::physical_constants
{

/**
 * The vacuum permittivity \f$ \epsilon_0 \f$ in \f$ F \cdot m^{-1} \f$.
 */
constexpr double EPSILON = 8.854 * 1E-12;
/**
 * The elementary charge \f$ e \f$ in \f$ C \f$.
 */
constexpr double ELECTRIC_CHARGE = 1.602 * 1E-19;
/**
 * The pop stability error is used for physical simulations to avoid floating-point errors.
 */
constexpr double POP_STABILITY_ERR = 1E-6;
/**
 * A value of \f$ \pi \f$ that is accurate to 11 decimal places.
 */
constexpr double PI = 3.14159265359;

}  // namespace fiction::physical_constants

#endif  // FICTION_PHYSICAL_CONSTANTS_HPP
