//
// Created by marcel on 28.06.21.
//

#ifndef FICTION_HASH_HPP
#define FICTION_HASH_HPP

#include <array>
#include <functional>
#include <set>
#include <utility>

namespace fiction
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

}  // namespace fiction

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
    std::size_t operator()(const std::array<T, N>& a) const noexcept
    {
        std::size_t h = 0;
        for (const auto& e : a)
        {
            fiction::hash_combine(h, e);
        }

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
    std::size_t operator()(const std::set<T>& s) const noexcept
    {
        std::size_t h = 0;
        for (const auto& e : s)
        {
            fiction::hash_combine(h, e);
        }

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
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept
    {
        std::size_t h = 0;
        fiction::hash_combine(h, p.first, p.second);

        return h;
    }
};

}  // namespace std

#endif  // FICTION_HASH_HPP
