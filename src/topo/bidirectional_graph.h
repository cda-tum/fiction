//
// Created by marcel on 04.01.17.
//

#ifndef FICTION_BIDIRECTIONAL_GRAPH_H
#define FICTION_BIDIRECTIONAL_GRAPH_H

#include "range.h"
#include "grid_topology.h"
#include <iostream>
#include <iterator>
#include <utility>
#include <unordered_map>
#include <cmath>
#include <chrono>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/optional.hpp>
#include <boost/functional/hash.hpp>
#include <boost/random/linear_congruential.hpp>

/* definition of basic boost::graph properties */
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };
enum graph_properties_t { graph_properties };

namespace boost
{
    BOOST_INSTALL_PROPERTY(vertex, properties);
    BOOST_INSTALL_PROPERTY(edge, properties);
    BOOST_INSTALL_PROPERTY(graph, properties);
}

/**
 * A template class representing a bidirectional graph using the Graph library by Boost.
 * It can be enriched by properties for the vertices and edges.
 *
 * @tparam VERTEX_PROPERTIES Desired properties for each vertex.
 * @tparam EDGE_PROPERTIES Desired properties for each edge.
 * @tparam GRAPH_PROPERTIES Desired properties for the graph.
 */
template <typename VERTEX_PROPERTIES, typename EDGE_PROPERTIES, typename GRAPH_PROPERTIES>
class bidirectional_graph
{
    /**
     * Graph data structure as an adjacency list. A set is used as edge container to avoid parallel edges.
     * Vertices are stored in a vector due to performance reasons. The graph is bi-directional to provide
     * iterators both in and against propagational direction.
     */
    using graph_container = boost::adjacency_list<
            boost::vecS,           // edge container
            boost::vecS,           // vertex container
            boost::bidirectionalS, // bi-directional graph
            boost::property<vertex_properties_t, VERTEX_PROPERTIES>,
            boost::property<edge_properties_t, EDGE_PROPERTIES>,
            boost::property<graph_properties_t, GRAPH_PROPERTIES>
        >;

protected:
    /**
     * Stored graph container. All operations are performed on this object.
     */
    graph_container graph;

    // ************************************************************
    // ************************* Aliases **************************
    // ************************************************************

public:
    using edge_t   = typename graph_container::edge_descriptor;
    using vertex_t = typename graph_container::vertex_descriptor;

protected:
    using edges_t    = std::vector<edge_t>;
    using vertices_t = std::vector<vertex_t>;

    using num_edges_t    = typename graph_container::edges_size_type;
    using num_vertices_t = typename graph_container::vertices_size_type;
    using degree_t       = typename graph_container::degree_size_type;

    using edge_iter          = typename graph_container::edge_iterator;
    using out_edge_iter      = typename graph_container::out_edge_iterator;
    using in_edge_iter       = typename graph_container::in_edge_iterator;
    using vertex_iter        = typename graph_container::vertex_iterator;
    using adjacency_iter     = typename graph_container::adjacency_iterator;
    using inv_adjacency_iter = typename graph_container::inv_adjacency_iterator;

    using edge_index_t   = std::size_t;
    using vertex_index_t = std::size_t;

    
public:
    // ************************************************************
    // *********************** Constructors ***********************
    // ************************************************************

    /**
     * Default standard constructor.
     */
    bidirectional_graph() = default;
    /**
     * Initializes a graph with n nodes.
     *
     * @param n Number of nodes to initialize the graph with.
     */
    explicit bidirectional_graph(const num_vertices_t n) noexcept : graph(n) {}
    /**
     * Copy constructor.
     *
     * @param g Graph to be copied.
     */
    bidirectional_graph(const bidirectional_graph& g) noexcept : graph(g.get_graph()) {}
    /**
     * Move constructor.
     *
     * @param g Graph to be moved.
     */
    bidirectional_graph(bidirectional_graph&& g) noexcept : graph(std::move(g.get_graph())) {}
    /**
     * Default destructor.
     */
    virtual ~bidirectional_graph() = default;


    // ************************************************************
    // ************************ Structure *************************
    // ************************************************************

