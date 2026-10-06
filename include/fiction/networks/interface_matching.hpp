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
 * @brief Matches declared primary interfaces by unique names and position.
 */

#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::networks
{

/** @brief Complete permutations from the left interface to the right interface. */
struct interface_matching
{
    /** @brief Right primary input index for each left primary input index. */
    std::vector<uint32_t> inputs{};
    /** @brief Right primary output index for each left primary output index. */
    std::vector<uint32_t> outputs{};
};

namespace detail
{

/**
 * @brief Reads primary input names in declared order from a layout or network.
 * @tparam NtkOrLyt Network or placed object layout.
 * @param ntk Interface owner.
 * @return Names, with empty strings for unnamed primary inputs.
 */
template <typename NtkOrLyt>
std::vector<std::string> interface_input_names(const NtkOrLyt& ntk)
{
    std::vector<std::string> names(ntk.num_pis());
    for (uint32_t i{}; i < ntk.num_pis(); ++i)
    {
        if constexpr (requires { ntk.get_input_name(i); })
        {
            names[i] = ntk.get_input_name(i);
        }
        else if constexpr (requires {
                               ntk.has_name(ntk.make_signal(ntk.pi_at(i)));
                               ntk.get_name(ntk.make_signal(ntk.pi_at(i)));
                           })
        {
            const auto pi = ntk.make_signal(ntk.pi_at(i));
            if (ntk.has_name(pi))
            {
                names[i] = ntk.get_name(pi);
            }
        }
    }
    return names;
}

/**
 * @brief Reads primary output names in declared order from a layout or network.
 * @tparam NtkOrLyt Network or placed object layout.
 * @param ntk Interface owner.
 * @return Names, with empty strings for unnamed primary outputs.
 */
template <typename NtkOrLyt>
std::vector<std::string> interface_output_names(const NtkOrLyt& ntk)
{
    std::vector<std::string> names(ntk.num_pos());
    if constexpr (requires {
                      ntk.has_output_name(0u);
                      ntk.get_output_name(0u);
                  })
    {
        for (uint32_t i{}; i < ntk.num_pos(); ++i)
        {
            if (ntk.has_output_name(i))
            {
                names[i] = ntk.get_output_name(i);
            }
        }
    }
    return names;
}

/**
 * @brief Matches equal names that occur once on each side, then matches remaining positions in declared order.
 * @param left Left interface names.
 * @param right Right interface names with the same count.
 * @return Right index for each left index.
 */
inline std::vector<uint32_t> match_interface_names(const std::vector<std::string>& left,
                                                   const std::vector<std::string>& right)
{
    /** @brief Name occurrence count and its last interface index. */
    using occurrence = std::pair<uint32_t, uint32_t>;
    std::unordered_map<std::string, occurrence> left_names{}, right_names{};
    for (uint32_t i{}; i < left.size(); ++i)
    {
        ++left_names[left[i]].first;
        left_names[left[i]].second = i;
        ++right_names[right[i]].first;
        right_names[right[i]].second = i;
    }
    constexpr auto        unmatched = std::numeric_limits<uint32_t>::max();
    std::vector<uint32_t> permutation(left.size(), unmatched);
    std::vector<bool>     used(right.size());
    for (uint32_t i{}; i < left.size(); ++i)
    {
        const auto candidate = right_names.find(left[i]);
        if (!left[i].empty() && left_names.at(left[i]).first == 1 && candidate != right_names.end() &&
            candidate->second.first == 1)
        {
            permutation[i]       = candidate->second.second;
            used[permutation[i]] = true;
        }
    }
    uint32_t next{};
    for (auto& index : permutation)
    {
        if (index == unmatched)
        {
            while (used[next])
            {
                ++next;
            }
            index = next++;
        }
    }
    return permutation;
}

}  // namespace detail

/**
 * @brief Matches primary inputs and outputs by unique nonempty names, then by remaining declared positions.
 *
 * A name matches only when it occurs once in each corresponding interface. Empty, duplicate, and unmatched names
 * fall back to the remaining positions in declared order. Every terminal receives exactly one match.
 * @tparam Left Network or placed object layout.
 * @tparam Right Network or placed object layout.
 * @param left Left interface owner.
 * @param right Right interface owner.
 * @return Complete permutations from left terminal indices to right terminal indices.
 * @throws std::invalid_argument If the primary input counts or primary output counts differ.
 */
template <typename Left, typename Right>
[[nodiscard]] interface_matching match_interfaces(const Left& left, const Right& right)
{
    if (left.num_pis() != right.num_pis() || left.num_pos() != right.num_pos())
    {
        throw std::invalid_argument("Primary interface sizes differ");
    }
    return {detail::match_interface_names(detail::interface_input_names(left), detail::interface_input_names(right)),
            detail::match_interface_names(detail::interface_output_names(left), detail::interface_output_names(right))};
}

}  // namespace fiction::networks
