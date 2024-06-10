//
// Created by marcel on 04.02.22.
//

#ifndef FICTION_GRAPH_COLORING_HPP
#define FICTION_GRAPH_COLORING_HPP

#include "fiction/utils/hash.hpp"

#include <bill/sat/cardinality.hpp>
#include <bill/sat/interface/common.hpp>
#include <bill/sat/solver.hpp>
#include <bill/sat/tseytin.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
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
     * Optimal coloring for chordal graphs proposed in \"Register Allocation via Coloring of Chordal Graphs\" by Jens
     * Palsberg in CATS 2007.
     */
    MCS,
    /**
     * Saturation degree algorithm proposed in \"New Methods to Color the Vertices of a Graph\" by Daniel Brélaz in
     * Communications of the ACM, 1979. This algorithm is a heuristic but is exact for bipartite graphs.
     */
    DSATUR,
    /**
     * A randomized heuristic algorithm that combines various paradigms like divide-and-conquer, objective functions,
     * reuse of intermediate solutions etc. It was proposed in \"Efficient Coloring of a Large Spectrum of Graphs\" by
     * Darko Kirovski and Miodrag Potkonjak in DAC 1998. While this algorithm is really performant, it tends to find
     * non-optimal solutions even for small instances.
     */
    LMXRLF,  // randomized
    /**
     * A \f$k\f$-coloring algorithm using tabu search proposed in \"Using Tabu Search Techniques for Graph Coloring\"
     * by A. Hertz and D. de Werra in Computing 1987. The authors claim that it significantly outperforms simulated
     * annealing. However, since it is a \f$k\f$-coloring algorithm, it is required to set `k_color_value` in
     * `determine_vertex_coloring_params` to the chromatic number that is to be checked for.
     */
    TABUCOL,  // k-coloring
    /**
     * Custom iterative SAT-based encoding that finds optimal colorings.
     */
    SAT
};
/**
 * An enumeration of search tactics to use for the SAT-based graph coloring to determine a min-coloring.
 */
enum class graph_coloring_sat_search_tactic
{
    /**
     * Ascend linearly by checking for \f$k = 1, 2, 3, \dots\f$ until SAT. If at least one clique is passed, \f$k\f$
     * starts at the largest clique size \f$|C|\f$ instead with \f$k = |C|, |C| + 1, |C| + 2, \dots\f$
     */
    LINEARLY_ASCENDING,
    /**
     * Descend linearly by checking for \f$k = |G|, |G| - 1, |G| - 2, \dots\f$ until UNSAT.
     */
    LINEARLY_DESCENDING,
    /**
     * First ascend exponentially by checking for \f$k = 2^0, 2^1, 2^2, \dots\f$ until SAT, then perform binary
     * search in the window \f$[2^{h-1}, 2^h]\f$, where \f$2^h\f$ was the first SAT. If at least one clique is
     * passed, \f$k\f$ starts at the largest clique size \f$|C|\f$ instead with \f$k = 2^0 \cdot |C|, 2^1 \cdot
     * |C|, 2^2 \cdot |C|, \dots\f$
     */
    BINARY_SEARCH
};

/**
 * Parameters for SAT-based graph coloring.
 *
 * @tparam Graph Type of the graph to color.
 */
template <typename Graph>
struct determine_vertex_coloring_sat_params
{
    /**
     * The SAT solver to use.
     */
    bill::solvers sat_engine = bill::solvers::ghack;
    /**
     * The search tactic to apply.
     */
    graph_coloring_sat_search_tactic sat_search_tactic = graph_coloring_sat_search_tactic::LINEARLY_ASCENDING;
    /**
     * If cliques in the passed graph are known, they can be used for symmetry breaking in the SAT engine which
     * significantly speeds up runtime. The bigger the cliques, the better.
     */
    std::vector<std::vector<typename Graph::vertex_id_type>> cliques{};
    /**
     * Tries to establish the color frequency of color 0 such that it equals the largest clique size.
     */
    bool clique_size_color_frequency = false;
};
/**
 * Parameters for heuristic graph coloring.
 */
struct determine_vertex_coloring_heuristic_params
{
    /**
     * \f$k\f$-color value for \f$k\f$-coloring algorithms, e.g., TABUCOL.
     */
    std::size_t k_color_value = 0;
};
/**
 * Common parameters for the graph coloring algorithm.
 *
 * @tparam Graph Type of the graph to color.
 */
