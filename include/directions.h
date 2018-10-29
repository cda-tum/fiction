//
// Created by marcel on 04.05.18.
//

#ifndef FICTION_DIRECTIONS_H
#define FICTION_DIRECTIONS_H

#include <bitset>
#include <iostream>

namespace layout
{
    /**
     * Input and output directions of tiles can be encoded as a bitset of size 4 where the individual
     * bits are interpreted as NESW (North, East, South, West) MSB -> LSB. The bitset 1010 means North and South
     * for instance.
     */
    using directions = std::bitset<4>;

    // ************************************************************
    // ************************* Presets **************************
    // ************************************************************

    constexpr const directions DIR_NONE {0b0000};
    constexpr const directions DIR_W    {0b0001};
    constexpr const directions DIR_S    {0b0010};
    constexpr const directions DIR_SW   {0b0011};
    constexpr const directions DIR_E    {0b0100};
    constexpr const directions DIR_EW   {0b0101};
    constexpr const directions DIR_ES   {0b0110};
    constexpr const directions DIR_ESW  {0b0111};
    constexpr const directions DIR_N    {0b1000};
    constexpr const directions DIR_NW   {0b1001};
    constexpr const directions DIR_NS   {0b1010};
    constexpr const directions DIR_NSW  {0b1011};
    constexpr const directions DIR_NE   {0b1100};
    constexpr const directions DIR_NEW  {0b1101};
    constexpr const directions DIR_NES  {0b1110};
    constexpr const directions DIR_NESW {0b1111};

    // ************************************************************
    // ************************ Functions *************************
    // ************************************************************

    /**
     * Returns the opposite directions of d where d is a set of directions. It is implemented by a rotation on the
     * underlying bitset container. By rotating the bitstring by 2 positions, it is possible to get exactly the desired
     * result.
     *
     * @param d A set of directions.
     * @return A set of directions in which only the opposites of d are set.
     */
    directions opposite(const directions d) noexcept;
}


#endif //FICTION_DIRECTIONS_H