    /**
     * Adds a vertex to the graph that should have the given properties.
     *
     * @param prop Properties for the vertex.
     * @return Newly inserted vertex.
     */
    vertex_t add_vertex(const VERTEX_PROPERTIES& prop = boost::no_property{}) noexcept
    {
        vertex_t v = boost::add_vertex(graph);
        properties(v) = prop;

        return v;
    }
    /**
     * Removes the given vertex from the graph.
     *
     * @param v Vertex to be removed.
     */
    void remove_vertex(const vertex_t v)
    {
        clear_vertex(v, graph);
        boost::remove_vertex(v, graph);
    }
    /**
     * Adds an edge to the graph that is defined by two given vertices and properties.
     *
     * @param v1 Source vertex.
     * @param v2 Target vertex.
     * @param prop Desired edge properties.
     * @return Newly inserted edge.
     */
    edge_t add_edge(const vertex_t v1, const vertex_t v2, const EDGE_PROPERTIES& prop = boost::no_property{}) noexcept
    {
        edge_t added_edge = boost::add_edge(v1, v2, graph).first;
        properties(added_edge) = prop;

        return added_edge;
    }
    /**
     * Removes the given edge from the graph.
     *
     * @param e Edge to be removed.
     */
    void remove_edge(const edge_t& e)
    {
        boost::remove_edge(e, graph);
    }


    // ************************************************************
    // ************************ Properties ************************
    // ************************************************************

    /**
     * Returns the properties of the given vertex.
     *
     * @param v The vertex whose properties should be returned.
     * @return Properties of vertex v.
     */
    VERTEX_PROPERTIES& properties(const vertex_t v) noexcept
    {
        typename boost::property_map<graph_container, vertex_properties_t>::type param = get(vertex_properties, graph);
        return param[v];
    }
    /**
     * Returns the properties of the given vertex.
     *
     * @param v The vertex whose properties should be returned.
     * @return Properties of vertex v.
     */
    const VERTEX_PROPERTIES& properties(const vertex_t v) const noexcept
    {
        typename boost::property_map<graph_container, vertex_properties_t>::const_type param = get(vertex_properties, graph);
        return param[v];
    }
    /**
     * Returns the properties of the given edge.
     *
     * @param e The edge whose properties should be returned.
     * @return Properties of edge e.
     */
    EDGE_PROPERTIES& properties(const edge_t& e) noexcept
    {
        typename boost::property_map<graph_container, edge_properties_t>::type param = get(edge_properties, graph);
        return param[e];
    }
    /**
     * Returns the properties of the given edge.
     *
     * @param e The edge whose properties should be returned.
     * @return Properties of edge e.
     */
    const EDGE_PROPERTIES& properties(const edge_t& e) const noexcept
    {
        typename boost::property_map<graph_container, edge_properties_t>::const_type param = get(edge_properties, graph);
        return param[e];
    }
    /**
     * Returns the graph's properties.
     *
     * @return Properties of the graph.
     */
    GRAPH_PROPERTIES& properties() noexcept
    {
        return get_property(graph, graph_properties);
    }
    /**
     * Returns the graph's properties.
     *
     * @return Properties of the graph.
     */
    const GRAPH_PROPERTIES& properties() const noexcept
    {
        return get_property(graph, graph_properties);
    }


    // ************************************************************
    // ************************* Getters **************************
    // ************************************************************