template <typename Graph>
struct determine_vertex_coloring_params
{
    /**
     * The engine to use.
     */
    graph_coloring_engine engine = graph_coloring_engine::MCS;
    /**
     * Parameters for `engine == SAT`.
     */
    determine_vertex_coloring_sat_params<Graph> sat_params{};
    /**
     * Parameters for `engine != SAT`.
     */
    determine_vertex_coloring_heuristic_params heuristic_params{};
    /**
     * Verify that the found coloring is valid.
     */
    bool verify_coloring_after_computation = false;
};

template <typename Color = std::size_t>
struct determine_vertex_coloring_stats
{
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
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

template <typename Graph, typename Color = std::size_t, bill::solvers SolverType = bill::solvers::ghack>
class sat_coloring_handler
{
  private:
    // forward declaration
    struct solver_instance;
    using solver_instance_ptr = std::shared_ptr<solver_instance>;
    using result_instance     = std::pair<bill::result::states, solver_instance_ptr>;
    using k_instance          = std::pair<std::size_t, solver_instance_ptr>;

  public:
    explicit sat_coloring_handler(const Graph& g, const determine_vertex_coloring_sat_params<Graph>& p,
                                  determine_vertex_coloring_stats<Color>& st) :
            graph{g},
            ps{p},
            pst{st},
            largest_clique{std::max_element(ps.cliques.cbegin(), ps.cliques.cend(),
                                            [](const auto& c1, const auto& c2) { return c1.size() < c2.size(); })},
            q{largest_clique == ps.cliques.cend() ? 1 : (largest_clique->size() ? largest_clique->size() : 1)}
    {}

    result_instance check_k_coloring(const std::size_t k) const noexcept
    {
        const auto k_color_instance = std::make_shared<solver_instance>(graph, k);

        at_least_one_color_per_vertex(k_color_instance);
        at_most_one_color_per_vertex(k_color_instance);
        exclude_identical_adjacent_colors(k_color_instance);
        symmetry_breaking(k_color_instance);

        // NOTE There is a problem with this color frequency constraint. It's best explained using an example: Let there
        // be two cliques of size 1. Color frequency says, they must both be assigned color 0 for frequency reasons. Now
        // let the respective paths be mutex, i.e., their nodes share an edge. Suddenly, the graph is not colorable
        // anymore. It would actually work fine if the cliques were total.
        // If SAT is not able to determine a solution because of this issue, the algorithm falls back to MCS.

        if (ps.clique_size_color_frequency)
        {
            color_frequency_equal_to_largest_clique_size(k_color_instance);
        }

        return {check_sat(k_color_instance), k_color_instance};
    }

    std::optional<k_instance> determine_min_coloring_with_linearly_ascending_search() const noexcept
    {
        for (std::size_t k = q; k < graph.size_vertices() + 1; ++k)
        {
            if (const auto [sat, instance] = check_k_coloring(k); sat == bill::result::states::satisfiable)
            {
                return k_instance{k, instance};
            }
        }

        return std::nullopt;
    }

    std::optional<k_instance> determine_min_coloring_with_linearly_descending_search() const noexcept
    {
        k_instance most_recent_sat_instance{};

        for (std::size_t k = graph.size_vertices(); k >= q - 1; --k)
        {
            const auto [sat, instance] = check_k_coloring(k);

            if (sat == bill::result::states::unsatisfiable)
            {
                return most_recent_sat_instance;
            }

            most_recent_sat_instance = {k, instance};
        }

        return std::nullopt;
    }

