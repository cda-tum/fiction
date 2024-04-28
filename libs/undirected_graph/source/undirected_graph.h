/*
	Header file for undirected_graph container
    Copyright (C) 2015  Fabian Löschner

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef UNDIRECTEDGRAPH
#define UNDIRECTEDGRAPH

#include <cstddef>
#include <utility>
#include <forward_list>
#include <unordered_map>

/**
 * @brief Undirected graph with vertices and edges
 *
 * A basic implementation of undirected graphs containing vertices connected by unique edges.
 * There may be multiple vertices with the same value but only one edge between two vertices.
 * The container stores data elements for the vertices and edges. The vertices and edges are identified
 * and sorted by ids which have to be unique in the container.
 * @tparam Key_vertex Type for the ids of vertices. Has to support the default comparison operators.
 * @tparam T_vertex The type that should be used for the data elements at the vertices.
 * @tparam Key_edge The type for the ids of edges. Has to provide a constructor taking two vertex ids as well as
 * @tparam T_edge The type that should be used for the data elments at the edges
 * support of the default comparison operators where the order of the two vertices is not important.
 * Furthermore it has to provide access to the two vertex ids with a public a and b member variable.
 */
template <class Key_vertex, class T_vertex, class Key_edge, class T_edge>
class undirected_graph
{
public:
	typedef T_vertex vertex_data_type;
	typedef Key_vertex vertex_id_type;
	typedef T_edge edge_data_type;
	typedef Key_edge edge_id_type;

private:	
	// TODO: Replace forward_list with map because of slow erasing. Use unique_ptr to store them.

	//! Type of the current graph
	typedef undirected_graph<vertex_id_type, vertex_data_type, edge_id_type, edge_data_type> graph_type;
	//! Type for the container of vertex data in the graph
	typedef std::unordered_map<vertex_id_type, vertex_data_type> vertex_container;
	//! Type for the container of edge data in the graph
	typedef std::unordered_map<edge_id_type, edge_data_type> edge_container;
	//! Type for the adjacency lists of the graph
	typedef std::forward_list<vertex_id_type> adjacency_list;
	//! Type for the container of the adjacency lists in the graph
	typedef std::unordered_map<vertex_id_type, adjacency_list> adjacency_container;

	vertex_container vertices;			/*!< Container for the vertex data */
	edge_container edges;				/*!< Container for the edge data */
	adjacency_container adjacency;		/*!< Container for adjacency lists */

public:
	// TODO: Erase by iterator

	//! Iterator for vertices in the graph. It behaves like a std::unordered_map iterator with it->first being the vertex id and it->second being the mapped vertex data.
	typedef typename vertex_container::iterator graph_vertex_iterator;
	//! Const iterator for vertices in the graph. It behaves like a std::unordered_map iterator with it->first being the vertex id and it->second being the mapped vertex data.
	typedef typename vertex_container::const_iterator graph_vertex_const_iterator;
	//! Iterator for edges in the graph. It behaves like a std::unordered_map iterator with it->first being the edge id and it->second being the mapped edge data.
	typedef typename edge_container::iterator graph_edge_iterator;
	//! Const iterator for edges in the graph. It behaves like a std::unordered_map iterator with it->first being the edge id and it->second being the mapped edge data.
	typedef typename edge_container::const_iterator graph_edge_const_iterator;
	//! Iterator for adjacencent vertices in the graph. It behaves like a std::forward_list iterator dereferencing to the id of the adjacent vertex.
	typedef typename adjacency_list::iterator graph_adjacency_iterator;
	//! Const iterator for adjacencent vertices in the graph. It behaves like a std::forward_list iterator dereferencing to the id of the adjacent vertex.
	typedef typename adjacency_list::const_iterator graph_adjacency_const_iterator;

	/**
	 * @brief Make edge id
	 *
	 * Creates the edge id for the edge between the two specified vertex ids. The two vertex ids
	 * don't have to be present in the graph.
	 * @param a First vertex id.
	 * @param b Second vertex id.
	 * @return Edge id for the edge between the two vertex ids.
	 */
	static edge_id_type make_edge_id(const vertex_id_type& a, const vertex_id_type &b)
	{
		return edge_id_type(a,b);
	}

