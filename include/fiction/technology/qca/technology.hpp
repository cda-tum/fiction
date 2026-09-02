/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 23.06.21.
//

#pragma once

#include <cstdint>

namespace fiction::qca
{

/**
 * Quantum-dot Cellular Automata (QCA) technology implementation of the FCN concept.
 */
struct qca_technology
{
    /**
     * Possible types of QCA cells.
     */
    enum class cell_type : uint8_t
    {
        /**
         * Symbol used for empty QCA cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input QCA cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output QCA cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for constant 0 input QCA cells.
         */
        CONST_0 = '0',
        /**
         * Symbol used for constant 1 input QCA cells.
         */
        CONST_1 = '1'
    };
    /**
     * Possible cell modes for QCA cells.
     */
    enum class cell_mode : uint8_t
    {
        NORMAL = 0u,
        ROTATED,
        VERTICAL,
        CROSSOVER
    };
    /**
     * Possible marks to be applied to a cell to change its type.
     */
    enum class cell_mark : uint8_t
    {
        EMPTY  = static_cast<uint8_t>(cell_type::EMPTY),
        INPUT  = static_cast<uint8_t>(cell_type::INPUT),
        OUTPUT = static_cast<uint8_t>(cell_type::OUTPUT)
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
     * Checks whether the given cell type is a constant-0 input cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_0`.
     */
    [[nodiscard]] static constexpr bool is_const_0_cell(const cell_type& c) noexcept
    {
        return c == cell_type::CONST_0;
    }

    /**
     * Checks whether the given cell type is a constant-1 input cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_1`.
     */
    [[nodiscard]] static constexpr bool is_const_1_cell(const cell_type& c) noexcept
    {
        return c == cell_type::CONST_1;
    }

    /**
     * Checks whether the given cell type is a constant input cell, i.e., either `CONST_0` or `CONST_1`.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_0` or `cell_type::CONST_1`.
     */
    [[nodiscard]] static constexpr bool is_constant_cell(const cell_type& c) noexcept
    {
        return is_const_0_cell(c) || is_const_1_cell(c);
    }

    /**
     * Checks whether the given cell mode is the normal mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::NORMAL`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::NORMAL;
    }

    /**
     * Checks whether the given cell mode is the rotated mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::ROTATED`.
     */
    [[nodiscard]] static constexpr bool is_rotated_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::ROTATED;
    }

    /**
     * Checks whether the given cell mode is the vertical mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::VERTICAL`.
     */
    [[nodiscard]] static constexpr bool is_vertical_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::VERTICAL;
    }

    /**
     * Checks whether the given cell mode is the crossover mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::CROSSOVER`.
     */
    [[nodiscard]] static constexpr bool is_crossover_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::CROSSOVER;
    }
    /**
     * Default width of a QCA cell in QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/).
     */
    static constexpr uint64_t CELL_WIDTH = 18ul;
    /**
     * Default height of a QCA cell in QCADesigner.
     */
    static constexpr uint64_t CELL_HEIGHT = 18ul;
    /**
     * Default horizontal spacing between two QCA cells in QCADesigner.
     */
    static constexpr uint64_t CELL_HSPACE = 2ul;
    /**
     * Default vertical spacing between two QCA cells in QCADesigner.
     */
    static constexpr uint64_t CELL_VSPACE = 2ul;

    // Deleted constructors to prevent instantiation
    qca_technology() = delete;
};

/**
 * Molecular Quantum-dot Cellular Automata (molQCA) technology implementation of the FCN concept.
 *
 * MolQCA normal cell symbols encode their SCERPA clock phase directly. The helper predicates below keep phase handling
 * centralized for writers and gate libraries that need to translate cell symbols into simulator-specific metadata.
 */
