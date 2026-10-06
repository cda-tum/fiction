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
 * @brief Critical path length and throughput of a gate-level layout.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/traits.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace fiction::verification
{

/**
 * Critical path length and throughput storage struct.
 */
struct cp_and_tp
{
    /**
     * Length of the critical path in tiles.
     */
    uint64_t critical_path_length{0ull};
    /**
     * Throughput of the layout in clock cycles as \f$\frac{1}{x}\f$ where only \f$x\f$ is stored.
     */
    uint64_t throughput{0ull};
};

namespace detail
{

/**
 * @brief Evaluates declared output dependencies with an explicit traversal stack.
 * @tparam Lyt Placed object layout type.
 */
template <typename Lyt>
class critical_path_length_and_throughput_impl
{
  public:
    /** @brief Stores the layout without copying it. @param src Layout to analyze. */
    explicit critical_path_length_and_throughput_impl(const Lyt& src) : lyt{src} {}

    /** @brief Computes physical path length and throughput. @return Path length and throughput denominator. */
    cp_and_tp run()
    {
        cp_and_tp result{};
        uint64_t  max_diff{};
        lyt.foreach_po(
            [&](const auto po)
            {
                if (!cache.contains(po))
                {
                    cache.emplace(po, std::nullopt);
                    pending.push_back({po, 0});
                }
                while (!pending.empty())
                {
                    auto& current = pending.back();
                    if (current.input < lyt.input_count(current.object))
                    {
                        const auto source = lyt.source({current.object, current.input++});
                        if (!source)
                        {
                            throw std::invalid_argument("A primary output dependency has a disconnected input");
                        }
                        const auto [entry, inserted] = cache.try_emplace(source->object, std::nullopt);
                        if (inserted)
                        {
                            pending.push_back({source->object, 0});
                        }
                        else if (!entry->second)
                        {
                            throw std::invalid_argument("A primary output dependency contains a cycle");
                        }
                        continue;
                    }
                    path_info path{1, lyt.get_clock_number(lyt.get_tile(current.object))};
                    if (lyt.input_count(current.object) != 0)
                    {
                        uint64_t shortest_delay = std::numeric_limits<uint64_t>::max();
                        path                    = {};
                        for (uint32_t input{}; input < lyt.input_count(current.object); ++input)
                        {
                            const auto child = *cache.at(lyt.source({current.object, input})->object);
                            path.length      = std::max(path.length, child.length);
                            path.delay       = std::max(path.delay, child.delay);
                            shortest_delay   = std::min(shortest_delay, child.delay);
                        }
                        max_diff = std::max(max_diff, path.delay - shortest_delay);
                        ++path.length;
                        ++path.delay;
                    }
                    cache.at(current.object) = path;
                    pending.pop_back();
                }
                result.critical_path_length = std::max(result.critical_path_length, cache.at(po)->length);
            });
        result.throughput = max_diff / lyt.num_clocks() + 1;
        return result;
    }

  private:
    /** @brief Source layout; analysis never mutates layout state. */
    const Lyt& lyt;
    /** @brief Completed path length and arrival phase. */
    struct path_info
    {
        /** @brief Number of placed objects on the longest path. */
        uint64_t length{};
        /** @brief Latest arrival phase, including the source's clock number. */
        uint64_t delay{};
    };
    /** @brief Suspended traversal of an object's ordered inputs. */
    struct frame
    {
        /** @brief Object under traversal. */
        typename Lyt::object_id object{};
        /** @brief Next input index to visit. */
        uint32_t input{};
    };
    /** @brief An empty entry marks an active dependency; a value marks a completed dependency. */
    std::unordered_map<typename Lyt::object_id, std::optional<path_info>> cache{};
    /** @brief Explicit traversal stack independent of native call stack size. */
    std::vector<frame> pending{};
};

}  // namespace detail

/**
 * Computes the critical path length (CP) length and the throughput (TP) of a gate-level layout.
 *
 * The critical path length counts every placed object on the longest path to any PO, including wires and terminals.
 * Traversal follows declared input ports, independent of physical adjacency and clocking legality.
 * Only output dependencies enter the analysis. Explicit placed zero-input functions act as path sources.
 *
 * The throughput is defined as \f$\frac{1}{x}\f$ where \f$x\f$ is the highest path length difference between any
 * sets of paths that lead to the same gate. This function provides only the denominator \f$x\f$, as the numerator is
 * always \f$1\f$. Furthermore, \f$x\f$ is given in clock cycles rather than clock phases because it is assumed that
 * a path length difference smaller than `lyt.num_clocks()` does not lead to any delay. Contrary, for any throughput
 * value \f$\frac{1}{x}\f$ with \f$x > 1\f$, the layout computes its represented Boolean function only every \f$x\f$
 * full clock cycles after the first inputs have been propagated through the design. Thereby, all PIs need to be
 * held constant for \f$x\f$ clock phases to ensure proper computation.
 *
 * For more information on the concept of throughput and delay see \"Synchronization of Clocked Field-Coupled Circuits\"
 * by F. Sill Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO 2018; or \"Design Automation for
 * Field-coupled Nanotechnologies\" by M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer
 * Nature in 2022.
 *
 * The complexity of this function is \f$\mathcal{O}(|T|)\f$ where \f$T\f$ is the set of all occupied tiles in `lyt`.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout whose CP and TP are desired.
 * @return A struct containing the CP and TP.
 * @throws std::invalid_argument If an output dependency has a disconnected input or a cycle.
 */
template <typename Lyt>
cp_and_tp critical_path_length_and_throughput(const Lyt& lyt)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate layout type");

    detail::critical_path_length_and_throughput_impl p{lyt};

    return p.run();
}

}  // namespace fiction::verification
