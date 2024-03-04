//
// Created by Willem Lambooy on 04/02/2024.
//

#ifndef FICTION_SIDB_CLUSTER_HIERARCHY_HPP
#define FICTION_SIDB_CLUSTER_HIERARCHY_HPP

// #define DEBUG_MODE

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/utils/hash.hpp"

#ifndef DEBUG_MODE
#include <btree.h>
#include <phmap.h>
#endif

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <dataanalysis.h>
#include <linalg.h>

#ifdef DEBUG_MODE
#include <set>
#include <unordered_map>
#include <unordered_set>
#endif

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

/*
 * The struct used to store a binary cluster hierarchy that may be used to store the result of the hierarchical
 * clustering returned by ALGLIB functionality.
 */
struct sidb_binary_cluster_hierarchy_node
{

#ifdef DEBUG_MODE
    std::set<uint64_t> c;
#else
    phmap::flat_hash_set<uint64_t> c;
#endif

    std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub;

#ifdef DEBUG_MODE
    sidb_binary_cluster_hierarchy_node(const std::set<uint64_t>& c_,
#else
    sidb_binary_cluster_hierarchy_node(const phmap::flat_hash_set<uint64_t>& c_,
#endif
                                       std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub_) noexcept :
            c{c_},
            sub{std::move(sub_)}
    {}
};

/*
 * This function performs the ALGLIB agglomerative clustering algorithm for a given SiDB layout. By default, the cluster
 * are created by a minimal positional variance heuristic, also known as Ward's method.
 */
template <typename Lyt>
static sidb_binary_cluster_hierarchy_node
sidb_cluster_hierarchy(Lyt& lyt, sidb_cluster_hierarchy_linkage_method linkage_method =
                                     sidb_cluster_hierarchy_linkage_method::MINIMUM_VARIANCE) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

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

#ifdef DEBUG_MODE
    std::unordered_map<uint64_t, std::unique_ptr<sidb_binary_cluster_hierarchy_node>> nodes{};
#else
    phmap::flat_hash_map<uint64_t, std::unique_ptr<sidb_binary_cluster_hierarchy_node>> nodes{};
#endif

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
#ifdef DEBUG_MODE
                    std::set<uint64_t>{cs[c]},
#else
                    phmap::flat_hash_set<uint64_t>{cs[c]},
#endif
                    std::array<sidb_binary_cluster_hierarchy_node_ptr, 2>{nullptr, nullptr});
            }
        }

        // rep.z assigns each new cluster to N + i
        const uint64_t new_n = charge_lyt.num_cells() + static_cast<uint64_t>(i);

#ifdef DEBUG_MODE
        std::set<uint64_t> set_union{};
#else
        phmap::flat_hash_set<uint64_t> set_union{};
#endif

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

/*
 * A receptor state pairs the potential recepting cluster with the identifier of the SiDB
 */
struct sidb_cluster_receptor_state
{
    const sidb_cluster_ptr& cluster;
    const uint64_t          sidb_ix;
};

static constexpr inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept;

/*
 * A projector state pairs the potential projecting cluster with the associated multiset charge configuration.
 */
struct sidb_cluster_projector_state
{
    const sidb_cluster_ptr& cluster;
    const uint64_t          multiset_conf;

    template <sidb_charge_state cs>
    constexpr inline uint64_t get_count() const noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: return multiset_conf >> 32;
            case sidb_charge_state::POSITIVE: return multiset_conf & 0xFFFFFFFF;
            default: return get_cluster_size(cluster) - (multiset_conf >> 32) - (multiset_conf & 0xFFFFFFFF);
        }
    }
};

enum class bound_direction : uint8_t
{
    LOWER = 0,
    UPPER
};

template <bound_direction bound>
static constexpr inline double potential_bound_top() noexcept
{
    if constexpr (bound == bound_direction::LOWER)
    {
        return std::numeric_limits<double>::infinity();
    }
    else if constexpr (bound == bound_direction::UPPER)
    {
        return -std::numeric_limits<double>::infinity();
    }
}

