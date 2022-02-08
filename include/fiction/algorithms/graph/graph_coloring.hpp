//
// Created by marcel on 04.02.22.
//

#ifndef FICTION_GRAPH_COLORING_HPP
#define FICTION_GRAPH_COLORING_HPP

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <coloring_algorithm.hpp>
#include <dsatur.hpp>
#include <lmxrlf.hpp>
#include <mcs.hpp>
#include <tabucol.hpp>

namespace fiction
{

/**
 * A vertex coloring is simply a hash map from vertex IDs to Color types where Color should be constructible/convertible
 * from int.
 *
 * @tparam Graph Graph type to color.
 * @tparam Color Color type to use. Must be convertible from int.
 */
template <typename Graph, typename Color>
class vertex_coloring : public std::unordered_map<typename Graph::vertex_id_type, Color>
{};
/**
 * An enumeration of coloring engines to use for the graph coloring. All but SAT are using the graph-coloring library by
 * Brian Crites.
 */
enum class graph_coloring_engine
{
    /**
     * Optimal coloring for chordal graphs proposed in "Register Allocation via Coloring of Chordal Graphs" by Jens
     * Palsberg in CATS 2007.
     */
    MCS,
    /**
     * Saturation degree algorithm proposed in "New Methods to Color the Vertices of a Graph" by Daniel Brélaz in
     * Communications of the ACM, 1979. This algorithm is a heuristic but is exact for bipartite graphs.
     */
    DSATUR,
    /**
     * A randomized heuristic algorithm that combines various paradigms like divide-and-conquer, objective functions,
     * reuse of intermediate solutions etc. It was proposed in "Efficient Coloring of a Large Spectrum of Graphs" by
     * Darko Kirovski and Miodrag Potkonjak in DAC 1998. While this algorithm is really performant, it tends to find
     * non-optimal solutions even for small instances.
     */
    LMXRLF,  // randomized
    /**
     * A k-coloring algorithm using tabu search proposed in "Using Tabu Search Techniques for Graph Coloring" by A.
     * Hertz and D. de Werra in Computing 1987. The authors claim that it significantly outperforms simulated annealing.
     * However, since it is a k-coloring algorithm, it is required to set k_color_value in
     * determine_vertex_coloring_params to the chromatic number that is to be checked for.
     */
    TABUCOL,  // k-coloring
    /**
     * Custom iterative SAT-based encoding that finds optimal colorings.
     */
    SAT,
};

struct determine_vertex_coloring_params
{
    /**
     * The engine to use.
     */
    graph_coloring_engine engine = graph_coloring_engine::MCS;
    /**
     * k-color value for k-coloring algorithms, e.g., TABUCOL.
     */
    std::size_t k_color_value = 0;
    /**
     * Verify that the found coloring is valid.
     */
    bool verify_coloring_after_computation = false;
};

template <typename Color = std::size_t>
struct determine_vertex_coloring_stats
{
    /**
     * The determined chromatic number (could be non-optimal depending on the applied engine).
     */
    std::size_t chromatic_number{0};
    /**
     * The color that appeared the most.
     */
    Color most_frequent_color{0};
    /**
     * The frequency of the most used color.
     */
    std::size_t color_frequency{0};
    /**
     * Validation result of the coloring (std::nullopt = none attempted, true = valid, false = invalid).
     */
    std::optional<bool> coloring_verified = std::nullopt;
};

namespace detail
{

template <typename Graph, typename Color>
class graph_coloring_impl
{
  public:
    graph_coloring_impl(const Graph& g, const determine_vertex_coloring_params& p,
                        determine_vertex_coloring_stats<Color>& st) :
            graph{g},
            ps{p},
            pst{st}
    {}

    vertex_coloring<Graph, Color> run()
    {
        if (ps.engine == graph_coloring_engine::DSATUR || ps.engine == graph_coloring_engine::MCS ||
            ps.engine == graph_coloring_engine::LMXRLF || ps.engine == graph_coloring_engine::TABUCOL)
        {
            const auto translated_graph = translate_to_brian_crites_graph(graph);

            std::unique_ptr<GraphColoring::GraphColor> coloring_engine_ptr;

            // select engine
            switch (ps.engine)
            {
                case graph_coloring_engine::MCS:
                {
                    coloring_engine_ptr = std::make_unique<GraphColoring::Mcs>(translated_graph);
                    break;
                }
                case graph_coloring_engine::DSATUR:
                {
                    coloring_engine_ptr = std::make_unique<GraphColoring::Dsatur>(translated_graph);
                    break;
                }
                case graph_coloring_engine::LMXRLF:
                {
                    coloring_engine_ptr = std::make_unique<GraphColoring::Lmxrlf>(translated_graph);
                    break;
                }
                case graph_coloring_engine::TABUCOL:
                {
                    coloring_engine_ptr = std::make_unique<GraphColoring::Tabucol>(translated_graph, ps.k_color_value);
                    break;
                }
                default:
                {
                    // unreachable
                    break;
                }
            }

            const auto brian_crites_clr = coloring_engine_ptr->color();
            pst.chromatic_number        = static_cast<std::size_t>(coloring_engine_ptr->get_num_colors());

            if (ps.verify_coloring_after_computation)
            {
                pst.coloring_verified = coloring_engine_ptr->is_valid();
            }

            return translate_to_vertex_coloring(brian_crites_clr);
        }

        // TODO SAT encoding
        return {};
    }

