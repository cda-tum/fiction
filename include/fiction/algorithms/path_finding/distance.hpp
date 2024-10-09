//
// Created by marcel on 17.02.22.
//

#ifndef FICTION_DISTANCE_HPP
#define FICTION_DISTANCE_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>

namespace fiction
{

/**
 * The Manhattan distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = |x_1 - x_2| + |y_1 - y_2|\f$
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Manhattan distance between `source` and `target`.
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
 * The Euclidean distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}\f$
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Floating-point type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Euclidean distance between `source` and `target`.
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
/**
 * The squared Euclidean distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}^2 = (x_1 - x_2)^2 + (y_1 - y_2)^2\f$
 *
 * In contrast to the regular Euclidean distance, this function is differentiable and can be used in optimization
 * algorithms that require gradients. Additionally, it is computationally cheaper by omitting the square root operation.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Squared euclidean distance between `source` and `target`.
 */
template <typename Lyt, typename Dist = uint64_t>
[[nodiscard]] constexpr Dist squared_euclidean_distance([[maybe_unused]] const Lyt& lyt, const coordinate<Lyt>& source,
                                                        const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Dist>, "Dist is not an integral type");

    const auto x = static_cast<double>(source.x) - static_cast<double>(target.x);
    const auto y = static_cast<double>(source.y) - static_cast<double>(target.y);

    return static_cast<Dist>(x * x + y * y);
}
/**
 * The 2DDWave distance \f$D\f$ between two layout coordinates \f$s = (x_1, y_1)\f$ and \f$t = (x_2, y_2)\f$ given
 * by
 *
 * \f$D = |x_1 - x_2| + |y_1 - y_2|\f$ iff \f$s \leq t\f$ and \f$\infty\f$, otherwise.
 *
 * Thereby, \f$s \leq t\f$ iff \f$x_1 \leq x_2\f$ and \f$y_1 \leq y_2\f$.
 *
 * @note To represent \f$\infty\f$, `std::numeric_limits<uint32_t>::max()` is returned for distances of infinite
 * length. We are using `uint32_t` to prevent overflows when adding distances in the default `uint64_t` number range.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return 2DDWave distance between `source` and `target`.
 */
template <typename Lyt, typename Dist = uint64_t>
[[nodiscard]] constexpr Dist twoddwave_distance([[maybe_unused]] const Lyt& lyt, const coordinate<Lyt>& source,
                                                const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Dist>, "Dist is not an integral type");

    return source.x <= target.x && source.y <= target.y ? manhattan_distance<Lyt, Dist>(lyt, source, target) :
                                                          static_cast<Dist>(std::numeric_limits<uint32_t>::max());
}
/**
 * The Chebyshev distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \max(|x_2 - x_1|, |y_2 - y_1|)\f$
 *
 * In contrast to the Manhattan distance, this function assumes the same cost for diagonal moves as it does for
 * horizontal and vertical ones.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral type for the distance.
 * @param lyt Layout.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Chebyshev distance between `source` and `target`.
 */
template <typename Lyt, typename Dist = uint64_t>
[[nodiscard]] constexpr Dist chebyshev_distance([[maybe_unused]] const Lyt& lyt, const coordinate<Lyt>& source,
                                                const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Dist>, "Dist is not an integral type");

    return static_cast<Dist>(std::max(std::abs(static_cast<int64_t>(source.x) - static_cast<int64_t>(target.x)),
                                      std::abs(static_cast<int64_t>(source.y) - static_cast<int64_t>(target.y))));
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
        const std::function<Dist(const Lyt&, const coordinate<Lyt>&, const coordinate<Lyt>&)>& dist_fn) :
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
    const std::function<Dist(const Lyt&, const coordinate<Lyt>&, const coordinate<Lyt>&)> distance_function;
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
/**
 * A pre-defined distance functor that uses the squared Euclidean distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral distance type.
 */
template <typename Lyt, typename Dist = uint64_t>
class squared_euclidean_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    squared_euclidean_distance_functor() : distance_functor<Lyt, Dist>(&squared_euclidean_distance<Lyt, Dist>) {}
};
/**
 * A pre-defined distance functor that uses the 2DDWave distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral distance type.
 */
template <typename Lyt, typename Dist = uint64_t>
class twoddwave_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    twoddwave_distance_functor() : distance_functor<Lyt, Dist>(&twoddwave_distance<Lyt, Dist>) {}
};
/**
 * A pre-defined distance functor that uses the Chebyshev distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Integral distance type.
 */
template <typename Lyt, typename Dist = uint64_t>
class chebyshev_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    chebyshev_distance_functor() : distance_functor<Lyt, Dist>(&chebyshev_distance<Lyt, Dist>) {}
};

}  // namespace fiction

#endif  // FICTION_DISTANCE_HPP
