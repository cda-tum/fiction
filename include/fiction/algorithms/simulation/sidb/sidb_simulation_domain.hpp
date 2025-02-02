//
// Created by Jan Drewniok on 29.01.25.
//

#include "fiction/utils/phmap_utils.hpp"

#include <cstdio>
#include <functional>
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
     * Checks whether a specified key exists in the given map and retrieves its associated value if present.
     * This function utilizes the `if_contains` method of the map to ensure thread-safe access.
     *
     * @tparam MapType The type of the map, which must provide the `if_contains` method.
     * @param map The map in which to search for the specified key.
     * @param key The key to search for in the map.
     * @return An `std::optional` containing the value associated with the key if it exists, `std::optional` otherwise.
     */
    [[nodiscard]] std::optional<std::tuple<MappedTypes...>> has_already_been_sampled(const Key& key) const noexcept
    {
        if (const auto v = contains_key(key); v.has_value())
        {
            return v.value();
        }

        return std::nullopt;
    }
    /**
     * Constructs a new `sidb_simulation_domain` instance.
     */
    sidb_simulation_domain()
    {
        static_assert(sizeof...(MappedTypes) > 0, "MappedTypes must not be empty");
    }
    /**
     * Retrieves the value associated with the provided key from the operational domain.
     * If the key is found, the value is returned; otherwise, `std::nullopt` is returned.
     *
     * @param key The key to look up in the domain.
     * @return The `std::tuple`` associated with the provided key is returned, `std::nullopt` otherwise.
     */
    [[nodiscard]] std::optional<std::tuple<MappedTypes...>> get_value(const Key& key) const
    {
        return contains_key(key);
    }
    /**
     * Adds a value to the operational domain.
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
     * @param key The key to remove from the domain.
     */
    [[nodiscard]] std::size_t number_of_values() const
    {
        return domain_values.size();
    }
    /**
     * Iterates over all key-value pairs in the operational domain and applies a provided callback function
     * to each pair. This method ensures thread-safe access to the underlying data by leveraging the
     * `for_each` method of the thread-safe `locked_parallel_flat_hash_map`.
     */
    void for_each(const std::function<void(const Key&, const std::tuple<MappedTypes...>&)>& callback) const
    {
        for (auto it = domain_values.cbegin(); it != domain_values.cend(); ++it)
        {
            callback(it->first, it->second);
        }
    }
    /**
     * Checks whether a specified key exists in the given map and retrieves its associated value if present.
     * This function utilizes the `if_contains` method of the map to ensure thread-safe access.
     *
     * @tparam MapType The type of the map, which must provide the `if_contains` method.
     * @param map The map in which to search for the specified key.
     * @param key The key to search for in the map.
     * @return An `std::optional` containing the value associated with the key if it exists, `std::optional` otherwise.
     */
    [[nodiscard]] std::optional<std::tuple<MappedTypes...>> contains_key(const Key& key) const
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
