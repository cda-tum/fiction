//
// Created by marcel on 27.05.22.
//

#ifndef FICTION_COST_HPP
#define FICTION_COST_HPP

#include "fiction/traits.hpp"

#include <functional>
#include <random>
#include <type_traits>

namespace fiction
{

/**
 * Unit cost between two layout coordinates that always returns 1.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Cost Integral type for the cost.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Unit cost between `source` and `target`, i.e., 1.
 */
template <typename Lyt, typename Cost = uint8_t>
[[nodiscard]] constexpr Cost unit_cost([[maybe_unused]] const coordinate<Lyt>& source,
                                       [[maybe_unused]] const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_integral_v<Cost>, "Cost is not an integral type");

    return static_cast<Cost>(1);
}
/**
 * Random cost between two layout coordinates that returns a random value between 0 and 1.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Cost Floating-point type for the cost.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Random cost between `source` and `target`, i.e., a random number between 0 and 1.
 */
template <typename Lyt, typename Cost = double>
[[nodiscard]] Cost random_cost([[maybe_unused]] const coordinate<Lyt>& source,
                               [[maybe_unused]] const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_floating_point_v<Cost>, "Cost is not a floating-point type");

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
 * @tparam Lyt Coordinate layout type.
 * @tparam Cost Cost value type.
 */
template <typename Lyt, typename Cost>
class cost_functor
{
  public:
    /**
     * Standard constructor that instantiates the cost function.
     *
     * @param cost_fn A function that maps from layout coordinates to a cost value.
     */
    explicit cost_functor(const std::function<Cost(const coordinate<Lyt>&, const coordinate<Lyt>&)>& cost_fn) :
            cost_function{cost_fn}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
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
    [[nodiscard]] virtual Cost operator()(const coordinate<Lyt>& source, const coordinate<Lyt>& target) const
    {
        return static_cast<Cost>(std::invoke(cost_function, source, target));
    }

  protected:
    /**
     * Cost function.
     */
    const std::function<Cost(const coordinate<Lyt>&, const coordinate<Lyt>&)> cost_function;
};

// NOLINTEND(*-special-member-functions)

/**
 * A pre-defined cost functor that uses unit costs.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Cost Integral cost type.
 */
template <typename Lyt, typename Cost = uint8_t>
class unit_cost_functor : public cost_functor<Lyt, Cost>
{
  public:
    unit_cost_functor() : cost_functor<Lyt, Cost>(&unit_cost<Lyt, Cost>) {}
};
/**
 * A pre-defined cost functor that uses random costs.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Cost Floating-point cost type.
 */
template <typename Lyt, typename Cost = double>
class random_cost_functor : public cost_functor<Lyt, Cost>
{
  public:
    random_cost_functor() : cost_functor<Lyt, Cost>(&random_cost<Lyt, Cost>) {}
};

}  // namespace fiction

#endif  // FICTION_COST_HPP
