//
// Created by marcel on 07.03.22.
//

#ifndef FICTION_SIDB_DEFECTS_HPP
#define FICTION_SIDB_DEFECTS_HPP

#include <cstdint>
#include <utility>

namespace fiction
{

/**
 * Specifies the types of fabrication defects that can occur on the H-Si(100) 2x1 surface according to \"Atomic defect
 * classification of the H–Si(100) surface through multi-mode scanning probe microscopy\" by Jeremiah Croshaw, Thomas
 * Dienel, Taleana Huff, and Robert Wolkow in Journal of Nanotechnology in 2020.
 */
enum class sidb_defect_type
{
    NONE,              // H-Si
    DB,                // an extra dangling bond
    SI_VACANCY,        // missing silicon
    SINGLE_DIHYDRIDE,  // double hydrogen passivation
    DIHYDRIDE_PAIR,    // missing bond between dimers leading to two double hydrogen passivations
    ONE_BY_ONE,        // collection of dihydride pairs
    THREE_BY_ONE,      // collection of 1 by 1's
    SILOXANE,          // oxidized dimer
    RAISED_SI,         // raised silicon dimer
    MISSING_DIMER,     // dimer missing altogether
    ETCH_PIT,          // collection of missing dimers
    STEP_EDGE,         // break in the surface
    GUNK,              // residual material
    UNKNOWN            // unknown defect
};
/**
 * In accordance with the paper mentioned above, the sidb_defect struct is used to represent a specific defect on the
 * H-Si(100) 2x1 surface that has a charge as well as relative permittivity (`epsilon_r`) and Thomas-Fermi screening
 * distance (`lambda_tf`) values associated to it.
 *
 * See \"SiQAD: A Design and Simulation Tool for Atomic Silicon Quantum Dot Circuits\" by S. S. H. Ng, J. Retallick, H.
 * N. Chiu, R. Lupoiu, L. Livadaru, T. Huff, M. Rashidi, W. Vine, T. Dienel, R. A. Wolkow, and K. Walus in IEEE
 * Transactions on Nanotechnology for more details on these values.
 */
struct sidb_defect
{
    /**
     * Standard constructor.
     */
    constexpr explicit sidb_defect(const sidb_defect_type defect_type = sidb_defect_type::UNKNOWN,
                                   const double electric_charge = 0.0, const double relative_permittivity = 0.0,
                                   const double screening_distance = 0.0) noexcept :
            type{defect_type},
            charge{electric_charge},
            epsilon_r{relative_permittivity},
            lambda_tf{screening_distance}
    {}
    /**
     * Type of defect.
     */
    const sidb_defect_type type;
    /**
     * Electrical charge.
     */
    const double charge;
    /**
     * Electric permittivity.
     */
    const double epsilon_r;
    /**
     * Thomas-Fermi screening distance in nm.
     */
    const double lambda_tf;
};
/**
 * This operator compares two sidb_defect instances for equality. It checks if the type, charge,
 * epsilon_r, and lambda_tf members of the two instances are equal.
 *
 * @param lhs The left-hand side sidb_defect instance.
 * @param rhs The right-hand side sidb_defect instance.
 * @return Returns true if the two sidb_defect instances are equal, false otherwise.
 */
static constexpr bool operator==(const sidb_defect& lhs, const sidb_defect& rhs) noexcept
{
    return lhs.type == rhs.type && lhs.charge == rhs.charge && lhs.epsilon_r == rhs.epsilon_r &&
           lhs.lambda_tf == rhs.lambda_tf;
}
/**
 * This operator compares two sidb_defect instances for inequality. It uses the operator== to check
 * if the two instances are equal and returns the negation of the result.
 *
 * @param lhs The left-hand side sidb_defect instance.
 * @param rhs The right-hand side sidb_defect instance.
 * @return Returns true if the two sidb_defect instances are not equal, false if they are equal.
 */
static constexpr bool operator!=(const sidb_defect& lhs, const sidb_defect& rhs) noexcept
{
    return !(lhs == rhs);
}
/**
 * Checks whether the given defect is charged. Charged defects are to be avoided by a larger distance.
 *
 * @param defect Defect type to check.
 * @return `true` iff defect is charged.
 */
[[nodiscard]] static constexpr bool is_charged_defect(const sidb_defect& defect) noexcept
{
    return defect.type == sidb_defect_type::DB || defect.type == sidb_defect_type::SI_VACANCY;
}
/**
 * Checks whether the given defect is not charged. Neutral defects are to be avoided but not by such a large distance.
 * Even though the `NONE` defect type is technically neutral, it is not a defect per se which is why this function
 * returns false on the `NONE` defect input. Additionally, the `UNKNOWN` defect cannot be guaranteed to be either
 * neutral or charged which is why this function returns false for `UNKNOWN` defect inputs as well.
 *
 * @param defect Defect type to check.
 * @return `true` iff defect is not charged.
 */
[[nodiscard]] static constexpr bool is_neutral_defect(const sidb_defect& defect) noexcept
{
    return defect.type != sidb_defect_type::NONE && !is_charged_defect(defect);
}
/**
 * Horizontal distance to keep from charged SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
inline constexpr const uint16_t SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING = 26u;
/**
 * Vertical distance to keep from charged SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
inline constexpr const uint16_t SIDB_CHARGED_DEFECT_VERTICAL_SPACING = 13u;
/**
 * Horizontal distance to keep from neutral SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
inline constexpr const uint16_t SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING = 1u;
/**
 * Vertical distance to keep from neutral SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
inline constexpr const uint16_t SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING = 0u;
/**
 * Returns the extent of a defect as a pair of SiDB distances in horizontal and vertical direction. If defect is the
 * `NONE` defect type, `{0, 0}` is returned.
 *
 * @param defect Defect type to evaluate.
 * @return Number of horizontal and vertical SiDBs that are affected by the given defect type.
 */
[[nodiscard]] static constexpr std::pair<uint16_t, uint16_t> defect_extent(const sidb_defect& defect) noexcept
{
    if (is_charged_defect(defect))
    {
        return {SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING, SIDB_CHARGED_DEFECT_VERTICAL_SPACING};
    }
    if (is_neutral_defect(defect))
    {
        return {SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING, SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING};
    }

    return {};
}

}  // namespace fiction

#endif  // FICTION_SIDB_DEFECTS_HPP
