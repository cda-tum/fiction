//
// Created by marcel on 29.11.22.
//

#ifndef FICTION_MOCKTURTLE_UTILS_HPP
#define FICTION_MOCKTURTLE_UTILS_HPP

#include <mockturtle/networks/detail/foreach.hpp>

namespace mockturtle::detail
{

template <class Iterator, class ElementType, class Pred, class Transform, class Fn>
Iterator foreach_element_if_transform(Iterator begin, Iterator end, Pred&& pred, Transform&& transform, Fn&& fn,
                                      uint32_t counter_offset = 0)
{
    static_assert(
        is_callable_with_index_v<Fn, ElementType, void> || is_callable_without_index_v<Fn, ElementType, void> ||
        is_callable_with_index_v<Fn, ElementType, bool> || is_callable_without_index_v<Fn, ElementType, bool>);

    if constexpr (is_callable_without_index_v<Fn, ElementType, bool>)
    {
        (void)counter_offset;
        while (begin != end)
        {
            if (!pred(*begin))
            {
                ++begin;
                continue;
            }
            if (!fn(transform(*begin++)))
            {
                return begin;
            }
        }
        return begin;
    }
    else if constexpr (is_callable_with_index_v<Fn, ElementType, bool>)
    {
        uint32_t index{counter_offset};
        while (begin != end)
        {
            if (!pred(*begin))
            {
                ++begin;
                continue;
            }
            if (!fn(transform(*begin++), index++))
            {
                return begin;
            }
        }
        return begin;
    }
    else if constexpr (is_callable_without_index_v<Fn, ElementType, void>)
    {
        (void)counter_offset;
        while (begin != end)
        {
            if (!pred(*begin))
            {
                ++begin;
                continue;
            }
            fn(transform(*begin++));
        }
        return begin;
    }
    else if constexpr (is_callable_with_index_v<Fn, ElementType, void>)
    {
        uint32_t index{counter_offset};
        while (begin != end)
        {
            if (!pred(*begin))
            {
                ++begin;
                continue;
            }
            fn(transform(*begin++), index++);
        }
        return begin;
    }
}

}  // namespace mockturtle::detail

#endif  // FICTION_MOCKTURTLE_UTILS_HPP
