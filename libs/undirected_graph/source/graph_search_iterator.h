/*
	Header file for graph_search_iterator for the undirected_graph container
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

#ifndef GRAPHSEARCHITERATOR
#define GRAPHSEARCHITERATOR

#include <map>
#include <queue>
#include <stack>
#include <unordered_set>

/**
 * @brief Base class for search iterators in a graph
 *
 * This class provides the base for search iterators on graphs with a container of
 * currently waiting vertices (/vertex iterators) and a list of already visited
 * vertices. Subclasses have to reimplement the next() function to increment the 'iterator'.
 * @tparam graph Type of the graph that should be traversed. Required to get the necessary iterator and data types.
 * @tparam waiting_container Type of the waiting container that should be used. (stack/queue)
 */
template <typename graph, typename waiting_container>
class graph_search_iterator
{
public:
	/**
	 * @brief Constructs a new graph search iterator
	 *
	 * Constructs a graph search iterator object, initializing it with the supplied values.
	 * @param g A reference to the graph that should be traversed.
	 * @param start Vertex iterator to the node that should be used as a starting point.
	 */
	graph_search_iterator(const graph& g, typename graph::graph_vertex_const_iterator start) : m_graph(&g)
	{
		if(!g.empty()) {
			m_waiting.push(start);
			m_discovered.emplace(start->first);
		}
	}

	/**
	 * @brief Check whether search has finished
	 *
	 * Returns whether the search alghorithms iterated through all accessible vertices.
	 * @return true if the search finished.
	 */
	bool end() const
	{
		return m_waiting.empty();
	}

	/**
	 * @brief Returns number of discovered vertices
	 *
	 * Returns the number of vertices the search algorithm already traveresed.
	 * @return Number of visited vertices.
	 */
	size_t discovered() const
	{
		return m_discovered.size();
	}

	/**
	 * @brief Returns iterator to next vertex in the search order
	 *
	 * This method should be reimplemented in subclasses to increment the search iterator
	 * to the next the element depending on the search algorithm
	 * @return Iterator to the next element
	 */
	virtual typename graph::graph_vertex_const_iterator next() = 0;

protected:
	//! Queue for waiting vertex iterators
	waiting_container m_waiting;
	//! The vertices that were already visited
	std::unordered_set<typename graph::vertex_id_type> m_discovered;
	//! The graph this iterator traverses
	const graph* m_graph;
};

/**
 * @brief Breadth first iterator
 *
 * This iterator traverses an undirected_graph using the breadth first algortihm.
 * @tparam graph Type of the graph that should be traversed.
 */
template <typename graph>
class breadth_first_iterator : public graph_search_iterator<graph, std::queue<typename graph::graph_vertex_const_iterator>>
{
public:
	/**
	 * @brief Constructs a new BFS iterator
	 *
	 * Constructs a breadth first search iterator object, initializing it with the supplied values.
	 * @param g A reference to the graph that should be traversed.
	 * @param start Vertex iterator to the node that should be used as a starting point.
	 */
	breadth_first_iterator(const graph& g, typename graph::graph_vertex_const_iterator start)
		: graph_search_iterator<graph, std::queue<typename graph::graph_vertex_const_iterator>>(g, start) {}

	/**
	 * @brief Returns iterator to the next vertex
	 *
	 * This method increments the iterator using the BFS algorithm and returns
	 * an iterator to the next element.
	 * @return Iterator to the next element
	 */
	typename graph::graph_vertex_const_iterator next()
	{
		auto next = this->m_waiting.front();
		this->m_waiting.pop();

		for(auto it = this->m_graph->begin_adjacent(next->first); it != this->m_graph->end_adjacent(next->first); ++it) {
			if(this->m_discovered.count(*it) == 0) {
				this->m_waiting.push(this->m_graph->find_vertex(*it));
				this->m_discovered.emplace(*it);
			}
		}

		return next;
	}
};

/**
 * @brief Depth first iterator
 *
 * This iterator traverses an undirected_graph using the depth first algortihm.
 * @tparam graph Type of the graph that should be traversed.
 */
template <typename graph>
class depth_first_iterator : public graph_search_iterator<graph, std::stack<typename graph::graph_vertex_const_iterator>>
{
public:
	/**
	 * @brief Constructs a new DFS iterator
	 *
	 * Constructs a depth first search iterator object, initializing it with the supplied values.
         * @param g A reference to the graph that should be traversed.
	 * @param start Vertex iterator to the node that should be used as a starting point.
	 */
	depth_first_iterator(const graph& g, typename graph::graph_vertex_const_iterator start)
		: graph_search_iterator<graph, std::stack<typename graph::graph_vertex_const_iterator>>(g, start) {}

	/**
	 * @brief Returns iterator to the next vertex
	 *
	 * This method increments the iterator using the DFS algorithm and returns
	 * an iterator to the next element.
	 * @return Iterator to the next element
	 */
	typename graph::graph_vertex_const_iterator next()
	{
		auto next = this->m_waiting.top();
		this->m_waiting.pop();

		for(auto it = this->m_graph->begin_adjacent(next->first); it != this->m_graph->end_adjacent(next->first); ++it) {
			if(this->m_discovered.count(*it) == 0) {
				this->m_waiting.push(this->m_graph->find_vertex(*it));
				this->m_discovered.emplace(*it);
			}
		}

		return next;
	}
};

#endif // GRAPHSEARCHITERATOR
