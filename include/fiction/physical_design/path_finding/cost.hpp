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
 * @brief Cost functions between adjacent layout coordinates and their functor wrappers.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/layout_base.hpp"

#include <concepts>
#include <cstdint>
#include <functional>
#include <random>

namespace fiction::physical_design::path_finding
{

/**
 * Unit cost between two layout coordinates that always returns 1.
 *
 * @tparam Cost Integral type for the cost.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Unit cost between `source` and `target`, i.e., 1.
 */
template <typename Cost = uint8_t>
    requires std::integral<Cost>
[[nodiscard]] constexpr Cost unit_cost([[maybe_unused]] const layouts::layout_base::coordinate& source,
                                       [[maybe_unused]] const layouts::layout_base::coordinate& target) noexcept
{
    return static_cast<Cost>(1);
}
/**
 * Random cost between two layout coordinates that returns a random value between 0 and 1.
 *
 * @tparam Cost Floating-point type for the cost.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Random cost between `source` and `target`, i.e., a random number between 0 and 1.
 */
template <typename Cost = double>
    requires std::floating_point<Cost>
[[nodiscard]] Cost random_cost([[maybe_unused]] const layouts::layout_base::coordinate& source,
                               [[maybe_unused]] const layouts::layout_base::coordinate& target) noexcept
{
    static std::default_random_engine           engine{std::random_device{}()};
    static std::uniform_real_distribution<Cost> distr{0, 1};

    return distr(engine);
}

// NOLINTBEGIN(*-special-member-functions): virtual destructor is prudent

/**
 * A functor that computes costs between coordinates and can be passed as an object to, e.g., path finding algorithms
 * with a standardized signature. This class is intended to be instantiated with concrete cost functions passed to the
 * constructor.
 *
 * @tparam Cost Cost value type.
 */
template <typename Cost>
class cost_functor
{
  public:
    /**
     * Standard constructor that instantiates the cost function.
     *
     * @param cost_fn A function that maps from layout coordinates to a cost value.
     */
    explicit cost_functor(const std::function<Cost(const layouts::layout_base::coordinate&,
                                                   const layouts::layout_base::coordinate&)>& cost_fn) :
            cost_function{cost_fn}
    {}
    /**
     * Destructor.
     */
    virtual ~cost_functor() = default;
    /**
     * Operator to call the cost function.
     *
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Cost between source and target.
     */
    [[nodiscard]] virtual Cost operator()(const layouts::layout_base::coordinate& source,
                                          const layouts::layout_base::coordinate& target) const
    {
        return static_cast<Cost>(std::invoke(cost_function, source, target));
    }

  protected:
    /**
     * Cost function.
     */
    const std::function<Cost(const layouts::layout_base::coordinate&, const layouts::layout_base::coordinate&)>
        cost_function;
};

// NOLINTEND(*-special-member-functions)

/**
 * A pre-defined cost functor that uses unit costs.
 *
 * @tparam Cost Integral cost type.
 */
template <typename Cost = uint8_t>
class unit_cost_functor : public cost_functor<Cost>
{
  public:
    unit_cost_functor() : cost_functor<Cost>(&unit_cost<Cost>) {}
};
/**
 * A pre-defined cost functor that uses random costs.
 *
 * @tparam Cost Floating-point cost type.
 */
template <typename Cost = double>
class random_cost_functor : public cost_functor<Cost>
{
  public:
    random_cost_functor() : cost_functor<Cost>(&random_cost<Cost>) {}
};

}  // namespace fiction::physical_design::path_finding
