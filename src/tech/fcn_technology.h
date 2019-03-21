//
// Created by marcel on 29.09.18.
//

#ifndef FICTION_FCN_TECHNOLOGY_H
#define FICTION_FCN_TECHNOLOGY_H

#include <iostream>

namespace fcn
{
    /**
     * Supported technology types:
     * - Qunatum-dot Cellular Automata (QCA)
     * (- Nanomagnet Logic (NML))
     * (- Dangling Bonds (DB))
     */
    enum class technology { QCA /*, NML, DB */ };
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
     * - QCA: (3 x 3,) 5 x 5
     * (- NML: 3 x 3)
     */
    enum class tile_size : std::size_t { /* THREE_X_THREE = 3u, */ FIVE_X_FIVE = 5u };
}

#endif //FICTION_FCN_TECHNOLOGY_H
