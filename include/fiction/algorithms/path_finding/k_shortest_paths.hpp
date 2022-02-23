//
// Created by marcel on 21.02.22.
//

#ifndef FICTION_K_SHORTEST_PATHS_HPP
#define FICTION_K_SHORTEST_PATHS_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
{

struct yen_k_shortest_paths_params
{};

namespace detail
{

template <typename Path, typename Lyt>
class yen_k_shortest_paths_impl
{
  public:
    yen_k_shortest_paths_impl(const Lyt& lyt, const coordinate<Lyt>& src, const coordinate<Lyt>& tgt, const uint32_t k,
                              const yen_k_shortest_paths_params p) :
            layout{lyt},
            source{src},
            target{tgt},
            num_shortest_paths{k},
            ps{p}
    {
        k_shortest_paths.push_back(a_star<Path>(layout, source, target));
    }

    path_collection<Path> run()
    {
        // if there was no path to begin with
        if (k_shortest_paths.back().empty())
        {
            return {};
        }

        for (uint32_t k = 1; k < num_shortest_paths; ++k)
        {
            const auto& latest_path = k_shortest_paths[k - 1];

            // for all coordinates of the latest path except the last one
            for (uint32_t i = 0; i < static_cast<int64_t>(latest_path.size()) - 1; ++i)
            {
                // the spur is the ith coordinate of the latest path
                const auto spur = latest_path[i];

                // path from the source to the spur coordinate
                Path root_path{latest_path.cbegin(), latest_path.cbegin() + i};

                // for all previous paths
                for (const auto& p : k_shortest_paths)
                {
                    // TODO this equality should be done without new memory allocation
                    const Path partial_path(p.cbegin(), p.cbegin() + i);

                    if (root_path == partial_path)
                    {
                        // block the connection that was already used in the previous shortest path
                        layout.obstruct_connection(p[i], p[i + 1]);
                    }
                }

                for (const auto& root : root_path)
                {
                    if (root != spur)
                    {
                        // additionally, block every coordinate from the root path except the spur
                        layout.obstruct_coordinate(root);
                    }
                }

                if (auto spur_path = a_star<Path>(layout, spur, target); !spur_path.empty())
                {
                    // allocate more memory for the root path
                    root_path.reserve(root_path.size() + spur_path.size());
                    // concatenate root_path and spur_path to get the final path
                    root_path.insert(root_path.end(), std::make_move_iterator(spur_path.begin()),
                                     std::make_move_iterator(spur_path.end()));

                    // TODO can b_list be implemented as a set (heap)?
                    // if B does not already contain the path, it is a potential k-shortest path
                    if (!root_path.empty() && !b_list.contains(root_path))
                    {
                        b_list.push_back(root_path);
                    }
                }

                // clear obstructions
                layout.clear_obstructed_coordinates();
                layout.clear_obstructed_connections();
            }

            // if there were no spur paths or if all spur paths have been added to A already
            if (b_list.empty())
            {
                break;
            }

            // fetch and remove the lowest cost path from B and add it to A
            const auto lowest_cost_path_it =
                std::min_element(b_list.cbegin(), b_list.cend(),
                                 [](const auto& p1, const auto& p2) { return path_cost(p1) < path_cost(p2); });

            if (lowest_cost_path_it != b_list.cend())
            {
                k_shortest_paths.push_back(*lowest_cost_path_it);
                b_list.erase(lowest_cost_path_it);
            }
        }

        return k_shortest_paths;
    }

  private:
    obstruction_layout<Lyt> layout;

    const coordinate<Lyt>&source, target;

    const uint32_t num_shortest_paths;

    yen_k_shortest_paths_params ps;

    // A list of paths
    path_collection<Path> k_shortest_paths{};
    // B heap of paths
    path_collection<Path> b_list{};
    /**
     * Computes the cost of a path. This function can be adjusted to fetch paths of differing costs.
     *
     * Currently, the cost is equal to its length.
     *
     * @param p Path whose costs are to be calculated.
     * @return Costs of path p.
     */
    static std::size_t path_cost(const Path& p) noexcept
    {
        return p.size();
    }
};

}  // namespace detail

template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path> yen_k_shortest_paths(const Lyt& layout, const coordinate<Lyt>& source,
                                                         const coordinate<Lyt>& target, const uint32_t k,
                                                         yen_k_shortest_paths_params ps = {}) noexcept
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    return detail::yen_k_shortest_paths_impl<Path, Lyt>{layout, source, target, k, ps}.run();
}

}  // namespace fiction

#endif  // FICTION_K_SHORTEST_PATHS_HPP