    std::optional<k_instance> determine_min_coloring_with_binary_search() const noexcept
    {
        std::optional<k_instance> most_recent_sat_instance{};

        std::size_t h{q};

        // exponentially ascending search
        for (; h < graph.size_vertices() + 1; h *= 2)
        {
            if (const auto [sat, instance] = check_k_coloring(h); sat == bill::result::states::satisfiable)
            {
                most_recent_sat_instance = {h, instance};

                // optimization to immediately return the found instance if h is equal to the clique size because the
                // clique size represents a lower bound. Thus, no binary search needs to be conducted
                if (h == q)
                {
                    return most_recent_sat_instance;
                }

                break;  // upper bound h has been found
            }
        }

        // create a vector of h / 2 + 1 numbers
        std::vector<std::size_t> potential_chromatic_numbers(h / 2 + 1);
        // fill the vector with the range of numbers [2^(h-1), 2^h]
        std::iota(potential_chromatic_numbers.begin(), potential_chromatic_numbers.end(), h / 2);

        // binary search for the chromatic number, i.e., the lower bound of potential ones
        std::lower_bound(potential_chromatic_numbers.cbegin(), potential_chromatic_numbers.cend(), h,
                         [this, &most_recent_sat_instance](const auto& k1, [[maybe_unused]] const auto& k2)
                         {
                             if (const auto [sat, instance] = check_k_coloring(k1);
                                 sat == bill::result::states::satisfiable)
                             {
                                 most_recent_sat_instance = {k1, instance};

                                 return false;
                             }

                             return true;
                         });

        return most_recent_sat_instance;
    }

    std::optional<vertex_coloring<Graph, Color>> color() noexcept
    {
        std::optional<k_instance> min_coloring = std::nullopt;

        switch (ps.sat_search_tactic)
        {
            case graph_coloring_sat_search_tactic::LINEARLY_ASCENDING:
            {
                min_coloring = determine_min_coloring_with_linearly_ascending_search();
                break;
            }
            case graph_coloring_sat_search_tactic::LINEARLY_DESCENDING:
            {
                min_coloring = determine_min_coloring_with_linearly_descending_search();
                break;
            }
            case graph_coloring_sat_search_tactic::BINARY_SEARCH:
            {
                min_coloring = determine_min_coloring_with_binary_search();
                break;
            }
        }

        if (min_coloring.has_value())
        {
            const auto [k, instance] = *min_coloring;

            pst.chromatic_number = k;
            return extract_vertex_coloring(instance, get_model(instance));
        }

        return std::nullopt;
    }

  private:
    /**
     * A reference to the graph to be colored.
     */
    const Graph& graph;
    /**
     * Parameters.
     */
    const determine_vertex_coloring_sat_params<Graph> ps;
    /**
     * Statistics.
     */
    determine_vertex_coloring_stats<Color>& pst;
    /**
     * Iterator to the largest given clique.
     */
    const typename std::vector<std::vector<typename Graph::vertex_id_type>>::const_iterator largest_clique;
    /**
     * Largest clique size.
     */
    const std::size_t q;
    /**
     * Alias for a vertex-color pair.
     */
    using vertex_color = std::pair<typename Graph::vertex_id_type, Color>;

    struct solver_instance
    {
        /**
         * Standard constructor. Initialize one variable for each vertex-color pair.
         *
         * @param graph The graph to color.
         * @param num_colors Number of colors to attempt the coloring with.
         */
        explicit solver_instance(const Graph& graph, const std::size_t num_colors) : k{num_colors}
        {
            std::for_each(graph.begin_vertices(), graph.end_vertices(),
                          [this, &num_colors](const auto& vp)
                          {
                              const auto& v = vp.first;
                              for (std::size_t c = 0; c < num_colors; ++c)
                              {
                                  variables[{v, c}] = solver.add_variable();
                              }
                          });
        }
        /**
         * Default constructor is not available.
         */
        solver_instance() = delete;
        /**
         * Number of colors.
         */
        std::size_t k;
        /**
         * SAT solver.
         */
        bill::solver<SolverType> solver{};
        /**
         * Stores all variables.
         */
        std::unordered_map<vertex_color, bill::var_type> variables{};
    };

    void at_least_one_color_per_vertex(const solver_instance_ptr& instance) const
    {
        // for each vertex
        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [&instance](const auto& vp)
                      {
                          const auto& v = vp.first;

                          std::vector<bill::var_type> vc(instance->k, 0);

                          // for each color
                          for (std::size_t c = 0; c < instance->k; ++c)
                          {
                              vc[c] = instance->variables[{v, c}];
                          }

                          bill::at_least_one(vc, instance->solver);
                      });
    }

