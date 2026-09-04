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
 * @brief The SiDB layout: cells and surface defects on lattice sites of one H-Si lattice.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/technology.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::sidb
{

/**
 * An SiDB layout: SiDBs of a given cell type and surface defects placed on lattice sites of one H-Si lattice. The
 * layout has value semantics; copies are independent.
 *
 * Cells and defects are kept sorted in raster order, so `sidbs()` is the canonical SiDB order that index-based
 * structures such as charge distributions refer to, and every `foreach_*` traversal is deterministic.
 */
class layout
{
  public:
    /**
     * SiDB cell types.
     */
    using cell_type = sidb_technology::cell_type;
    /**
     * Creates an empty layout on the H-Si(100)-2x1 lattice.
     */
    layout() = default;
    /**
     * Creates an empty layout on the given lattice.
     *
     * @param lat Lattice of the surface.
     * @param name Layout name.
     */
    explicit layout(lattice lat, std::string name = "") : surface_lattice{std::move(lat)}, layout_name{std::move(name)}
    {}
    /**
     * The lattice of the surface.
     *
     * @return The lattice.
     */
    [[nodiscard]] const lattice& get_lattice() const noexcept
    {
        return surface_lattice;
    }
    /**
     * Reassigns the lattice. Sites are kept as they are; only their physical positions change.
     *
     * @param lat New lattice.
     */
    void set_lattice(const lattice& lat)
    {
        surface_lattice = lat;
    }
    /**
     * The layout name.
     *
     * @return Layout name.
     */
    [[nodiscard]] const std::string& get_layout_name() const noexcept
    {
        return layout_name;
    }
    /**
     * Sets the layout name.
     *
     * @param name New layout name.
     */
    void set_layout_name(const std::string& name)
    {
        layout_name = name;
    }

    // ------------------------------------------------------------------------------------------------------- cells

    /**
     * Assigns a cell type to a site. Assigning `cell_type::EMPTY` removes the SiDB from the site.
     *
     * @param s Site.
     * @param ct Cell type to assign.
     */
    void assign_cell_type(const lattice_site& s, const cell_type ct)
    {
        const auto it = std::ranges::lower_bound(cell_sites, s);
        const auto i  = static_cast<std::size_t>(std::distance(cell_sites.begin(), it));

        if (it != cell_sites.end() && *it == s)
        {
            if (ct == cell_type::EMPTY)
            {
                cell_sites.erase(it);
                cell_types.erase(std::next(cell_types.cbegin(), static_cast<std::ptrdiff_t>(i)));
            }
            else
            {
                cell_types[i] = ct;
            }
        }
        else if (ct != cell_type::EMPTY)
        {
            cell_sites.insert(it, s);
            cell_types.insert(std::next(cell_types.cbegin(), static_cast<std::ptrdiff_t>(i)), ct);
        }
    }
    /**
     * The cell type at a site.
     *
     * @param s Site.
     * @return Cell type at `s`, `cell_type::EMPTY` if no SiDB is there.
     */
    [[nodiscard]] cell_type get_cell_type(const lattice_site& s) const noexcept
    {
        const auto i = index_of(s);

        return i.has_value() ? cell_types[*i] : cell_type::EMPTY;
    }
    /**
     * Whether no SiDB sits at a site.
     *
     * @param s Site.
     * @return `true` iff `s` holds no SiDB.
     */
    [[nodiscard]] bool is_empty_cell(const lattice_site& s) const noexcept
    {
        return !index_of(s).has_value();
    }
    /**
     * Whether the layout holds no SiDB. Defects do not count.
     *
     * @return `true` iff there is no SiDB.
     */
    [[nodiscard]] bool is_empty() const noexcept
    {
        return cell_sites.empty();
    }
    /**
     * Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] uint64_t num_cells() const noexcept
    {
        return cell_sites.size();
    }
    /**
     * Number of SiDBs of a given cell type.
     *
     * @param ct Cell type.
     * @return Number of SiDBs of type `ct`.
     */
    [[nodiscard]] uint64_t num_cells_of_type(const cell_type ct) const noexcept
    {
        return static_cast<uint64_t>(std::ranges::count(cell_types, ct));
    }
    /**
     * All sites holding an SiDB of a given cell type, in raster order.
     *
     * @param ct Cell type.
     * @return Sites of type `ct`.
     */
    [[nodiscard]] std::vector<lattice_site> cells_of_type(const cell_type ct) const
    {
        std::vector<lattice_site> result{};

        for (std::size_t i = 0; i < cell_sites.size(); ++i)
        {
            if (cell_types[i] == ct)
            {
                result.push_back(cell_sites[i]);
            }
        }

        return result;
    }
    /**
     * All sites holding an SiDB, in raster order. This is the canonical SiDB order that index-based structures refer
     * to.
     *
     * @return Sorted SiDB sites.
     */
    [[nodiscard]] const std::vector<lattice_site>& sidbs() const noexcept
    {
        return cell_sites;
    }
    /**
     * The index of a site in `sidbs()`.
     *
     * @param s Site.
     * @return Index of `s`, or `std::nullopt` if `s` holds no SiDB.
     */
    [[nodiscard]] std::optional<std::size_t> index_of(const lattice_site& s) const noexcept
    {
        const auto it = std::ranges::lower_bound(cell_sites, s);

        if (it != cell_sites.cend() && *it == s)
        {
            return static_cast<std::size_t>(std::distance(cell_sites.cbegin(), it));
        }

        return std::nullopt;
    }
    /**
     * Applies a function to every SiDB site in raster order. `fn` takes the site and optionally its index, and may
     * return `false` to stop the traversal.
     *
     * @tparam Fn Callable on `(const lattice_site&)` or `(const lattice_site&, std::size_t)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_cell(Fn&& fn) const
    {
        for_each_site(cell_sites, std::forward<Fn>(fn));
    }
    /**
     * Number of input SiDBs.
     *
     * @return Number of input SiDBs.
     */
    [[nodiscard]] uint32_t num_pis() const noexcept
    {
        return static_cast<uint32_t>(num_cells_of_type(cell_type::INPUT));
    }
    /**
     * Number of output SiDBs.
     *
     * @return Number of output SiDBs.
     */
    [[nodiscard]] uint32_t num_pos() const noexcept
    {
        return static_cast<uint32_t>(num_cells_of_type(cell_type::OUTPUT));
    }
    /**
     * Whether a site holds an input SiDB.
     *
     * @param s Site.
     * @return `true` iff `s` holds an input SiDB.
     */
    [[nodiscard]] bool is_pi(const lattice_site& s) const noexcept
    {
        return get_cell_type(s) == cell_type::INPUT;
    }
    /**
     * Whether a site holds an output SiDB.
     *
     * @param s Site.
     * @return `true` iff `s` holds an output SiDB.
     */
    [[nodiscard]] bool is_po(const lattice_site& s) const noexcept
    {
        return get_cell_type(s) == cell_type::OUTPUT;
    }
    /**
     * Applies a function to every input SiDB site in raster order. Same callable contract as `foreach_cell`.
     *
     * @tparam Fn Callable on `(const lattice_site&)` or `(const lattice_site&, std::size_t)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        for_each_site(cells_of_type(cell_type::INPUT), std::forward<Fn>(fn));
    }
    /**
     * Applies a function to every output SiDB site in raster order. Same callable contract as `foreach_cell`.
     *
     * @tparam Fn Callable on `(const lattice_site&)` or `(const lattice_site&, std::size_t)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        for_each_site(cells_of_type(cell_type::OUTPUT), std::forward<Fn>(fn));
    }

    // ----------------------------------------------------------------------------------------------------- defects

    /**
     * Places a surface defect at a site. A defect of type `defect_type::NONE` removes the defect from the site.
     *
     * @param s Site.
     * @param d Defect to place.
     */
    void assign_defect(const lattice_site& s, const model::defect& d)
    {
        const auto it =
            std::ranges::lower_bound(surface_defects, s, {}, &std::pair<lattice_site, model::defect>::first);

        if (it != surface_defects.cend() && it->first == s)
        {
            if (d.type == model::defect_type::NONE)
            {
                surface_defects.erase(it);
            }
            else
            {
                it->second = d;
            }
        }
        else if (d.type != model::defect_type::NONE)
        {
            surface_defects.emplace(it, s, d);
        }
    }
    /**
     * Moves the defect at one site to another, replacing whatever defect the target site held.
     *
     * @param source Site holding the defect.
     * @param target Site to move the defect to.
     */
    void move_defect(const lattice_site& source, const lattice_site& target)
    {
        const auto d = get_defect(source);

        assign_defect(source, model::defect{model::defect_type::NONE});
        assign_defect(target, d);
    }
    /**
     * The defect at a site.
     *
     * @param s Site.
     * @return Defect at `s`, or a defect of type `defect_type::NONE` if there is none.
     */
    [[nodiscard]] model::defect get_defect(const lattice_site& s) const noexcept
    {
        const auto it =
            std::ranges::lower_bound(surface_defects, s, {}, &std::pair<lattice_site, model::defect>::first);

        if (it != surface_defects.cend() && it->first == s)
        {
            return it->second;
        }

        return model::defect{model::defect_type::NONE};
    }
    /**
     * Number of surface defects.
     *
     * @return Number of defects.
     */
    [[nodiscard]] uint64_t num_defects() const noexcept
    {
        return surface_defects.size();
    }
    /**
     * Number of positively charged defects.
     *
     * @return Number of defects with positive charge.
     */
    [[nodiscard]] uint64_t num_positively_charged_defects() const noexcept
    {
        return count_defects(model::is_positively_charged_defect);
    }
    /**
     * Number of negatively charged defects.
     *
     * @return Number of defects with negative charge.
     */
    [[nodiscard]] uint64_t num_negatively_charged_defects() const noexcept
    {
        return count_defects(model::is_negatively_charged_defect);
    }
    /**
     * Number of charged defects.
     *
     * @return Number of defects with non-zero charge.
     */
    [[nodiscard]] uint64_t num_charged_defects() const noexcept
    {
        return count_defects(model::is_charged_defect_type);
    }
    /**
     * Number of neutral defects.
     *
     * @return Number of defects with zero charge.
     */
    [[nodiscard]] uint64_t num_neutral_defects() const noexcept
    {
        return count_defects(model::is_neutrally_charged_defect);
    }
    /**
     * All defects with their sites, in raster order.
     *
     * @return Site-defect pairs.
     */
    [[nodiscard]] const std::vector<std::pair<lattice_site, model::defect>>& defects() const noexcept
    {
        return surface_defects;
    }
    /**
     * Applies a function to every defect in raster order. `fn` takes the `(site, defect)` pair and optionally its
     * index, and may return `false` to stop the traversal.
     *
     * @tparam Fn Callable on `(const std::pair<lattice_site, model::defect>&)` or the same plus `std::size_t`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_defect(Fn&& fn) const
    {
        for_each_site(surface_defects, std::forward<Fn>(fn));
    }
    /**
     * The sites whose SiDBs the defect at a given site would influence: the rectangle around the defect spanned by
     * `model::defect_extent`, counted in unit cells along the lattice vectors and keeping the defect's basis site.
     * The rectangle is clipped to the representable lattice-site coordinates.
     *
     * @param s Site of the defect.
     * @param charged_defect_spacing_overwrite Horizontal and vertical extent to use for charged defects instead of the
     * default.
     * @param neutral_defect_spacing_overwrite Horizontal and vertical extent to use for neutral defects instead of the
     * default.
     * @return Sites influenced by the defect at `s`; empty if `s` holds no defect.
     */
    [[nodiscard]] std::unordered_set<lattice_site> affected_sidbs(
        const lattice_site&                                 s,
        const std::optional<std::pair<uint16_t, uint16_t>>& charged_defect_spacing_overwrite = std::nullopt,
        const std::optional<std::pair<uint16_t, uint16_t>>& neutral_defect_spacing_overwrite = std::nullopt) const
    {
        std::unordered_set<lattice_site> influenced{};

        if (const auto d = get_defect(s); d.type != model::defect_type::NONE)
        {
            const auto [horizontal_extent, vertical_extent] =
                model::defect_extent(d, charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite);

            const auto min_x = std::max<int64_t>(int64_t{s.x} - horizontal_extent, std::numeric_limits<int32_t>::min());
            const auto max_x = std::min<int64_t>(int64_t{s.x} + horizontal_extent, std::numeric_limits<int32_t>::max());
            const auto min_y = std::max<int64_t>(int64_t{s.y} - vertical_extent, std::numeric_limits<int32_t>::min());
            const auto max_y = std::min<int64_t>(int64_t{s.y} + vertical_extent, std::numeric_limits<int32_t>::max());

            for (auto y = min_y; y <= max_y; ++y)
            {
                for (auto x = min_x; x <= max_x; ++x)
                {
                    influenced.insert(lattice_site{x, y, s.z});
                }
            }
        }

        return influenced;
    }
    /**
     * The union of `affected_sidbs` over every defect.
     *
     * @param charged_defect_spacing_overwrite Horizontal and vertical extent to use for charged defects instead of the
     * default.
     * @param neutral_defect_spacing_overwrite Horizontal and vertical extent to use for neutral defects instead of the
     * default.
     * @return Sites influenced by any defect.
     */
    [[nodiscard]] std::unordered_set<lattice_site> all_affected_sidbs(
        const std::optional<std::pair<uint16_t, uint16_t>>& charged_defect_spacing_overwrite = std::nullopt,
        const std::optional<std::pair<uint16_t, uint16_t>>& neutral_defect_spacing_overwrite = std::nullopt) const
    {
        std::unordered_set<lattice_site> influenced{};

        for (const auto& [s, d] : surface_defects)
        {
            influenced.merge(affected_sidbs(s, charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite));
        }

        return influenced;
    }

    // ---------------------------------------------------------------------------------------------------- geometry

    /**
     * The smallest rectangle of sites containing every SiDB and every defect. The corners are the north-western and
     * south-eastern site; an empty layout yields `{(0,0,0), (0,0,0)}`.
     *
     * @return North-western and south-eastern corner.
     */
    [[nodiscard]] std::pair<lattice_site, lattice_site> bounding_box() const noexcept
    {
        if (cell_sites.empty() && surface_defects.empty())
        {
            return {};
        }

        auto min_x = std::numeric_limits<int32_t>::max(), max_x = std::numeric_limits<int32_t>::min();
        auto min_row = std::numeric_limits<int64_t>::max(), max_row = std::numeric_limits<int64_t>::min();

        const auto extend = [&](const lattice_site& s) noexcept
        {
            min_x   = std::min(min_x, s.x);
            max_x   = std::max(max_x, s.x);
            min_row = std::min(min_row, row_of(s));
            max_row = std::max(max_row, row_of(s));
        };

        std::ranges::for_each(cell_sites, extend);
        std::ranges::for_each(surface_defects, [&](const auto& sd) { extend(sd.first); });

        return {site_at_row(min_x, min_row), site_at_row(max_x, max_row)};
    }
    /**
     * Compares two layouts for equality: same lattice, name, SiDBs, and defects.
     *
     * @param other Right-hand side layout.
     * @return `true` iff both layouts are identical.
     */
    [[nodiscard]] bool operator==(const layout& other) const noexcept = default;

  private:
    /**
     * The lattice of the surface.
     */
    lattice surface_lattice{lattice::si_100_2x1()};
    /**
     * Layout name.
     */
    std::string layout_name{};
    /**
     * SiDB sites in raster order.
     */
    std::vector<lattice_site> cell_sites{};
    /**
     * Cell type of the SiDB at the same index in `cell_sites`.
     */
    std::vector<cell_type> cell_types{};
    /**
     * Defects with their sites in raster order.
     */
    std::vector<std::pair<lattice_site, model::defect>> surface_defects{};
    /**
     * Applies `fn` to every element of `range`, passing the index too if `fn` takes it, and stopping early if `fn`
     * returns `false`.
     */
    template <typename Range, typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): each call reuses the callback as an lvalue.
    static void for_each_site(const Range& range, Fn&& fn)
    {
        std::size_t i = 0;

        for (const auto& e : range)
        {
            using element = std::remove_cvref_t<decltype(e)>;

            if constexpr (std::is_invocable_v<Fn, const element&, std::size_t>)
            {
                if constexpr (std::is_same_v<std::invoke_result_t<Fn, const element&, std::size_t>, bool>)
                {
                    if (!fn(e, i))
                    {
                        return;
                    }
                }
                else
                {
                    fn(e, i);
                }
            }
            else
            {
                if constexpr (std::is_same_v<std::invoke_result_t<Fn, const element&>, bool>)
                {
                    if (!fn(e))
                    {
                        return;
                    }
                }
                else
                {
                    fn(e);
                }
            }

            ++i;
        }
    }
    /**
     * Counts the defects satisfying a predicate.
     */
    template <typename Pred>
    [[nodiscard]] uint64_t count_defects(Pred&& pred) const noexcept
    {
        return static_cast<uint64_t>(std::ranges::count_if(surface_defects, std::forward<Pred>(pred),
                                                           &std::pair<lattice_site, model::defect>::second));
    }
};

}  // namespace fiction::sidb

namespace std
{
/**
 * Hash for SiDB layouts, over the SiDBs and their cell types.
 */
template <>
struct hash<fiction::sidb::layout>
{
    /**
     * @param lyt Layout to hash.
     * @return Hash of the SiDB sites and cell types.
     */
    std::size_t operator()(const fiction::sidb::layout& lyt) const noexcept
    {
        std::size_t h = lyt.num_cells();

        lyt.foreach_cell(
            [&h, &lyt](const auto& s)
            {
                h ^= std::hash<fiction::sidb::lattice_site>{}(s) + 0x9e3779b97f4a7c15ULL + (h << 6u) + (h >> 2u);
                h ^= static_cast<std::size_t>(lyt.get_cell_type(s));
            });

        return h;
    }
};
}  // namespace std
