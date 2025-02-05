//
// Created by marcel on 11.01.22.
//

#ifndef FICTION_SHIFTED_CARTESIAN_LAYOUT_HPP
#define FICTION_SHIFTED_CARTESIAN_LAYOUT_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"

#include <type_traits>

namespace fiction
{

/**
 * \verbatim
  +-------+
  |       |
  |       |
  |       |
  +---+---+---+
      |       |
      |       |
      |       |
      +-------+
  \endverbatim
 */
struct horizontal_shift_cartesian
{
    using orientation = horizontal_shift_cartesian;
};

/**
 * \verbatim
  +-------+
  |       |
  |       +-------+
  |       |       |
  +-------+       |
          |       |
          +-------+
  \endverbatim
 */
struct vertical_shift_cartesian
{
    using orientation = vertical_shift_cartesian;
};
/**
 * \verbatim
  +-------+-------+-------+
  |       |       |       |
  | (0,0) | (1,0) | (2,0) |
  |       |       |       |
  +---+---+---+---+---+---+---+
      |       |       |       |
      | (0,1) | (1,1) | (2,1) |
      |       |       |       |
  +---+---+---+---+---+---+---+
  |       |       |       |
  | (0,2) | (1,2) | (2,2) |
  |       |       |       |
  +-------+-------+-------+
  \endverbatim
 */
struct odd_row_cartesian : horizontal_shift_cartesian
{};
/**
 * \verbatim
      +-------+-------+-------+
      |       |       |       |
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
  +---+---+---+---+---+---+---+
  |       |       |       |
  | (0,1) | (1,1) | (2,1) |
  |       |       |       |
  +---+---+---+---+---+---+---+
      |       |       |       |
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
      +-------+-------+-------+
 \endverbatim
*/
struct even_row_cartesian : horizontal_shift_cartesian
{};
/**
 * \verbatim
   +-------+       +-------+
   |       |       |       |
   | (0,0) +-------+ (2,0) +-------+
   |       |       |       |       |
   +-------+ (1,0) +-------+ (3,0) |
   |       |       |       |       |
   | (0,1) +-------+ (2,1) +-------+
   |       |       |       |       |
   +-------+ (1,1) +-------+ (3,1) |
   |       |       |       |       |
   | (0,2) +-------+ (2,2) +-------+
   |       |       |       |
   +-------+       +-------+
 \endverbatim
*/
struct odd_column_cartesian : vertical_shift_cartesian
{};
/**
 * \verbatim
          +-------+       +-------+
          |       |       |       |
  +-------+ (1,0) +-------+ (3,0) |
  |       |       |       |       |
  | (0,0) +-------+ (2,0) +-------+
  |       |       |       |       |
  +-------+ (1,1) +-------+ (3,1) |
  |       |       |       |       |
  | (0,1) +-------+ (2,1) +-------+
  |       |       |       |       |
  +-------+ (1,2) +-------+ (3,2) |
          |       |       |       |
          +-------+       +-------+
\endverbatim
*/
struct even_column_cartesian : vertical_shift_cartesian
{};

/**
 * A layout type that utilizes offset coordinates to represent a Cartesian layout with shifted coordinates. Its faces
 * are organizes in an offset coordinate system as provided. These can either be the horizontal_shift_cartesian or
 * vertical_shift_cartesian orientation. Based on that, two respectively possible coordinate systems emerge accordingly:
 * odd_row_cartesian and even_row_cartesian for horizontal shifts and odd_column_cartesian and even_column_cartesian for
 * vertical shifts. All are sketched in ASCII above.
 *
 * @tparam OffsetCoordinateType The coordinate implementation to be used. Offset coordinates are required.
 * @tparam ShiftedCartesianCoordinateSystem One of the following: odd_row_cartesian, even_row_cartesian,
 * odd_column_cartesian, even_column_cartesian.
 */
template <typename OffsetCoordinateType             = offset::ucoord_t,
          typename ShiftedCartesianCoordinateSystem = even_row_cartesian>
class shifted_cartesian_layout
        : public hexagonal_layout<
              OffsetCoordinateType,
              std::conditional_t<
                  std::is_same_v<ShiftedCartesianCoordinateSystem, odd_row_cartesian>, odd_row_hex,
                  std::conditional_t<
                      std::is_same_v<ShiftedCartesianCoordinateSystem, even_row_cartesian>, even_row_hex,
                      std::conditional_t<
                          std::is_same_v<ShiftedCartesianCoordinateSystem, odd_column_cartesian>, odd_column_hex,
                          std::conditional_t<std::is_same_v<ShiftedCartesianCoordinateSystem, even_column_cartesian>,
                                             even_column_hex, void>>>>,
              cube::coord_t>
{
  private:
    using HexagonalLayout = hexagonal_layout<
        OffsetCoordinateType,
        std::conditional_t<
            std::is_same_v<ShiftedCartesianCoordinateSystem, odd_row_cartesian>, odd_row_hex,
            std::conditional_t<
                std::is_same_v<ShiftedCartesianCoordinateSystem, even_row_cartesian>, even_row_hex,
                std::conditional_t<
                    std::is_same_v<ShiftedCartesianCoordinateSystem, odd_column_cartesian>, odd_column_hex,
                    std::conditional_t<std::is_same_v<ShiftedCartesianCoordinateSystem, even_column_cartesian>,
                                       even_column_hex, void>>>>,
        cube::coord_t>;

  public:
    using cartesian_arrangement = ShiftedCartesianCoordinateSystem;
    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the even_column_cartesian ASCII layout representation above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the
     * layout has exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit shifted_cartesian_layout(const typename HexagonalLayout::aspect_ratio_type& ar = {}) : HexagonalLayout(ar)
    {
        static_assert(
            std::is_same_v<ShiftedCartesianCoordinateSystem, odd_row_cartesian> ||
                std::is_same_v<ShiftedCartesianCoordinateSystem, even_row_cartesian> ||
                std::is_same_v<ShiftedCartesianCoordinateSystem, odd_column_cartesian> ||
                std::is_same_v<ShiftedCartesianCoordinateSystem, even_column_cartesian>,
            "ShiftedCartesianCoordinateSystem has to be one of the following: odd_row_cartesian, even_row_cartesian, "
            "odd_column_cartesian, even_column_cartesian");
    }

    // NOLINTNEXTLINE(*-explicit-constructor, *-explicit-conversions)
    shifted_cartesian_layout(const HexagonalLayout& lyt) : HexagonalLayout(lyt) {}

  private:
    // intentionally hide members of HexagonalLayout
    using HexagonalLayout::to_cube_coordinate;
    using HexagonalLayout::to_offset_coordinate;
    using typename HexagonalLayout::hex_arrangement;
};

}  // namespace fiction

#endif  // FICTION_SHIFTED_CARTESIAN_LAYOUT_HPP
