//
// Created by Willem Lambooy on 04/02/2024.
//

#ifndef FICTION_SIDB_CLUSTER_HIERARCHY_HPP
#define FICTION_SIDB_CLUSTER_HIERARCHY_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/utils/hash.hpp"

#include <array>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

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

struct sidb_binary_cluster_hierarchy_node;
using sidb_binary_cluster_hierarchy_node_ptr = std::unique_ptr<sidb_binary_cluster_hierarchy_node>;

struct sidb_binary_cluster_hierarchy_node
{
    std::set<uint64_t>                                    c;
    std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub;

    sidb_binary_cluster_hierarchy_node(const std::set<uint64_t>&                             c_,
                                       std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub_) :
            c{c_},
            sub{std::move(sub_)}
    {}
};

template <typename Lyt>
sidb_binary_cluster_hierarchy_node sidb_cluster_hierarchy(
    Lyt&                                  lyt,
    sidb_cluster_hierarchy_linkage_method linkage_method = sidb_cluster_hierarchy_linkage_method::MINIMUM_VARIANCE)
{
    if (lyt.num_cells() == 0)
    {
        return sidb_binary_cluster_hierarchy_node{{}, {nullptr, nullptr}};
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

    std::map<uint64_t, std::unique_ptr<sidb_binary_cluster_hierarchy_node>> nodes{};

    // build hierarchy from N - 1 merges represented in rep.z
    for (int i = 0; i < rep.npoints - 1; ++i)
    {
        const std::array<uint64_t, 2> cs = {static_cast<uint64_t>(rep.z[i][0]), static_cast<uint64_t>(rep.z[i][1])};
        for (uint8_t c = 0; c < 2; ++c)
        {
            // create leaf nodes
            if (cs[c] < charge_lyt.num_cells())
            {
                nodes[cs[c]] = std::make_unique<sidb_binary_cluster_hierarchy_node>(
                    std::set{cs[c]}, std::array<sidb_binary_cluster_hierarchy_node_ptr, 2>{nullptr, nullptr});
            }
        }

        // rep.z assigns each new cluster to N + i
        const uint64_t new_n = charge_lyt.num_cells() + static_cast<uint64_t>(i);

        std::set<uint64_t> set_union{};
        std::set_union(nodes.at(cs[0])->c.cbegin(), nodes.at(cs[0])->c.cend(), nodes.at(cs[1])->c.cbegin(),
                       nodes.at(cs[1])->c.cend(), std::inserter(set_union, set_union.begin()));

        nodes[new_n] = std::make_unique<sidb_binary_cluster_hierarchy_node>(
            set_union, std::array<sidb_binary_cluster_hierarchy_node_ptr, 2>{std::move(nodes.at(cs[0])),
                                                                             std::move(nodes.at(cs[1]))});
        nodes.erase(cs[0]);
        nodes.erase(cs[1]);
    }

    return std::move(*nodes.cbegin()->second);
};

struct sidb_cluster;
using sidb_cluster_ptr = std::shared_ptr<sidb_cluster>;

struct sidb_cluster_charge_state;
// using sidb_cluster_charge_state_ptr = std::unique_ptr<sidb_cluster_charge_state>;

struct sidb_cluster_charge_state
{
    uint64_t neg_count : 32;
    uint64_t pos_count : 32;

    mutable std::vector<std::vector<std::pair<sidb_cluster_ptr, uint64_t>>> decompositions{};

    explicit sidb_cluster_charge_state() : neg_count{0}, pos_count{0} {}

    explicit sidb_cluster_charge_state(const sidb_charge_state cs) :
            neg_count{cs == sidb_charge_state::NEGATIVE},
            pos_count{cs == sidb_charge_state::POSITIVE}
    {}

    explicit sidb_cluster_charge_state(const uint64_t m) : neg_count{m >> 32ull}, pos_count{(m << 32ull) >> 32ull} {}

    explicit constexpr operator uint64_t() const noexcept
    {
        return (static_cast<uint64_t>(neg_count) << 32ull) | pos_count;
    }

    sidb_cluster_charge_state copy_with_one_less_negative() const noexcept
    {
        assert(neg_count != 0);
        return sidb_cluster_charge_state{((static_cast<uint64_t>(neg_count) - 1ull) << 32ull) | pos_count};
    }

    sidb_cluster_charge_state copy_with_one_less_positive() const noexcept
    {
        assert(pos_count != 0);
        return sidb_cluster_charge_state{(static_cast<uint64_t>(neg_count) << 32ull) | (pos_count - 1ull)};
    }

    void add_charge(const sidb_charge_state cs) noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: neg_count++; break;
            case sidb_charge_state::POSITIVE: pos_count++; break;
            default: break;
        }
    }

    explicit sidb_cluster_charge_state(const std::initializer_list<sidb_charge_state>& charge_states) :
            neg_count{0},
            pos_count{0}
    {
        for (const sidb_charge_state cs : charge_states)
        {
            add_charge(cs);
        }
    }

    inline bool contains_neutral_charge(const uint64_t cluster_size) const noexcept
    {
        return cluster_size > neg_count + pos_count;
    }

    inline bool operator==(const sidb_cluster_charge_state& other) const noexcept
    {
        return static_cast<uint64_t>(*this) == static_cast<uint64_t>(other);
        //        return neg_count == other.neg_count && pos_count == other.pos_count;
    }

    inline std::size_t operator()(const sidb_cluster_charge_state& m) const noexcept
    {
        return static_cast<std::size_t>(static_cast<uint64_t>(m));
    }

    inline sidb_cluster_charge_state& operator+=(const sidb_cluster_charge_state& other) noexcept
    {
        neg_count += other.neg_count;
        pos_count += other.pos_count;
        return *this;
    }

    inline sidb_cluster_charge_state& operator-=(const sidb_cluster_charge_state& other) noexcept
    {
        assert(neg_count >= other.neg_count && pos_count >= other.pos_count);
        neg_count -= other.neg_count;
        pos_count -= other.pos_count;
        return *this;
    }
};

