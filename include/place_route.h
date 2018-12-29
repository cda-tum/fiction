//
// Created by marcel on 12.07.17.
//

#ifndef FICTION_PLACE_ROUTE_H
#define FICTION_PLACE_ROUTE_H

#include "fcn_gate_layout.h"
#include "json.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>
#include <string>
#include <chrono>
#include <itertools.hpp>

/**
 * Abstract class that specifies an interface for place and route algorithms.
 */
class place_route
{

protected:
    /**
     * Result type for placement and routing approaches.
     */
    struct pr_result
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
    explicit place_route(std::shared_ptr<logic_network>&& ln)
            :
            network{std::move(ln)}
    {}
    /**
     * Default destructor. Declared as virtual to manage memory correctly.
     */
    virtual ~place_route() = default;
    /**
     * Copy constructor is not available.
     */
    place_route(const place_route& rhs) = delete;
    /**
     * Move constructor is not available.
     */
    place_route(place_route&& rhs) = delete;
    /**
     * Assignment operator is not available.
     */
    place_route& operator=(const place_route& rhs) = delete;
    /**
     * Move assignment operator is not available.
     */
    place_route& operator=(place_route&& rhs) = delete;
    /**
     * Starts the P&R process. This function need to be overridden by each sub-class.
     * It returns a PRResult from which one can extract all crucial information about the process.
     *
     * @return pr_result containing placed and routed layout as well as statistical information.
     */
    virtual pr_result perform_place_and_route() = 0;
    /**
     * Returns the stored layout.
     *
     * @return The stored layout.
     */
    std::shared_ptr<fcn_gate_layout> get_layout() noexcept
    {
        return layout;
    }
    /**
     * Returns the stored logic network.
     *
     * @return The stored logic network.
     */
    std::shared_ptr<logic_network> get_logic_network() noexcept
    {
        return network;
    }

protected:
    /**
     * Logic network to be mapped to a layout.
     */
    logic_network_ptr network = nullptr;
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
    /**
     * Alias for chrono's system clock.
     */
    using chrono = std::chrono::system_clock;
    /**
     * Calculates the runtime in milliseconds passed between two time points.
     *
     * @param b Begin of time measurement.
     * @param e End of time measurement.
     * @return Time duration in milliseconds between b and e.
     */
    auto calc_runtime(const chrono::time_point b, const chrono::time_point e) const noexcept
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
    }
};


#endif //FICTION_PLACE_ROUTE_H
