//
// Created by marcel on 02.08.19.
//

#ifndef FICTION_NETWORK_HIERARCHY_H
#define FICTION_NETWORK_HIERARCHY_H


#include "logic_network.h"
#include <itertools.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>


/**
 * This class represents a hierarchy of a logic_network, i.e. a levelizing of the vertices in ranks. Those are
 * determined by a modified depth first search. Several functions can be used to manipulate the network according to
 * the rank information, e.g. inserting auxiliary vertices to balance paths.
 */
class network_hierarchy
{
public:
    /**
     * Levels are numbers of ranks.
     */
    using level = uint32_t;
    /**
     * Standard constructor. Creates a network hierarchy from the given pointer to a logic_network ln. Furthermore, it
     * can be specified whether primary inputs/outputs and constants should be taken into account for all further
     * computations. This cannot be changed later!
     *
     * @param ln Pointer to a logic network to create this hierarchy from. All references to vertices and edges will
     *           be constructed with respect to this network.
     * @param store_original_edges Indicates that for each inserted balance vertex, its original subdivided edge should
     *                             be stored so that it can be accessed later.
     */
    explicit network_hierarchy(logic_network_ptr ln, const bool store_original_edges = true) noexcept;
    /**
     * Returns the height of the hierarchy, i.e. the highest stored level value.
     *
     * @return Highest level number stored.
     */
    level height() const noexcept;
    /**
     * Returns the inverse height of the hierarchy, i.e. the highest stored inverse level value.
     *
     * @return Highest inverse level number stored.
     */
    level inv_height() const noexcept;
    /**
     * Assigns level l to vertex v.
     *
     * @param v Vertex whose level should be assigned.
     * @param l Level to assign to vertex v.
     */
    void set_level(const logic_network::vertex v, const level l) noexcept;
    /**
     * Returns level of given vertex v.
     *
     * @param v Vertex whose level is desired.
     * @return Stored level of v or 0 if no level was assigned yet.
     */
    level get_level(const logic_network::vertex v) const noexcept;
    /**
     * Assigns inverse level l to vertex v.
     *
     * @param v Vertex whose inverse level should be assigned.
     * @param l Inverse level to assign to vertex v.
     */
    void set_inv_level(const logic_network::vertex v, const level l) noexcept;
    /**
     * Returns inverse level of given vertex v.
     *
     * @param v Vertex whose inverse level is desired.
     * @return Stored inverse level of v or 0 if no level was assigned yet.
     */
    level get_inv_level(const logic_network::vertex v) const noexcept;
    /**
     * Subdivides a given edge e and inserts a balance vertex with operation::W in between.
     *
     * @param e Edge to subdivide.
     */
    logic_network::vertex subdivide_edge(const logic_network::edge& e) noexcept;
    /**
     * Returns the original edge of given balance vertex v. Returns std::nullopt if v is not a balance vertex.
     *
     * @return Original edge of balance vertex v or std::nullopt if v is not a balance vertex.
     */
    std::optional<logic_network::edge> get_balance_edge(const logic_network::vertex v) const noexcept;
    /**
     * Uses iterative subdivision to elongate paths which skip levels.
     */
    void balance_paths() noexcept;
    /**
     * Unifies the height of all POs by moving them all to the same highest level. This does not subdivide any edges.
     * A call to balance_paths is necessary for that.
     */
    void unify_output_ranks() noexcept;
    /**
     * Unifies the inverse height of all PIs by moving them all to the same highest inverse level. This does not
     * subdivide any edges.
     */
    void unify_inv_input_ranks() noexcept;

private:
    /**
     * Pointer to the logic_network the hierarchy was constructed for.
     */
    logic_network_ptr network;
    /**
     * Flag to indicate that the original subdivided edge for each balance vertex should be stored.
     */
    const bool store_edges;
    /**
     * Maximum level and inverse level in hierarchy, i.e. height.
     */
    level max_level = 0lu, max_inv_level = 0ul;
    /**
     * Alias for a hashed bidirectional map that assigns levels to vertices and vice versa.
     */
    using level_map = std::unordered_map<logic_network::vertex, level>;
    /**
     * Assigns one level to each vertex.
     */
    level_map l_map{};
    /**
     * Assigns one level to each vertex inversely.
     */
    level_map il_map{};
    /**
     * Maps balance vertices to their corresponding original edges.
     */
    using balance_map = std::unordered_map<logic_network::vertex, logic_network::edge>;
    /**
     * Stores a mapping from balance vertices to corresponding original edges. Useful if one wants to place original
     * edges (e.g. for crossings) and needs access to them.
     */
    balance_map b_map{};
    /**
     * Assigns the given balance vertex v its original edge e. Uses lookup to prevent storing balance vertices as edge
     * sources or targets. That means, should any of e's source or target appear on the left side of the map,
     * they will be substituted to find the original edge accordingly.
     *
     * @param v Balance vertex to store.
     * @param e Original edge v should be assigned to.
     */
    void store_balance_edge(const logic_network::vertex v, const logic_network::edge& e) noexcept;
    /**
     * Creates the hierarchy by levelizing the network using a custom depth first search. Every vertex without
     * predecessors gets level 0 assigned, while every other vertex gets assigned the maximum level of its predecessors.
     */
    void levelize() noexcept;
};

using network_hierarchy_ptr = std::shared_ptr<network_hierarchy>;


#endif //FICTION_NETWORK_HIERARCHY_H