using sidb_cluster_charge_state_space = std::unordered_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;

using potential_bounds            = std::pair<double, double>;
using cluster_charge_state_bounds = std::pair<uint64_t, uint64_t>;

using interaction_bounds = std::pair<potential_bounds, cluster_charge_state_bounds>;

using sidb_clustering = std::set<sidb_cluster_ptr>;

using ordered_eqv_classes = std::vector<std::set<uint64_t>>;

struct ordered_potential_information
{
    bool                                                    b;
    std::shared_ptr<const std::vector<std::vector<double>>> pot_mat{};
    ordered_eqv_classes                                     eqv_classes{};
};

struct sidb_cluster
{
    const uint64_t uid{0};

    std::set<uint64_t> sidbs;
    sidb_clustering    children;
    sidb_cluster_ptr   parent{};

    potential_bounds                       local_pot_bounds{};
    std::map<uint64_t, interaction_bounds> projected_pot_bounds{};
    //    std::map<sidb_cluster_charge_state, potential_bounds> int_pot_bounds{{sidb_cluster_charge_state{}, {0, 0}}};

    std::map<uint64_t, const ordered_potential_information> potential_equivalence_classes{};

    sidb_cluster_charge_state_space charge_space{};

    explicit sidb_cluster(std::set<uint64_t> c, sidb_clustering v, uint64_t uid_) noexcept :
            uid{c.size() == 1 ? *c.cbegin() : std::move(uid_)},
            sidbs{std::move(c)},
            children{std::move(v)}
    {}

    explicit sidb_cluster(const sidb_clustering& v) noexcept
    {
        if (v.size() == 1)
        {
            sidbs    = (*v.cbegin())->sidbs;
            children = (*v.cbegin())->children;
            return;
        }

        for (const sidb_cluster_ptr& h : v)
        {
            sidbs.insert(h->sidbs.cbegin(), h->sidbs.cend());
        }

        children = v;
    };

    void initialize_singleton_cluster_charge_space() noexcept
    {
        assert(sidbs.size() == 1);
        for (const sidb_charge_state cs : sidb_charge_state_iterator{})
        {
            charge_space.emplace(sidb_cluster_charge_state{cs});
        }
    }

    //    bool has_positive_charge_in_state_space() const noexcept
    //    {
    //        return std::find(charge_space.cbegin(), charge_space.cend(),
    //                         [](const sidb_cluster_charge_state& m)
    //                         { return m.count(sidb_charge_state::POSITIVE) != 0; }) != charge_space.cend();
    //    }

    inline bool operator==(const sidb_cluster& other) const noexcept
    {
        return uid == other.uid;
    }

    inline bool operator<(const sidb_cluster& other) const noexcept
    {
        return uid < other.uid;
    }
};

sidb_cluster_ptr to_unique_sidb_cluster(const sidb_binary_cluster_hierarchy_node& n, uint64_t& uid)
{
    sidb_clustering children;

    for (uint8_t c = 0; c < 2; ++c)
    {
        if (n.sub[c])
        {
            children.insert(to_unique_sidb_cluster(*n.sub[c], uid));
        }
    }

    const sidb_cluster_ptr parent = std::make_shared<sidb_cluster>(n.c, std::move(children), uid);
    uid++;

    if (parent->children.empty())
    {
        return parent;
    }

    for (uint8_t c = 0; c < 2; (*std::next(parent->children.begin(), c++))->parent = parent)
        ;

    return parent;
}

sidb_cluster_ptr to_sidb_cluster(const sidb_binary_cluster_hierarchy_node& n)
{
    uint64_t uid = n.c.size();
    return to_unique_sidb_cluster(n, uid);
}

}  // namespace fiction

#endif  // FICTION_SIDB_CLUSTER_HIERARCHY_HPP
