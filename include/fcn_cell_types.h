//
// Created by marcel on 26.07.18.
//

#ifndef FICTION_FCN_CELL_TYPES_H
#define FICTION_FCN_CELL_TYPES_H

#include <fcn_technology.h>

namespace fcn
{
    /**
     * Alias for a char defining different FCN cell types.
     */
    using cell_type = char;
    /**
     * Symbol used for empty FCN cells.
     */
    constexpr const cell_type EMPTY_CELL = ' ';
    /**
     * Symbol used for normal FCN cells.
     */
    constexpr const cell_type NORMAL_CELL = 'x';
    /**
     * Symbol used for constant 0 input FCN cells.
     */
    constexpr const cell_type CONST_0_CELL = '0';
    /**
     * Symbol used for constant 1 input FCN cells.
     */
    constexpr const cell_type CONST_1_CELL = '1';
    /**
     * Symbol used for input FCN cells.
     */
    constexpr const cell_type INPUT_CELL = 'i';
    /**
     * Symbol used for output FCN cells.
     */
    constexpr const cell_type OUTPUT_CELL = 'o';
    /**
     * Possible cell modes for FCN cells.
     */
    enum class cell_mode { NORMAL, ROTATED, VERTICAL, CROSSOVER };
    /**
     * Possible marks to be applied to a cell to change its type.
     */
    enum class cell_mark { INPUT = INPUT_CELL, OUTPUT = OUTPUT_CELL, EMPTY = EMPTY_CELL };
}


#endif //FICTION_FCN_CELL_TYPES_H
