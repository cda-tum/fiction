//
// Created by marcel on 07.03.22.
//

#ifndef FICTION_SIDB_DEFECTS_HPP
#define FICTION_SIDB_DEFECTS_HPP

#include <cassert>
#include <cstdint>
#include <optional>
#include <utility>

namespace fiction
{

/**
 * Specifies the types of fabrication defects that can occur on the H-Si(100) 2x1 surface according to \"Atomic defect
 * classification of the H–Si(100) surface through multi-mode scanning probe microscopy\" by Jeremiah Croshaw, Thomas
 * Dienel, Taleana Huff, and Robert Wolkow in Journal of Nanotechnology in 2020.
 */
enum class sidb_defect_type : uint8_t
{
    /**
     * Defect-free H-Si.
     */
    NONE,
    /**
     * A stray dangling bond.
     */
    DB,
    /**
     * A missing silicon atom.
     */
    SI_VACANCY,
    /**
     * Double hydrogen passivation.
     */
    SINGLE_DIHYDRIDE,
    /**
     * A missing bond between dimers that leads to two double hydrogen passivations.
     */
    DIHYDRIDE_PAIR,
    /**
     * A collection of dihydride pairs.
     */
    ONE_BY_ONE,
    /**
     * A collection of 1 by 1's.
     */
    THREE_BY_ONE,
    /**
     * An oxidized dimer.
     */
    SILOXANE,
    /**
     * A raised silicon dimer.
     */
    RAISED_SI,
    /**
     * The dimer is missing altogether.
     */
    MISSING_DIMER,
    /**
     * A collection of missing dimers.
     */
    ETCH_PIT,
    /**
     * A step edge, which is a break in the surface reconstruction.
     */
    STEP_EDGE,
    /**
     * Residual material.
     */
    GUNK,
    /**
     * Ionized arsenic atom as mentioned in \"Electrostatic landscape of a Hydrogen-terminated Silicon Surface Probed by
     * a Moveable Quantum Dot\" by T. R. Huff, T. Dienel, M. Rashidi, R. Achal, L. Livadaru, J. Croshaw, and R. A.
     * Wolkow in ACS Nano in 2019.
     */
    ARSENIC,
    /**
     * Unknown defect.
     */
    UNKNOWN
};
/**
 * In accordance with the paper mentioned above, the `sidb_defect` struct is used to represent a specific defect on the
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
                                   const int64_t electric_charge = 0.0, const double relative_permittivity = 0.0,
                                   const double screening_distance = 0.0) noexcept :
            type{defect_type},
            charge{electric_charge},
            epsilon_r{relative_permittivity},
            lambda_tf{screening_distance}
    {
        assert((epsilon_r >= 0) && "epsilon_r has to be >= 0.0");
        assert((lambda_tf >= 0.0) && "lambda_tf has to be >= 0.0 nanometer");
    }
    /**
     * Type of defect.
     */
    sidb_defect_type type;
    /**
     * Electrical charge in units of the elementary charge e (e.g., 1 ^= 1*e, -2 ^= -2*e).
     */
    int64_t charge;
    /**
     * Electric permittivity (unitless).
     */
    double epsilon_r;
    /**
     * Thomas-Fermi screening distance in nm.
     */
    double lambda_tf;
    /**
     * This operator compares two `sidb_defect` instances for equality. It checks if the `type`, `charge`,
     * `epsilon_r`, and `lambda_tf` members of the two instances are equal.
     *
     * @param rhs `sidb_defect` instance to compare against.
     */
    constexpr bool operator==(const sidb_defect& rhs) const noexcept
    {
        return type == rhs.type && charge == rhs.charge && epsilon_r == rhs.epsilon_r && lambda_tf == rhs.lambda_tf;
    }
    /**
     * This operator compares two `sidb_defect` instances for inequality. It uses the `operator==` to check
     * if the two instances are equal and returns the negation of the result.
     *
     * @param rhs `sidb_defect` instance to compare against.
     */
    constexpr bool operator!=(const sidb_defect& rhs) const noexcept
    {
        return !(*this == rhs);
    }
};
/**
 * Checks whether the given defect type is a charged one. `DB` and `SI_VACANCY` types are charged. Those charged defects
 * are to be avoided by a larger distance.
 *
 * @param defect Defect to check.
 * @return `true` iff `defect` is of a charged type.
 */
