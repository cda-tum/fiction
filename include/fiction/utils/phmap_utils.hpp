//
// Created by marcel on 08.08.23.
//

#ifndef FICTION_PHMAP_UTILS_HPP
#define FICTION_PHMAP_UTILS_HPP

#include <phmap.h>

#include <memory>
#include <mutex>
#include <utility>

namespace fiction
{

/**
 * A parallel flat hash map with built-in mutexes. This enables thread-safe access to the map without the need to
 * manually lock and unlock the map. Since the map uses internal buckets that can be accessed in parallel, this
 * implementation is a lot faster than a regular hash map with a single mutex.
 */
template <typename K, typename V>
using locked_parallel_flat_hash_map =
    phmap::parallel_flat_hash_map<K, V, phmap::priv::hash_default_hash<K>, phmap::priv::hash_default_eq<K>,
                                  std::allocator<std::pair<const K, V>>, 4, std::mutex>;

}  // namespace fiction

#endif  // FICTION_PHMAP_UTILS_HPP