    void at_most_one_color_per_vertex(const solver_instance_ptr& instance) const
    {
        // for each pair of colors
        for (std::size_t c1 = 0; c1 < instance->k; ++c1)
        {
            // use an optimization here: c2 > c1 instead of c2 != c1 to save half the clauses
            for (std::size_t c2 = c1 + 1; c2 < instance->k; ++c2)
            {
                // for each vertex
                std::for_each(graph.begin_vertices(), graph.end_vertices(),
                              [&instance, &c1, &c2](const auto& vp)
                              {
                                  const auto& v = vp.first;
                                  // not vertex has color 1 OR not vertex has color 2
                                  instance->solver.add_clause(
                                      {{bill::lit_type{instance->variables[{v, c1}], bill::negative_polarity},
                                        bill::lit_type{instance->variables[{v, c2}], bill::negative_polarity}}});
                              });
            }
        }
    }

    void exclude_identical_adjacent_colors(const solver_instance_ptr& instance) const
    {
        // for each edge
        std::for_each(graph.begin_edges(), graph.end_edges(),
                      [&instance](const auto& e)
                      {
                          // source and target of edge e
                          const auto& [v1, v2] = e.first;

                          // for each color
                          for (std::size_t c = 0; c < instance->k; ++c)
                          {
                              // not vertex 1 has color c OR not vertex 2 has color c
                              instance->solver.add_clause(
                                  {{bill::lit_type{instance->variables[{v1, c}], bill::negative_polarity},
                                    bill::lit_type{instance->variables[{v2, c}], bill::negative_polarity}}});
                          }
                      });
    }

    void color_frequency_equal_to_largest_clique_size(const solver_instance_ptr& instance) const
    {
        std::vector<bill::lit_type> same_color_in_each_clique{};
        same_color_in_each_clique.reserve(ps.cliques.size() * q);

        // for each color
        for (std::size_t c = 0; c < instance->k; ++c)
        {
            std::vector<bill::lit_type> color_c_in_each_clique{};
            color_c_in_each_clique.reserve(ps.cliques.size());

            // for each clique
            std::for_each(ps.cliques.cbegin(), ps.cliques.cend(),
                          [&instance, &c, &color_c_in_each_clique](const auto& clique)
                          {
                              std::vector<bill::lit_type> vc{};
                              vc.reserve(clique.size());

                              // for each vertex in clique
                              std::for_each(clique.cbegin(), clique.cend(), [&instance, &c, &vc](const auto& v)
                                            { vc.push_back({instance->variables[{v, c}], bill::positive_polarity}); });

                              color_c_in_each_clique.push_back(bill::add_tseytin_or(instance->solver, vc));
                          });

            same_color_in_each_clique.push_back(bill::add_tseytin_and(instance->solver, color_c_in_each_clique));
        }

        instance->solver.add_clause(same_color_in_each_clique);
    }
    /**
     * Reduce the search space by symmetry breaking. To this end, each vertex in the provided clique gets a different
     * color assigned from the beginning.
     *
     * @param instance Pointer to the solver instance.
     * @return A clique-first ordering of the vertices that assigns the clique vertices the lowest indices.
     */
    std::vector<typename Graph::vertex_id_type> pre_assign_largest_clique(const solver_instance_ptr& instance) const
    {
        // a particular ordering of the graph's vertices to use in the subsequent lexicographical solution transformer
        // that ensures that the pre-assigned clique vertices are of the lowest indices
        std::vector<typename Graph::vertex_id_type> clique_first_ordering{};

        // if there is no clique given, this symmetry breaking cannot be performed
        if (largest_clique != ps.cliques.cend() && !largest_clique->empty())
        {
            // for each color index up to min(k, size of the largest clique)
            for (std::size_t c = 0; c < std::min(instance->k, q); ++c)
            {
                // assign color c to vertex at index c
                const auto v = (*largest_clique)[c];
                instance->solver.add_clause(bill::lit_type{instance->variables[{v, c}], bill::positive_polarity});

                // create a clique first vertex ordering
                clique_first_ordering.push_back(v);
            }
        }
        else  // without clique information, at least the first vertex can be painted
        {
            std::find_if(
                graph.begin_vertices(), graph.end_vertices(),
                [this, &instance, &clique_first_ordering](const auto& vp)
                {
                    const auto& v = vp.first;
                    // assign color 0 to any vertex
                    instance->solver.add_clause(bill::lit_type{instance->variables[{v, 0}], bill::positive_polarity});

                    // create a clique first vertex ordering
                    clique_first_ordering.push_back(v);

                    std::find_if(graph.begin_adjacent(v), graph.end_adjacent(v),
                                 [&instance, &clique_first_ordering](const auto& av)
                                 {
                                     // assign color 1 to an adjacent vertex
                                     instance->solver.add_clause(
                                         bill::lit_type{instance->variables[{av, 1}], bill::positive_polarity});

                                     // create a clique first vertex ordering
                                     clique_first_ordering.push_back(av);

                                     return true;  // abort loop after first iteration
                                 });

                    return true;  // abort loop after first iteration
                });
        }

        // thus far, the ordering only contains the clique vertices; add the missing ones in an arbitrary order
        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [&clique_first_ordering](const auto& vp)
                      {
                          const auto& v = vp.first;
                          // if vertex v is not yet in the ordering
                          if (std::find(clique_first_ordering.cbegin(), clique_first_ordering.cend(), v) ==
                              clique_first_ordering.cend())
                          {
                              clique_first_ordering.push_back(v);
                          }
                      });