[[nodiscard]] static constexpr bool is_charged_defect_type(const sidb_defect& defect) noexcept
{
    return defect.charge != 0 || defect.type == sidb_defect_type::DB || defect.type == sidb_defect_type::SI_VACANCY;
}
/**
 * Checks whether the given defect type is not a charged one. Neutral defects are to be avoided as well, but not by such
 * a large distance. Even though the `NONE` defect type is technically neutral, it is not a defect per se which is why
 * this function returns false on the `NONE` defect input.
 *
 * @param defect Defect to check.
 * @return `true` iff `defect` is not of a charged type.
 */
[[nodiscard]] static constexpr bool is_neutral_defect_type(const sidb_defect& defect) noexcept
{
    return defect.type != sidb_defect_type::NONE && !is_charged_defect_type(defect);
}
/**
 * Checks whether the given defect has a positive charge value assigned to it. This function is irrespective of the
 * associated defect type.
 *
 * @param defect Defect to check.
 * @return `true` iff `defect` has a positive charge value.
 */
[[nodiscard]] static constexpr bool is_positively_charged_defect(const sidb_defect& defect) noexcept
{
    return defect.charge > 0;
}
/**
 * Checks whether the given defect has a negative charge value assigned to it. This function is irrespective of the
 * associated defect type.
 *
 * @param defect Defect to check.
 * @return `true` iff `defect` has a negative charge value.
 */
[[nodiscard]] static constexpr bool is_negatively_charged_defect(const sidb_defect& defect) noexcept
{
    return defect.charge < 0;
}
/**
 * Checks whether the given defect has a neutral charge value, i.e., `0`, assigned to it. This function is irrespective
 * of the associated defect type.
 *
 * @param defect Defect to check.
 * @return `true` iff `defect` has a neutral charge value.
 */
[[nodiscard]] static constexpr bool is_neutrally_charged_defect(const sidb_defect& defect) noexcept
{
    return defect.charge == 0;
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
 * Returns the extent of a defect as a pair of SiDB distances in the horizontal and vertical directions.
 * If the defect type is `NONE`, `{0, 0}` is returned.
 *
 * @param defect Defect type to evaluate.
 * @param charged_defect_spacing_overwrite Override the default influence distance of charged atomic defects on SiDBs
 * with an optional pair of horizontal and vertical distances.
 * @param neutral_defect_spacing_overwrite Override the default influence distance of neutral atomic defects on SiDBs
 * with an optional pair of horizontal and vertical distances.
 * @return A pair of uint16_t values representing the number of horizontal and vertical SiDBs affected by the given
 * defect type.
 */
[[nodiscard]] static constexpr std::pair<uint16_t, uint16_t> defect_extent(
    const sidb_defect&                                  defect,
    const std::optional<std::pair<uint16_t, uint16_t>>& charged_defect_spacing_overwrite = std::nullopt,
    const std::optional<std::pair<uint16_t, uint16_t>>& neutral_defect_spacing_overwrite = std::nullopt) noexcept
{
    if (is_charged_defect_type(defect))
    {
        if (charged_defect_spacing_overwrite.has_value())
        {
            return charged_defect_spacing_overwrite.value();
        }
        return {SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING, SIDB_CHARGED_DEFECT_VERTICAL_SPACING};
    }
    if (is_neutral_defect_type(defect))
    {
        if (neutral_defect_spacing_overwrite.has_value())
        {
            return neutral_defect_spacing_overwrite.value();
        }
        return {SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING, SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING};
    }

    return {};
}

}  // namespace fiction

#endif  // FICTION_SIDB_DEFECTS_HPP
