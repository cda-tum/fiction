//
// Created by marcel on 17.02.22.
//

#ifndef FICTION_DISTANCE_HPP
#define FICTION_DISTANCE_HPP

#include "fiction/traits.hpp"

#include <cmath>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace fiction
{

/**
 * The Manhattan distance \f$ D \f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 *  \f$ D = |x_1 - x_2| + |y_1 - y_2| \f$
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Manhattan distance between source and target.
 */
template <typename Lyt, typename Dist = uint64_t>
[[nodiscard]] constexpr Dist manhattan_distance([[maybe_unused]] const Lyt& lyt, const coordinate<Lyt>& source,
                                                const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Dist>, "Dist is not an integral type");

    return static_cast<Dist>(std::abs(static_cast<int64_t>(source.x) - static_cast<int64_t>(target.x)) +
                             std::abs(static_cast<int64_t>(source.y) - static_cast<int64_t>(target.y)));
}
/**
 * The Euclidean distance \f$ D \f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 *  \f$ D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2} \f$
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Floating-point type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Euclidean distance between source and target.
 */
template <typename Lyt, typename Dist = double>
[[nodiscard]] constexpr Dist euclidean_distance([[maybe_unused]] const Lyt& lyt, const coordinate<Lyt>& source,
                                                const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_floating_point_v<Dist>, "Dist is not a floating-point type");

    const auto x = static_cast<double>(source.x) - static_cast<double>(target.x);
    const auto y = static_cast<double>(source.y) - static_cast<double>(target.y);

    return static_cast<Dist>(std::hypot(x, y));
}

// NOLINTBEGIN(*-special-member-functions): virtual destructor is prudent

/**
 * A functor that computes distances between coordinates and can be passed as an object to, e.g., path finding
 * algorithms with a standardized signature. This class is intended to be instantiated with concrete distance functions
 * passed to the constructor.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance value type.
 */
template <typename Lyt, typename Dist>
class distance_functor
{
  public:
    /**
     * Standard constructor that instantiates the distance function.
     *
     * @param dist_fn A function that maps from layout coordinates to a distance value.
     */
    explicit distance_functor(
        const std::function<Dist(const Lyt& lyt, const coordinate<Lyt>&, const coordinate<Lyt>&)>& dist_fn) :
            distance_function{dist_fn}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
    /**
     * Destructor.
     */
    virtual ~distance_functor() = default;
    /**
     * Operator to call the distance function.
     *
     * @param lyt Layout.
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Distance between source and target.
     */
    [[nodiscard]] virtual Dist operator()(const Lyt& lyt, const coordinate<Lyt>& source,
                                          const coordinate<Lyt>& target) const
    {
        return static_cast<Dist>(std::invoke(distance_function, lyt, source, target));
    }

  protected:
    /**
     * Distance function.
     */
    const std::function<Dist(const Lyt& lyt, const coordinate<Lyt>&, const coordinate<Lyt>&)> distance_function;
};

// NOLINTEND(*-special-member-functions)

/**
 * A pre-defined distance functor that uses the Manhattan distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral distance type.
 */
template <typename Lyt, typename Dist = uint64_t>
class manhattan_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    manhattan_distance_functor() : distance_functor<Lyt, Dist>(&manhattan_distance<Lyt, Dist>) {}
};
/**
 * A pre-defined distance functor that uses the Euclidean distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Floating-point distance type.
 */
template <typename Lyt, typename Dist = double>
class euclidean_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    euclidean_distance_functor() : distance_functor<Lyt, Dist>(&euclidean_distance<Lyt, Dist>) {}
};

}  // namespace fiction

#endif  // FICTION_DISTANCE_HPP
