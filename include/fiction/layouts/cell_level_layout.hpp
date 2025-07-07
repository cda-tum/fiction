//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_CELL_LEVEL_LAYOUT_HPP
#define FICTION_CELL_LEVEL_LAYOUT_HPP

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/networks/detail/foreach.hpp>
#include <phmap.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * A layout type to layer on top of a clocked layout that allows the assignment of individual cells to clock zones in
 * accordance with an FCN technology, e.g., QCA, iNML, or SiDB. This type, thereby, represents layouts on a
 * cell-accurate abstraction without a notion of logic functions. Gate libraries can be used to transform gate-level
 * layouts into cell-level ones. Furthermore, cell-level layouts can be written to files for various physical simulators
 * like QCADesigner, ToPoliNano & MagCAD, SiQAD, etc.
 *
 * In this layout, each coordinate, i.e., clock zone has the dimensions of a single cell. Clock numbers can, however, be
 * assigned in a way, that they form larger zones, e.g., of \f$5 \times 5\f$ cells. These dimensions can be specified
 * in the constructor. They affect the way, clock numbers are fetched from the underlying clocked layout.
 *
 * The de-facto standard of cell-level FCN design is to group multiple cells into tiles large enough to be addressable
 * by individual clocking electrodes buried in the layout substrate. Cell-based clocking, i.e., clock zones of size
 * \f$1 \times 1\f$ cells are not recommended as they are most likely not fabricable in reality.
 *
 * On the implementation side, this layout distinguishes between `cell`, `cell_type`, and `cell_mode`. A `cell` is a
 * coordinate, i.e., a position on the layout where a `cell_type` can be assigned. A `cell_type` is a concrete variation
 * of a fabricated cell and depends on the given technology. QCA offers regular and constant cell types while SiDB only
 * provides regular ones. Cell types can also include primary input and output cells if they are being treated
 * differently in a simulator for instance. A `cell_mode`, on the other hand, is a variation of a cell (thus far only
 * known from QCADesigner) that provides further attributes like its functionality as a crossing or via cell.
 *
 * @tparam Technology An FCN technology that provides notions of cell types.
 * @tparam ClockedLayout The clocked layout that is to be extended by cell positions.
 */
template <typename Technology, typename ClockedLayout>
class cell_level_layout : public ClockedLayout
{
  public:
#pragma region Types and constructors

    using cell      = typename ClockedLayout::coordinate;
    using cell_type = typename Technology::cell_type;
    using cell_mode = typename Technology::cell_mode;

    template <typename Cell>
    struct cell_level_layout_storage
    {
        explicit cell_level_layout_storage(const std::string_view& name, uint16_t tile_x = 1u, uint16_t tile_y = 1u) :
                layout_name{name},
                tile_size_x{tile_x},
                tile_size_y{tile_y}
        {}

        std::string layout_name;

        uint16_t tile_size_x;
        uint16_t tile_size_y;

        phmap::parallel_flat_hash_map<Cell, cell_type> cell_type_map{};
        phmap::parallel_flat_hash_map<Cell, cell_mode> cell_mode_map{};

        phmap::flat_hash_map<Cell, std::string> cell_name_map{};

        phmap::flat_hash_set<Cell> inputs{}, outputs{};
    };

    using base_type = cell_level_layout;

    using technology = Technology;

    using storage = std::shared_ptr<cell_level_layout_storage<cell>>;

