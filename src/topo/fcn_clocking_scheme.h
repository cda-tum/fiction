//
// Created by marcel on 26.04.18.
//

#ifndef FICTION_FCN_CLOCKING_SCHEME_H
#define FICTION_FCN_CLOCKING_SCHEME_H

#include <utility>
#include <vector>

namespace fcn_clock
{
    /**
     * Alias for clock zones used for layout tiles.
     */
    using zone = unsigned;
    /**
     * Alias for the number of different clocks in a scheme.
     */
    using number = zone;
    /**
     * Alias for a cutout of clock zone tiles.
     */
    using cutout = std::vector<std::vector<zone>>;
    /**
     * Representing the BANCS clocking as defined in "BANCS: Bidirectional Alternating Nanomagnetic Clocking Scheme" by
     * Ruan Evangelista Formigoni, Omar P. Vilela Neto, and Jose Augusto M. Nacif in SBCCI 2018.
     */
//    const cutout bancs_3 =
//    {{
//         {{0, 1, 2}},
//         {{2, 1, 0}},
//         {{2, 0, 1}},
//         {{1, 0, 2}},
//         {{1, 2, 0}},
//         {{0, 2, 1}}
//    }};
    /**
     * Representing the USE clocking as defined in "USE: A Universal, Scalable, and Efficient Clocking Scheme for QCA"
     * by Caio Araujo T. Campos, Abner L. Marciano, Omar P. Vilela Neto, and Frank Sill Torres in TCAD 2015.
     */
    const cutout use_4 =
    {{
         {{0, 1, 2, 3}},
         {{3, 2, 1, 0}},
         {{2, 3, 0, 1}},
         {{1, 0, 3, 2}}
    }};
    /**
     * Representing the RES clocking as defined in "An efficient clocking scheme for quantum-dot cellular automata" by
     * Mrinal Goswami, Anindan Mondal, Mahabub Hasan Mahalat, Bibhash Sen, and Biplab K. Sikdar in International Journal
     * of Electronics Letters 2019.
     */
    const cutout res_4 =
    {{
         {{3, 0, 1, 2}},
         {{0, 1, 0, 3}},
         {{1, 2, 3, 0}},
         {{0, 3, 2, 1}}
    }};
    /**
     * Representing a simple diagonal 3-phase clocking which can be easily adopted for combinatorial circuits.
     */
    const cutout diagonal_3 =
    {{
         {{0, 1, 2}},
         {{1, 2, 0}},
         {{2, 0, 1}}
    }};
    /**
     * Representing a simple diagonal 4-phase clocking which can be easily adopted for combinatorial circuits.
     */
    const cutout diagonal_4 =
    {{
         {{0, 1, 2, 3}},
         {{1, 2, 3, 0}},
         {{2, 3, 0, 1}},
         {{3, 0, 1, 2}}
    }};
}

/**
 * Class representing a clocking scheme for tile-based FCN layouts. It holds a cutout of the whole
 * scheme. All the other tiles can be calculated from that.
 */
class fcn_clocking_scheme
{
public:
    /**
     * A single CLOCKS x CLOCKS tiles clocking scheme cutout. If scheme is regular, all other
     * positions can be determined easily.
     */
    const fcn_clock::cutout scheme;
    /**
     * Number of different clocks in this scheme.
     */
    const fcn_clock::number num_clocks;
    /**
     * Defines the clocking as regular and well-defined by the scheme.
     */
    const bool regular;
    /**
     * Standard constructor.
     * Initializes a clocking scheme with the given cutout.
     *
     * @param c Cutout of tiles representing a regular scheme.
     * @param n Number of different clocks.
     * @param r Flag to indicate clocking as regular.
     */
    fcn_clocking_scheme(const fcn_clock::cutout& c, const fcn_clock::number n, const bool r);
    /**
     * Move constructor.
     * Initializes a clocking scheme with the given cutout.
     *
     * @param c Cutout of tiles representing a regular scheme.
     * @param n Number of different clocks.
     * @param r Flag to indicate clocking as regular.
     */
    fcn_clocking_scheme(fcn_clock::cutout&& c, fcn_clock::number&& n, bool&& r);
    /**
     * Default copy constructor.
     */
    fcn_clocking_scheme(const fcn_clocking_scheme& s) = default;
    /**
     * Default move constructor.
     */
    fcn_clocking_scheme(fcn_clocking_scheme&& s) = default;
    /**
     * Default destructor.
     */
    ~fcn_clocking_scheme() = default;
    /**
     * Assignment operator is not available.
     */
    fcn_clocking_scheme& operator=(const fcn_clocking_scheme& rhs) = delete;
    /**
     * Move assignment operator is not available.
     */
    fcn_clocking_scheme& operator=(fcn_clocking_scheme&& rhs) = delete;
};

/**
 * The BANCS clocking is not supported yet.
 */
// static fcn_clocking_scheme bancs_3_clocking{fcn_clock::bancs_3, 3u, true};
/**
 * Pre-defined 4 x 4 USE clocking instance.
 */
static fcn_clocking_scheme use_4_clocking{fcn_clock::use_4, 4u, true};
/**
 * Pre-defined 4 x 4 RES clocking instance.
 */
static fcn_clocking_scheme res_4_clocking{fcn_clock::res_4, 4u, true};
/**
 * Pre-defined 3 x 3 diagonal clocking instance.
 */
static fcn_clocking_scheme diagonal_3_clocking{fcn_clock::diagonal_3, 3u, true};
/**
 * Pre-defined 4 x 4 diagonal clocking instance.
 */
static fcn_clocking_scheme diagonal_4_clocking{fcn_clock::diagonal_4, 4u, true};
/**
 * Pre-defined open clocking with 3 clock numbers.
 */
static fcn_clocking_scheme open_3_clocking{fcn_clock::cutout{}, 3u, false};
/**
 * Pre-defined open clocking with 4 clock numbers.
 */
static fcn_clocking_scheme open_4_clocking{fcn_clock::cutout{}, 4u, false};


#endif //FICTION_FCN_CLOCKING_SCHEME_H