struct mol_qca_technology
{
    /**
     * Possible types of molQCA cells.
     */
    enum class cell_type : uint8_t
    {
        /**
         * Symbol used for empty molQCA cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal molQCA cells with clocking 0.
         */
        NORMAL1 = 'a',
        /**
         * Symbol used for normal molQCA cells with clocking 1.
         */
        NORMAL2 = 'b',
        /**
         * Symbol used for normal molQCA cells with clocking 2.
         */
        NORMAL3 = 'c',
        /**
         * Symbol used for normal molQCA cells with clocking 3.
         */
        NORMAL4 = 'd',
        /**
         * Symbol used for input molQCA cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output molQCA cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for constant 0 input molQCA cells.
         */
        CONST_0 = '0',
        /**
         * Symbol used for constant 1 input molQCA cells.
         */
        CONST_1 = '1'
    };
    /**
     * Possible cell modes for molQCA cells.
     */
    enum class cell_mode : uint8_t
    {
        NORMAL = 0u,
        ROTATED,
        VERTICAL,  // reserved for a future crossing gate; writer branches are kept so they won't silently break on
                   // addition
        CROSSOVER  // reserved for a future crossing gate; writer branches are kept so they won't silently break on
                   // addition
    };
    /**
     * Possible marks to be applied to a cell to change its type.
     */
    enum class cell_mark : uint8_t
    {
        EMPTY  = static_cast<uint8_t>(cell_type::EMPTY),
        INPUT  = static_cast<uint8_t>(cell_type::INPUT),
        OUTPUT = static_cast<uint8_t>(cell_type::OUTPUT)
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
     * Checks whether the given cell type is a normal cell, i.e., any of the four clocking phases.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL1`, `cell_type::NORMAL2`, `cell_type::NORMAL3`, or
     * `cell_type::NORMAL4`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return is_normal_cell1(c) || is_normal_cell2(c) || is_normal_cell3(c) || is_normal_cell4(c);
    }

    /**
     * Checks whether the given cell type is a normal cell with clocking 0.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL1`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell1(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL1;
    }

    /**
     * Checks whether the given cell type is a normal cell with clocking 1.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL2`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell2(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL2;
    }

    /**
     * Checks whether the given cell type is a normal cell with clocking 2.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL3`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell3(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL3;
    }

    /**
     * Checks whether the given cell type is a normal cell with clocking 3.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::NORMAL4`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell4(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL4;
    }

    /**
     * Returns the SCERPA clock number encoded by a molQCA normal cell type.
     *
     * Non-normal cell types do not encode a clock phase and are mapped to phase 0 for callers that need a deterministic
     * fallback.
     *
     * @param c Cell type to inspect.
     * @return Clock number in the range 0 to 3.
     */
    [[nodiscard]] static constexpr uint64_t cell_clock_number(const cell_type& c) noexcept
    {
        if (is_normal_cell2(c))
        {
            return 1u;
        }
        if (is_normal_cell3(c))
        {
            return 2u;
        }
        if (is_normal_cell4(c))
        {
            return 3u;
        }

        return 0u;
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
     * Checks whether the given cell type is a constant-0 input cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_0`.
     */
    [[nodiscard]] static constexpr bool is_const_0_cell(const cell_type& c) noexcept
    {
        return c == cell_type::CONST_0;
    }

    /**
     * Checks whether the given cell type is a constant-1 input cell.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_1`.
     */
    [[nodiscard]] static constexpr bool is_const_1_cell(const cell_type& c) noexcept
    {
        return c == cell_type::CONST_1;
    }

    /**
     * Checks whether the given cell type is a constant input cell, i.e., either `CONST_0` or `CONST_1`.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CONST_0` or `cell_type::CONST_1`.
     */
    [[nodiscard]] static constexpr bool is_constant_cell(const cell_type& c) noexcept
    {
        return is_const_0_cell(c) || is_const_1_cell(c);
    }

    /**
     * Checks whether the given cell mode is the normal mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::NORMAL`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::NORMAL;
    }

    /**
     * Checks whether the given cell mode is the rotated mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::ROTATED`.
     */
    [[nodiscard]] static constexpr bool is_rotated_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::ROTATED;
    }

    /**
     * Checks whether the given cell mode is the vertical mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::VERTICAL`.
     */
    [[nodiscard]] static constexpr bool is_vertical_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::VERTICAL;
    }

    /**
     * Checks whether the given cell mode is the crossover mode.
     *
     * @param m Cell mode to check.
     * @return `true` iff `m` is `cell_mode::CROSSOVER`.
     */
    [[nodiscard]] static constexpr bool is_crossover_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::CROSSOVER;
    }

    /**
     * Default width of a molQCA cell.
     */
    static constexpr uint64_t CELL_WIDTH = 2ul;
    /**
     * Default height of a molQCA cell.
     */
    static constexpr uint64_t CELL_HEIGHT = 2ul;
    /**
     * Default horizontal spacing between two molQCA cells.
     */
    static constexpr uint64_t CELL_HSPACE = 0ul;
    /**
     * Default vertical spacing between two molQCA cells.
     */
    static constexpr uint64_t CELL_VSPACE = 0ul;

    // Deleted constructors to prevent instantiation
    mol_qca_technology() = delete;
};

}  // namespace fiction::qca