  private:
    /**
     * The graph to be colored.
     */
    const Graph& graph;
    /**
     * Parameters.
     */
    const determine_vertex_coloring_params ps;
    /**
     * Statistics.
     */
    determine_vertex_coloring_stats<Color>& pst;
    /**
     * An alias for the graph structure used in Brian Crites' graph-coloring library. The usage of his algorithms
     * require the translation into this data structure.
     */
    using brian_crites_graph = std::map<std::string, std::vector<std::string>>;
    /**
     * An alias for the coloring type used in Brian Crites' graph-coloring library.
     */
    using brian_crites_coloring = std::map<std::string, int>;
    /**
     * Converts the given node ID of a Brian Crites graph to the corresponding one used in Graph.
     *
     * @param node Node ID to convert between graph structures.
     * @return Corresponding node ID in Graph.
     */
    typename Graph::vertex_id_type convert_node_index(const std::string& node) const noexcept
    {
        if constexpr (std::is_same_v<typename Graph::vertex_id_type, std::string>)
        {
            return node;
        }
        else
        {
            return static_cast<typename Graph::vertex_id_type>(std::stoi(node));
        }
    }
    /**
     * Converts the given node ID of a Graph to the corresponding one used in Brian Crites' graph structure. This
     * function is automatically removed from overload resolution if both graphs use std::string because it would clash
     * with the function above.
     *
     * @param node Node ID to convert between graph structures.
     * @return Corresponding node ID in the Brian Crites graph.
     */
    template <typename Graph_ = Graph,
              typename        = std::enable_if_t<!std::is_same_v<typename Graph_::vertex_id_type, std::string>>>
    std::string convert_node_index(const typename Graph::vertex_id_type& node) const noexcept
    {
        return std::to_string(node);
    }
    /**
     * Translates the given graph to a equivalent Brian Crites graph data structure.
     *
     * @param g Graph to translate.
     * @return Translated graph.
     */
    brian_crites_graph translate_to_brian_crites_graph(const Graph& g) noexcept
    {
        brian_crites_graph translated_graph{};

        // iterate over all vertices of the original graph
        std::for_each(g.begin_vertices(), g.end_vertices(),
                      [this, &g, &translated_graph](const auto& v_pair)
                      {
                          const auto v1 = v_pair.first;

                          // if v does not have any adjacent vertices
                          if (g.begin_adjacent(v1) == g.end_adjacent(v1))
                          {
                              // create an isolated vertex in the brian_crites_graph
                              translated_graph[convert_node_index(v1)] = {};
                          }
                          else
                          {
                              // iterate over all vertices v2 adjacent to v1
                              std::for_each(
                                  g.begin_adjacent(v1), g.end_adjacent(v1),
                                  [this, &translated_graph, &v1](const auto& v2)
                                  {
                                      // add an edge in the brian_crites_graph that leads from v1 to v2
                                      translated_graph[convert_node_index(v1)].push_back(convert_node_index(v2));
                                      // and ones that leads from v2 to v1
                                      translated_graph[convert_node_index(v2)].push_back(convert_node_index(v1));
                                  });
                          }
                      });

        return translated_graph;
    }
    /**
     * Translates the given Brian Crites coloring to the corresponding vertex coloring used here.
     *
     * @param bc_coloring Brian Crites coloring to translate.
     * @return Translated vertex coloring.
     */
    vertex_coloring<Graph, Color> translate_to_vertex_coloring(const brian_crites_coloring& bc_coloring) const noexcept
    {
        vertex_coloring<Graph, Color> v_coloring{};

        // determine the color frequency alongside; index represents the color, value its frequency
        std::vector<Color> color_frequency(pst.chromatic_number, Color{0});

        std::for_each(bc_coloring.cbegin(), bc_coloring.cend(),
                      [this, &v_coloring, &color_frequency](const auto& c_pair)
                      {
                          // convert color
                          v_coloring[convert_node_index(c_pair.first)] = static_cast<Color>(c_pair.second);
                          // increment the color frequency
                          color_frequency[static_cast<std::size_t>(c_pair.second)]++;
                      });

        if (const auto it = std::max_element(color_frequency.cbegin(), color_frequency.cend());
            it != color_frequency.cend())
        {
            // get index from iterator; index represents the color
            pst.most_frequent_color = static_cast<Color>(it - color_frequency.cbegin());
            // the value of the iterator represents the color frequency
            pst.color_frequency = *it;
        }

        return v_coloring;
    }
};

}  // namespace detail

/**
 * This function provides an interface to call various vertex coloring algorithms on the given graph. A vertex coloring
 * is the assignment of colors to graph vertices such that no two vertices that share an edge receive the same color. If
 * a graph is colorable with k colors, the graph is said to be k-colorable. The minimum value of k for a graph is called
 * its chromatic number. To determine the chromatic number of a graph is NP-complete in general. The provided algorithms
 * attempt to get as close to the optimum coloring as possible. However, no heuristic can give an optimality guarantee.
 * If the exact chromatic number is required, the SAT-based engine must be used. This may require exponential runtime in
 * the worst case but is, on average, a lot faster due to the smart traversal of search spaces that SAT solvers provide.
 *
 * See graph_coloring_engine for a list of all supported engines.
 *
 * @tparam Graph Graph type to color.
 * @tparam Color Color type to use.
 * @param graph The graph whose vertices are to be colored.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return An assignment of graph vertices to colors such that no two adjacent vertices share the same color.
 */
template <typename Graph, typename Color = std::size_t>
vertex_coloring<Graph, Color> determine_vertex_coloring(const Graph& graph, determine_vertex_coloring_params ps = {},
                                                        determine_vertex_coloring_stats<Color>* pst = nullptr)
{
    determine_vertex_coloring_stats st{};

    detail::graph_coloring_impl<Graph, Color> p{graph, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_GRAPH_COLORING_HPP
