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

template <typename Graph, typename Color>
class vertex_coloring : public std::unordered_map<typename Graph::vertex_id_type, Color>
{};

enum class graph_coloring_engine
{
    MCS,
    DSATUR,
    LMXRLF,   // randomized
    TABUCOL,  // k-coloring
    SAT,
};

struct determine_vertex_coloring_params
{
    graph_coloring_engine engine = graph_coloring_engine::MCS;

    std::size_t k_color_value = 0;

    bool verify_coloring_after_computation = false;
};

struct determine_vertex_coloring_stats
{
    std::size_t chromatic_number{0};

    std::optional<bool> coloring_verified = std::nullopt;
};

namespace detail
{

template <typename Graph, typename Color>
class graph_coloring_impl
{
  public:
    graph_coloring_impl(const Graph& g, const determine_vertex_coloring_params& p,
                        determine_vertex_coloring_stats& st) :
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
    const Graph& graph;

    const determine_vertex_coloring_params ps;

    determine_vertex_coloring_stats& pst;

    using brian_crites_graph    = std::map<std::string, std::vector<std::string>>;
    using brian_crites_coloring = std::map<std::string, int>;

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

    template <typename Graph_ = Graph,
              typename        = std::enable_if_t<!std::is_same_v<typename Graph_::vertex_id_type, std::string>>>
    std::string convert_node_index(const typename Graph::vertex_id_type& node) const noexcept
    {
        return std::to_string(node);
    }

    brian_crites_graph translate_to_brian_crites_graph(const Graph& g) noexcept
    {
        brian_crites_graph translated_graph{};

        // iterate over all edges e of the original graph
        std::for_each(g.begin_edges(), g.end_edges(),
                      [this, &translated_graph](const auto& e_pair)
                      {
                          const auto e = e_pair.first;

                          // add an edge in the brian_crites_graph that leads from 'source' of e to 'target' of e
                          translated_graph[convert_node_index(e.a)].push_back(convert_node_index(e.b));
                          // add an edge in the brian_crites_graph that leads from 'target' of e to 'source' of e
                          translated_graph[convert_node_index(e.b)].push_back(convert_node_index(e.a));
                      });

        return translated_graph;
    }

    vertex_coloring<Graph, Color> translate_to_vertex_coloring(const brian_crites_coloring& bc_coloring) const noexcept
    {
        vertex_coloring<Graph, Color> v_coloring{};

        std::for_each(bc_coloring.cbegin(), bc_coloring.cend(),
                      [this, &v_coloring](const auto& c_pair)
                      { v_coloring[convert_node_index(c_pair.first)] = static_cast<Color>(c_pair.second); });

        return v_coloring;
    }
};

}  // namespace detail

template <typename Graph, typename Color = std::size_t>
vertex_coloring<Graph, Color> determine_vertex_coloring(const Graph& graph, determine_vertex_coloring_params ps = {},
                                                        determine_vertex_coloring_stats* pst = nullptr)
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
