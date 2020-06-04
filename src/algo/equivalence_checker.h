//
// Created by marcel on 05.03.20.
//

#ifndef FICTION_EQUIVALENCE_CHECKER_H
#define FICTION_EQUIVALENCE_CHECKER_H

#include "logic_network.h"
#include "fcn_gate_layout.h"
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <nlohmann/json.hpp>
#include <vector>


/**
 * Performs logical and delay equivalence checks of gate layouts against a specification. That can either be its own
 * associated logic network or another gate layout. Three cases are to be distinguished:
 *
 *  (1) The gate layout performs a different functionality under at least one input pattern than its specification.
 *      The layout is NOT EQUIVALENT to its specification.
 *  (2) The gate layout performs the same functionality like its specification and has a throughput of 1/1.
 *      The layout is STRONGLY EQUIVALENT to its specification.
 *  (3) The gate layout performs the same functionality like its specification and has a throughput of 1/x, where x > 1.
 *      The layout is WEAKLY EQUIVALENT to its specification.
 *
 * Logical equivalence is checked with a SAT solver by extracting a logic description from the topological structure of
 * the gate layout and transforming it to CNF via Tseitin transformation.
 *
 * The technique is presented in "Verification for Field-coupled Nanocomputing Circuits" by Marcel Walter, Robert Wille,
 * Frank Sill Torres, Daniel Große, and Rolf Drechsler in DAC 2020.
 */
class equivalence_checker
{
public:
    /**
     * Standard constructor. Creates a miter structure from a gate layout and its associated logic network.
     *
     * @param fgl Gate layout to check.
     */
    explicit equivalence_checker(fcn_gate_layout_ptr fgl);
    /**
     * Standard constructor. Creates a miter structure from two gate layouts.
     *
     * @param fgl1 Gate layout to check.
     * @param fgl2 Gate layout serving as logical specification.
     */
    equivalence_checker(fcn_gate_layout_ptr fgl1, fcn_gate_layout_ptr fgl2);
    /**
     * Encapsulates the resulting information.
     */
    struct equiv_result
    {
        /**
         * The different equivalence types possible. See above for more information.
         */
        enum class eq_type { NONE, WEAK, STRONG };
        /**
         * Stores the equivalence type.
         */
        eq_type eq;
        /**
         * Delay value at which weak equivalence manifests.
         */
        std::size_t delay = 0ul;
        /**
         * Stores a possible counter example.
         */
        std::vector<bool> counter_example{};
        /**
         * Stores the runtime.
         */
        mockturtle::stopwatch<>::duration runtime;
    };
    /**
     * Performs the logical and delay equivalence checks. Calls the SAT solver and handles the three possible cases.
     *
     * @return Result container.
     */
    equiv_result operator()();

private:
    /**
     * The miter structure created from a layout and its specification.
     */
    std::optional<logic_network::mig_nt> miter;
    /**
     * The gate layout to check.
     */
    fcn_gate_layout_ptr layout;
};


#endif //FICTION_EQUIVALENCE_CHECKER_H