        return clique_first_ordering;
    }
    /**
     * Reduce the search space by symmetry breaking. To this end, the solution is transformed to be lexicographical
     * minimal, i.e., no vertex is assigned color c unless color c - 1 has been assigned to any vertex of lower index.
     *
     * @param instance Pointer to the solver instance.
     */
    void transform_solution_to_lexicographical_minimum(
        const solver_instance_ptr&                         instance,
        const std::vector<typename Graph::vertex_id_type>& clique_first_ordering) const
    {
        // for each vertex v1
        for (auto v1i = clique_first_ordering.cbegin(); v1i != clique_first_ordering.cend(); ++v1i)
        {
            const auto& v1 = *v1i;

            // for each color c1 starting with the second
            for (std::size_t c1 = 1; c1 < instance->k; ++c1)
            {
                // create a vector of literals and already add the first one being NOT assign color c1 to vertex v1
                std::vector<bill::lit_type> vc{
                    {bill::lit_type{instance->variables[{v1, c1}], bill::negative_polarity}}};

                // for each vertex v2 up to v1
                for (auto v2i = clique_first_ordering.cbegin(); v2i != v1i + 1; ++v2i)
                {
                    const auto& v2 = *v2i;

                    // for each color c2 starting from c1 - 1
                    for (std::size_t c2 = c1 - 1; c2 < instance->k; ++c2)
                    {
                        // add a literal to the vector stating assign c2 to v2
                        vc.emplace_back(instance->variables[{v2, c2}], bill::positive_polarity);
                    }
                }

                // add the disjunction of literals to the solver
                instance->solver.add_clause(vc);
            }
        }
    }
    /**
     * Reduce the search space by symmetry breaking. Two tactics are implemented:
     *
     *  - pre-assigning different colors to the largest given clique, and
     *  - transforming the coloring solution to be lexicographically minimal
     *
     * @param instance Pointer to the solver instance.
     */
    void symmetry_breaking(const solver_instance_ptr& instance) const
    {
        const auto clique_first_ordering = pre_assign_largest_clique(instance);
        transform_solution_to_lexicographical_minimum(instance, clique_first_ordering);
    }

    auto check_sat(const solver_instance_ptr& instance) const
    {
        return instance->solver.solve();
    }

    auto get_model(const solver_instance_ptr& instance) const
    {
        return instance->solver.get_model().model();
    }

    vertex_coloring<Graph, Color> extract_vertex_coloring(const solver_instance_ptr&      instance,
                                                          const bill::result::model_type& model) const noexcept
    {
        vertex_coloring<Graph, Color> coloring{};
        // determine the color frequency alongside
        std::unordered_map<Color, std::size_t> color_frequency{};

        // for each vertex
        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [&instance, &model, &coloring, &color_frequency](const auto& vp)
                      {
                          const auto& v = vp.first;
                          // for each color
                          for (std::size_t c = 0; c < instance->k; ++c)
                          {
                              // if vertex v is colored with color c
                              if (model.at(instance->variables.at({v, c})) == bill::lbool_type::true_)
                              {
                                  // paint the vertex
                                  coloring[v] = c;
                                  // increment the color frequency
                                  color_frequency[c]++;
                              }
                          }
                      });

        if (const auto it = std::max_element(color_frequency.cbegin(), color_frequency.cend(),
                                             [](const auto& cf1, const auto& cf2) { return cf1.second < cf2.second; });
            it != color_frequency.cend())
        {
            pst.most_frequent_color = it->first;
            pst.color_frequency     = it->second;
        }

        return coloring;
    }
};

