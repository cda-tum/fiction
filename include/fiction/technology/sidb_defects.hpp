//
// Created by marcel on 07.03.22.
//

#ifndef FICTION_SIDB_DEFECTS_HPP
#define FICTION_SIDB_DEFECTS_HPP

namespace fiction
{

/**
 * Specifies the types of fabrication defects that can occur on the H-Si(100) surface according to "Atomic defect
 * classification of the H–Si(100) surface through multi-mode scanning probe microscopy" by Jeremiah Croshaw, Thomas
 * Dienel, Taleana Huff, and Robert Wolkow in Journal of Nanotechnology in 2020.
 */
enum class sidb_defect_type
{
    NONE,        // HSi
    DB,          // an extra dangling bond
    SI_VACANCY,  // missing silicon
    DIHYDRIDE_PAIR,
    SINGLE_DIHYDRIDE,
    ONE_BY_ONE,  // collection of dihydride pairs
    THREE_BY_ONE,
    SILOXANE,      // oxidized silicon
    RAISED_SI,     // raised silicon
    ETCH_PIT,      // collection of missing dimers
    MISSING_DIMER  // dimer missing altogether
};
/**
 * Checks whether the given defect type is charged. Charged defects are to be avoided by a larger distance.
 *
 * @param defect Defect type to check.
 * @return True iff defect is charged.
 */
[[nodiscard]] static constexpr bool is_charged_defect(const sidb_defect_type defect) noexcept
{
    return defect == sidb_defect_type::DB || defect == sidb_defect_type::SI_VACANCY;
}
/**
 * Checks whether the given defect type is not charged. Neutral defects are to be avoided but not by such a large
 * distance. Even though the NONE defect is technically neutral, it is not a defect per se which is why this function
 * returns false on the NONE defect input.
 *
 * @param defect Defect type to check.
 * @return True iff defect is not charged.
 */
[[nodiscard]] static constexpr bool is_neutral_defect(const sidb_defect_type defect) noexcept
{
    return !is_charged_defect(defect) && defect != sidb_defect_type::NONE;
}
/**
 * Horizontal distance to keep from charged SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
static constexpr const uint16_t sidb_charged_defect_horizontal_spacing = 3u;
/**
 * Vertical distance to keep from charged SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
static constexpr const uint16_t sidb_charged_defect_vertical_spacing = 3u;
/**
 * Horizontal distance to keep from neutral SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
static constexpr const uint16_t sidb_neutral_defect_horizontal_spacing = 1u;
/**
 * Vertical distance to keep from neutral SiDB defects. The value is to be understood as the number of DB positions
 * rather than the number of dimers. This is true even though each defect always affects the entire dimer.
 */
static constexpr const uint16_t sidb_neutral_defect_vertical_spacing = 0u;

}  // namespace fiction

#endif  // FICTION_SIDB_DEFECTS_HPP