template <bound_direction bound>
static constexpr inline void take_meet_of_potential_bounds(double& a, const double b) noexcept
{
    if constexpr (bound == bound_direction::LOWER)
    {
        a = std::min(a, b);
    }
    else if constexpr (bound == bound_direction::UPPER)
    {
        a = std::max(a, b);
    }
}

static constexpr inline uint64_t get_singleton_sidb_ix(const sidb_cluster_ptr& c) noexcept;

static constexpr inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept;

#ifdef DEBUG_MODE
using intra_cluster_potential_bounds = std::map<uint64_t, std::array<double, 2>>;
#else
using intra_cluster_potential_bounds = phmap::flat_hash_map<uint64_t, std::array<double, 2>>;
#endif

struct sidb_cluster_state
{
    const sidb_cluster_projector_state proj_st;
    intra_cluster_potential_bounds     internal_pot_bounds{};

    explicit sidb_cluster_state(const sidb_cluster_ptr& singleton, const uint64_t multiset_conf,
                                [[maybe_unused]] bool is_singleton) noexcept :
            proj_st{singleton, multiset_conf},
            internal_pot_bounds{{get_singleton_sidb_ix(singleton), std::array{0.0, 0.0}}}
    {}

    explicit sidb_cluster_state(const sidb_cluster_ptr& c, const uint64_t multiset_conf) noexcept :
            proj_st{c, multiset_conf}
    {}

    template <bound_direction bound>
    constexpr inline double get_pot_bound(const uint64_t sidb_ix) const noexcept
    {
        return internal_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound)];
    }

    constexpr inline void set_pot_bounds(const uint64_t sidb_ix, const double min, const double max) noexcept
    {
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::LOWER)] = min;
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::UPPER)] = max;
    }

    constexpr inline void update_pot_bounds(const uint64_t sidb_ix, const double min_diff,
                                            const double max_diff) noexcept
    {
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::LOWER)] += min_diff;
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::UPPER)] += max_diff;
    }

    constexpr inline bool operator==(const sidb_cluster_state& other) const noexcept
    {
        return get_unique_cluster_id(proj_st.cluster) == get_unique_cluster_id(other.proj_st.cluster);
    }

    constexpr inline bool operator!=(const sidb_cluster_state& other) const noexcept
    {
        return !(*this == other);
    }
};

using sidb_cluster_state_composition = std::vector<sidb_cluster_state>;

using sidb_cluster_state_ptr = std::unique_ptr<sidb_cluster_state>;
using sidb_clustering_state  = std::vector<sidb_cluster_state_ptr>;

struct sidb_cluster_charge_state
{
    uint64_t neg_count : 32;
    uint64_t pos_count : 32;

    mutable std::vector<sidb_cluster_state_composition> compositions{};

    explicit sidb_cluster_charge_state() noexcept : neg_count{0}, pos_count{0} {}

    explicit sidb_cluster_charge_state(const sidb_charge_state cs, const sidb_cluster_ptr& singleton) noexcept :
            neg_count{cs == sidb_charge_state::NEGATIVE},
            pos_count{cs == sidb_charge_state::POSITIVE},
            compositions{{sidb_cluster_state{singleton, static_cast<uint64_t>(*this), true}}}
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

// static constexpr inline const uint64_t SINGLETON_NEGATIVE_MULTISET_CHARGE_CONF = 1ull << 32ull;
// static constexpr inline const uint64_t SINGLETON_POSITIVE_MULTISET_CHARGE_CONF = 1ull;

static constexpr inline sidb_charge_state singleton_multiset_conf_to_charge_state(const uint64_t m) noexcept
{
    return sign_to_charge_state(static_cast<int8_t>(static_cast<uint32_t>(m) - (static_cast<uint32_t>(m) < m)));
}

struct potential_projection
{
    double   V{0.0};
    uint64_t M{0};

    explicit potential_projection() noexcept = default;

