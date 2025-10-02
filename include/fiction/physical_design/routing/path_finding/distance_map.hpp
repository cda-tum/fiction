//
// Created by marcel on 18.06.23.
//

#ifndef FICTION_DISTANCE_MAP_HPP
#define FICTION_DISTANCE_MAP_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"

#include <phmap.h>

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * A distance map is a two-dimensional array of distances between coordinates. The distance map is accessed via the
 * indices of the coordinates in the associated layout. A coordinate index \f$i\f$ is calculated as follows:
 * \f$i = y \cdot W + x\f$, with \f$W\f$ being the width of the layout. As an example, in a layout of size
 * \f$3 \times 3\f$, the coordinate index of the coordinate \f$(2, 1)\f$ has index \f$1 \cdot 3 + 2 = 5\f$.
 *
 * The `distance_map` is to be preferred over the `sparse_distance_map` in most cases when performance is the main goal.
 */
template <typename Dist>
using distance_map = std::vector<std::vector<Dist>>;
/**
 * A sparse distance map is a flat hash map of distances between coordinates. The sparse distance map is accessed via
 * coordinate pairs. The sparse distance map is to be preferred over the distance map if only a small subset of the
 * distances between coordinates is to be stored.
 *
 * The `distance_map` is to be preferred over the `sparse_distance_map` in most cases when performance is the main goal.
 */
template <typename Lyt, typename Dist>
using sparse_distance_map = phmap::parallel_flat_hash_map<std::pair<coordinate<Lyt>, coordinate<Lyt>>, Dist>;
/**
 * This function fully initializes a `distance_map` for a given layout and distance functor. It computes the distances
 * between all pairs of coordinates in the layout and stores them in the distance map for quick subsequent access.
 *
 * This function performs \f$\mathcal{O}(|L|^2)\f$ distance computations, where \f$|L|\f$ is the number of
 * coordinates in the layout.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 * @param lyt Layout to compute distances for.
 * @param dist_fn Distance functor to apply to all pairs of coordinates in `lyt`.
 * @return Fully initialized `distance_map` for `lyt`.
 */
template <typename Lyt, typename Dist>
[[nodiscard]] distance_map<Dist> initialize_distance_map(const Lyt&                         lyt,
                                                         const distance_functor<Lyt, Dist>& dist_fn) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    // initialize distance map
    distance_map<Dist> dist_map(lyt.area(), std::vector<Dist>(lyt.area(), Dist{}));

    // compute distances and store them via coordinate index
    lyt.foreach_coordinate(
        [&lyt, &dist_fn, &dist_map](const auto& c1, const unsigned src)
        {
            lyt.foreach_coordinate([&lyt, &dist_fn, &dist_map, &c1, src](const auto& c2, const unsigned tgt)
                                   { dist_map[src][tgt] = dist_fn(lyt, c1, c2); });
        });

    return dist_map;
}
/**
 * This function fully initializes a `sparse_distance_map` for a given layout and distance functor. It computes the
 * distances between all pairs of coordinates in the layout and stores them in the distance map for quick subsequent
 * access.
 *
 * This function performs \f$\mathcal{O}(|L|^2)\f$ distance computations, where \f$|L|\f$ is the number of
 * coordinates in the layout.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 * @param lyt Layout to compute distances for.
 * @param dist_fn Distance functor to apply to all pairs of coordinates in `lyt`.
 * @return Fully initialized `sparse_distance_map` for `lyt`.
 */
template <typename Lyt, typename Dist>
[[nodiscard]] sparse_distance_map<Lyt, Dist>
initialize_sparse_distance_map(const Lyt& lyt, const distance_functor<Lyt, Dist>& dist_fn) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    // initialize distance map
    sparse_distance_map<Lyt, Dist> dist_map{};
    dist_map.reserve(lyt.area() * lyt.area());

    // compute distances and store them via coordinate lookup
    lyt.foreach_coordinate(
        [&lyt, &dist_fn, &dist_map](const auto& c1)
        {
            lyt.foreach_coordinate([&lyt, &dist_fn, &dist_map, &c1](const auto& c2)
                                   { dist_map[{c1, c2}] = dist_fn(lyt, c1, c2); });
        });

    return dist_map;
}

/**
 * A distance functor that uses a fully precomputed `distance_map` to determine distances between coordinates. It can be
 * used as a drop-in replacement for any other distance functor in path-finding algorithms.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 */
