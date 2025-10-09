//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_CELL_TECHNOLOGIES_HPP
#define FICTION_CELL_TECHNOLOGIES_HPP

#include <cstdint>

namespace fiction
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

    [[nodiscard]] static constexpr bool is_empty_cell(cell_type c) noexcept
    {
        return c == cell_type::EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(cell_type c) noexcept
    {
        return c == cell_type::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(cell_type c) noexcept
    {
        return c == cell_type::INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(cell_type c) noexcept
    {
        return c == cell_type::OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_const_0_cell(cell_type c) noexcept
    {
        return c == cell_type::CONST_0;
    }

    [[nodiscard]] static constexpr bool is_const_1_cell(cell_type c) noexcept
    {
        return c == cell_type::CONST_1;
    }

    [[nodiscard]] static constexpr bool is_constant_cell(cell_type c) noexcept
    {
        return is_const_0_cell(c) || is_const_1_cell(c);
    }

    [[nodiscard]] static constexpr bool is_normal_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_rotated_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::ROTATED;
    }

    [[nodiscard]] static constexpr bool is_vertical_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::VERTICAL;
    }

    [[nodiscard]] static constexpr bool is_crossover_cell_mode(cell_mode m) noexcept
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

struct mol_qca_technology
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
         * Symbol used for normal QCA cells.
         */
        NORMAL1 = 'a',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL2 = 'b',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL3 = 'c',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL4 = 'd',
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

    [[nodiscard]] static constexpr bool is_empty_cell(cell_type c) noexcept
    {
        return c == cell_type::EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(cell_type c) noexcept
    {
        return c == cell_type::NORMAL || c == cell_type::NORMAL1 || c == cell_type::NORMAL2 ||
               c == cell_type::NORMAL3 || c == cell_type::NORMAL4;
    }

    [[nodiscard]] static constexpr bool is_normal_cell1(cell_type c) noexcept
    {
        return c == cell_type::NORMAL1;
    }

    [[nodiscard]] static constexpr bool is_normal_cell2(cell_type c) noexcept
    {
        return c == cell_type::NORMAL2;
    }

    [[nodiscard]] static constexpr bool is_normal_cell3(cell_type c) noexcept
    {
        return c == cell_type::NORMAL3;
    }

    [[nodiscard]] static constexpr bool is_normal_cell4(cell_type c) noexcept
    {
        return c == cell_type::NORMAL4;
    }

    [[nodiscard]] static constexpr bool is_input_cell(cell_type c) noexcept
    {
        return c == cell_type::INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(cell_type c) noexcept
    {
        return c == cell_type::OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_const_0_cell(cell_type c) noexcept
    {
        return c == cell_type::CONST_0;
    }

    [[nodiscard]] static constexpr bool is_const_1_cell(cell_type c) noexcept
    {
        return c == cell_type::CONST_1;
    }

    [[nodiscard]] static constexpr bool is_constant_cell(cell_type c) noexcept
    {
        return is_const_0_cell(c) || is_const_1_cell(c);
    }

    [[nodiscard]] static constexpr bool is_normal_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_rotated_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::ROTATED;
    }

    [[nodiscard]] static constexpr bool is_vertical_cell_mode(cell_mode m) noexcept
    {
        return m == cell_mode::VERTICAL;
    }

    [[nodiscard]] static constexpr bool is_crossover_cell_mode(cell_mode m) noexcept
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
    mol_qca_technology() = delete;
};

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

    [[nodiscard]] static constexpr bool is_empty_cell(cell_type c) noexcept
    {
        return c == cell_type::EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(cell_type c) noexcept
    {
        return c == cell_type::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(cell_type c) noexcept
    {
        return c == cell_type::INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(cell_type c) noexcept
    {
        return c == cell_type::OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_up_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::SLANTED_EDGE_UP_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_down_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::SLANTED_EDGE_DOWN_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_magnet(const cell_type& c) noexcept
    {
        return is_slanted_edge_up_magnet(c) || is_slanted_edge_down_magnet(c);
    }

    [[nodiscard]] static constexpr bool is_inverter_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::INVERTER_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_crosswire_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::CROSSWIRE_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_fanout_coupler_magnet(const cell_type& c) noexcept
    {
        return c == cell_type::FANOUT_COUPLER_MAGNET;
    }

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

/**
 * Silicon Dangling Bond (SiDB) technology implementation of the FCN concept.
 */
struct sidb_technology
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

    [[nodiscard]] static constexpr bool is_empty_cell(const cell_type& c) noexcept
    {
        return c == cell_type::EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return c == cell_type::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(const cell_type& c) noexcept
    {
        return c == cell_type::INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(const cell_type& c) noexcept
    {
        return c == cell_type::OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_logic_cell(const cell_type& c) noexcept
    {
        return c == cell_type::LOGIC;
    }

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
    sidb_technology() = delete;
};

}  // namespace fiction

#endif  // FICTION_CELL_TECHNOLOGIES_HPP
