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
 * @brief 2D bounding box that encloses all non-empty coordinates of a layout.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/layouts/cell_grid.hpp"
#include "fiction/layouts/gate_layout.hpp"
#include "fiction/layouts/layout_base.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <optional>

namespace fiction::layouts
{
/**
 * @brief Cached two-dimensional bounds of every occupied coordinate, including positions outside the geometry.
 * @tparam Lyt Gate-level layout or cell grid.
 */
template <typename Lyt>
    requires(
        requires { requires std::same_as<typename Lyt::object_id, layout_object_id>; } ||
        requires { requires std::derived_from<Lyt, cell_grid<typename Lyt::cell_type>>; })
class bounding_box
{
  public:
    /** @brief Computes occupied bounds. @param lyt Layout whose occupied positions are enclosed. */
    explicit bounding_box(const Lyt& lyt) : layout{lyt}
    {
        update_bounding_box();
    }
    /** @brief Recomputes bounds from live occupied coordinates. Empty layouts have no bounds. */
    void update_bounding_box()
    {
        min.reset();
        max.reset();
        x_size             = 0;
        y_size             = 0;
        const auto include = [&](const auto& c)
        {
            if (!min)
            {
                min = fiction::layouts::layout_base::coordinate{c.x, c.y, 0};
                max = min;
            }
            else
            {
                min->x = std::min(min->x, c.x);
                min->y = std::min(min->y, c.y);
                max->x = std::max(max->x, c.x);
                max->y = std::max(max->y, c.y);
            }
        };
        if constexpr (requires { requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>; })
        {
            layout.foreach_object([&](const auto id) { include(layout.get_tile(id)); });
        }
        else
        {
            layout.foreach_cell(include);
        }
        if (min)
        {
            x_size = static_cast<uint64_t>(static_cast<int64_t>(max->x) - min->x) + 1;
            y_size = static_cast<uint64_t>(static_cast<int64_t>(max->y) - min->y) + 1;
        }
    }
    /** @brief Returns the minimum occupied corner, or no coordinate for an empty layout. */
    [[nodiscard]] std::optional<fiction::layouts::layout_base::coordinate> get_min() const noexcept
    {
        return min;
    }
    /** @brief Returns the maximum occupied corner, or no coordinate for an empty layout. */
    [[nodiscard]] std::optional<fiction::layouts::layout_base::coordinate> get_max() const noexcept
    {
        return max;
    }
    /** @brief Counts occupied bounding columns; zero for an empty layout. */
    [[nodiscard]] uint64_t get_x_size() const noexcept
    {
        return x_size;
    }
    /** @brief Counts occupied bounding rows; zero for an empty layout. */
    [[nodiscard]] uint64_t get_y_size() const noexcept
    {
        return y_size;
    }

  private:
    /** @brief Layout observed when bounds are recomputed. */
    const Lyt& layout;
    /** @brief Minimum occupied corner. */
    std::optional<fiction::layouts::layout_base::coordinate> min{};
    /** @brief Maximum occupied corner. */
    std::optional<fiction::layouts::layout_base::coordinate> max{};
    /** @brief Number of bounding columns. */
    uint64_t x_size{};
    /** @brief Number of bounding rows. */
    uint64_t y_size{};
};
}  // namespace fiction::layouts
