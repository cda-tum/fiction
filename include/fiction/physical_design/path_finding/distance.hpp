/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Distance functions between layout coordinates and their functor wrappers.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/layout_base.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <functional>
#include <limits>

namespace fiction::physical_design::path_finding
{

/**
 * The Manhattan distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = |x_1 - x_2| + |y_1 - y_2|\f$
 *
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Manhattan distance between `source` and `target`.
 */
[[nodiscard]] constexpr uint64_t manhattan_distance(const layouts::layout_base::coordinate& source,
                                                    const layouts::layout_base::coordinate& target) noexcept
{
    /** Signed column difference. */
    const auto dx = static_cast<int64_t>(source.x) - target.x;
    /** Signed row difference. */
    const auto dy = static_cast<int64_t>(source.y) - target.y;
    return static_cast<uint64_t>((dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy));
}
/**
 * The Euclidean distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}\f$
 *
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Euclidean distance between `source` and `target`.
 */
[[nodiscard]] inline double euclidean_distance(const layouts::layout_base::coordinate& source,
                                               const layouts::layout_base::coordinate& target) noexcept
{
    /** Column difference. */
    const auto x = static_cast<double>(source.x) - static_cast<double>(target.x);
    /** Row difference. */
    const auto y = static_cast<double>(source.y) - static_cast<double>(target.y);

    return std::sqrt(x * x + y * y);
}
/**
 * The squared Euclidean distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}^2 = (x_1 - x_2)^2 + (y_1 - y_2)^2\f$
 *
 * In contrast to the regular Euclidean distance, this function is differentiable and can be used in optimization
 * algorithms that require gradients. Additionally, it is computationally cheaper by omitting the square root operation.
 *
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Squared euclidean distance between `source` and `target`.
 * @pre The squared distance is representable by `uint64_t`.
 */
[[nodiscard]] constexpr uint64_t squared_euclidean_distance(const layouts::layout_base::coordinate& source,
                                                            const layouts::layout_base::coordinate& target) noexcept
{
    /** Signed column difference, widened before subtraction. */
    const auto dx = static_cast<int64_t>(source.x) - target.x;
    /** Signed row difference, widened before subtraction. */
    const auto dy = static_cast<int64_t>(source.y) - target.y;
    /** Absolute column difference within the unsigned 32-bit range. */
    const auto x = static_cast<uint64_t>(dx < 0 ? -dx : dx);
    /** Absolute row difference. */
    const auto y = static_cast<uint64_t>(dy < 0 ? -dy : dy);
    /** Squared column difference; signed 32-bit coordinates keep each square representable. */
    const auto x_squared = x * x;
    /** Squared row difference. */
    const auto y_squared = y * y;
    return x_squared + y_squared;
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
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return 2DDWave distance between `source` and `target`.
 */
[[nodiscard]] constexpr uint64_t twoddwave_distance(const layouts::layout_base::coordinate& source,
                                                    const layouts::layout_base::coordinate& target) noexcept
{
    return source.x <= target.x && source.y <= target.y ? manhattan_distance(source, target) :
                                                          static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
}
/**
 * The Chebyshev distance \f$D\f$ between two layout coordinates \f$(x_1, y_1)\f$ and \f$(x_2, y_2)\f$ given by
 *
 * \f$D = \max(|x_2 - x_1|, |y_2 - y_1|)\f$
 *
 * In contrast to the Manhattan distance, this function assumes the same cost for diagonal moves as it does for
 * horizontal and vertical ones.
 *
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Chebyshev distance between `source` and `target`.
 */
[[nodiscard]] constexpr uint64_t chebyshev_distance(const layouts::layout_base::coordinate& source,
                                                    const layouts::layout_base::coordinate& target) noexcept
{
    /** Signed column difference. */
    const auto dx = static_cast<int64_t>(source.x) - target.x;
    /** Signed row difference. */
    const auto dy = static_cast<int64_t>(source.y) - target.y;
    return static_cast<uint64_t>(std::max(dx < 0 ? -dx : dx, dy < 0 ? -dy : dy));
}

// NOLINTBEGIN(*-special-member-functions): virtual destructor is prudent

/**
 * A functor that computes distances between coordinates and can be passed as an object to, e.g., path finding
 * algorithms with a standardized signature. This class is intended to be instantiated with concrete distance functions
 * passed to the constructor.
 *
 * @tparam Dist Distance value type.
 */
template <typename Dist>
class distance_functor
{
  public:
    /**
     * Standard constructor that instantiates the distance function.
     *
     * @param dist_fn A function that maps from layout coordinates to a distance value.
     */
    explicit distance_functor(const std::function<Dist(const layouts::layout_base::coordinate&,
                                                       const layouts::layout_base::coordinate&)>& dist_fn) :
            distance_function{dist_fn}
    {}
    /**
     * Destructor.
     */
    virtual ~distance_functor() = default;
    /**
     * Operator to call the distance function.
     *
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Distance between source and target.
     */
    [[nodiscard]] virtual Dist operator()(const layouts::layout_base::coordinate& source,
                                          const layouts::layout_base::coordinate& target) const
    {
        return std::invoke(distance_function, source, target);
    }

  protected:
    /**
     * Distance function.
     */
    const std::function<Dist(const layouts::layout_base::coordinate&, const layouts::layout_base::coordinate&)>
        distance_function;
};

// NOLINTEND(*-special-member-functions)

/**
 * A pre-defined distance functor that uses the Manhattan distance.
 */
class manhattan_distance_functor : public distance_functor<uint64_t>
{
  public:
    /** @brief Constructs the coordinate distance functor. */
    manhattan_distance_functor() : distance_functor<uint64_t>(&manhattan_distance) {}
};
/**
 * A pre-defined distance functor that uses the Euclidean distance.
 */
class euclidean_distance_functor : public distance_functor<double>
{
  public:
    /** @brief Constructs the coordinate distance functor. */
    euclidean_distance_functor() : distance_functor<double>(&euclidean_distance) {}
};
/**
 * A pre-defined distance functor that uses the squared Euclidean distance.
 */
class squared_euclidean_distance_functor : public distance_functor<uint64_t>
{
  public:
    /** @brief Constructs the coordinate distance functor. */
    squared_euclidean_distance_functor() : distance_functor<uint64_t>(&squared_euclidean_distance) {}
};
/**
 * A pre-defined distance functor that uses the 2DDWave distance.
 */
class twoddwave_distance_functor : public distance_functor<uint64_t>
{
  public:
    /** @brief Constructs the coordinate distance functor. */
    twoddwave_distance_functor() : distance_functor<uint64_t>(&twoddwave_distance) {}
};
/**
 * A pre-defined distance functor that uses the Chebyshev distance.
 */
class chebyshev_distance_functor : public distance_functor<uint64_t>
{
  public:
    /** @brief Constructs the coordinate distance functor. */
    chebyshev_distance_functor() : distance_functor<uint64_t>(&chebyshev_distance) {}
};

}  // namespace fiction::physical_design::path_finding
