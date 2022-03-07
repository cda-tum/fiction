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
    ONE_BY_ONE,
    THREE_BY_ONE,
    SILOXANE,   // oxidized silicon
    RAISED_SI,  // raised silicon
    ETCH_PIT,
    MISSING_DIMER  // dimer missing altogether
};

}  // namespace fiction

#endif  // FICTION_SIDB_DEFECTS_HPP
