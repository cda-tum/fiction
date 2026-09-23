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
 * @brief Cartesian grid of typed and named cells, the storage the QCA, molQCA, and iNML layouts share.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/coordinates.hpp"

#include <phmap.h>

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace fiction::layouts
{

/**
 * A Cartesian grid of cells, each holding a value of `CellType`, plus the names of input and output cells and a layout
 * name. It is the storage and geometry that the QCA, molQCA, and iNML layouts share, not a layout of its own: each
 * technology derives its layout from it and adds what that technology needs.
 *
 * `CellType` is an enumeration with the enumerators `EMPTY`, `INPUT`, and `OUTPUT`. Assigning `EMPTY` removes the cell,
 * so the grid stores only occupied positions. Primary inputs and outputs are the cells of type `INPUT` and `OUTPUT`.
 *
 * The grid has value semantics; copies are independent.
 *
 * @tparam CellType Enumeration of the cell types of one technology.
 */
template <typename CellType>
class cell_grid : public cartesian_layout<coords::offset>
{
    static_assert(std::is_enum_v<CellType>, "CellType must be an enumeration");

  public:
    /**
     * Cell position.
     */
    using cell = coords::offset;
    /**
     * Cell type.
     */
    using cell_type = CellType;
    /**
     * Creates an empty grid.
     *
     * @param ar Highest cell position in the grid.
     * @param name Layout name.
     */
    explicit cell_grid(const aspect_ratio& ar = {}, std::string name = "") :
            cartesian_layout<coords::offset>(ar),
            layout_name{std::move(name)}
    {}
    /**
     * Copies a grid, including its dimensions.
     *
     * @param other Grid to copy.
     */
    cell_grid(const cell_grid& other) :
            cartesian_layout<coords::offset>(other.clone()),
            layout_name{other.layout_name},
            cell_types{other.cell_types},
            cell_names{other.cell_names}
    {}
    /**
     * Moves a grid.
     *
     * @param other Grid to move.
     */
    cell_grid(cell_grid&& other) noexcept = default;
    /**
     * Copies a grid, including its dimensions.
     *
     * @param other Grid to copy.
     * @return This grid.
     */
    cell_grid& operator=(const cell_grid& other)
    {
        if (this != &other)
        {
            *this = cell_grid{other};
        }

        return *this;
    }
    /**
     * Moves a grid.
     *
     * @param other Grid to move.
     * @return This grid.
     */
    cell_grid& operator=(cell_grid&& other) noexcept = default;
    /**
     * Destroys the grid.
     */
    ~cell_grid() = default;
    /**
     * Compares two grids: same dimensions, name, cell types, and cell names.
     *
     * @param other Right-hand side grid.
     * @return `true` iff both grids are identical.
     */
    [[nodiscard]] bool operator==(const cell_grid& other) const noexcept
    {
        return x() == other.x() && y() == other.y() && z() == other.z() && layout_name == other.layout_name &&
               cell_types == other.cell_types && cell_names == other.cell_names;
    }
    /**
     * Assigns a cell type to a position. Assigning `EMPTY` removes the cell and its name.
     *
     * @param c Cell position.
     * @param ct Cell type.
     */
    void assign_cell_type(const cell& c, const cell_type ct)
    {
        if (ct == cell_type::EMPTY)
        {
            cell_types.erase(c);
            cell_names.erase(c);
        }
        else
        {
            cell_types[c] = ct;
        }
    }
    /**
     * The cell type at a position.
     *
     * @param c Cell position.
     * @return Cell type at `c`, `EMPTY` if no cell is there.
     */
    [[nodiscard]] cell_type get_cell_type(const cell& c) const noexcept
    {
        if (const auto it = cell_types.find(c); it != cell_types.cend())
        {
            return it->second;
        }

        return cell_type::EMPTY;
    }
    /**
     * Whether no cell sits at a position.
     *
     * @param c Cell position.
     * @return `true` iff `c` holds no cell.
     */
    [[nodiscard]] bool is_empty_cell(const cell& c) const noexcept
    {
        return !cell_types.contains(c);
    }
    /**
     * Assigns a name to a cell. The empty string removes the name.
     *
     * @param c Cell position.
     * @param n Cell name.
     */
    void assign_cell_name(const cell& c, const std::string& n)
    {
        if (n.empty())
        {
            cell_names.erase(c);
        }
        else
        {
            cell_names[c] = n;
        }
    }
    /**
     * The name of a cell.
     *
     * @param c Cell position.
     * @return Name of the cell at `c`, or the empty string if it has none.
     */
    [[nodiscard]] std::string get_cell_name(const cell& c) const
    {
        if (const auto it = cell_names.find(c); it != cell_names.cend())
        {
            return it->second;
        }

        return {};
    }
    /**
     * Sets the layout name.
     *
     * @param name Layout name.
     */
    void set_layout_name(const std::string& name)
    {
        layout_name = name;
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
     * Number of cells.
     *
     * @return Number of cells.
     */
    [[nodiscard]] uint64_t num_cells() const noexcept
    {
        return static_cast<uint64_t>(cell_types.size());
    }
    /**
     * Whether the grid holds no cell.
     *
     * @return `true` iff there is no cell.
     */
    [[nodiscard]] bool is_empty() const noexcept
    {
        return cell_types.empty();
    }
    /**
     * Whether a cell is a primary input, i.e., of type `INPUT`.
     *
     * @param c Cell position.
     * @return `true` iff `c` holds an input cell.
     */
    [[nodiscard]] bool is_pi(const cell& c) const noexcept
    {
        return get_cell_type(c) == cell_type::INPUT;
    }
    /**
     * Whether a cell is a primary output, i.e., of type `OUTPUT`.
     *
     * @param c Cell position.
     * @return `true` iff `c` holds an output cell.
     */
    [[nodiscard]] bool is_po(const cell& c) const noexcept
    {
        return get_cell_type(c) == cell_type::OUTPUT;
    }
    /**
     * Number of primary input cells.
     *
     * @return Number of input cells.
     */
    [[nodiscard]] uint32_t num_pis() const noexcept
    {
        return count(cell_type::INPUT);
    }
    /**
     * Number of primary output cells.
     *
     * @return Number of output cells.
     */
    [[nodiscard]] uint32_t num_pos() const noexcept
    {
        return count(cell_type::OUTPUT);
    }
    /**
     * Applies a function to every cell position that holds a cell, in unspecified order. `fn` takes the position and
     * may return `false` to stop the traversal. The grid must not be modified during the traversal.
     *
     * @tparam Fn Callable on `(const cell&)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_cell(Fn&& fn) const
    {
        for_each_of_type(std::forward<Fn>(fn), [](const cell_type) { return true; });
    }
    /**
     * Applies a function to every position of the grid, occupied or not. Same callable contract as
     * `foreach_coordinate`.
     *
     * @tparam Fn Callable on `(const cell&)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_cell_position(Fn&& fn) const
    {
        foreach_coordinate(std::forward<Fn>(fn));
    }
    /**
     * Applies a function to every primary input cell. Same callable contract as `foreach_cell`.
     *
     * @tparam Fn Callable on `(const cell&)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        for_each_of_type(std::forward<Fn>(fn), [](const cell_type ct) { return ct == cell_type::INPUT; });
    }
    /**
     * Applies a function to every primary output cell. Same callable contract as `foreach_cell`.
     *
     * @tparam Fn Callable on `(const cell&)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        for_each_of_type(std::forward<Fn>(fn), [](const cell_type ct) { return ct == cell_type::OUTPUT; });
    }

  private:
    /**
     * Layout name.
     */
    std::string layout_name;
    /**
     * Types of the occupied cells.
     */
    phmap::flat_hash_map<cell, cell_type> cell_types{};
    /**
     * Names of the named cells.
     */
    phmap::flat_hash_map<cell, std::string> cell_names{};
    /**
     * Counts the cells of a type.
     */
    [[nodiscard]] uint32_t count(const cell_type ct) const noexcept
    {
        uint32_t n{0};

        for (const auto& [c, t] : cell_types)
        {
            n += static_cast<uint32_t>(t == ct);
        }

        return n;
    }
    /**
     * Applies `fn` to every cell whose type satisfies `pred`, stopping early if `fn` returns `false`.
     */
    template <typename Fn, typename Pred>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): each call reuses the callback as an lvalue.
    void for_each_of_type(Fn&& fn, Pred pred) const
    {
        for (const auto& [c, t] : cell_types)
        {
            if (!pred(t))
            {
                continue;
            }

            if constexpr (std::is_same_v<std::invoke_result_t<Fn, const cell&>, bool>)
            {
                if (!fn(c))
                {
                    return;
                }
            }
            else
            {
                fn(c);
            }
        }
    }
};

}  // namespace fiction::layouts
