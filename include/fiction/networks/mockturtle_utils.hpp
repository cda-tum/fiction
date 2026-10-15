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
 * @brief Filtering-and-transforming variant of mockturtle's `foreach_element` helper.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <mockturtle/networks/detail/foreach.hpp>

#include <cstdint>

namespace mockturtle::detail
{

/**
 * @brief Applies a transform and callback to elements that satisfy a predicate. A false callback result stops
 * traversal.
 * @tparam Iterator Iterator over the source elements.
 * @tparam ElementType Transformed element type passed to the callback.
 * @tparam Pred Predicate selecting source elements.
 * @tparam Transform Transform applied to selected elements.
 * @tparam Fn Void or Boolean callback, optionally accepting an index.
 * @param begin First source element.
 * @param end End of the source range.
 * @param pred Selection predicate.
 * @param transform Element transform.
 * @param fn Callback for transformed elements.
 * @param counter_offset First callback index.
 * @return Iterator after the last visited source element.
 */
template <class Iterator, class ElementType, class Pred, class Transform, class Fn>
    requires(is_callable_with_index_v<Fn, ElementType, void> || is_callable_without_index_v<Fn, ElementType, void> ||
             is_callable_with_index_v<Fn, ElementType, bool> || is_callable_without_index_v<Fn, ElementType, bool>)
Iterator foreach_element_if_transform(
    Iterator begin, Iterator end,
    Pred&&      pred,       // NOLINT(cppcoreguidelines-missing-std-forward): preserve repeated lvalue invocation
    Transform&& transform,  // NOLINT(cppcoreguidelines-missing-std-forward): preserve repeated lvalue invocation
    Fn&&        fn,         // NOLINT(cppcoreguidelines-missing-std-forward): preserve repeated lvalue invocation
    uint32_t    counter_offset = 0)
{
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
