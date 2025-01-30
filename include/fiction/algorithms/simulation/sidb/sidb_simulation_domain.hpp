//
// Created by Jan Drewniok on 29.01.25.
//

#include "fiction/technology/physical_constants.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <optional>
#include <tuple>

namespace fiction
{

/**
 * Checks whether a specified key exists in the given map and retrieves its associated value if present.
 * This function utilizes the `if_contains` method of the map to ensure thread-safe access.
 *
 * @tparam MapType The type of the map, which must provide the `if_contains` method.
 * @param map The map in which to search for the specified key.
 * @param key The key to search for in the map.
 * @return An `std::optional` containing the value associated with the key if it exists, `std::optional` otherwise.
 */
template <typename MapType>
std::optional<typename MapType::mapped_type> contains_key(const MapType& map, const typename MapType::key_type& key)
{
    std::optional<typename MapType::mapped_type> result;

    map.if_contains(key, [&result](const typename MapType::value_type& entry) { result = entry.second; });

    return result;
}

/**
 * This function searches for a floating-point value specified by the `key` in the provided map `map`, applying a
 * tolerance specified by `fiction::physical_constants::POP_STABILITY_ERR`. Each key in the map is compared to the
 * specified key within this tolerance.
 *
 * @tparam MapType The type of the map containing parameter points as keys.
 * @param map The map containing parameter points as keys and associated values.
 * @param key The parameter point to search for in the map.
 * @return An iterator to the found parameter point in the map, or `map.cend()` if not found.
 */
template <typename MapType>
typename MapType::const_iterator find_key_with_tolerance(const MapType& map, const typename MapType::key_type& key)
{
    static_assert(std::is_floating_point_v<typename MapType::key_type>, "Map key type must be floating-point");

    constexpr double tolerance = physical_constants::POP_STABILITY_ERR;

    auto compare_keys = [&key, &tolerance](const auto& pair) { return std::abs(pair.first - key) < tolerance; };

    return std::find_if(map.cbegin(), map.cend(), compare_keys);
}

/**
 * The `sidb_simulation_domain` is designed to represent a generic simulation domain where keys are
 * associated with values stored as tuples. It uses a `locked_parallel_flat_hash_map` to ensure
 * thread-safe access to the stored data. This is especially useful for parallel simulations or
 * multithreaded environments.
 *
 * @tparam Key The type of the key used to identify entries in the domain.
 * @tparam Value1 The first value type stored in the tuple associated with each key.
 * @tparam Valuetypes Additional value types stored in the tuple.
 */
template <typename Key, typename Value1, typename... Valuetypes>
struct sidb_simulation_domain
{
    locked_parallel_flat_hash_map<Key, std::tuple<Value1, Valuetypes...>> domain_values{};
    /**
     * Retrieves the value associated with the provided key from the operational domain.
     * If the key is found, the value is returned; otherwise, `std::nullopt` is returned.
     *
     * @param key The key to look up in the domain.
     * @return The `std::tuple`` associated with the provided key is returned, `std::nullopt` otherwise.
     */
    [[nodiscard]] std::optional<std::tuple<Value1, Valuetypes...>> get_value(const Key& key) const
    {
        return contains_key(domain_values, key);
    }

    /**
     * Adds a value to the operational domain.
     *
     * @param key The key to associate with the value.
     * @param value The value to add, which must be a tuple.
     */
    void add_value(const Key& key, const std::tuple<Value1, Valuetypes...>& value)
    {
        domain_values.try_emplace(key, value);
    }
    /**
     * Returns the entire operational domain.
     *
     * @return The operational domain as a map.
     */
    [[nodiscard]] const auto& get_domain() const
    {
        return domain_values;
    }
};

}  // namespace fiction
