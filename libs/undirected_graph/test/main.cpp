/*
	Main testing file for the undirected_graph
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

#include <iostream>
#include <string>
#include <cassert>
#include <chrono>
#include <random>
#include <memory>

#include "../source/undirected_graph.h"
#include "../source/undirected_pair.h"
#include "../source/graph_search_iterator.h"

void msg(const std::string& string) {
	std::cout << string << "... ";
}

void ok() {
	std::cout << "ok." << std::endl;
}

int run_pair_test() {

	msg("Generating pair");
	typedef undirected_pair<int> upair;
	upair pair1(5,10);
	ok();

	msg("Checking values");
	assert(pair1.smaller_element() == 5);
	assert(pair1.bigger_element() == 10);
	assert(pair1.other_element(5) == 10);
	assert(pair1.other_element(10) == 5);
	assert(pair1.a == 5 || pair1.b == 5);
	assert(pair1.a == 10 || pair1.b == 10);
	ok();

	msg("Generating second pair");
	undirected_pair<int> pair2(-22,0);
	std::cout << "ok." << std::endl;

	msg("Checking values");
	assert(pair2.smaller_element() == -22);
	assert(pair2.bigger_element() == 0);
	assert(pair2.other_element(-22) == 0);
	assert(pair2.other_element(0) == -22);
	assert(pair2.a == -22 || pair2.b == -22);
	assert(pair2.a == 0 || pair2.b == 0);
	ok();

	msg("Checking comparison");
	assert(pair2 < pair1);
	assert(!(pair1 < pair2));
	assert(pair1 > pair2);
	assert(!(pair2 > pair1));
	assert(pair1 != pair2);
	assert(pair2 != pair1);
	assert(!(pair2 == pair1));
	assert(!(pair1 == pair2));
	ok();

	msg("Modifying values");
	pair1.a = -22;
	pair1.b = 0;
	assert(pair1.a == -22);
	assert(pair2.b == 0);
	ok();

	msg("Checking comparison");
	assert(pair1 == pair2);
	assert(pair2 == pair1);
	assert(!(pair1 < pair2));
	assert(!(pair2 < pair1));
	assert(!(pair1 > pair2));
	assert(!(pair1 > pair2));
	assert(!(pair1 != pair2));
	assert(!(pair2 != pair1));
	ok();

	msg("Checking more comparisons");
	assert(upair(1,3) < upair(2,4));
	assert(upair(2,3) < upair(2,4));
	assert(upair(2,2) < upair(2,4));
	ok();

	return 0;
}

int run_graph_vertex_test() {
	// Initialize random number generator
	std::default_random_engine random(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	// Number of test entries
	const size_t n = 100000;

	// Set graph type
	typedef undirected_graph<size_t, std::string, undirected_pair<size_t>, std::string> graph_type;
	graph_type graph;
	msg("Checking if graph is empty");
	assert(graph.empty());
	ok();

	// Vector that stores strings for comparison
	std::vector<std::string> strings;
	strings.reserve(n);

	// Insert vertices into graph
	std::cout << "Inserting " << std::to_string(n) << " values... ";
	for(size_t i = 0; i < n; i++) {
		std::string string(std::to_string(random()));
		strings.push_back(string);
		auto pair = graph.insert_vertex(i, string);
		assert(pair.second);
		assert(string.compare(pair.first->second) == 0);
	}
	ok();

	// Try inserting vertices already present
	msg("Trying to reinsert some values");
	for(size_t i = (n/4); i < (n/2); i++) {
		auto pair = graph.insert_vertex(i, "Already there");
		assert(!pair.second);
		assert(strings[i].compare(pair.first->second) == 0);
	}
	ok();

	// Check values
	msg("Checking inserted values");
	{
		size_t i = 0;
		for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
			assert(strings.at(it->first).compare(it->second) == 0);
			i++;
		}
		// Check value count
		assert(i == graph.size_vertices());
	}
	ok();

	// Remove vertices
	msg("Removing vertices");
	for(size_t i = n/2; i < n; i++) {
		graph.erase_vertex(i);
	}
	ok();

	// Check if entries were removed
	msg("Checking removed values");
	{
		size_t i = 0;
		for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
			assert(strings.at(it->first).compare(it->second) == 0);
			i++;
		}
		// Check value count
		assert(i == graph.size_vertices());
		assert(i == n/2);
	}
	ok();

	// Clear
	msg("Clearing graph");
	graph.clear();
	assert(graph.size_vertices() == 0);
	assert(graph.empty());
	ok();

	return 0;
}

int run_graph_edge_test()
{
	// Initialize random number generator
	std::default_random_engine random(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	// Number of test entries
	const size_t n = 500;

	typedef undirected_graph<size_t, std::string, undirected_pair<size_t>, std::string> graph_type;
	graph_type graph;

	// Vector that stores strings for comparison
	std::vector<std::string> stringsVertices;
	stringsVertices.reserve(n);

	msg("Adding some vertices");
	for(size_t i = 0; i < n; i++) {
		std::string string(std::to_string(random()));
		stringsVertices.push_back(string);
		auto pair = graph.insert_vertex(i, string);
		assert(pair.second);
		assert(string.compare(pair.first->second) == 0);
	}
	ok();

	std::map<undirected_pair<size_t>, std::string> stringsEdges;

	msg("Adding edges");
	for(size_t i = 0; i < n; i++) {
		for(size_t j = i; j < n; j++) {
			std::string string(std::to_string(random()));
			stringsEdges.insert(std::make_pair(graph.make_edge_id(i,j),string));
			auto pair = graph.insert_edge(i, j, string);
			assert(pair.second);
			assert(string.compare(pair.first->second) == 0);
		}
	}
	ok();

	msg("Checking adjacency");
	// Loop through all vertices
	for(size_t i = 0; i < n; i++) {
		// Counter for adjacent entries
		size_t c = 0;
		// Loop through adjacent vertices
		for(auto it = graph.begin_adjacent(i); it != graph.end_adjacent(i); ++it) {
			// Id of the adjacent vertex
			const size_t j = *it;

			// Check data of adjacent vertex
			assert(graph.at_vertex(*it).compare(stringsVertices.at(*it)) == 0);

			// Check edge data
			auto id = graph_type::make_edge_id(i,j);
			assert(graph.at_edge(id).compare(stringsEdges.at(id)) == 0);

			c++;
		}

		// Check number of adjacent entries
		assert(c == n);
	}
	ok();

	msg("Removing some edges");
	for(size_t i = 0; i < n; i+=2) {
		for(size_t j = 1; j < n; j+=2) {
			graph.erase_edge(graph_type::make_edge_id(i,j));
		}
	}
	ok();

	msg("Checking adjacency after removing");
	for(size_t i = 0; i < n; i++) {
		for(auto it = graph.begin_adjacent(i); it != graph.end_adjacent(i); ++it) {
			// Check edge data
			auto id = graph_type::make_edge_id(i,*it);
			assert(graph.at_edge(id).compare(stringsEdges.at(id)) == 0);
		}
	}
	ok();

	msg("Removing some vertices");
	for(size_t i = 0; i < n; i+=2) {
		assert(graph.erase_vertex(i));
	}
	ok();

	msg("Checking adjacency after removing");
	for(auto it = graph.begin_vertices(); it != graph.end_vertices(); it++) {
		for(auto itAdj = graph.begin_adjacent(it->first); itAdj != graph.end_adjacent(it->first); ++itAdj) {
			// Check edge data
			auto id = graph_type::make_edge_id(it->first,*itAdj);
			assert(graph.at_edge(id).compare(stringsEdges.at(id)) == 0);
		}
	}
	ok();

	return 0;
}

int run_iterator_test()
{
	// Number of test entries
	const int m = 10;
	const double n = std::pow(2, m+1)-1;

	// Set graph type
	typedef undirected_graph<size_t, std::string, undirected_pair<size_t>, std::string> graph_type;
	graph_type graph;

	msg("Inserting vertices");
	for(size_t i = 0; i < n; i++) {
		graph.insert_vertex(i,std::to_string(i));
	}
	ok();

	{
		msg("Inserting edges");
		graph.insert_edge(0,1,"0");
		graph.insert_edge(0,2,"1");
		size_t counter = 2;
		for(size_t i = 1; i < m; i++) {
			size_t entries = static_cast<size_t>(std::pow(2, i));
			size_t start = entries - 1;
			size_t next = start + entries;
			for(size_t j = start; j < start + entries; j++) {
				graph.insert_edge(j, next++, std::to_string(counter));
				counter++;
				graph.insert_edge(j, next++, std::to_string(counter));
				counter++;
			}
		}
		ok();
	}

	/* Because of the forward_list containers the iterator will traverse the tree
	 * in the order 0 -> 2->1 -> 6->5->4->3 -> ...
	 */
	{
		msg("Checking breadth first iterator");
		breadth_first_iterator<graph_type> bfs(graph, graph.find_vertex(0));
		size_t next = 0;
		size_t level = 0;
		size_t start = 0;
		while(!bfs.end()) {
			assert(bfs.next()->first == next);

			if(next == start) {
				level++;
				start = static_cast<size_t>(std::pow(2,level) - 1);
				next = (start)*2;
			} else {
				next--;
			}
		}
		ok();
	}

	// TODO: Check depth first iterator

	return 0;
}

int main()
{
	#ifdef NDEBUG
		std::cout << "WARNING: This is no debug build! Asserts (i.e. the tests) won't work!" << std::endl;
		std::cout << "Please rebuild without NDEBUG!" << std::endl;
		return 1;
	#endif

	int state = 0;

	std::cout << "Running pair tests..." << std::endl;
	state += run_pair_test();
	std::cout << std::endl;

	std::cout << "Running graph vertex tests..." << std::endl;
	state += run_graph_vertex_test();
	std::cout << std::endl;

	std::cout << "Running graph edge tests..." << std::endl;
	state += run_graph_edge_test();
	std::cout << std::endl;

	std::cout << "Running iterator tests..." << std::endl;
	state += run_iterator_test();
	std::cout << std::endl;

	return state;
}