	/**
	 * @brief Test whether graph is empty
	 *
	 * Returns whether the graph is empty (i.e. whether there are no vertices).
	 * This function does not modify the graph in any way. To clear the content
	 * of a graph container, see undirected_graph::clear.
	 * @return true if the there no vertices, false otherwise.
	 */
	bool empty() const
	{
		return vertices.empty();
	}

	/**
	 * @brief Return vertex container size
	 *
	 * Returns the number of vertices in the undirected_graph container.
	 * @return The number of vertices in the graph.
	 */
	size_t size_vertices() const
	{
		return vertices.size();
	}

	/**
	 * @brief Return edge container size
	 *
	 * Returns the number of edges in the undirected_graph container.
	 * @return The number of edges in the graph.
	 */
	size_t size_edges() const
	{
		return edges.size();
	}

	/**
	 * @brief Access vertex
	 *
	 * Returns a reference to the mapped data of the vertex identified with the specified id.
	 * If it does not match the id of any vertex in the container, the function throws an out_of_range exception.
	 * @param id The id of the vertex whose mapped data is accessed.
	 * @return A reference to the mapped data of the vertex.
	 */
	vertex_data_type& at_vertex(const vertex_id_type& id)
	{
		return vertices.at(id);
	}

	const vertex_data_type& at_vertex(const vertex_id_type& id) const { return vertices.at(id); }

	/**
	 * @brief Access edge
	 *
	 * Returns a reference to the mapped data of the edge identified with the specified id.
	 * If it does not match the id of any edge in the container, the function throws an out_of_range exception.
	 * @param id The id of the edge whose mapped data is accessed.
	 * @return A reference to the mapped data of the edge.
	 */
	edge_data_type& at_edge(const edge_id_type& id)
	{
		return edges.at(id);
	}

	const edge_data_type& at_edge(const edge_id_type& id) const { return edges.at(id); }

	/**
	 * @brief Get iterator to vertex
	 *
	 * Searches the container for a vertex with an id equivalent to the one specified and returns an iterator
	 * to it if found, otherwise it returns an iterator to undirected_graph::end_vertices.
	 * Two ids are considered equivalent if the container's comparison object returns false reflexively
	 * (i.e., no matter the order in which the ids are passed as arguments).
	 * @param id Id to be searched for.
	 * @return An iterator to the vertex, if a vertex with specified id is found, or undirected_graph::end_vertices otherwise.
	 */
	graph_vertex_iterator find_vertex(const vertex_id_type& id)
	{
		return vertices.find(id);
	}

	graph_vertex_const_iterator find_vertex(const vertex_id_type& id) const { return vertices.find(id); }

	/**
	 * @brief Get iterator to edge
	 *
	 * Searches the container for an edge with an id equivalent to the one specified and returns an iterator
	 * to it if found, otherwise it returns an iterator to undirected_graph::end_edges.
	 * Two ids are considered equivalent if the container's comparison object returns false reflexively
	 * (i.e., no matter the order in which the ids are passed as arguments).
	 * @param id Id to be searched for.
	 * @return An iterator to the edge, if an edge with specified id is found, or undirected_graph::end_edges otherwise.
	 */
	graph_edge_iterator find_edge(const edge_id_type& id)
	{
		return edges.find(id);
	}

	graph_edge_const_iterator find_edge(const edge_id_type& id) const { return edges.find(id); }

	/**
	 * @brief Return iterator to beginning of vertices
	 *
	 * Returns an iterator referring to the first vertex in the graph container.
	 * If the container is empty, the returned iterator value shall not be dereferenced.
	 * @return An iterator to the first vertex in the container.
	 */
	graph_vertex_iterator begin_vertices() { return vertices.begin(); }
	graph_vertex_const_iterator begin_vertices() const { return vertices.begin(); }

