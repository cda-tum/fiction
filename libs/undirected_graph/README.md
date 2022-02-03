# undirected_graph
A simple implementation of an undirected graph in c++ using the stl.

During my work on a group assignment in university I was in need for a graph
container with methods for breadth/depth-first traversal. I quickly found the
implementation of the boost library (http://www.boost.org/doc/libs/1_58_0/libs/graph/doc/)
but I did not want to use a premade solution immediately. As optimal performance
was not required, I decided to develop my own container with corresponding
traversal iterators.

If you have ideas for improvements or find bugs, feel free to notify me about
them!

The project is licensed under the GNU GPL v2 license. Have a look into the
LICENSE file for more information.

## Quickstart

To start using the `undirect_graph` simply include the `.h` files in your
project and instantiate the graph with the corresponding template arguments:
```c++
template <class Key_vertex, class T_vertex,
		  class Key_edge, class T_edge>
class undirected_graph;
```
where `T_vertex`/`T_edge` are the datatypes that are stored at the
vertices/edges  and `Key_vertex`/`Key_edge` are the identifier types used to
retrieve or remove objects from the graph.
The `Key_edge` type has to meet certain criteria, for an easy start use the
supplied `undirected_pair<T>` where `T` would be the `Key_vertex` type of the
graph.
```c++
template <typename T>
class undirected_pair;
```
Most of the time you can use `int` or `size_t` as the `Key_vertex` type and
`undirected_pair<int>` or respectively `undirected_pair<size_t>` as the
`Key_edge` type. If you want to use your own id types (maybe you already have
ids for the objects in your code) make sure to check the requirements some
paragraphs below.

Note: The header files require C++11 to compile.

## Features

The graph allows to store vertex data types with unique ids which are connected
by edge data objects which have in turn unique ids. Naturally the graph is
implemented using templates and therefore every data type is supported.

In most operations the graph behaves similar to the `std::map` container because
(a) it is built using maps and (b) it's what you're probably used to after
working with stl containers. The only difference is that most methods have an
appended `_vertex` or `_edge` to indicate on which objects the method works.

It is possible to:
 - add vertex data with a unique id
 - add edge data by specifying two vertex ids that aren't already connected
 - access the data by reference using the corresponding id
 - remove vertices and egdes by id
 - iterate through all vertices or edges (unordered)
 - iterate through all adjacent vertices of a vertex with a specified id
 - iterate through the graph vertices by using an breadth- or depth-first search
   iterator

Some example code:
```c++
typedef undirected_graph<size_t, std::string, undirected_pair<size_t>, double> graph_type;
graph_type graph;

// Build graph
graph.insert_vertex(1, "Hello world!");
graph.insert_vertex(2, "Some more text.");
graph.insert_vertex(3, "Hi");
graph.insert_edge(1, 2, 44.12);
graph.insert_edge(2, 3, 22.34);

// Print edge data
for(auto it = graph.begin_edges(); it != graph.end_edges(); ++it) {
		std::cout << "Connection" << it->first.a << "to" << it->first.b
				  << "with value:" << it->second << std::endl;
}

breadth_first_iterator<graph_type> bfs(graph, graph.find_vertex(1));
// Iterate through the graph and print the strings
while(!bfs.end()) {
	std::cout << bfs.next()->second << std::endl
}

// Remove a vertex and its edge(s)
graph.erase_vertex(2);
```
You can find more example code in the `main.cpp` in the test subfolder.

## What does this project contain?

The project consist mainly of three parts:
- `undirected_graph` the main component of the graph I've been writing about
  this whole document.
- `undirected_pair` a simple data type which behaves similar to a `std::pair`
  except for the fact that it is just for one data type, pair(a,b) and pair(b,a)
  compare as equal and it's sortable. The sorting order is determined by the
  smaller and bigger elements of the pairs. It's a simple data type to use for
	the edge ids in the graph.
- `graph_search_iterator` is an abstract class that provides an interface for
  graph traversal iterators. Two subclasses are supplied: a breadth-first and a
  depth-first iterator.

## Requirements for the data types

There are no requirements or restrictions on the data types supplied as
`T_vertex` or `T_edge`. However there are some requirements for
the id types.

`Key_vertex` basically has to fulfil the same requirements as a key for an
`std::unordered_set` and a `std::map`. Namely:
- Equality operator `==`
- Strict weak ordering operator `<`
- Specialized `std::hash` functor (see http://stackoverflow.com/a/17017281/929037)

The same applies to the `Key_edge` with some more restrictions:
- Additional constructor with the signature `edge_id(vertex_id a, vertex_id b)`
- Equality operator where `edge_id(a,b) == edge_id(b,a)` (this also affects the
	`<`-operator and hash functor, see below)
- Access to the two connected `vertex_id`s via a public `a` and `b` member

For a data type that implements all these requirements have a look at the
`undirected_pair`. Nevertheless here are the axioms for the `<`-operator of the
`Key_edge`:
```
Let a, b, c, d be of type vertex_id and idA(a,b), idB(c,d) be of type edge_id. Then:
1. min(a,b) < min(c,d) then: (idA < idB)
2. If not 1 and min(c,d) < min(a,b) then: (idB < idA)
3. If not 1 and not 2 and max(a,b) < max(c,d) then: (idA < idB)
4. If not 1 and not 2 and not 3 then: not (idA < idB)
```