    /**
     * Returns the graph container itself, i.e. the adjacency list.
     *
     * @return Graph container.
     */
    const graph_container& get_graph() const noexcept
    {
        return graph;
    }
    /**
     * Returns a range of all vertices of the stored graph.
     *
     * @return All vertices.
     */
    range_t<vertex_iter> get_vertices() const noexcept
    {
        return range_t<vertex_iter>{boost::vertices(graph)};
    }
    /**
     * Returns a range of all adjacent vertices to the given vertex v, i.e. all vertices u for which v -> u holds.
     *
     * @param v Vertex whose adjacencies are desired.
     * @return All adjacent vertices to v.
     */
    range_t<adjacency_iter> get_adjacent_vertices(const vertex_t v) const noexcept
    {
        return range_t<adjacency_iter>{boost::adjacent_vertices(v, graph)};
    }
    /**
     * Returns a range of all inversely adjacent vertices to the given vertex v, i.e. all vertices u for which v <- u holds.
     *
     * @param v Vertex whose inverse adjacencies are desired.
     * @return All inversely adjacent vertices to v.
     */
    range_t<inv_adjacency_iter> get_inv_adjacent_vertices(const vertex_t v) const noexcept
    {
        return range_t<inv_adjacency_iter>{boost::inv_adjacent_vertices(v, graph)};
    }
    /**
     * Returns a range containing the edges of the stored graph.
     *
     * @return Range including the graph's edges.
     */
    range_t<edge_iter> get_edges() const noexcept
    {
        return range_t<edge_iter>{boost::edges(graph)};
    }
    /**
     * Returns the edge from given vertex v1 to given vertex v2 if there exists one.
     *
     * @param v1 Source vertex.
     * @param v2 Target vertex.
     * @return Optional containing edge from v1 to v2.
     */
    boost::optional<edge_t> get_edge(const vertex_t v1, const vertex_t v2) const noexcept
    {
        auto e = boost::edge(v1, v2, graph);
        if (!e.second)
            return boost::none;

        return e.first;
    }
    /**
     * Returns a range containing the outgoing edges of the given vertex.
     *
     * @param v Vertex whose outgoing edges are desired.
     * @return Range including the outgoing edges of vertex v.
     */
    range_t<out_edge_iter> get_out_edges(const vertex_t v) const noexcept
    {
        return range_t<out_edge_iter>{boost::out_edges(v, graph)};
    }
    /**
     * Returns a range containing the ingoing edges of the given vertex.
     *
     * @param v Vertex whose ingoing edges are desired.
     * @return Range including the ingoing edges of vertex v.
     */
    range_t<in_edge_iter> get_in_edges(const vertex_t v) const noexcept
    {
        return range_t<in_edge_iter>{boost::in_edges(v, graph)};
    }
    /**
     * Returns the source vertex of the given edge.
     *
     * @param e Edge whose source vertex is desired.
     * @return Source vertex of edge e.
     */
    vertex_t get_source(const edge_t& e) const noexcept
    {
        return boost::source(e, graph);
    }
    /**
     * Returns the target vertex of the given edge.
     *
     * @param e Edge whose target vertex is desired.
     * @return Target vertex of edge e.
     */
    vertex_t get_target(const edge_t& e) const noexcept
    {
        return boost::target(e, graph);
    }
    /**
     * Returns the number of edges in the graph.
     *
     * @return Number of edges.
     */
    num_edges_t get_edge_count() const noexcept
    {
        return boost::num_edges(graph);
    }
    /**
     * Returns the number of vertices in the graph.
     *
     * @return Number of vertices
     */
    num_vertices_t get_vertex_count() const noexcept
    {
        return boost::num_vertices(graph);
    }
    /**
     * Returns the out degree of the given vertex in the graph.
     *
     * @param v Vertex whose out degree is desired.
     * @return Out degree of v.
     */
    degree_t get_out_degree(const vertex_t v) const noexcept
    {
        return boost::out_degree(v, graph);
    }
    /**
     * Returns the in degree of the given vertex in the graph.
     *
     * @param v Vertex whose in degree is desired.
     * @return In degree of v.
     */
    degree_t get_in_degree(const vertex_t v) const noexcept
    {
        return boost::in_degree(v, graph);
    }
    /**
     * Returns a stored vertex_index for the given vertex_t. May lead to unexpected behavior when
     * property_vertex_index_map and the vertices are out of sync.
     *
     * @param v Vertex whose stored index is desired.
     * @return Stored index of vertex v.
     */
    vertex_index_t get_index(const vertex_t v) const
    {
        auto param = get(boost::vertex_index, graph);
        return param[v];
    }
    /**
     * Sorts the graph topologically. Throws a not_a_dag exception if
     * the graph is not directed and acyclic.
     *
     * @return Vector of vertices in reversed topological order.
     */
    vertices_t topological_sort() const noexcept
    {
        vertices_t topo_order;
        boost::topological_sort(graph, std::back_inserter(topo_order));

        return topo_order;
    }
//    /**
//     * Determines whether the stored graph is planar i.e. whether it does not contain a Kuratowski subgraph.
//     *
//     * @return True, iff the stored graph is planar.
//     */
//    bool is_planar() const noexcept
//    {
//        return boyer_myrvold_planarity_test(*graph);
//    }


    // ************************************************************
    // ************************* Printing *************************
    // ************************************************************

    /**
     * Writes a standard graph representation as GraphViz format to the given
     * std::ostream object.
     *
     * @param os Stream object to write graph representation into.
     */
    void write_graphviz(std::ostream& os = std::cout)
    {
        boost::write_graphviz(os, graph);
    }


    // ************************************************************
    // ************************ Operators *************************
    // ************************************************************

    /**
     * Overloaded assignment operator.
     *
     * @param rhs Graph to replace the stored one.
     * @return Reference to this object holding the overridden graph container.
     */
    bidirectional_graph& operator=(const bidirectional_graph& rhs) noexcept
    {
        graph = rhs.graph;
        return *this;
    }
    /**
     * Overloaded move assignment operator.
     *
     * @param rhs Graph to replace the stored one.
     * @return Reference to this object holding the overridden graph container.
     */
    bidirectional_graph& operator=(bidirectional_graph&& rhs) noexcept
    {
        graph = std::move(rhs.graph);
        return *this;
    }
};

#endif //FICTION_BIDIRECTIONAL_GRAPH_H
