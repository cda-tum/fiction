//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP
#define FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP

#include "fiction/algorithms/graph/enumerate_all_paths.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"

#include <set>
#include <unordered_map>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

namespace fiction
{
/**
 * Routing objectives are source-target pairs.
 *
 * @tparam Lyt Layout type whose coordinates are to be used.
 */
template <typename Lyt>
struct objective
{
    const coordinate<Lyt>&source, target;
};
/**
 * Define an edge_intersection_graph as an undirected graph of layout_coordinate_path elements assigned to its nodes.
 */
template <typename Lyt>
using edge_intersection_graph =
    undirected_graph<std::size_t, layout_coordinate_path<Lyt>, undirected_pair<std::size_t>, std::size_t>;

struct generate_edge_intersection_graph_params
{
    /**
     * Allow crossings by not creating edges between paths that only share single-coordinate sub-paths.
     */
    bool crossings = false;
};

struct generate_edge_intersection_graph_stats
{
    std::size_t number_of_unsatisfiable_objectives{0};
};

namespace detail
{

template <typename Lyt>
class generate_edge_intersection_graph_impl
{
  public:
    generate_edge_intersection_graph_impl(const Lyt& lyt, const std::vector<objective<Lyt>>& obj,
                                          const generate_edge_intersection_graph_params& p,
                                          generate_edge_intersection_graph_stats&        st) :
            layout{lyt},
            objectives{obj},
            ps{p},
            pst{st}
    {}

    edge_intersection_graph<Lyt> run()
    {
        std::for_each(objectives.cbegin(), objectives.cend(),
                      [this](const auto& obj)
                      {
                          // enumerate all paths for the current objective
                          auto obj_paths = enumerate_all_clocking_paths<clk_path>(obstruction_layout{layout},
                                                                                  obj.source, obj.target);
                          // assign a unique label to each path and create a corresponding node in the graph
                          initiate_objective_nodes(obj_paths);

                          // if there are no paths, the objective could not be fulfilled
                          if (obj_paths.empty())
                          {
                              pst.number_of_unsatisfiable_objectives++;
                          }
                          else if (obj_paths.size() > 1)
                          {
                              // since all paths of the same objective have intersections by definition, create edges
                              // between all of them by iterating over all possible combinations of size 2
                              connect_scc(obj_paths);

                              // TODO store SCC in stats
                          }
                          // for each previously stored path, create an edge if there is an intersection
                          create_intersection_edges(obj_paths);

                          // add the collection to all paths gathered thus far
                          all_paths.insert(all_paths.end(), obj_paths.cbegin(), obj_paths.cend());
                      });

        return graph;
    }

  private:
    const Lyt& layout;

    const std::vector<objective<Lyt>> objectives;

    const generate_edge_intersection_graph_params ps;

    generate_edge_intersection_graph_stats& pst;

    edge_intersection_graph<Lyt> graph{};

    std::size_t path_id{0}, edge_id{0};
    /**
     * Assign an additional label to each path.
     */
    class labeled_layout_coordinate_path : public layout_coordinate_path<Lyt>
    {
      public:
        void append(const coordinate<Lyt>& c) noexcept
        {
            path_elements.insert(c);
            layout_coordinate_path<Lyt>::append(c);
        }

        template <typename Path>
        bool has_intersection_with(const Path& other) const noexcept
        {
            return std::any_of(std::cbegin(other), std::cend(other),
                               [this](const auto& c) { return path_elements.count(c) > 0; });
        }

        std::size_t label{};

      private:
        std::set<coordinate<Lyt>> path_elements{};
    };

    using clk_path = labeled_layout_coordinate_path;

    path_collection<clk_path> all_paths{};

    void initiate_objective_nodes(path_collection<clk_path>& objective_paths) noexcept
    {
        std::for_each(objective_paths.begin(), objective_paths.end(),
                      [this](auto& p)
                      {
                          p.label = path_id++;
                          graph.insert_vertex(p.label, p);
                      });
    }

    void connect_scc(path_collection<clk_path>& objective_paths) noexcept
    {
        for_each_combination(objective_paths.begin(), objective_paths.begin() + 2, objective_paths.end(),
                             [this, &objective_paths](const auto begin, [[maybe_unused]] const auto end)
                             {
                                 graph.insert_edge(begin->label, (begin + 1)->label, edge_id++);

                                 return false;  // keep looping
                             });
    }

    void create_intersection_edges(path_collection<clk_path>& objective_paths) noexcept
    {
        std::for_each(objective_paths.cbegin(), objective_paths.cend(),
                      [this](const auto& obj_p)
                      {
                          std::for_each(all_paths.cbegin(), all_paths.cend(),
                                        [this, &obj_p](const auto& stored_p)
                                        {
                                            if (obj_p.has_intersection_with(stored_p))
                                            {
                                                graph.insert_edge(obj_p.label, stored_p.label, edge_id++);
                                            }
                                        });
                      });
    }
};

}  // namespace detail

template <typename Lyt>
edge_intersection_graph<Lyt> generate_edge_intersection_graph(const Lyt&                              lyt,
                                                              const std::vector<objective<Lyt>>&      objectives,
                                                              generate_edge_intersection_graph_params ps  = {},
                                                              generate_edge_intersection_graph_stats* pst = nullptr)
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    generate_edge_intersection_graph_stats st{};

    detail::generate_edge_intersection_graph_impl<Lyt> p{lyt, objectives, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP
