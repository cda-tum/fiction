//
// Created by marcel on 15.09.21.
//

#ifndef FICTION_HEXAGONAL_LAYOUT_HPP
#define FICTION_HEXAGONAL_LAYOUT_HPP

namespace fiction
{

/**
 * \verbatim
 *
 *    / \
 *  /     \
 * |       |
 * |       |
 *  \     /
 *    \ /
 *
 */
struct pointy_top
{};

/**
 * \verbatim
 *    _____
 *   /     \
 *  /       \
 *  \       /
 *   \_____/
 *
 */
struct flat_top
{};

/**
 * \verbatim
 *
 *        / \     / \     / \
 *      /     \ /     \ /     \
 *     | (0,0) | (1,0) | (2,0) |
 *     |       |       |       |
 *      \     / \     / \     /  \
 *        \ /     \ /     \ /     \
 *         | (1,1) | (2,1) | (2,1) |
 *         |       |       |       |
 *        / \     / \     /  \    /
 *      /     \ /     \ /     \ /
 *     | (0,2) | (1,2) | (2,2) |
 *     |       |       |       |
 *      \     / \     / \     /
 *        \ /     \ /     \ /
 */
struct odd_row : pointy_top
{};
/**
 * \verbatim
 *
 *        / \     / \     / \
 *      /     \ /     \ /     \
 *     | (0,0) | (1,0) | (2,0) |
 *     |       |       |       |
 *    / \     / \     / \     /
 *  /     \ /     \ /     \ /
 * | (0,1) | (1,1) | (2,1) |
 * |       |       |       |
 *  \     / \     / \     /  \
 *    \ /     \ /     \ /     \
 *     | (0,2) | (1,2) | (2,2) |
 *     |       |       |       |
 *      \     / \     / \     /
 *        \ /     \ /     \ /
 */
struct even_row : pointy_top
{};
/**
 * \verbatim
 *    _____         _____
 *   /     \       /     \
 *  / (0,0) \_____/ (2,0) \_____
 *  \       /     \       /     \
 *   \_____/ (1,0) \_____/ (3,0) \
 *   /     \       /     \       /
 *  / (0,1) \_____/ (2,1) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,1) \_____/ (3,1) \
 *   /     \       /     \       /
 *  / (0,2) \_____/ (2,2) \_____/
 *  \       /     \       /
 *   \_____/       \_____/
 *
 */
struct odd_column : flat_top
{};

/**
 * \verbatim
 *           _____         _____
 *          /     \       /     \
 *    _____/ (1,0) \_____/ (3,0) \
 *   /     \       /     \       /
 *  / (0,0) \_____/ (2,0) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,1) \_____/ (3,1) \
 *   /     \       /     \       /
 *  / (0,1) \_____/ (2,1) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,2) \_____/ (3,2) \
 *         \       /     \       /
 *          \_____/       \_____/
 *
 */
struct even_column : flat_top
{};

/**
 * A layout type that utilizes offset coordinates to represent a hexagonal grid. It's faces are organized in a
 * coordinate system as provided. Hexagons can be in the 'pointy top' or 'flat top' orientation. Based on that, two
 * respectively possible coordinate systems emerge accordingly: 'odd row' and 'even row' for pointy tops and 'odd
 * column' and 'even column' for flat tops. Both are sketched in ASCII above.
 *
 * @tparam OffsetCoordinateType The coordinate implementation to be used. Offset coordinates are required.
 * @tparam CoordinateSystem One of the following: odd_row, even_row, odd_column, even_column.
 */
template <typename OffsetCoordinateType, typename CoordinateSystem>
class hexagonal_layout
{
    explicit hexagonal_layout()
    {
        static_assert(std::is_same_v<CoordinateSystem, odd_row> || std::is_same_v<CoordinateSystem, even_row> ||
                          std::is_same_v<CoordinateSystem, odd_column> || std::is_same_v<CoordinateSystem, even_column>,
                      "CoordinateSystem has to be one of the following: odd_row, even_row, odd_column, even_column");
    }
};

}  // namespace fiction

#endif  // FICTION_HEXAGONAL_LAYOUT_HPP