    explicit potential_projection(const double v, const uint64_t m) noexcept : V{v}, M{m} {}

    explicit potential_projection(const double init, const sidb_charge_state cs) noexcept :
            V{init},
            M{static_cast<uint64_t>(sidb_cluster_charge_state{cs})}
    {}

    constexpr inline bool operator<(const potential_projection& other) const noexcept
    {
        return V < other.V || (V == other.V && M < other.M);
    }

    constexpr inline potential_projection& operator+=(const potential_projection& other) noexcept
    {
        V += other.V;
        M += other.M;
        return *this;
    }
};

struct potential_projection_order
{

#ifdef DEBUG_MODE
    using pot_proj_order = std::set<potential_projection>;
#else
    using pot_proj_order = phmap::btree_set<potential_projection>;
#endif

    // 0 : NEG ; 1 : NEUT ; 2 : POS
    pot_proj_order order{};

    explicit potential_projection_order() noexcept = default;

    explicit potential_projection_order(const double inter_sidb_pot, const uint8_t base) noexcept :
            order{base == 3 ? pot_proj_order{potential_projection{-inter_sidb_pot, sidb_charge_state::POSITIVE},
                                             potential_projection{0.0, sidb_charge_state::NEUTRAL},
                                             potential_projection{inter_sidb_pot, sidb_charge_state::NEGATIVE}} :
                              pot_proj_order{potential_projection{0.0, sidb_charge_state::NEUTRAL},
                                             potential_projection{inter_sidb_pot, sidb_charge_state::NEGATIVE}}}
    {}

    template <bound_direction bound>
    constexpr inline const potential_projection& get() const noexcept
    {
        if constexpr (bound == bound_direction::LOWER)
        {
            return *order.cbegin();
        }
        else if constexpr (bound == bound_direction::UPPER)
        {
            return *order.crbegin();
        }
    }

    template <bound_direction bound>
    inline const potential_projection& get_next() const noexcept
    {
        const uint64_t bound_m = get<bound>().M;

        if constexpr (bound == bound_direction::LOWER)
        {
            return *std::find_if(order.cbegin(), order.cend(),
                                 [&](const potential_projection& pp) { return pp.M != bound_m; });
        }
        else if constexpr (bound == bound_direction::UPPER)
        {
            return *std::find_if(order.crbegin(), order.crend(),
                                 [&](const potential_projection& pp) { return pp.M != bound_m; });
        }
    }

    template <bound_direction bound>
    const potential_projection& get_pot_proj_for_m_conf(const uint64_t m_conf) const noexcept
    {
        if constexpr (bound == bound_direction::LOWER)
        {
            return *std::find_if(order.cbegin(), order.cend(),
                                 [&](const potential_projection& pp) { return pp.M == m_conf; });
        }
        else if constexpr (bound == bound_direction::UPPER)
        {
            return *std::prev(std::find_if(order.crbegin(), order.crend(),
                                           [&](const potential_projection& pp) { return pp.M == m_conf; })
                                  .base(),
                              1);
        }
    }

    void remove_m_conf(const uint64_t m_conf) noexcept
    {

        for (pot_proj_order::const_iterator it = order.cbegin(); it != order.cend();)
        {
            it->M == m_conf ? it = order.erase(it) : ++it;
        }
    }

    constexpr inline void add(const potential_projection& pp) noexcept
    {
        order.emplace(pp);
    }
};

#ifdef DEBUG_MODE
using sidb_cluster_charge_state_space = std::unordered_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;
#else
using sidb_cluster_charge_state_space = phmap::flat_hash_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;
#endif

struct sidb_cluster_ptr_hash
{
    constexpr inline std::size_t operator()(const sidb_cluster_ptr& c) const noexcept
    {
        return std::hash<uint64_t>{}(get_unique_cluster_id(c));
    }
};

#ifdef DEBUG_MODE
using sidb_clustering = std::set<sidb_cluster_ptr>;
#else
using sidb_clustering = phmap::flat_hash_set<sidb_cluster_ptr, sidb_cluster_ptr_hash>;
#endif

struct sidb_cluster
{
    using uid_t   = uint64_t;
    using sidb_ix = uint64_t;