	/**
	 * @brief Return iterator to end of vertices
	 *
	 * Returns an iterator referring to the past-the-end vertex in the graph container.
	 * It does not point to any element, and thus shall not be dereferenced.
	 * If the container is empty, this function returns the same as undirected_graph::begin_vertices.
	 * @return An iterator to the past-the-end vertex in the container.
	 */
	graph_vertex_iterator end_vertices() { return vertices.end(); }
	graph_vertex_const_iterator end_vertices() const { return vertices.end(); }

	/**
	 * @brief Return iterator to beginning of edges
	 *
	 * Returns an iterator referring to the first edge in the graph container.
	 * If the container is empty, the returned iterator value shall not be dereferenced.
	 * @return An iterator to the first edge in the container.
	 */
	graph_edge_iterator begin_edges() { return edges.begin(); }
	graph_edge_const_iterator begin_edges() const { return edges.begin(); }

	/**
	 * @brief Return iterator to end of edges
	 *
	 * Returns an iterator referring to the past-the-end edges in the graph container.
	 * It does not point to any element, and thus shall not be dereferenced.
	 * If the container is empty, this function returns the same as undirected_graph::begin_edges.
	 * @return An iterator to the past-the-end edge in the container.
	 */
	graph_edge_iterator end_edges() { return edges.end(); }
	graph_edge_const_iterator end_edges() const { return edges.end(); }

	/**
	 * @brief Return iterator to beginning of adjacent vertices
	 *
	 * Returns an iterator referring to the first adjacent vertex of the specified vertex.
	 * If the adjacency list is empty, the returned iterator value shall not be dereferenced.
	 * It behaves like a forward_list iterator.
	 * @return An iterator to the first adjacent vertex in the container.
	 */
	graph_adjacency_iterator begin_adjacent(const vertex_id_type& vertex)
	{
		return adjacency.at(vertex).begin();
	}
	graph_adjacency_const_iterator begin_adjacent(const vertex_id_type& vertex) const { return adjacency.at(vertex).begin(); }

	/**
	 * @brief Return iterator to end of adjacent vertices
	 *
	 * Returns an iterator referring to the past-the-end adjacent vertex to the specified vertex.
	 * It does not point to any element, and thus shall not be dereferenced.
	 * If the adjacency list is empty, this function returns the same as undirected_graph::begin_adjacent.
	 * It behaves like a forward_list iterator.
	 * @return An iterator to the past-the-end adjacent vertex of the specified vertex.
	 */
	graph_adjacency_iterator end_adjacent(const vertex_id_type& vertex)
	{
		return adjacency.at(vertex).end();
	}
	graph_adjacency_const_iterator end_adjacent(const vertex_id_type& vertex) const { return adjacency.at(vertex).end(); }

	/**
	 * @brief Clear content
	 *
	 * Removes all elements from the graph container (which are destroyed), leaving the container with a size of 0.
	 */
	void clear()
	{
		vertices.clear();
		edges.clear();
		adjacency.clear();
	}

	/**
	 * @brief Insert vertex
	 *
	 * Inserts a new vertex to the graph, effectively increasing the container size by one. Multiple vertices with the same
	 * value may exist in one graph but ids have to be unique.
	 * @param vertex_id The id of the vertex.
	 * @param vertex_data Value to be copied to the inserted vertex.
	 * @return Returns a pair with an iterator to the inserted vertex and a bool indicating whether the vertex was newly inserted or not.
	 */
	std::pair<graph_vertex_iterator, bool> insert_vertex(const vertex_id_type& vertex_id, const vertex_data_type& vertex_data)
	{
		auto pair = vertices.emplace(vertex_id, vertex_data);
		adjacency.emplace(vertex_id, adjacency_list());
		return pair;
	}

