//
// Created by marcel on 12.07.17.
//

#ifndef FICTION_PHYSICAL_DESIGN_H
#define FICTION_PHYSICAL_DESIGN_H

#include "fcn_gate_layout.h"
#include "network_hierarchy.h"
#include "mockturtle/utils/stopwatch.hpp"
#include "mockturtle/utils/progress_bar.hpp"
#include "nlohmann/json.hpp"
#include <boost/predef/os/windows.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>
#include <string>
#include <itertools.hpp>

/**
 * Abstract class that specifies an interface for physical design algorithms.
 */
class physical_design
{

protected:
    /**
     * Result type for physical design approaches.
     */
    struct pd_result
    {
        bool success = false;
        nlohmann::json json;
    };

public:
    /**
     * Standard constructor.
     *
     * @param ln Logic network.
     */
    explicit physical_design(std::shared_ptr<logic_network>&& ln)
            :
            network{std::move(ln)}
    {}
    /**
     * Starts a physical design process. This function need to be overridden by each sub-class.
     * It returns a pd_result from which one can extract statistical information about the process.
     *
     * @return Result type containing statistical information.
     */
    virtual pd_result operator()() = 0;
    /**
     * Returns the stored layout.
     *
     * @return The stored layout.
     */
    fcn_gate_layout_ptr get_layout() noexcept
    {
        return layout;
    }
    /**
     * Returns the stored logic network.
     *
     * @return The stored logic network.
     */
    logic_network_ptr get_logic_network() noexcept
    {
        return network;
    }

protected:
    /**
     * Logic network to be mapped to a layout.
     */
    logic_network_ptr network = nullptr;
    /**
     * Network hierarchy for certain algorithms.
     */
    network_hierarchy_ptr hierarchy = nullptr;
    /**
     * Actual layout to which a logic network should be mapped.
     */
    fcn_gate_layout_ptr layout = nullptr;
    /**
     * Constructs all possible layout dimensions with n layout tiles by factorization.
     * NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs.
     * See https://svn.boost.org/trac10/ticket/11735 for details.
     *
     * @param n Number of layout tiles.
     * @return All possible layout dimensions with n layout tiles.
     */
    std::vector<fcn_dimension_xy> factorize(const unsigned n) const noexcept
    {
        std::vector<fcn_dimension_xy> vs;

        for (auto i = 1u; i <= std::sqrt(n); ++i)
        {
            if (n % i == 0)
            {
                auto x = i;
                auto y = n / i;

                if (x > 1 && y > 1) // workaround for BGL bug
                {
                    vs.emplace_back(fcn_dimension_xy{x, y});
                    if (x != y)
                        vs.emplace_back(fcn_dimension_xy{y, x});
                }
            }
        }

        return vs;
    }
};


#endif //FICTION_PHYSICAL_DESIGN_H
