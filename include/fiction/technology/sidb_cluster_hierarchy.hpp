//
// Created by Willem Lambooy on 04/02/2024.
//

#ifndef FICTION_SIDB_CLUSTER_HIERARCHY_HPP
#define FICTION_SIDB_CLUSTER_HIERARCHY_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/utils/hash.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
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
                                       std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub_) noexcept :
            c{c_},
            sub{std::move(sub_)}
    {}
};

template <typename Lyt>
static sidb_binary_cluster_hierarchy_node
sidb_cluster_hierarchy(Lyt& lyt, sidb_cluster_hierarchy_linkage_method linkage_method =
                                     sidb_cluster_hierarchy_linkage_method::MINIMUM_VARIANCE) noexcept
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

static constexpr inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept;

struct sidb_cluster_projector_state
{
    const sidb_cluster_ptr& cluster;
    const uint64_t          multiset_conf;

    template <sidb_charge_state cs>
    constexpr inline bool contains_cs() const noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: return static_cast<bool>(multiset_conf >> 32);
            case sidb_charge_state::POSITIVE: return static_cast<bool>(multiset_conf & 0xFFFFFFFF);
            default: return (multiset_conf >> 32) + (multiset_conf & 0xFFFFFFFF) < get_cluster_size(cluster);
        }
    }
};

template <sidb_charge_state cs>
static constexpr inline bool get_onto_cs_pruned_for_sidb_ix(const sidb_cluster_ptr& c, const uint64_t sidb_ix) noexcept;

struct sidb_cluster_receptor_state
{
    const sidb_cluster_ptr& cluster;
    const uint64_t          sidb_ix;
};

using sidb_cluster_charge_state_composition = std::vector<sidb_cluster_projector_state>;

struct sidb_cluster_charge_state
{
    uint64_t neg_count : 32;
    uint64_t pos_count : 32;

    mutable std::vector<sidb_cluster_charge_state_composition> compositions{};

    explicit sidb_cluster_charge_state() noexcept : neg_count{0}, pos_count{0} {}

    explicit sidb_cluster_charge_state(const sidb_charge_state cs) noexcept :
            neg_count{cs == sidb_charge_state::NEGATIVE},
            pos_count{cs == sidb_charge_state::POSITIVE}
    {}

    explicit sidb_cluster_charge_state(const uint64_t m) noexcept :
            neg_count{m >> 32ull},
            pos_count{(m << 32ull) >> 32ull}
    {}

    explicit constexpr operator uint64_t() const noexcept
    {
        return (static_cast<uint64_t>(neg_count) << 32ull) | pos_count;
    }

    constexpr void add_charge(const sidb_charge_state cs) noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: neg_count++; break;
            case sidb_charge_state::POSITIVE: pos_count++; break;
            default: break;
        }
    }

    constexpr void remove_charge(const sidb_charge_state cs) noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE:
                assert(neg_count > 0);
                neg_count--;
                break;
            case sidb_charge_state::POSITIVE:
                assert(pos_count > 0);
                pos_count--;
                break;
            default: break;
        }
    }

    explicit sidb_cluster_charge_state(const std::initializer_list<sidb_charge_state>& charge_states) noexcept :
            neg_count{0},
            pos_count{0}
    {
        for (const sidb_charge_state cs : charge_states)
        {
            add_charge(cs);
        }
    }

    constexpr inline bool operator==(const sidb_cluster_charge_state& other) const noexcept
    {
        return static_cast<uint64_t>(*this) == static_cast<uint64_t>(other);
    }

    constexpr inline std::size_t operator()(const sidb_cluster_charge_state& m) const noexcept
    {
        return std::hash<uint64_t>{}(static_cast<uint64_t>(m));
    }

    constexpr inline sidb_cluster_charge_state& operator+=(const sidb_cluster_charge_state& other) noexcept
    {
        neg_count += other.neg_count;
        pos_count += other.pos_count;
        return *this;
    }

    constexpr inline sidb_cluster_charge_state& operator-=(const sidb_cluster_charge_state& other) noexcept
    {
        assert(neg_count >= other.neg_count && pos_count >= other.pos_count);
        neg_count -= other.neg_count;
        pos_count -= other.pos_count;
        return *this;
    }
};