template <typename Lyt, typename Dist>
class distance_map_functor : public distance_functor<Lyt, Dist>
{
  public:
    /**
     * Construct the distance functor from a `distance_map`.
     *
     * @param dm Distance map.
     */
    explicit distance_map_functor(const distance_map<Dist>& dm) :
            distance_functor<Lyt, Dist>([](const Lyt&, const coordinate<Lyt>&, const coordinate<Lyt>&)
                                        { return Dist{}; }),  // dummy distance function
            dist_map{dm}
    {}
    /**
     * Override the call operator to query the distance map instead of the distance function.
     *
     * @note This function will throw an exception if the queried distance is not stored in the distance map.
     *
     * @param lyt Layout.
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Distance between source and target according to the stored distance map.
     */
    [[nodiscard]] Dist operator()(const Lyt& lyt, const coordinate<Lyt>& source,
                                  const coordinate<Lyt>& target) const override
    {
        return static_cast<Dist>(dist_map.at(coordinate_index(lyt, source)).at(coordinate_index(lyt, target)));
    }

  protected:
    /**
     * Distance map.
     */
    const distance_map<Dist> dist_map;

  private:
    /**
     * This function calculates the coordinate index of a given coordinate in a given layout.
     *
     * @param lyt Layout.
     * @param c Coordinate.
     * @return Coordinate index.
     */
    [[nodiscard]] static constexpr std::size_t coordinate_index(const Lyt& lyt, const coordinate<Lyt>& c) noexcept
    {
        return c.y * (lyt.x() + 1) + c.x;
    }
};
/**
 * A distance functor that uses a fully precomputed `sparse_distance_map` to determine distances between coordinates. It
 * can be used as a drop-in replacement for any other distance functor in path-finding algorithms.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 */
template <typename Lyt, typename Dist>
class sparse_distance_map_functor : public distance_functor<Lyt, Dist>
{
  public:
    /**
     * Construct the distance functor from a sparse distance map.
     *
     * @param sdm Sparse distance map.
     */
    explicit sparse_distance_map_functor(const sparse_distance_map<Lyt, Dist>& sdm) :
            distance_functor<Lyt, Dist>([](const Lyt&, const coordinate<Lyt>&, const coordinate<Lyt>&)
                                        { return Dist{}; }),  // dummy distance function
            sparse_dist_map{sdm}
    {}
    /**
     * Override the call operator to query the sparse distance map instead of the distance function.
     *
     * @note This function will throw an exception if the queried distance is not stored in the sparse distance map.
     *
     * @param lyt Layout.
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Distance between source and target according to the stored sparse distance map.
     */
    [[nodiscard]] Dist operator()(const Lyt&, const coordinate<Lyt>& source,
                                  const coordinate<Lyt>& target) const override
    {
        return static_cast<Dist>(sparse_dist_map.at({source, target}));
    }

  protected:
    /**
     * Sparse distance map.
     */
    const sparse_distance_map<Lyt, Dist> sparse_dist_map;
};

/**
 * A distance functor that internally uses a `sparse_distance_map` as a cache to prevent re-computing distances that
 * have already been evaluated. In contrast to `distance_map_functor` and `sparse_distance_map_functor`, this functor
 * does not require a pre-computed distance map upon construction, but instead will gradually build up its own cache
 * when queried multiple times. It can be used as a drop-in replacement for any other distance functor in path-finding
 * algorithms.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 */
template <typename Lyt, typename Dist>
class smart_distance_cache_functor : public distance_functor<Lyt, Dist>
{
  public:
    /**
     * Construct a distance functor from a layout and a distance function.
     *
     * The internal cache will be initialized empty. Distances will be computed on the fly and stored in the cache
     * whenever they are queried.
     *
     * @param lyt Layout.
     * @param dist_fn Distance function.
     */
    explicit smart_distance_cache_functor(
        const Lyt&                                                                             lyt,
        const std::function<Dist(const Lyt&, const coordinate<Lyt>&, const coordinate<Lyt>&)>& dist_fn) :
            distance_functor<Lyt, Dist>(dist_fn)
    {
        distance_cache.reserve(lyt.area() * lyt.area());
    }
    /**
     * Override the call operator to first query the cache instead of the distance function. Only on a cache miss,
     * the distance function will be called and the result will be stored in the cache.
     *
     * @param lyt Layout.
     * @param source Source coordinate.
     * @param target Target coordinate.
     * @return Distance between source and target according to the cache or the distance function.
     */
    [[nodiscard]] Dist operator()(const Lyt& lyt, const coordinate<Lyt>& source,
                                  const coordinate<Lyt>& target) const override
    {
        if (const auto it = distance_cache.find({source, target}); it != distance_cache.cend())  // cache hit
        {
            return it->second;
        }

        // cache miss

        const auto d = distance_functor<Lyt, Dist>::operator()(lyt, source, target);

        distance_cache[{source, target}] = d;

        return d;
    }

  protected:
    /**
     * Sparse distance map serving as a cache.
     */
    mutable sparse_distance_map<Lyt, Dist> distance_cache{};
};

}  // namespace fiction

#endif  // FICTION_DISTANCE_MAP_HPP