    /**
     * Standard constructor. Creates a named cell-level layout of the given aspect ratio. To this end, it calls
     * `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param name Layout name.
     * @param tile_size_x Clock zone size in x-dimension in cells.
     * @param tile_size_y Clock zone size in y-dimension in cells.
     */
    explicit cell_level_layout(const aspect_ratio_t<ClockedLayout>& ar = {}, const std::string& name = "",
                               const uint16_t tile_size_x = 1u, const uint16_t tile_size_y = 1u) :
            ClockedLayout(ar),
            strg{std::make_shared<cell_level_layout_storage<cell>>(name, tile_size_x, tile_size_y)}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Standard constructor. Creates a named cell-level layout of the given aspect ratio and clocks it via the given
     * clocking scheme. To this end, it calls `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     * @param name Layout name.
     * @param tile_size_x Clock zone size in x-dimension in cells.
     * @param tile_size_y Clock zone size in y-dimension in cells.
     */
    cell_level_layout(const aspect_ratio_t<ClockedLayout>& ar, const clocking_scheme<cell>& scheme,
                      const std::string& name = "", const uint16_t tile_size_x = 1u, const uint16_t tile_size_y = 1u) :
            ClockedLayout(ar, scheme),
            strg{std::make_shared<cell_level_layout_storage<cell>>(name, tile_size_x, tile_size_y)}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another cell_level_layout.
     */
    explicit cell_level_layout(std::shared_ptr<cell_level_layout_storage<cell>> s) : strg{std::move(s)} {}
    /**
     * Copy constructor from another `ClockedLayout`.
     *
     * @param lyt Clocked layout.
     */
    explicit cell_level_layout(const ClockedLayout& lyt) :
            ClockedLayout(lyt),
            strg{std::make_shared<cell_level_layout_storage<cell>>("", 1, 1)}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] cell_level_layout clone() const noexcept
    {
        cell_level_layout copy{ClockedLayout::clone()};
        copy.strg = std::make_shared<cell_level_layout_storage<cell>>(*strg);

        return copy;
    }

#pragma endregion

#pragma region Cell types

    /**
     * Assigns a cell type `ct` to a cell position `c` in the layout. If `ct` is the empty cell, a potentially stored
     * cell type is being erased. If `ct` is a primary input or output type, the number of primary inputs or outputs in
     * the layout is increased respectively.
     *
     * @param c Cell position.
     * @param ct Cell type to assign to `c`.
     */
    void assign_cell_type(const cell& c, const cell_type& ct) noexcept
    {
        strg->inputs.erase(c);
        strg->outputs.erase(c);

        if (Technology::is_empty_cell(ct))
        {
            strg->cell_type_map.erase(c);
            strg->cell_mode_map.erase(c);

            return;
        }

        strg->cell_type_map[c] = ct;

        if (Technology::is_input_cell(ct))
        {
            strg->inputs.insert(c);
        }
        else if (Technology::is_output_cell(ct))
        {
            strg->outputs.insert(c);
        }
    }
    /**
     * Returns the cell type assigned to cell position `c`.
     *
     * @param c Cell position whose assigned cell type is desired.
     * @return Cell type assigned to cell position `c`.
     */
    [[nodiscard]] cell_type get_cell_type(const cell& c) const noexcept
    {
        if (const auto it = strg->cell_type_map.find(c); it != strg->cell_type_map.cend())
        {
            return it->second;
        }

        return Technology::cell_type::EMPTY;
    }

    /**
     * Returns all cells of the given type.
     *
     * @param type Type of cells to return.
     * @return All cells of the layout that have the given type.
     */
    [[nodiscard]] std::vector<cell> get_cells_by_type(const typename Technology::cell_type type) const noexcept
    {
        std::vector<cell> cells;
        cells.reserve(num_cells());

        foreach_cell(
            [&cells, &type, this](const auto& c)
            {
                const auto c_type = get_cell_type(c);
                if (c_type == type)
                {
                    cells.push_back(c);
                }
            });

        return cells;
    }
    /**
     * Returns the numbers of cells of the given type.
     *
     * @param type Type of cells which are counted.
     * @return Number of the cells with the given type.
     */
    [[nodiscard]] uint64_t num_cells_of_given_type(const typename Technology::cell_type type) const noexcept
    {
        return get_cells_by_type(type).size();
    }
    /**
     * Returns `true` if no cell type is assigned to cell position `c` or if the empty type was assigned.
     *
     * @param c Cell position to check for emptiness.
     * @return `true` iff no cell type was assigned to cell position `c`.
     */
    [[nodiscard]] bool is_empty_cell(const cell& c) const noexcept
    {
        return Technology::is_empty_cell(get_cell_type(c));
    }
    /**
     * Assigns a cell mode `m` to a cell position `c` in the layout. If `m` is the normal cell mode, a potentially
     * stored cell mode is being erased.
     *
     * @param c Cell position to assign cell mode `m` to.
     * @param m Cell mode to assign to cell position `c`.
     */
    void assign_cell_mode(const cell& c, const cell_mode& m) noexcept
    {
        if (Technology::is_normal_cell_mode(m))
        {
            strg->cell_mode_map.erase(c);
        }
        else
        {
            strg->cell_mode_map[c] = m;
        }
    }
    /**
     * Returns the cell mode assigned to cell position `c`. If no cell mode is assigned, the default mode is returned.
     *
     * @param c Cell position whose assigned cell mode is desired.
     * @return Cell mode assigned to cell position `c`.
     */
    [[nodiscard]] cell_mode get_cell_mode(const cell& c) const noexcept
    {
        if (auto it = strg->cell_mode_map.find(c); it != strg->cell_mode_map.cend())
        {
            return it->second;
        }

        return {};
    }
    /**
     * Assigns a cell name `n` to a cell position `c` in the layout. If `n` is the empty string, a potentially stored
     * cell name is being erased.
     *
     * @param c Cell position to assign cell name `n` to.
     * @param n Cell name to assign to cell position `c`.
     */
    void assign_cell_name(const cell& c, const std::string& n) noexcept
    {
        if (n.empty())
        {
            strg->cell_name_map.erase(c);
        }
        else
        {
            strg->cell_name_map[c] = n;
        }
    }
    /**
     * Returns the cell name assigned to cell position `c`. If no cell name is assigned, the empty string is returned.
     *
     * @param c Cell position whose assigned cell name is desired.
     * @return Cell name assigned to cell position `c`.
     */
    [[nodiscard]] std::string get_cell_name(const cell& c) const noexcept
    {
        if (const auto it = strg->cell_name_map.find(c); it != strg->cell_name_map.cend())
        {
            return it->second;
        }

        return {};
    }

#pragma endregion

#pragma region Properties

    /**
     * Assigns or overrides the layout name.
     *
     * @param name Layout name to assign.
     */
    void set_layout_name(const std::string& name) noexcept
    {
        strg->layout_name = name;
    }
    /**
     * Returns the assigned layout name.
     *
     * @return The layout name.
     */
    [[nodiscard]] std::string get_layout_name() const noexcept
    {
        return strg->layout_name.data();
    }
    /**
     * Returns the number of non-empty cell types that were assigned to the layout.
     *
     * @return Number of non-empty cell types in the layout.
     */
    [[nodiscard]] uint64_t num_cells() const noexcept
    {
        return static_cast<uint64_t>(strg->cell_type_map.size());
    }
    /**
     * Checks whether there are no cells assigned to the layout's coordinates.
     *
     * @return `true` iff the layout is empty.
     */
    [[nodiscard]] bool is_empty() const noexcept
    {
        return strg->cell_type_map.empty();
    }
    /**
     * Returns the number of primary input cells in the layout.
     *
     * @return Number of primary input cells.
     */
    [[nodiscard]] uint32_t num_pis() const noexcept
    {
        return static_cast<uint32_t>(strg->inputs.size());
    }
    /**
     * Returns the number of primary output cells in the layout.
     *
     * @return Number of primary output cells.
     */
    [[nodiscard]] uint32_t num_pos() const noexcept
    {
        return static_cast<uint32_t>(strg->outputs.size());
    }
    /**
     * Checks whether a given cell position is marked as primary input. This function does not check against the
     * assigned cell type but whether the cell position is stored in the list of PIs. Assigning a cell position the
     * primary input cell type automatically enlists it there.
     *
     * @param c Cell position to check.
     * @return `true` iff cell position `c` is marked as primary input.
     */
    [[nodiscard]] bool is_pi(const cell& c) const noexcept
    {
        return std::find(strg->inputs.cbegin(), strg->inputs.cend(), c) != strg->inputs.cend();
    }
    /**
     * Checks whether a given cell position is marked as primary output. This function does not check against the
     * assigned cell type but whether the cell position is stored in the list of POs. Assigning a cell position the
     * primary output cell type automatically enlists it there.
     *
     * @param c Cell position to check.
     * @return `true` iff cell position `c` is marked as primary output.
     */
    [[nodiscard]] bool is_po(const cell& c) const noexcept
    {
        return std::find(strg->outputs.cbegin(), strg->outputs.cend(), c) != strg->outputs.cend();
    }
    /**
     * Returns the underlying clock zone x-dimension size. That is, if this cell-level layout was obtained from the
     * application of a gate library, this function returns the cell size in x-dimension of each gate in the library.
     *
     * @return The clock zone size in cells in the x-dimension.
     */
    [[nodiscard]] auto get_tile_size_x() const noexcept
    {
        return strg->tile_size_x;
    }
    /**
     * Sets the underlying clock zone x-dimension size.
     *
     * @param tile_size_x Tile size in the x-dimension in number of cells.
     */
    void set_tile_size_x(const uint16_t tile_size_x) noexcept
    {
        strg->tile_size_x = tile_size_x;
    }
    /**
     * Returns the underlying clock zone y-dimension size. That is, if this cell-level layout was obtained from the
     * application of a gate library, this function returns the cell size in y-dimension of each gate in the library.
     *
     * @return The clock zone size in cells in the y-dimension.
     */
    [[nodiscard]] auto get_tile_size_y() const noexcept
    {
        return strg->tile_size_y;
    }
    /**
     * Sets the underlying clock zone y-dimension size.
     *
     * @param tile_size_y Tile size in the y-dimension in number of cells.
     */
    void set_tile_size_y(const uint16_t tile_size_y) noexcept
    {
        strg->tile_size_y = tile_size_y;
    }

#pragma endregion

#pragma region Clocking

    /**
     * Returns the clock number of cell position `c` by accessing `ClockedLayout`'s underlying clocking scheme and
     * respecting this layout's clock zone size.
     *
     * @param c Cell position whose clock number is desired.
     * @return Clock number of cell position `c`.
     */
    [[nodiscard]] typename ClockedLayout::clock_number_t get_clock_number(const cell& c) const noexcept
    {
        return ClockedLayout::get_clock_number({c.x / strg->tile_size_x, c.y / strg->tile_size_y, c.z});
    }
    /**
     * Function is deleted for cell-level layouts.
     */
    [[maybe_unused]] bool is_incoming_clocked(const typename ClockedLayout::clock_zone& cz1,
                                              const typename ClockedLayout::clock_zone& cz2) const noexcept = delete;
    /**
     * Function is deleted for cell-level layouts.
     */
    [[maybe_unused]] bool is_outgoing_clocked(const typename ClockedLayout::clock_zone& cz1,
                                              const typename ClockedLayout::clock_zone& cz2) const noexcept = delete;

#pragma endregion

#pragma region Iteration

    /**
     * Applies a function to all cell positions in the layout that have non-empty cell types assigned.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @param fn Functor to apply to each non-empty cell position.
     */
    template <typename Fn>
    void foreach_cell(Fn&& fn) const
    {
        using iterator_type = decltype(strg->cell_type_map.cbegin());
        mockturtle::detail::foreach_element_transform<iterator_type, cell>(
            strg->cell_type_map.cbegin(), strg->cell_type_map.cend(),
            [](const auto& ct) { return static_cast<cell>(ct.first); }, std::forward<Fn>(fn));
    }
    /**
     * Applies a function to all cell positions in the layout, even empty ones. This function, thereby, renames
     * `ClockedLayout::foreach_coordinate`.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by the functor type in
     * `ClockedLayout::foreach_coordinate`.
     * @param fn Functor to apply to each cell position.
     */
    template <typename Fn>
    void foreach_cell_position(Fn&& fn) const
    {
        ClockedLayout::foreach_coordinate(std::forward<Fn>(fn));
    }
    /**
     * Applies a function to all primary input cell positions in the layout.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @param fn Functor to apply to each primary input cell.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        using iterator_type = decltype(strg->inputs.cbegin());
        mockturtle::detail::foreach_element_transform<iterator_type, cell>(
            strg->inputs.cbegin(), strg->inputs.cend(), [](const auto& i) { return static_cast<cell>(i); },
            std::forward<Fn>(fn));
    }
    /**
     * Applies a function to all primary output cells in the layout.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @param fn Functor to apply to each primary output cell.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        using iterator_type = decltype(strg->outputs.cbegin());
        mockturtle::detail::foreach_element_transform<iterator_type, cell>(
            strg->outputs.cbegin(), strg->outputs.end(), [](const auto& o) { return static_cast<cell>(o); },
            std::forward<Fn>(fn));
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CELL_LEVEL_LAYOUT_HPP