enum class bound_calculation_mode : uint8_t
{
    LOWER = 0,
    UPPER
};

static constexpr inline bool is_potential_bound_bottom(const double bound) noexcept
{
    return std::isinf(bound);
}

template <bound_calculation_mode bound>
static constexpr inline bool potential_bound_meet_is_weaker(const double pot_bound, const double candidate) noexcept
{
    if constexpr (bound == bound_calculation_mode::LOWER)
    {
        return pot_bound > candidate;
    }
    else
    {
        return pot_bound < candidate;
    }
}

template <bound_calculation_mode bound>
static constexpr inline void take_meet_of_potential_bounds(double& pot_bound, const double other_pot_bound) noexcept
{
    if (potential_bound_meet_is_weaker<bound>(pot_bound, other_pot_bound))
    {
        pot_bound = other_pot_bound;
    }
}

struct potential_projection
{
    double   V{0.0};
    uint64_t M{0};

    explicit potential_projection() noexcept = default;

    explicit potential_projection(const double init, const sidb_charge_state cs) noexcept :
            V{init},
            M{static_cast<uint64_t>(sidb_cluster_charge_state{cs})}
    {}

    constexpr inline bool operator<(const potential_projection& other) const noexcept
    {
        return V < other.V || (V == other.V && M < other.M);
    }

    template <bound_calculation_mode bound>
    static constexpr inline potential_projection bottom() noexcept
    {
        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return potential_projection{-std::numeric_limits<double>::infinity(), sidb_charge_state::NONE};
        }
        else
        {
            return potential_projection{std::numeric_limits<double>::infinity(), sidb_charge_state::NONE};
        }
    }

    template <bound_calculation_mode bound>
    static constexpr inline potential_projection top() noexcept
    {
        return potential_projection{-bottom<bound>().V, sidb_charge_state::NONE};
    }

    constexpr inline potential_projection& operator+=(const potential_projection& other) noexcept
    {
        V += other.V;
        M += other.M;
        return *this;
    }
};

struct potential_projection_orders
{
    using pot_proj_order        = std::set<potential_projection>;
    using pot_proj_order_onto_sidb_ix = std::unordered_map<uint64_t, std::set<potential_projection>>;

    // ix : onto_cs
    // 0 : NEG ; 1 : NEUT ; 2 : POS
    std::array<pot_proj_order_onto_sidb_ix, 3> orders{};

    explicit potential_projection_orders() noexcept = default;

    template <sidb_charge_state cs>
    static inline pot_proj_order_onto_sidb_ix singleton_self_projection_onto_cs(const uint64_t sidb_ix) noexcept
    {
        return pot_proj_order_onto_sidb_ix{{sidb_ix, pot_proj_order{potential_projection{0.0, cs}}}};
    }

    explicit potential_projection_orders(const uint64_t sidb_ix) noexcept :
            orders{singleton_self_projection_onto_cs<sidb_charge_state::NEGATIVE>(sidb_ix),
                   singleton_self_projection_onto_cs<sidb_charge_state::NEUTRAL>(sidb_ix),
                   singleton_self_projection_onto_cs<sidb_charge_state::POSITIVE>(sidb_ix)}
    {}

    static inline pot_proj_order_onto_sidb_ix initial_singleton_proj_pot_order(const double   inter_sidb_pot,
                                                                               const uint64_t onto_sidb_ix) noexcept
    {
        return pot_proj_order_onto_sidb_ix{
            {onto_sidb_ix, pot_proj_order{potential_projection{-inter_sidb_pot, sidb_charge_state::POSITIVE},
                                          potential_projection{0.0, sidb_charge_state::NEUTRAL},
                                          potential_projection{inter_sidb_pot, sidb_charge_state::NEGATIVE}}}};
    }