    const uid_t uid{0};

    sidb_clustering             children;
    std::weak_ptr<sidb_cluster> parent{};

#ifdef DEBUG_MODE
    std::set<sidb_ix>                             sidbs;
    std::map<sidb_ix, std::array<double, 2>>      recv_ext_pot_bounds{};
    std::map<sidb_ix, potential_projection_order> pot_projs{};
#else
    phmap::flat_hash_set<sidb_ix> sidbs;
    phmap::flat_hash_map<sidb_ix, std::array<double, 2>> recv_ext_pot_bounds{};
    phmap::flat_hash_map<sidb_ix, potential_projection_order> pot_projs{};
#endif

    sidb_cluster_charge_state_space charge_space{};

#ifdef DEBUG_MODE
    explicit sidb_cluster(std::set<sidb_ix> c, sidb_clustering v, uid_t uid_) noexcept :
#else
    explicit sidb_cluster(phmap::flat_hash_set<sidb_ix> c, sidb_clustering v, uid_t uid_) noexcept :
#endif
            uid{c.size() == 1 ? *c.cbegin() : uid_},
            children{std::move(v)},
            sidbs{std::move(c)}
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

    inline sidb_cluster_ptr get_parent() const
    {
        return parent.lock();
    }

    void initialize_singleton_cluster_charge_space(const sidb_ix i, const double loc_pot_min, const double loc_pot_max,
                                                   const uint8_t base) noexcept
    {
        assert(sidbs.size() == 1);

        const sidb_cluster_ptr& this_ptr =
            *std::find_if(get_parent()->children.cbegin(), get_parent()->children.cend(),
                          [&](const sidb_cluster_ptr& c) { return *c->sidbs.cbegin() == i; });

        for (const sidb_charge_state cs :
             sidb_charge_state_reversed_iterator{base == 3 ? sidb_charge_state::POSITIVE : sidb_charge_state::NEUTRAL})
        {
            charge_space.emplace(sidb_cluster_charge_state{cs, this_ptr});
        }

        recv_ext_pot_bounds[i][static_cast<uint8_t>(bound_direction::LOWER)] = loc_pot_min;
        recv_ext_pot_bounds[i][static_cast<uint8_t>(bound_direction::UPPER)] = loc_pot_max;
    }

    template <bound_direction bound>
    constexpr inline double get_recv_ext_pot_bound(const sidb_ix i) noexcept
    {
        return recv_ext_pot_bounds.at(i)[static_cast<uint8_t>(bound)];
    }

    template <bound_direction bound>
    constexpr inline void set_recv_ext_pot_bound(const sidb_ix i, const double new_bound) noexcept
    {
        recv_ext_pot_bounds[i][static_cast<uint8_t>(bound)] = new_bound;
    }

    template <bound_direction bound>
    constexpr inline void update_recv_ext_pot_bound(const sidb_ix i, const double diff) noexcept
    {
        recv_ext_pot_bounds[i][static_cast<uint8_t>(bound)] += diff;
    }

    constexpr inline uint64_t size() const noexcept
    {
        return sidbs.size();
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

static constexpr inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept
{
    return c->sidbs.size();
}

static constexpr inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept
{
    return c->uid;
}

static constexpr inline uint64_t get_singleton_sidb_ix(const sidb_cluster_ptr& c) noexcept
{
    return get_unique_cluster_id(c);
}

static inline std::vector<sidb_cluster_state_composition>
get_projector_state_compositions(const sidb_cluster_projector_state& pst) noexcept
{
    return pst.cluster->charge_space.find(sidb_cluster_charge_state{pst.multiset_conf})->compositions;
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

    for (uint8_t c                                           = 0; c < 2;
         (*std::next(parent->children.begin(), c++))->parent = std::weak_ptr<sidb_cluster>(parent))
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
