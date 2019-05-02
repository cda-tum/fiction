//
// Created by marcel on 10.04.17.
//

#ifndef EXACT_PR_CONFIGURATION_H
#define EXACT_PR_CONFIGURATION_H

#include <string>
#include <memory>
#include <limits>
#include "fcn_clocking_scheme.h"

/**
 * Default timeout defined by Z3.
 */
constexpr const unsigned DEFAULT_TIMEOUT = 4294967295u;

/**
 * Configuration struct to enter to exact_pr calls.
 */
struct exact_pr_config
{
    /**
     * Number of tiles to use.
     */
    unsigned upper_bound = std::numeric_limits<unsigned>::max();
    /**
     * Flag to indicate that crossings should be used.
     */
    bool crossings = false;
    /**
     * Maximum number of crossing zones to use. If value is 0, the number of crossings is unbounded.
     */
    unsigned crossings_limit = 0u;
    /**
     * Maximum number of wire elements to use to connect two operations on the grid.
     */
    unsigned wire_limit = 0u;
    /**
     * Flag to indicate that upper_bound is in fact a fixed grid size.
     */
    bool fixed_size = false;
    /**
     * Flag to indicate that designated wires should be routed to balance I/O port paths.
     */
    bool io_ports = false;
    /**
     * Flag to indicate that I/Os should be placed at the grid's border.
     */
    bool border_io = false;
    /**
     * Flag to indicate that a discrepancy in fan-in path lengths was allowed.
     */
    bool path_discrepancy = false;
    /**
     * Flag to indicate that artificial clock latch delays should be used to balance paths.
     */
    bool artificial_latch = false;
    /**
     * Clocking scheme to be used.
     */
    std::shared_ptr<fcn_clocking_scheme> scheme = nullptr;
    /**
     * Sets a timeout in ms for the solving process. Standard is 4294967295 as defined by Z3.
     */
    unsigned timeout = DEFAULT_TIMEOUT;

    /**
     * Default standard constructor.
     */
    exact_pr_config() = default;
    /**
     * Default destructor.
     */
    ~exact_pr_config() = default;
    /**
     * Default copy constructor.
     *
     * @param rhs Configuration object to copy from.
     */
    exact_pr_config(const exact_pr_config& rhs) = default;
    /**
     * Default move constructor.
     *
     * @param rhs Configuration object to be moved.
     */
    exact_pr_config(exact_pr_config&& rhs) = default;
    /**
     * Default assignment operator.
     *
     * @param rhs Configuration object to assign from.
     * @return Newly created Configuration object.
     */
    exact_pr_config& operator=(const exact_pr_config& rhs) = default;
    /**
     * Default move assignment operator.
     *
     * @param rhs Configuration object to move assign from.
     * @return Newly created Configuration object.
     */
    exact_pr_config& operator=(exact_pr_config&& rhs) = default;
};

#endif //EXACT_PR_CONFIGURATION_H
