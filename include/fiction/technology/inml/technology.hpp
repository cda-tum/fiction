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

namespace fiction::inml
{

/**
 * in-plane Nanomagnet Logic (iNML) technology implementation of the FCN concept.
 */
struct inml_technology
{
    /**
     * Possible types of iNML cells.
     */
    enum class cell_type : uint8_t
    {
        /**
         * Symbol used for empty iNML cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal iNML cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input iNML cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output iNML cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for upper slanted edge magnets.
         */
        SLANTED_EDGE_UP_MAGNET = 'u',
        /**
         * Symbol used for lower slanted edge magnets.
         */
        SLANTED_EDGE_DOWN_MAGNET = 'd',
        /**
         * Symbol used for inverter magnets.
         */
        INVERTER_MAGNET = 'n',
        /**
         * Symbol used for cross-wire magnets.
         */
        CROSSWIRE_MAGNET = 'c',
        /**
         * Symbol used for coupler (fan-out) magnets.
         */
        FANOUT_COUPLER_MAGNET = 'f'
    };

    /**
     * iNML cells do not have modes.
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
     * Checks whether the given cell type is an up-slanted edge magnet.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::SLANTED_EDGE_UP_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_slanted_edge_up_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::SLANTED_EDGE_UP_MAGNET;
    }

    /**
     * Checks whether the given cell type is a down-slanted edge magnet.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::SLANTED_EDGE_DOWN_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_slanted_edge_down_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::SLANTED_EDGE_DOWN_MAGNET;
    }

    /**
     * Checks whether the given cell type is a slanted edge magnet, i.e., either up- or down-slanted.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::SLANTED_EDGE_UP_MAGNET` or `cell_type::SLANTED_EDGE_DOWN_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_slanted_edge_magnet(const cell_type& c) noexcept
    {
        return is_slanted_edge_up_magnet(c) || is_slanted_edge_down_magnet(c);
    }

    /**
     * Checks whether the given cell type is an inverter magnet.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::INVERTER_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_inverter_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::INVERTER_MAGNET;
    }

    /**
     * Checks whether the given cell type is a crosswire magnet.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::CROSSWIRE_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_crosswire_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::CROSSWIRE_MAGNET;
    }

    /**
     * Checks whether the given cell type is a fanout coupler magnet.
     *
     * @param c Cell type to check.
     * @return `true` iff `c` is `cell_type::FANOUT_COUPLER_MAGNET`.
     */
    [[nodiscard]] static constexpr bool is_fanout_coupler_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::FANOUT_COUPLER_MAGNET;
    }

    /**
     * Checks whether the given cell mode is the normal mode. iNML cells do not have modes, so this always holds.
     *
     * @return `true`.
     */
    [[nodiscard]] static constexpr bool is_normal_cell_mode([[maybe_unused]] const cell_mode& m) noexcept
    {
        return true;
    }

    /**
     * Default width of a iNML cell in NMLSim.
     */
    static constexpr uint64_t CELL_WIDTH = 50ul;
    /**
     * Default height of a iNML cell in NMLSim.
     */
    static constexpr uint64_t CELL_HEIGHT = 100ul;
    /**
     * Default horizontal spacing between two iNML cells in NMLSim.
     */
    static constexpr uint64_t CELL_HSPACE = 10ul;
    /**
     * Default vertical spacing between two iNML cells in NMLSim.
     */
    static constexpr uint64_t CELL_VSPACE = 25ul;

    // Deleted constructors to prevent instantiation
    inml_technology() = delete;
};

}  // namespace fiction::inml
