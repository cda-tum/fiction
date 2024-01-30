//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_A_STAR_PR_HPP
#define FICTION_A_STAR_PR_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <queue>


using namespace std;

namespace fiction
{

/**
 * This struct stores statistics about the wiring reduction process.
 */
struct a_star_pr_stats
{
    /**
     * Runtime of the wiring reduction process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

class PriorityQueue {
    std::priority_queue<std::pair<int, std::string>, std::vector<std::pair<int, std::string>>, std::greater<std::pair<int, std::string>>> elements;

  public:
    bool empty() const {
        return elements.empty();
    }

    void put(const std::string& item, int priority) {
        elements.emplace(priority, item);
    }

    std::string get() {
        std::string item = elements.top().second;
        elements.pop();
        return item;
    }
};

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt a_star_pr(const Ntk& ntk, a_star_pr_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // Initialize stats for runtime measurement
    a_star_pr_stats stats{};

    // Measure runtime
    {
        const mockturtle::stopwatch stop{stats.time_total};
    }

    // Record runtime statistics if a valid pointer is provided
    if (pst != nullptr)
    {
        *pst = stats;
    }
}

}  // namespace fiction

#endif  // FICTION_A_STAR_PR_HPP