//
// Created by marcel on 29.09.18.
//

#ifndef FICTION_FCN_TECHNOLOGY_H
#define FICTION_FCN_TECHNOLOGY_H

#include <iostream>
#include <string>

namespace fcn
{
    /**
     * Supported technology types:
     * - Qunatum-dot Cellular Automata (QCA)
     * - In-plane Nanomagnet Logic (iNML)
     * (- Perpendicular Nanomagnet Logic (pNML))
     * (- Dangling Bonds (DB))
     */
    enum class technology { QCA, INML /*, DB */ };
    /**
     * Overload for the stream operator on fcn::technology to print a string representation.
     *
     * @param os Stream to write into.
     * @param tech Technology to print.
     * @return os extended by string representation of tech.
     */
    std::ostream& operator<<(std::ostream& os, const technology tech);
    /**
     * Returns string representation of an fcn::technology.
     *
     * @param tech Technology to transform to string.
     * @return String representation of tech.
     */
    std::string to_string(const technology tech);
    /**
     * Supported tile sizes for available technologies.
     * - QCA: 5 x 5
     * - iNML: 4 x 4
     */
    enum class tile_size : uint8_t { /* THREE_X_THREE = 3u, */ FOUR_X_FOUR = 4u, FIVE_X_FIVE = 5u };
}

#endif //FICTION_FCN_TECHNOLOGY_H
