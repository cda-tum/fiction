//
// Created by marcel on 24.04.18.
//

#ifndef FICTION_RANGE_HPP
#define FICTION_RANGE_HPP

#include <utility>

namespace fiction
{

/**
 * Defines a range type utilizing iterators. It implements `begin()` and `end()` as well as `cbegin()` and `cend()` as
 * member functions to work for range based for-loops.
 *
 * @tparam Iterator type for which the range should be created.
 */
template <typename I>
struct range_t
{
  private:
    /**
     * Iterators pointing to the begin and the end of the represented range.
     */
    const I b, e;

  public:
    /**
     * Standard constructor with forward reference.
     *
     * @param range Begin and end iterator pair.
     */
    constexpr explicit range_t(std::pair<I, I>&& range) : b{std::move(range.first)}, e{std::move(range.second)} {}
    /**
     * Returns the iterator pointing to the begin of the represented range.
     *
     * @return Begin iterator.
     */
    constexpr I begin() const
    {
        return b;
    }
    /**
     * Returns the iterator pointing to the end of the represented range.
     *
     * @return End iterator.
     */
    constexpr I end() const
    {
        return e;
    }
    /**
     * Returns a const iterator pointing to the begin of the represented range.
     *
     * @return const begin iterator.
     */
    constexpr I cbegin() const
    {
        return b;
    }
    /**
     * Returns a const iterator pointing to the end of the represented range.
     *
     * @return const end iterator.
     */
    constexpr I cend() const
    {
        return e;
    }
};

}  // namespace fiction

#endif  // FICTION_RANGE_HPP
