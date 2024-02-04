//
// Created by Willem Lambooy on 04/02/2024.
//

#ifndef FICTION_SIDB_CLUSTER_HIERARCHY_HPP
#define FICTION_SIDB_CLUSTER_HIERARCHY_HPP

#include "fiction/technology/charge_distribution_surface.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <set>

#include <dataanalysis.h>
#include <linalg.h>

namespace fiction
{

/**
 * An enumeration of cluster linkage methods. The chosen method defines how clusters are merged in the agglomerative
 * clustering procedure, by, e.g., defining an inter-cluster distance to minimize for the cluster to merge.
 */
enum class sidb_cluster_hierarchy_linkage_method
{
    /*
     * Complete linkage takes the maximum distance between nodes in a cluster.
     */
    COMPLETE = 0,
    /*
     * Single linkage takes the minimum distance between nodes in a cluster.
     */
    SINGLE,
    /*
     * Unweighted average linkage takes the average distance between nodes in a cluster, disregarding the number of
     * elements in a node.
     */
    UNWEIGHTED_AVERAGE,
    /*
     * Weighted average linkage takes the average distance between nodes in a cluster, weighing in the number of
     * elements in a node.
     */
    WEIGHTED_AVERAGE,
    /*
     * Known as Ward's method, this type of linkage merges clusters based on a minimum variance measure.
     */
    MINIMUM_VARIANCE
};

struct sidb_cluster_hierarchy_node
{
    std::set<uint64_t>                                          c;
    std::array<std::unique_ptr<sidb_cluster_hierarchy_node>, 2> sub;

    sidb_cluster_hierarchy_node(const std::set<uint64_t>&                                   c_,
                                std::array<std::unique_ptr<sidb_cluster_hierarchy_node>, 2> sub_) :
            c{c_},
            sub{std::move(sub_)}
    {}
};

template <typename Lyt>
sidb_cluster_hierarchy_node sidb_cluster_hierarchy(
    Lyt&                                  lyt,
    sidb_cluster_hierarchy_linkage_method linkage_method = sidb_cluster_hierarchy_linkage_method::MINIMUM_VARIANCE)
{
    if (lyt.num_cells() == 0)
    {
        return sidb_cluster_hierarchy_node{{}, {nullptr, nullptr}};
    }

    charge_distribution_surface<Lyt> charge_lyt{lyt};

    alglib::real_2d_array d;
    d.setlength(static_cast<alglib::ae_int_t>(charge_lyt.num_cells()), 2);
    for (uint64_t i = 0; i < charge_lyt.num_cells(); ++i)
    {
        const auto [x, y] = charge_lyt.get_all_sidb_locations_in_nm()[i];

        d[static_cast<int>(i)][0] = x;
        d[static_cast<int>(i)][1] = y;
    }

    alglib::clusterizerstate s;
    clusterizercreate(s);
    clusterizersetpoints(s, d, 2);                               // Assign data and specify L2 norm (Euclidean metric)
    clusterizersetahcalgo(s, static_cast<int>(linkage_method));  // Set the linkage method (default: Ward's method)
    alglib::ahcreport rep;
    clusterizerrunahc(s, rep);

    std::map<uint64_t, std::unique_ptr<sidb_cluster_hierarchy_node>> nodes{};

    // build hierarchy from N - 1 merges represented in rep.z
    for (int i = 0; i < rep.npoints - 1; ++i)
    {
        const std::array<uint64_t, 2> cs = {static_cast<uint64_t>(rep.z[i][0]), static_cast<uint64_t>(rep.z[i][1])};
        for (uint8_t c = 0; c < 2; ++c)
        {
            // create leaf nodes
            if (cs[c] < charge_lyt.num_cells())
            {
                nodes[cs[c]] = std::make_unique<sidb_cluster_hierarchy_node>(
                    std::set{cs[c]}, std::array<std::unique_ptr<sidb_cluster_hierarchy_node>, 2>{nullptr, nullptr});
            }
        }

        // rep.z assigns each new cluster to N + i
        const uint64_t new_n = charge_lyt.num_cells() + static_cast<uint64_t>(i);

        std::set<uint64_t> set_union{};
        std::set_union(nodes.at(cs[0])->c.cbegin(), nodes.at(cs[0])->c.cend(), nodes.at(cs[1])->c.cbegin(),
                       nodes.at(cs[1])->c.cend(), std::inserter(set_union, set_union.begin()));

        nodes[new_n] = std::make_unique<sidb_cluster_hierarchy_node>(
            set_union, std::array<std::unique_ptr<sidb_cluster_hierarchy_node>, 2>{std::move(nodes.at(cs[0])),
                                                                                   std::move(nodes.at(cs[1]))});
        nodes.erase(cs[0]);
        nodes.erase(cs[1]);
    }

    return std::move(*nodes.cbegin()->second);
};

struct ch_node;
using ch_node_ptr = std::shared_ptr<ch_node>;  // unique_ptr seems to be impossible with std::set?

struct ch_node
{
    std::set<uint64_t>    c;
    std::set<ch_node_ptr> v;

    explicit ch_node(const std::set<uint64_t>& c_, std::set<ch_node_ptr> v_) : c{c_}, v{std::move(v_)} {}

    explicit ch_node(const std::set<ch_node_ptr>& v_)
    {
        if (v_.size() == 1)
        {
            c = (*v_.cbegin())->c;
            v = (*v_.cbegin())->v;
            return;
        }

        for (const ch_node_ptr& h : v_)
        {
            c.insert(h->c.cbegin(), h->c.cend());
        }
        v = v_;
    };
};

ch_node_ptr to_ch_node(const sidb_cluster_hierarchy_node& n)
{
    std::set<ch_node_ptr> v;

    for (uint8_t c = 0; c < 2; ++c)
    {
        if (n.sub[c])
        {
            v.insert(to_ch_node(*n.sub[c]));
        }
    }

    return std::make_shared<ch_node>(n.c, std::move(v));
}

}  // namespace fiction

#endif  // FICTION_SIDB_CLUSTER_HIERARCHY_HPP