    explicit potential_projection_orders(const double inter_sidb_pot, const uint64_t onto_sidb_ix) noexcept :
            orders{initial_singleton_proj_pot_order(inter_sidb_pot, onto_sidb_ix),
                   initial_singleton_proj_pot_order(inter_sidb_pot, onto_sidb_ix),
                   initial_singleton_proj_pot_order(inter_sidb_pot, onto_sidb_ix)}
    {}

    template <sidb_charge_state cs>
    static constexpr inline uint8_t cs_ix() noexcept
    {
        return static_cast<uint8_t>(charge_state_to_sign(cs) + 1);
    }

    template <sidb_charge_state cs>
    constexpr inline bool onto_cs_pruned() const noexcept
    {
        return orders[cs_ix<cs>()].empty();
    }

    template <sidb_charge_state cs>
    constexpr inline void prune_cs() noexcept
    {
        orders[cs_ix<cs>()].clear();
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    constexpr inline potential_projection get(const uint64_t sidb_ix) const noexcept
    {
        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return *orders[cs_ix<cs>()].at(sidb_ix).cbegin();
        }
        else
        {
            return *orders[cs_ix<cs>()].at(sidb_ix).crbegin();
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    inline potential_projection get_next(const uint64_t sidb_ix) const noexcept
    {
        assert(!onto_cs_pruned<cs>());

        const uint64_t bound_m = get<bound, cs>(sidb_ix).M;

        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return *std::find_if(orders[cs_ix<cs>()].at(sidb_ix).cbegin(), orders[cs_ix<cs>()].at(sidb_ix).cend(),
                                 [&](const potential_projection& pp) { return pp.M != bound_m; });
        }
        else
        {
            return *std::find_if(orders[cs_ix<cs>()].at(sidb_ix).crbegin(), orders[cs_ix<cs>()].at(sidb_ix).crend(),
                                 [&](const potential_projection& pp) { return pp.M != bound_m; });
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    potential_projection get_pot_proj_for_m_conf(const uint64_t m_conf, const uint64_t sidb_ix) const noexcept
    {
        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return *std::find_if(orders[cs_ix<cs>()].at(sidb_ix).cbegin(), orders[cs_ix<cs>()].at(sidb_ix).cend(),
                                 [&](const potential_projection& pp) { return pp.M == m_conf; });
        }
        else
        {
            return *std::prev(std::find_if(orders[cs_ix<cs>()].at(sidb_ix).crbegin(),
                                           orders[cs_ix<cs>()].at(sidb_ix).crend(),
                                           [&](const potential_projection& pp) { return pp.M == m_conf; })
                                  .base(),
                              1);
        }
    }

    template <sidb_charge_state cs>
    void remove_m_conf(const uint64_t m_conf, const uint64_t sidb_ix) noexcept
    {
        assert(!onto_cs_pruned<cs>());
        for (pot_proj_order::const_iterator it = orders[cs_ix<cs>()].at(sidb_ix).cbegin();
             it != orders[cs_ix<cs>()].at(sidb_ix).cend();)
        {
            it->M == m_conf ? it = orders[cs_ix<cs>()][sidb_ix].erase(it) : ++it;
        }
    }

    //    template <sidb_charge_state cs>
    //    constexpr inline void remove_onto_m_conf_if_present(const uint64_t sidb_ix) noexcept
    //    {
    //        if (onto_cs_pruned<cs>())  // || orders[cs_ix<cs>()].count(onto_m) == 0)
    //        {
    //            return;
    //        }
    //
    //        orders[cs_ix<cs>()].erase(sidb_ix);
    //    }

    template <sidb_charge_state cs>
    constexpr inline void add(const potential_projection& pp, const uint64_t sidb_ix) noexcept
    {
        orders[cs_ix<cs>()][sidb_ix].emplace(pp);
    }

    template <sidb_charge_state cs>
    inline void add_multiple(const pot_proj_order_onto_sidb_ix& pps_onto_sidb_ix) noexcept
    {
        for (const auto& [sidb_ix, pps] : pps_onto_sidb_ix)
        {
            orders[cs_ix<cs>()][sidb_ix].insert(pps.cbegin(), pps.cend());
        }
    }
};

using sidb_cluster_charge_state_space = std::unordered_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;

static constexpr inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept;

struct sidb_cluster_ptr_hash
{
    constexpr inline std::size_t operator()(const sidb_cluster_ptr& c) const noexcept
    {
        return std::hash<uint64_t>{}(get_unique_cluster_id(c));
    }
};

using sidb_clustering = std::unordered_set<sidb_cluster_ptr, sidb_cluster_ptr_hash>;

struct sidb_cluster
{
    using uid_t   = uint64_t;
    using sidb_ix = uint64_t;

    const uid_t uid{0};

    std::set<sidb_ix> sidbs;
    sidb_clustering   children;
    sidb_cluster_ptr  parent{};

    std::unordered_map<uid_t, potential_projection_orders> pot_projs{};
    std::unordered_map<sidb_ix, std::array<double, 4>>     recv_ext_pot_bounds{};

    sidb_cluster_charge_state_space charge_space{};

    explicit sidb_cluster(std::set<sidb_ix> c, sidb_clustering v, uid_t uid_) noexcept :
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

    template <bound_calculation_mode bound, sidb_charge_state cs>
    static constexpr inline uint8_t cs_bound_ix() noexcept
    {
        if constexpr (cs == sidb_charge_state::NEGATIVE)
        {
            return 0;
        }
        else if constexpr (cs == sidb_charge_state::POSITIVE)
        {
            return 1;
        }
        else if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }

    void initialize_singleton_cluster_charge_space(const sidb_ix i, const double loc_pot_min,
                                                   const double loc_pot_max) noexcept
    {
        assert(sidbs.size() == 1);
        for (const sidb_charge_state cs : sidb_charge_state_iterator{})
        {
            charge_space.emplace(sidb_cluster_charge_state{cs});
        }

        recv_ext_pot_bounds[i][cs_bound_ix<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>()] = loc_pot_min;
        recv_ext_pot_bounds[i][cs_bound_ix<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>()] = loc_pot_max;
        recv_ext_pot_bounds[i][cs_bound_ix<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>()]  = loc_pot_min;
        recv_ext_pot_bounds[i][cs_bound_ix<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>()]  = loc_pot_max;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    constexpr inline double get_recv_ext_pot_bound(const sidb_ix i) noexcept
    {
        return recv_ext_pot_bounds.at(i)[cs_bound_ix<bound, onto_cs>()];
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    constexpr inline void set_recv_ext_pot_bound(const sidb_ix i, const double new_bound) noexcept
    {
        recv_ext_pot_bounds[i][cs_bound_ix<bound, onto_cs>()] = new_bound;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    constexpr inline void update_recv_ext_pot_bound(const sidb_ix i, const double diff) noexcept
    {
        recv_ext_pot_bounds[i][cs_bound_ix<bound, onto_cs>()] += diff;
    }

    constexpr inline bool operator==(const sidb_cluster& other) const noexcept
    {
        return uid == other.uid;
    }

    constexpr inline bool operator<(const sidb_cluster& other) const noexcept
    {
        return uid < other.uid;
    }
};

static constexpr inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept
{
    return c->uid;
}

static constexpr inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept
{
    return c->sidbs.size();
}

static sidb_cluster_ptr to_unique_sidb_cluster(const sidb_binary_cluster_hierarchy_node& n, uint64_t& uid)
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