template <typename Graph, typename Color>
class graph_coloring_impl
{
  public:
    graph_coloring_impl(const Graph& g, const determine_vertex_coloring_params<Graph>& p,
                        determine_vertex_coloring_stats<Color>& st) :
            graph{g},
            ps{p},
            pst{st}
    {}

    vertex_coloring<Graph, Color> run()
    {
        // measure runtime
        mockturtle::stopwatch stop{pst.time_total};

        std::optional<vertex_coloring<Graph, Color>> coloring = std::nullopt;

        if (ps.engine == graph_coloring_engine::SAT)
        {
            switch (ps.sat_params.sat_engine)
            {
                case bill::solvers::ghack:
                {
                    coloring =
                        sat_coloring_handler<Graph, Color, bill::solvers::ghack>{graph, ps.sat_params, pst}.color();
                    break;
                }
                case bill::solvers::glucose_41:
                {
                    coloring = sat_coloring_handler<Graph, Color, bill::solvers::glucose_41>{graph, ps.sat_params, pst}
                                   .color();
                    break;
                }
                case bill::solvers::bsat2:
                {
                    assert(false && "At the time of writing this code, bsat2 was not able to solve coloring instances "
                                    "properly. It is, therefore, not recommended using it. If in the future an update "
                                    "to bill and/or bsat2 has been published, feel free to remove this assertion.");

                    coloring =
                        sat_coloring_handler<Graph, Color, bill::solvers::bsat2>{graph, ps.sat_params, pst}.color();
                    break;
                }
#if !defined(BILL_WINDOWS_PLATFORM)
                case bill::solvers::maple:
                {
                    coloring =
                        sat_coloring_handler<Graph, Color, bill::solvers::maple>{graph, ps.sat_params, pst}.color();
                    break;
                }
                case bill::solvers::bmcg:
                {
                    coloring =
                        sat_coloring_handler<Graph, Color, bill::solvers::bmcg>{graph, ps.sat_params, pst}.color();
                    break;
                }
#endif
                default:
                {
                    coloring = sat_coloring_handler<Graph, Color>{graph, ps.sat_params, pst}.color();
                    break;
                }
            }

            // if SAT was not able to determine a coloring, try MCS
            if (!coloring.has_value())
            {
                ps.engine = graph_coloring_engine::MCS;
            }
        }

        if (is_brian_crites_engine(ps.engine))
        {
            coloring = run_brian_crites_engine();
        }

        assert(coloring.has_value() && "No coloring could be determined.");

        if (ps.verify_coloring_after_computation)
        {
            pst.coloring_verified = is_valid_vertex_coloring(*coloring);
        }

        return *coloring;
    }