	/**
	 * @brief Erase vertex
	 *
	 * Removes a single vertex from the graph container. This effectively reduces the vertex container
	 * size by one and the vertex data is destroyed. Also all edge data connected to this vertex
	 * is destroyed. WARNING: Linear in the number of adjacency entries of connected vertices.
	 * @param vertex_id Id of the vertex that should be removed.
	 * @return Returns whether the vertex was removed.
	 */
	bool erase_vertex(const vertex_id_type& vertex_id)
	{
		// Try to remove vertex
		auto count = vertices.erase(vertex_id);
		if(count == 0) return false;

		/*
		 * Check if there is a self-adjacency entry because this
		 * would invalidate iterators in the second loop
		 */
		if(edges.count(edge_id_type(vertex_id, vertex_id)) == 1) {
			// Delete edge
			edges.erase(edge_id_type(vertex_id, vertex_id));

			// Find and delete adjacency entry
			auto& adj_list = adjacency.at(vertex_id);
			auto it_prev = adj_list.before_begin();
			for(auto it = adj_list.begin(); it != adj_list.end(); ++it) {
				if(*it == vertex_id) {
					adj_list.erase_after(it_prev);
					break;
				}
				it_prev = it;
			}
		}

		// Loop through all adjacency entries
		auto& adj_list = adjacency.at(vertex_id);
		for(auto it = adj_list.begin(); it != adj_list.end(); ++it) {
			const auto other_id = *it;

			// Remove edge data
			edges.erase(edge_id_type(vertex_id, other_id));

			// Delete reverse adjacency entries
			auto& other_adj_list = adjacency.at(other_id);
			auto it_prev = other_adj_list.before_begin();
			for(auto other_it = other_adj_list.begin(); other_it != other_adj_list.end(); ++other_it) {
				if(*other_it == vertex_id) {
					other_adj_list.erase_after(it_prev);
					break;
				}
				it_prev = other_it;
			}
		}

		// Remove adjacency list of the vertex
		adjacency.erase(vertex_id);

		return true;
	}

	/**
	 * @brief Erase edge
	 *
	 * Removes a single edge from the graph container. This effectively reduces the edge container
	 * size by one and the edge data is destroyed. WARNING: Linear in the number of adjacent vertices
	 * of the two connected vertices.
	 * @param edge_id Id of the edge that should be removed.
	 * @return Returns whether the edge was removed.
	 */
	bool erase_edge(const edge_id_type& edge_id)
	{
		// Try to remove edge
		auto count = edges.erase(edge_id);
		if(count == 0) return false;

		// Remove adjacency of b from a
		auto& adjacency_list_a = adjacency.at(edge_id.a);		
		auto it_prev_a = adjacency_list_a.before_begin();
		for(auto it = adjacency_list_a.begin(); it != adjacency_list_a.end(); ++it) {
			if(*it == edge_id.b) {
				adjacency_list_a.erase_after(it_prev_a);
				break;
			}
			it_prev_a = it;
		}

		// Remove adjacency of a from b
		auto& adjacency_list_b = adjacency.at(edge_id.b);
		auto it_prev_b = adjacency_list_b.before_begin();
		for(auto it = adjacency_list_b.begin(); it != adjacency_list_b.end(); ++it) {
			if(*it == edge_id.a) {
				adjacency_list_b.erase_after(it_prev_b);
				break;
			}
			it_prev_b = it;
		}

		return true;
	}

	/**
	 * @brief Add edge between two vertices
	 *
	 * This method inserts a new edge data element to the graph connecting two vertices.
	 * If the edge already exists the edge will not be modified.
	 * @param vertex_a Id of the first vertex to connect.
	 * @param vertex_b Id of the second vertex to connect.
	 * @param edge_data	The data element for the edge between the two vertices.
	 * @return Returns a pair with an iterator to the inserted edge and a bool indicating whether the edge was inserted or not.
	 */
	std::pair<graph_edge_iterator, bool> insert_edge(const vertex_id_type& vertex_a, const vertex_id_type& vertex_b, edge_data_type edge_data) {
		// Check if vertices are in the graph
		if(vertices.count(vertex_a) == 0 || vertices.count(vertex_b) == 0) {
			return std::make_pair(edges.end(), false);
		}

		// Try to place new edge
		auto pair = edges.emplace(edge_id_type(vertex_a, vertex_b), edge_data);

		// Create adjacency entries if the edge was added
		if(pair.second) {
			adjacency.at(vertex_a).push_front(vertex_b);
			if(vertex_a != vertex_b) adjacency.at(vertex_b).push_front(vertex_a);
		}

		// Return iterator
		return pair;
	}
};

#endif // UNDIRECTEDGRAPH
