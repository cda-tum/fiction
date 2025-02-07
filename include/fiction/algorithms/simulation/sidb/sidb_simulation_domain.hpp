//
// Created by Jan Drewniok on 29.01.25.
//

#ifndef FICTION_SIDB_SIMULATION_DOMAIN_HPP
#define FICTION_SIDB_SIMULATION_DOMAIN_HPP

#include "fiction/utils/phmap_utils.hpp"

#include <cstdio>
#include <optional>
#include <tuple>

namespace fiction
{

/**
 * The `sidb_simulation_domain` is designed to represent a generic simulation domain where keys are
 * associated with values stored as tuples. It uses a `locked_parallel_flat_hash_map` to ensure
 * thread-safe access to the stored data. All methods of this class are thread-safe.
 *
 * @tparam Key The type of the key used to identify entries in the domain.
 * @tparam MappedTypes Value types stored in the tuple.
 */
template <typename Key, typename... MappedTypes>
class sidb_simulation_domain
{
  public:
    /**
     * Constructs a new `sidb_simulation_domain` instance.
     */
    sidb_simulation_domain()
    {
        static_assert(sizeof...(MappedTypes) > 0, "MappedTypes must not be empty");
    }
    /**
     * Adds a value to the operational domain. This function is thread-safe and uses the `try_emplace` method.
     *
     * @param key The key to associate with the value.
     * @param value The value to add, which must be a tuple.
     */
    void add_value(const Key& key, const std::tuple<MappedTypes...>& value)
    {
        domain_values.try_emplace(key, value);
    }
    /**
     * Counts the number of key-value pairs in the operational domain.
     *
     * @return The size of the operational domain.
     */
    [[nodiscard]] std::size_t size() const
    {
        return domain_values.size();
    }
    /**
     * Applies a callable to all key-value pairs in the container.
     *
     * @tparam Fn Functor type.
     * @param fn Functor to apply to each key-value pair.
     */
    template <typename Fn>
    void for_each(Fn&& fn) const
    {
        // copy for thread-safety
        const auto domain_values_copy = domain_values;

        std::for_each(domain_values_copy.cbegin(), domain_values_copy.cend(),
                      [&fn](const auto& pair) { fn(pair.first, pair.second); });
    }
    /**
     * Checks whether a specified key exists in the given map and retrieves its associated value if present.
     * This function utilizes the `if_contains` method of the map to ensure thread-safe access.
     *
     * @param key The key to search for in the map.
     * @return An `std::optional` containing the value associated with the key if it exists, `std::optional` otherwise.
     */
    [[nodiscard]] std::optional<std::tuple<MappedTypes...>> contains(const Key& key) const
    {
        std::optional<std::tuple<MappedTypes...>> result;

        domain_values.if_contains(key, [&result](const auto& entry) { result = entry.second; });

        return result;
    }

  private:
    /**
     * The domain values stored in a thread-safe map.
     */
    locked_parallel_flat_hash_map<Key, std::tuple<MappedTypes...>> domain_values{};
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_DOMAIN_HPP