  private:
    /**
     * The graph to be colored.
     */
    const Graph& graph;
    /**
     * Parameters.
     */
    determine_vertex_coloring_params<Graph> ps;
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
     * Checks whether the given engine points towards the usage of an algorithm from the graph-coloring library by Brian
     * Crites.
     *
     * @param engine Graph coloring engine to check.
     * @return `true` iff the given engine is DSATUR, MCS, LMXRLF, or TABUCOL.
     */
    [[nodiscard]] bool is_brian_crites_engine(const graph_coloring_engine& engine) const noexcept
    {
        return engine == graph_coloring_engine::DSATUR || engine == graph_coloring_engine::MCS ||
               engine == graph_coloring_engine::LMXRLF || engine == graph_coloring_engine::TABUCOL;
    }
    /**
     * Converts the given node ID of a Brian Crites graph to the corresponding one used in Graph.
     *
     * @param node Node ID to convert between graph structures.
     * @return Corresponding node ID in Graph.
     */
    [[nodiscard]] typename Graph::vertex_id_type convert_node_index(const std::string& node) const noexcept
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
    template <typename GraphProxy = Graph,
              typename            = std::enable_if_t<!std::is_same_v<typename GraphProxy::vertex_id_type, std::string>>>
    [[nodiscard]] std::string convert_node_index(const typename Graph::vertex_id_type& node) const noexcept
    {
        return std::to_string(node);
    }
    /**
     * Translates the given graph to a equivalent Brian Crites graph data structure.
     *
     * @param g Graph to translate.
     * @return Translated graph.
     */
    [[nodiscard]] brian_crites_graph translate_to_brian_crites_graph(const Graph& g) const noexcept
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
    [[nodiscard]] vertex_coloring<Graph, Color>
    translate_to_vertex_coloring(const brian_crites_coloring& bc_coloring) const noexcept
    {
        vertex_coloring<Graph, Color> v_coloring{};

        // determine the color frequency alongside; index represents the color, value its frequency
        std::vector<Color> color_frequency(pst.chromatic_number, Color{0});

        std::for_each(bc_coloring.cbegin(), bc_coloring.cend(),
                      [this,  // NOLINT(clang-diagnostic-unused-lambda-capture): false positive
                       &v_coloring, &color_frequency](const auto& c_pair)
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

    [[nodiscard]] vertex_coloring<Graph, Color> run_brian_crites_engine() const noexcept
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
                coloring_engine_ptr =
                    std::make_unique<GraphColoring::Tabucol>(translated_graph, ps.heuristic_params.k_color_value);
                break;
            }
            default:
            {
                // unreachable
                break;
            }
        }

        if (coloring_engine_ptr != nullptr)
        {
            const auto brian_crites_clr = coloring_engine_ptr->color();
            pst.chromatic_number        = static_cast<std::size_t>(coloring_engine_ptr->get_num_colors());

            return translate_to_vertex_coloring(brian_crites_clr);
        }

        return {};
    }
    /**
     * Checks whether the given coloring is valid, i.e., if no two adjacent vertices have the same color assigned.
     *
     * @param v_coloring Vertex coloring to check.
     * @return `true` iff no two adjacent vertices in the stored graph have the same color assigned according to the
     * given coloring.
     */
    [[nodiscard]] bool is_valid_vertex_coloring(const vertex_coloring<Graph, Color>& v_coloring) const noexcept
    {
        if (v_coloring.size() != graph.size_vertices())
        {
            return false;
        }

        return std::none_of(graph.begin_vertices(), graph.end_vertices(),
                            [this, &v_coloring](const auto& vp1)
                            {
                                const auto v1 = vp1.first;

                                return static_cast<bool>(
                                    std::any_of(graph.begin_adjacent(v1), graph.end_adjacent(v1),
                                                [&v_coloring, c1 = v_coloring.at(v1)](const auto& v2)
                                                { return c1 == v_coloring.at(v2); }));
                            });
    }
};

}  // namespace detail

/**
 * This function provides an interface to call various vertex coloring algorithms on the given graph. A vertex coloring
 * is the assignment of colors to graph vertices such that no two vertices that share an edge receive the same color. If
 * a graph is colorable with \f$k\f$ colors, the graph is said to be \f$k\f$-colorable. The minimum value of \f$k\f$
 * for a graph is called its chromatic number. To determine the chromatic number of a graph is \f$NP\f$-complete
 * in general. The provided algorithms attempt to get as close to the optimum coloring as possible. However, no
 * heuristic can give an optimality guarantee. If the exact chromatic number is required, the SAT-based engine must be
 * used. This may require exponential runtime in the worst case but is, on average, a lot faster due to the smart
 * traversal of search spaces that SAT solvers provide.
 *
 * See graph_coloring_engine for a list of all supported engines.
 *
 * @note If the `clique_size_color_frequency` parameter is set together with the SAT engine, there is no guarantee that
 * the SAT solver is able to find a valid coloring. In that case, this algorithm falls back to MCS.
 *
 * @tparam Graph Graph type to color.
 * @tparam Color Color type to use.
 * @param graph The graph whose vertices are to be colored.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return An assignment of graph vertices to colors such that no two adjacent vertices share the same color.
 */
template <typename Graph, typename Color = std::size_t>
vertex_coloring<Graph, Color> determine_vertex_coloring(const Graph&                            graph,
                                                        determine_vertex_coloring_params<Graph> ps  = {},
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
