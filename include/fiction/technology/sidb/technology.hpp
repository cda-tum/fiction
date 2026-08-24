//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_TECHNOLOGY_SIDB_TECHNOLOGY_HPP
#define FICTION_TECHNOLOGY_SIDB_TECHNOLOGY_HPP

#include <cstdint>

namespace fiction::sidb
{

/**
 * Silicon Dangling Bond (SiDB) technology implementation of the FCN concept.
 */
struct technology
{
    /**
     * Possible types of SiDB cells.
     */
    enum class cell_type : uint8_t
    {
        /**
         * Symbol used for empty SiDB cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal SiDB cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input SiDB cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output SiDB cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for logic SiDB cells (e.g. canvas SiDBs).
         */
        LOGIC = 'l'
    };

    /**
     * SiDB cells do not have modes.
     */
    struct cell_mode
    {};
    /**
     * Possible marks to be applied to a cell to change its type.
     */
    enum class cell_mark : uint8_t
    {
        EMPTY  = static_cast<uint8_t>(cell_type::EMPTY),
        INPUT  = static_cast<uint8_t>(cell_type::INPUT),
        OUTPUT = static_cast<uint8_t>(cell_type::OUTPUT),
        LOGIC  = static_cast<uint8_t>(cell_type::LOGIC)
    };

    /**
     * Checks whether the given cell type is empty.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::EMPTY`.
     */
    [[nodiscard]] static constexpr bool is_empty_cell(const cell_type& c) noexcept
    {
        return c == cell_type::EMPTY;
    }

    /**
     * Checks whether the given cell type is a normal cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL;
    }

    /**
     * Checks whether the given cell type is an input cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::INPUT`.
     */
    [[nodiscard]] static constexpr bool is_input_cell(const cell_type& c) noexcept
    {
        return c == cell_type::INPUT;
    }

    /**
     * Checks whether the given cell type is an output cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::OUTPUT`.
     */
    [[nodiscard]] static constexpr bool is_output_cell(const cell_type& c) noexcept
    {
        return c == cell_type::OUTPUT;
    }

    /**
     * Checks whether the given cell type is a logic cell (e.g., a canvas SiDB).
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::LOGIC`.
     */
    [[nodiscard]] static constexpr bool is_logic_cell(const cell_type& c) noexcept
    {
        return c == cell_type::LOGIC;
    }

    /**
     * Checks whether the given cell mode is the normal mode. SiDB cells do not have modes, so this always holds.
     *
     * @return `true`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell_mode([[maybe_unused]] const cell_mode& m) noexcept
    {
        return true;
    }

    /**
     * Default width of a SiDB in SiQAD (https://github.com/siqad/siqad).
     * Dots are considered to be 0-dimensional entities for simulation purposes.
     */
    static constexpr double CELL_WIDTH = 0.0;
    /**
     * Default height of a SiDB in SiQAD.
     * Dots are considered to be 0-dimensional entities for simulation purposes.
     */
    static constexpr double CELL_HEIGHT = 0.0;
    /**
     * Default horizontal spacing between two SiDBs in SiQAD.
     */
    static constexpr double CELL_HSPACE = 0.384;
    /**
     * Default average vertical spacing between two SiDBs in SiQAD.
     * Depending on whether they are on the same or different dimer rows, SiDBs can be closer together or further apart.
     */
    static constexpr double CELL_VSPACE = 0.384;

    // Deleted constructors to prevent instantiation
    technology() = delete;
};

}  // namespace fiction::sidb

#endif  // FICTION_TECHNOLOGY_SIDB_TECHNOLOGY_HPP
