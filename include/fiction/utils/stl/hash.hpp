/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 28.06.21.
//

#ifndef FICTION_UTILS_STL_HASH_HPP
#define FICTION_UTILS_STL_HASH_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <utility>

namespace fiction::utils::stl
{

/**
 * A recursive `hash_combine` implementation from
 * https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
 *
 * Overrides the passed seed.
 *
 * @tparam T Type to hash.
 * @tparam Rest Parameter pack.
 * @param seed Hashing seed. This value is overridden with the hash value.
 * @param v Value to hash next.
 * @param rest Remaining values to hash.
 */
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    (hash_combine(seed, rest), ...);
}
/**
 * @brief Combines a hash value into a seed independently of the order in which the values arrive.
 *
 * `hash_combine` is order-dependent by construction, which rules it out for folding over a container whose
 * iteration order is not canonical, such as a hash map. This function adds instead, which is commutative, so the
 * seed depends only on which values were combined and how often, not on their order.
 *
 * The scrambling step is not optional. `std::hash` of the cube and SiQAD coordinate types weighs the coordinate
 * components linearly, so a plain sum over their hash values makes the cell sets `{(0, 0), (3, 0)}` and
 * `{(1, 0), (2, 0)}` collide. The splitmix64 finalizer applied here spreads every input bit across the whole word
 * before the sum sees it.
 *
 * Overrides the passed seed.
 *
 * @tparam T Type to hash.
 * @param seed Hashing seed. This value is overridden with the combined hash value.
 * @param v Value to hash next.
 */
template <typename T>
void hash_combine_unordered(std::size_t& seed, const T& v) noexcept
{
    auto scrambled = static_cast<uint64_t>(std::hash<T>{}(v));

    scrambled ^= scrambled >> 30u;
    scrambled *= 0xbf58476d1ce4e5b9ull;
    scrambled ^= scrambled >> 27u;
    scrambled *= 0x94d049bb133111ebull;
    scrambled ^= scrambled >> 31u;

    seed += static_cast<std::size_t>(scrambled);
}

}  // namespace fiction::utils::stl
namespace std
{

/**
 * Provides a hash implementation for `std::array<T, N>`.
 *
 * @tparam T Object type in `std::array`.
 * @tparam N Size of the array.
 */
template <typename T, std::size_t N>
struct hash<std::array<T, N>>
{
    /**
     * Computes the hash value of a given `std::array`.
     *
     * @param a Array to hash.
     * @return Hash value of `a`.
     */
    std::size_t operator()(const std::array<T, N>& a) const noexcept
    {
        std::size_t h = 0;
        std::ranges::for_each(a, [&h](const auto& e) { fiction::utils::stl::hash_combine(h, e); });

        return h;
    }
};
/**
 * Provides a hash implementation for `std::set<T>`.
 *
 * @tparam T Object type in `std::set`.
 */
template <typename T>
struct hash<std::set<T>>
{
    /**
     * Computes the hash value of a given `std::set`.
     *
     * @param s Set to hash.
     * @return Hash value of `s`.
     */
    std::size_t operator()(const std::set<T>& s) const noexcept
    {
        std::size_t h = 0;
        std::ranges::for_each(s, [&h](const auto& e) { fiction::utils::stl::hash_combine(h, e); });

        return h;
    }
};
/**
 * Provides a hash implementation for `std::multiset<T>`.
 *
 * @tparam T Object type in `std::multiset`.
 */
template <typename T>
struct hash<std::multiset<T>>
{
    /**
     * Computes the hash value of a given `std::multiset`.
     *
     * @param s Multiset to hash.
     * @return Hash value of `s`.
     */
    std::size_t operator()(const std::multiset<T>& s) const noexcept
    {
        std::size_t h = 0;
        std::ranges::for_each(s, [&h](const auto& e) { fiction::utils::stl::hash_combine(h, e); });

        return h;
    }
};
/**
 * Provides a hash implementation for `std::pair<T1, T2>`.
 *
 * @tparam T1 First object type in `std::pair`.
 * @tparam T2 Second object type in `std::pair`.
 */
template <typename T1, typename T2>
struct hash<std::pair<T1, T2>>
{
    /**
     * @brief Computes the hash value of a given `std::pair`.
     *
     * @param p Pair to hash.
     * @return Hash value of `p`.
     */
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept
    {
        std::size_t h = 0;
        fiction::utils::stl::hash_combine(h, p.first, p.second);

        return h;
    }
};

}  // namespace std

#endif  // FICTION_UTILS_STL_HASH_HPP
