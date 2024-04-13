//
// Created by Willem Lambooy on 04.02.2024.
//

#ifndef FICTION_SIDB_CLUSTER_HIERARCHY_HPP
#define FICTION_SIDB_CLUSTER_HIERARCHY_HPP

#if (FICTION_ALGLIB_ENABLED)

/**
 * Uncomment this line to switch to STL containers, which are slower than their respective analogues from the
 * Parallel-Hashmap library by Gregory Popovitch, but may be inspected with ease in a debugger.
 */
// #define DEBUG_SIDB_CLUSTER_HIERARCHY

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/utils/hash.hpp"

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
#include <set>
#include <unordered_map>
#include <unordered_set>
#else
#include <btree.h>
#include <phmap.h>
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <dataanalysis.h>
#include <linalg.h>

namespace fiction
{

/**
 * An enumeration of cluster linkage methods. The chosen method defines how clusters are merged in the agglomerative
 * clustering procedure, by, e.g., defining an inter-cluster distance to minimize for the cluster to merge. For more
 * information, visit: https://docs.tibco.com/pub/spotfire/6.5.1/doc/html/hc/hc_clustering_methods_overview.htm.
 */
enum class sidb_cluster_hierarchy_linkage_method
{
    /**
     * Complete linkage takes the maximum distance between nodes in a cluster.
     */
    COMPLETE = 0,
    /**
     * Single linkage takes the minimum distance between nodes in a cluster.
     */
    SINGLE,
    /**
     * Unweighted average linkage takes the average distance between nodes in a cluster, disregarding the number of
     * elements in a node.
     */
    UNWEIGHTED_AVERAGE,
    /**
     * Weighted average linkage takes the average distance between nodes in a cluster, weighing in the number of
     * elements in a node.
     */
    WEIGHTED_AVERAGE,
    /**
     * Known as Ward's method, this type of linkage merges clusters based on a minimum variance measure.
     */
    MINIMUM_VARIANCE
};

struct sidb_binary_cluster_hierarchy_node;
using sidb_binary_cluster_hierarchy_node_ptr = std::unique_ptr<sidb_binary_cluster_hierarchy_node>;
/**
 * The struct used to store a binary cluster hierarchy that may be used to store the result of the hierarchical
 * clustering returned by ALGLIB functionality.
 */
struct sidb_binary_cluster_hierarchy_node
{
    /**
     * The set of SiDB indices contained in the node
     */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    std::set<uint64_t> c;
#else
    phmap::flat_hash_set<uint64_t> c;
#endif
    /**
     * The two children of the node.
     */
    std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> sub;
    /**
     * Binary cluster hierarchy node constructor.
     *
     * @param sidbs The set of SiDBs to contain in this node.
     * @param children The pair of binary cluster hierarchy node pointers that become the children of this node.
     */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    sidb_binary_cluster_hierarchy_node(const std::set<uint64_t>& sidbs,
#else
    sidb_binary_cluster_hierarchy_node(phmap::flat_hash_set<uint64_t> sidbs,
#endif
                                       std::array<sidb_binary_cluster_hierarchy_node_ptr, 2> children) noexcept :
            c{std::move(sidbs)},
            sub{std::move(children)}
    {}
};
/**
 * This function performs the ALGLIB agglomerative clustering algorithm for a given SiDB layout. By default, the cluster
 * are created by a minimal positional variance heuristic, also known as Ward's method.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to create a cluster hierarchy of.
 * @param linkage_method The agglomerative clustering linking heuristic that is used by ALGLIB.
 */
template <typename Lyt>
static sidb_binary_cluster_hierarchy_node
sidb_cluster_hierarchy(Lyt& lyt, sidb_cluster_hierarchy_linkage_method linkage_method =
                                     sidb_cluster_hierarchy_linkage_method::MINIMUM_VARIANCE) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    // no clusterizer call required for <= 1 SiDBs
    if (lyt.num_cells() == 0)
    {
        return sidb_binary_cluster_hierarchy_node{{}, {nullptr, nullptr}};
    }
    else if (lyt.num_cells() == 1)
    {
        return sidb_binary_cluster_hierarchy_node{{0}, {nullptr, nullptr}};
    }

    charge_distribution_surface<Lyt> charge_lyt{lyt};

    alglib::real_2d_array d;
    d.setlength(static_cast<alglib::ae_int_t>(charge_lyt.num_cells()), 2);
    for (uint64_t i = 0; i < charge_lyt.num_cells(); ++i)
    {
        const auto [x, y] = charge_lyt.get_all_sidb_locations_in_nm().at(i);

        d(static_cast<int>(i), 0) = x;
        d(static_cast<int>(i), 1) = y;
    }

    alglib::clusterizerstate s;
    clusterizercreate(s);
    clusterizersetpoints(s, d, 2);                               // assign data and specify L2 norm (Euclidean metric)
    clusterizersetahcalgo(s, static_cast<int>(linkage_method));  // set the linkage method (default: Ward's method)
    alglib::ahcreport rep;
    clusterizerrunahc(s, rep);

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    std::unordered_map<uint64_t, std::unique_ptr<sidb_binary_cluster_hierarchy_node>> nodes{};
#else
    phmap::flat_hash_map<uint64_t, std::unique_ptr<sidb_binary_cluster_hierarchy_node>> nodes{};
#endif

    // build hierarchy from N - 1 merges represented in rep.z
    for (int i = 0; i < rep.npoints - 1; ++i)
    {
        const std::array<uint64_t, 2> cs = {static_cast<uint64_t>(rep.z(i, 0)), static_cast<uint64_t>(rep.z(i, 1))};
        for (uint8_t c = 0; c < 2; ++c)
        {
            // create leaf nodes
            if (cs.at(c) < charge_lyt.num_cells())
            {
                nodes[cs.at(c)] = std::make_unique<sidb_binary_cluster_hierarchy_node>(
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
                    std::set<uint64_t>{cs[c]},
#else
                    phmap::flat_hash_set<uint64_t>{cs.at(c)},
#endif
                    std::array<sidb_binary_cluster_hierarchy_node_ptr, 2>{nullptr, nullptr});
            }
        }

        // rep.z assigns each new cluster to N + i
        const uint64_t new_n = charge_lyt.num_cells() + static_cast<uint64_t>(i);

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
        std::set<uint64_t> set_union{};
#else
        phmap::flat_hash_set<uint64_t> set_union{};
#endif

        std::set_union(nodes.at(cs.at(0))->c.cbegin(), nodes.at(cs.at(0))->c.cend(), nodes.at(cs.at(1))->c.cbegin(),
                       nodes.at(cs.at(1))->c.cend(), std::inserter(set_union, set_union.begin()));

        nodes[new_n] = std::make_unique<sidb_binary_cluster_hierarchy_node>(
            set_union, std::array<sidb_binary_cluster_hierarchy_node_ptr, 2>{std::move(nodes.at(cs.at(0))),
                                                                             std::move(nodes.at(cs.at(1)))});
        nodes.erase(cs.at(0));
        nodes.erase(cs.at(1));
    }

    return std::move(*nodes.cbegin()->second);
}
/**
 * Forward declaration of the SiDB cluster hierarchy required for the mutual recursive structure in this file.
 * Here we define a pointer to a SiDB cluster to be a shared pointer, which enables us to also have pointers to parents.
 */
struct sidb_cluster;
using sidb_cluster_ptr = std::shared_ptr<sidb_cluster>;
/**
 * A receptor state pairs the potential recepting cluster with the identifier of the SiDB.
 */
struct sidb_cluster_receptor_state
{
    /**
     * Receptor cluster.
     */
    const sidb_cluster_ptr& cluster;
    /**
     * SiDB index. It is contained in the receptor cluster.
     */
    const uint64_t sidb_ix;
};
/**
 * Forward declaration. Required for compilation due to the mutually recursive structure in this file.
 */
static inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept;
/**
 * A projector state pairs the potential projecting cluster with the associated multiset charge configuration.
 */
struct sidb_cluster_projector_state
{
    /**
     * Projector cluster.
     */
    const sidb_cluster_ptr& cluster;
    /**
     * Multiset charge configuration. It is an element of the charge space of the projector cluster.
     */
    const uint64_t multiset_conf;
    /**
     * Getter for the number of a given charge state in the multiset configuration.
     *
     * @tparam cs Charge state to count the number of occurrences in the projector state of.
     * @return The number of occurrences of the given charge state in the multiset charge configuration. For a neutral
     * charge, the number of occurrences is inferred by considering the size of the cluster in the projector state.
     */
    template <sidb_charge_state cs>
    [[nodiscard]] constexpr inline uint64_t get_count() const noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE: return multiset_conf >> 32ull;
            case sidb_charge_state::POSITIVE: return multiset_conf & 0xFFFFFFFF;
            default: return get_cluster_size(cluster) - (multiset_conf >> 32ull) - (multiset_conf & 0xFFFFFFFF);
        }
    }
};
/**
 * The two types of electrostatic potential bounds required for the *Ground State Space* algorithm. As the domain in
 * which our potential bounds live are simply the real numbers, we may think of the lower bound and upper bound domains
 * to be separate partial order structures on the real number line, inverse to each other. The *Ground State Space*
 * algorithm requires the properties of a lower semi-lattice for these domains, ie. all finite meets must exist. This is
 * implemented for the lower and upper bound respectively simply by taking a minimum or a maximum. One may think of
 * meets as follows, which is very relevant to intention of their application: a meet, or greatest lower bound, is the
 * *minimal information* common to a set (of potential bounds). This semantic operation is essential to the *Ground
 * State Space* algorithm, which envelops without loss of accuracy.
 */
enum class bound_direction : uint8_t
{
    LOWER = 0,
    UPPER
};
/**
 * The respective *top* elements of the lower semi-lattices in which our potential bounds live, ie., the respective
 * elements of *most* information. This means that any meet with a top element (weakly) reduces the information, and
 * thus any meet computed in iteration through binary application may start out with this element.
 *
 * @tparam bound Potential bound domain to return the element of most information of.
 * @return The element of most information respective to the potential bound domain.
 */
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
/**
 * This function computes a binary meet, overwriting the first argument with the result. It takes the minimum in case of
 * a lower bound, and a maximum in case of an upper bound, each corresponding to the minimal information common to the
 * arguments.
 *
 * @tparam bound The potential bound domain which defines the implementation of the meet.
 * @param a First potential bound which is overwritten with the result of the meet.
 * @param b Second potential bound.
 */
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
/**
 * Forward declaration. Required for compilation due to the mutually recursive structure in this file.
 */
static inline uint64_t get_singleton_sidb_ix(const sidb_cluster_ptr& c) noexcept;
/**
 * Forward declaration. Required for compilation due to the mutually recursive structure in this file.
 */
static inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept;
/**
 * This defines the type for a store in which the bounds on the local electrostatic potential for an SiDB (index) may be
 * stored. For the *Ground State Space* algorithm, this is used to keep track of the bounds on the fraction of local
 * potential that is projected on the SiDB from within a cluster, though during *ClusterComplete* simulation, this is
 * used as a dynamic store in which also external potentials are accumulated.
 */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
using intra_cluster_potential_bounds = std::map<uint64_t, std::array<double, 2>>;
#else
using intra_cluster_potential_bounds = phmap::flat_hash_map<uint64_t, std::array<double, 2>>;
#endif
/**
 * A cluster state is a projector state (see `sidb_cluster_projector_state`) paired with a potential bound store in
 * which local potentials are stored for each SiDB in the cluster in the projector state. Throughout the *Ground State
 * Space* algorithm, these are internal potential bounds for each SiDB, corresponding to the meet on the potential
 * bounds for each composition of the cluster charge state associated with the multiset charge configuration of the
 * projector state. To clarify, each composition gives information of a multiset charge state assignment of all children
 * of this cluster. Thereby, we have bounds on the fraction of the local potential for an SiDB in the parent cluster
 * under such a composition. These bounds are then merged into a single bound without loss of accuracy through a meet.
 */
struct sidb_cluster_state
{
    /**
     * Projector state.
     */
    const sidb_cluster_projector_state proj_st;
    /**
     * Internal potential bounds.
     */
    intra_cluster_potential_bounds internal_pot_bounds{};
    /**
     * Constructor for the creation of cluster state. Simply creates the contained projector state while leaving the
     * internal potential store defaulted.
     *
     * @param singleton Cluster to form into a cluster state.
     * @param multiset_conf Multiset charge configuration for this cluster state.
     */
    explicit sidb_cluster_state(const sidb_cluster_ptr& c, const uint64_t multiset_conf) noexcept :
            proj_st{c, multiset_conf}
    {}
    /**
     * Getter for the internal potential bound for an SiDB in the cluster, corresponding to the meet on the potential
     * bounds under each composition of the multiset charge configuration in the cluster state's projector state.
     *
     * @tparam bound Internal potential bound type to obtain.
     * @param sidb_ix SiDB (index) to obtain the internal potential bound of.
     * @return The internal potential bound for this SiDB.
     */
    template <bound_direction bound>
    [[nodiscard]] constexpr inline double get_pot_bound(const uint64_t sidb_ix) const noexcept
    {
        return internal_pot_bounds.at(sidb_ix).at(static_cast<uint8_t>(bound));
    }
    /**
     * Setter for internal potential bound for an SiDB in the cluster.
     *
     * @param sidb_ix SiDB (index) to set the internal potential bound for.
     * @param min New lower bound to set.
     * @param max New upper bound to set.
     */
    inline void set_pot_bounds(const uint64_t sidb_ix, const double min, const double max) noexcept
    {
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::LOWER)] = min;
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::UPPER)] = max;
    }
    /**
     * Relative setter for internal potential bound for an SiDB in the cluster.
     *
     * @param sidb_ix SiDB (index) to update the internal potential bound of.
     * @param min_diff Difference in lower bound potential to apply.
     * @param max_diff Difference in upper bound potential to apply.
     */
    inline void update_pot_bounds(const uint64_t sidb_ix, const double min_diff, const double max_diff) noexcept
    {
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::LOWER)] += min_diff;
        internal_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::UPPER)] += max_diff;
    }
    /**
     * Defines the equality operation on cluster states. Since only cluster states need only be separated by the
     * associated clusters throughout operation, it suffices to compare the respective unique identifiers.
     *
     * @param other Other cluster state to compare.
     * @return `true` if and only if the cluster states contain the same cluster.
     */
    inline bool operator==(const sidb_cluster_state& other) const noexcept
    {
        return get_unique_cluster_id(proj_st.cluster) == get_unique_cluster_id(other.proj_st.cluster);
    }
    /**
     * Defines the inequality operation on cluster states, inverse to the former.
     *
     * @param other Other cluster state to compare.
     * @return `true` if and only if the cluster states do not contain the same cluster.
     */
    inline bool operator!=(const sidb_cluster_state& other) const noexcept
    {
        return !(*this == other);
    }
};
/**
 * A cluster state composition holds a number of cluster states of sibling clusters. Summing the multiset charge
 * configuration associated with each, we obtain an element of the charge space of their parent.
 */
using sidb_cluster_state_composition = std::vector<sidb_cluster_state>;
/**
 * Cluster state pointers are unique.
 */
using sidb_cluster_state_ptr = std::unique_ptr<sidb_cluster_state>;
/**
 * A clustering state is very similar to a cluster state composition, though it uses unique pointers to the cluster
 * states that may be moved. Thereby, this is the essential type of the dynamic objects in *ClusterComplete*'s
 * operation.
 */
using sidb_clustering_state = std::vector<sidb_cluster_state_ptr>;
/**
 * A cluster charge state is a multiset charge configuration. We may compress it into a 64 bit unsigned integer by
 * putting the number of negative and positive charges in the upper and lower 32 bits respectively. The number of
 * neutral charges may then be inferred for a given cluster by considering its size. Cluster charge states are the
 * crucial objects of the state spaces, called charge spaces, since they not only hold information of the multiset
 * charge configuration, but also the set of compositions, each of which compose into the current cluster charge state.
 */
struct sidb_cluster_charge_state
{
    /**
     * Number of negative charges in the cluster charge state (32 available bits).
     */
    uint64_t neg_count : 32;
    /**
     * Number of positive charges in the cluster charge state (32 available bits).
     */
    uint64_t pos_count : 32;

    mutable std::vector<sidb_cluster_state_composition> compositions{};
    /**
     * Default constructor, creates a cluster charge state without any negative and positive charges.
     */
    explicit sidb_cluster_charge_state() noexcept : neg_count{0}, pos_count{0} {}
    /**
     * Constructor for a charge space element of a singleton cluster. It has a single composition, which is a cluster
     * state of the singleton cluster and the singleton multiset charge configuration itself.
     *
     * @param singleton Singleton cluster to put in the compositions of this cluster charge state.
     * @param cs Charge state to lift to a singleton multiset charge configuration.
     * @param loc_ext_pot The local external potential at the SiDB in the singleton cluster. Specifically, this is the
     * sum of the local defect potential and the local external potential.
     */
    explicit sidb_cluster_charge_state(const sidb_cluster_ptr& singleton, const sidb_charge_state cs,
                                       const double loc_ext_pot) noexcept :
            neg_count{static_cast<decltype(neg_count)>(cs == sidb_charge_state::NEGATIVE)},
            pos_count{static_cast<decltype(pos_count)>(cs == sidb_charge_state::POSITIVE)},
            compositions{{sidb_cluster_state{singleton, static_cast<uint64_t>(*this)}}}
    {
        compositions.front().front().set_pot_bounds(get_singleton_sidb_ix(singleton), loc_ext_pot, loc_ext_pot);
    }
    /**
     * Constructor for cluster charge state given a multiset charge configuration represented in its compressed form. It
     * allows the compressed form to be lifted to the full type to facilitate equality checks.
     *
     * @param m The multiset charge configuration to create a cluster charge state of.
     */
    explicit sidb_cluster_charge_state(const uint64_t m) noexcept :
            neg_count{static_cast<decltype(neg_count)>(m >> 32ull)},
            pos_count{static_cast<decltype(pos_count)>((m << 32ull) >> 32ull)}
    {}
    /**
     * Explicit instructions for the compiler on how to cast a cluster charge state to an 64 bit unsigned integer.
     *
     * @return The 64 bit unsigned integer representing the compressed form of the cluster charge state.
     */
    explicit constexpr operator uint64_t() const noexcept
    {
        return (static_cast<uint64_t>(neg_count) << 32ull) | pos_count;
    }
    /**
     * Modifier of the cluster charge state, adding a single charge state.
     *
     * @param cs The charge state to add.
     */
    constexpr void add_charge(const sidb_charge_state cs) noexcept
    {
        switch (cs)
        {
            case sidb_charge_state::NEGATIVE:
            {
                neg_count++;
                return;
            }
            case sidb_charge_state::POSITIVE:
            {
                pos_count++;
                return;
            }
            default:
            {
                return;
            }
        }
    }
    /**
     * Constructor of a cluster charge state allowing initializer list construction.
     *
     * @param charge_states initializer list of charge states to form into a cluster charge state.
     */
    sidb_cluster_charge_state(const std::initializer_list<sidb_charge_state>& charge_states) noexcept :
            neg_count{0},
            pos_count{0}
    {
        for (const sidb_charge_state cs : charge_states)
        {
            add_charge(cs);
        }
    }
    /**
     * Defines the equality operation of cluster charge states, which disregards the compositions.
     *
     * @param other Other cluster charge state to test for equality with the current.
     * @return `true` if and only if the compressed forms are equal.
     */
    constexpr inline bool operator==(const sidb_cluster_charge_state& other) const noexcept
    {
        return static_cast<uint64_t>(*this) == static_cast<uint64_t>(other);
    }
    /**
     * Defined a hashing of a cluster charge state. Since we need only separate cluster charge states by their
     * compressed form, we may compute a hash over this for optimal performance when used in a hash set.
     *
     * @param m Cluster charge state to compute the hash of.
     * @return The hash of the given cluster charge state.
     */
    inline std::size_t operator()(const sidb_cluster_charge_state& m) const noexcept
    {
        return std::hash<uint64_t>{}(static_cast<uint64_t>(m));
    }
    /**
     * Defines addition of cluster charge states through multiset concatenation. Disregards compositions.
     *
     * @param other Other cluster charge state to concatenate with the current.
     * @return The concatenated cluster charge state, which is the modified version of the current.
     */
    constexpr inline sidb_cluster_charge_state& operator+=(const sidb_cluster_charge_state& other) noexcept
    {
        neg_count += other.neg_count;
        pos_count += other.pos_count;
        return *this;
    }
    /**
     * Defines subtraction of cluster charge states through multiset difference. Disregards compositions.
     *
     * @param other Other cluster charge state to take the difference of w.r.t. with the current.
     * @return The cluster charge state that is their difference, which is the modified version of the current.
     */
    constexpr inline sidb_cluster_charge_state& operator-=(const sidb_cluster_charge_state& other) noexcept
    {
        assert(neg_count >= other.neg_count && pos_count >= other.pos_count);
        neg_count -= other.neg_count;
        pos_count -= other.pos_count;
        return *this;
    }
};
/**
 * Function to convert a singleton cluster charge state in its compressed form to a charge state.
 *
 * @param m A singleton multiset charge configuration.
 * @return The charge state associated with the sole element contained in the given multiset charge configuration.
 */
static constexpr inline sidb_charge_state singleton_multiset_conf_to_charge_state(const uint64_t m) noexcept
{
    return sign_to_charge_state(
        static_cast<int8_t>(static_cast<uint32_t>(m) - static_cast<uint32_t>(static_cast<uint32_t>(m) < m)));
}
/**
 * This struct defines the type of an electrostatic potential projection, which pairs a multiset charge configuration
 * with the potential value (in eV) associated with the potential projection in the given context. The context is given
 * by potential projection stores in the decorated cluster hierarchies, which links the projecting cluster with SiDB
 * that receives this potential projection.
 */
struct potential_projection
{
    /**
     * Potential projection value (unit: eV).
     */
    double pot_val{0.0};
    /**
     * Associated multiset charge configuration.
     */
    uint64_t multiset{0};
    /**
     * Default constructor, used as a starting point for an accumulation of potential projections.
     */
    explicit potential_projection() noexcept = default;
    /**
     * Trivial copy constructor.
     *
     * @param pot Potential value to copy.
     * @param mul Multiset charge configuration to copy.
     */
    explicit potential_projection(const double pot, const uint64_t mul) noexcept : pot_val{pot}, multiset{mul} {}
    /**
     * Constructor for a potential projection from a singleton cluster, thereby lifting a value in the potential matrix
     * to a potential projection.
     * @param inter_sidb_pot Potential value of which the absolute value may be found in the potential matrix in a
     * associated `charge_distribution_surface` object.
     * @param cs Charge state associated with the singleton cluster projector for this potential projection.
     */
    explicit potential_projection(const double inter_sidb_pot, const sidb_charge_state cs) noexcept :
            pot_val{inter_sidb_pot},
            multiset{static_cast<uint64_t>(sidb_cluster_charge_state{cs})}
    {}
    /**
     * Defines an ordering of potential projections through comparison of the potential value. To prevent potential
     * projections of equal potential value but different associated multiset charge configurations to be regarded as
     * equal, a comparison of the latter is used as a "fail-safe".
     *
     * @param other Other potential projection to compare with the current.
     * @return `true` if and only if the potential value of the current is lower than that of `other`, or if the
     * potential values are equal and the compressed form of the multiset charge configuration is strictly less than
     * that of `other`.
     */
    constexpr inline bool operator<(const potential_projection& other) const noexcept
    {
        return pot_val < other.pot_val || (pot_val == other.pot_val && multiset < other.multiset);
    }
    /**
     * Defines summation of potential projections through addition of the potential values and concatenation of the
     * associated multiset charge configurations. The latter may simply be implemented by addition of the compressed
     * forms.
     *
     * @param other Other potential projection to sum with the current.
     * @return The current potential projection to which the other potential projection is now added.
     */
    constexpr inline potential_projection& operator+=(const potential_projection& other) noexcept
    {
        pot_val += other.pot_val;
        multiset += other.multiset;
        return *this;
    }
};
/**
 * This struct defines the type of potential projection order, along with getter and setter operations. Essentially, a
 * potential projection order is an ordered collection of potential projections, which allow rapid access and
 * modifications of the potential projection bounds by relying on the ordering defined on potential projections, and
 * canonical operations on ordered set containers. In the decorated cluster hierarchy, each cluster has a potential
 * projection order onto each SiDB in the layout from which the hierarchy was created.
 */
struct potential_projection_order
{
    /**
     * A potential projection order is an ordered set.
     */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    using pot_proj_order = std::set<potential_projection>;
#else
    using pot_proj_order = phmap::btree_set<potential_projection>;
#endif
    /**
     * The potential projection ordering.
     */
    pot_proj_order order{};
    /**
     * Default constructor, creating the empty potential projection order.
     */
    explicit potential_projection_order() noexcept = default;
    /**
     * Constructor for a potential projection from a singleton cluster onto the SiDB contained in it.
     *
     * @param loc_ext_pot The local external potential at the SiDB in the singleton cluster. Specifically, this is the
     * sum of the local defect potential and the local external potential.
     * @param base The simulation base. This defines whether positive charges are considered.
     * @param self_projection Separates the constructor type from inter-SiDB potential projections.
     */
    explicit potential_projection_order(const double loc_ext_pot, const uint8_t base,
                                        [[maybe_unused]] bool self_projection) noexcept :
            order{base == 3 ? pot_proj_order{potential_projection{loc_ext_pot, sidb_charge_state::POSITIVE},
                                             potential_projection{loc_ext_pot, sidb_charge_state::NEUTRAL},
                                             potential_projection{loc_ext_pot, sidb_charge_state::NEGATIVE}} :
                              pot_proj_order{potential_projection{loc_ext_pot, sidb_charge_state::NEUTRAL},
                                             potential_projection{loc_ext_pot, sidb_charge_state::NEGATIVE}}}
    {}
    /**
     * Constructor for a potential projection from a singleton cluster onto an SiDB.
     *
     * @param inter_sidb_pot The chargeless potential between the SiDB in the singleton cluster and the one projected
     * onto, as found in the potential matrix in an associated `charge_distribution_surface` object.
     * @param base The simulation base. This defines whether positive charges are considered.
     */
    explicit potential_projection_order(const double inter_sidb_pot, const uint8_t base) noexcept :
            order{base == 3 ? pot_proj_order{potential_projection{-inter_sidb_pot, sidb_charge_state::POSITIVE},
                                             potential_projection{0.0, sidb_charge_state::NEUTRAL},
                                             potential_projection{inter_sidb_pot, sidb_charge_state::NEGATIVE}} :
                              pot_proj_order{potential_projection{0.0, sidb_charge_state::NEUTRAL},
                                             potential_projection{inter_sidb_pot, sidb_charge_state::NEGATIVE}}}
    {}
    /**
     * A getter for a potential projection bound, which is the first or last item in the ordered set.
     *
     * @tparam bound The type of bound to obtain.
     * @return The potential projection that forms the requested bound on the potential projection order.
     */
    template <bound_direction bound>
    [[nodiscard]] constexpr inline const potential_projection& get_bound() const noexcept
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
    /**
     * A getter for the next potential projection bound, which is the first potential projection when traversing the
     * ordering either from below or from above, that differs in its multiset charge configuration from the relevant
     * potential projection bound.
     *
     * @tparam bound The type of bound to obtain.
     * @return The potential projection that would be the requested bound on the potential projection order if the
     * current relevant bound would be erased.
     */
    template <bound_direction bound>
    [[nodiscard]] inline const potential_projection& get_next_bound() const noexcept
    {
        const uint64_t bound_m = get_bound<bound>().multiset;

        if constexpr (bound == bound_direction::LOWER)
        {
            return *std::find_if(order.cbegin(), order.cend(),
                                 [&](const potential_projection& pp) { return pp.multiset != bound_m; });
        }
        else if constexpr (bound == bound_direction::UPPER)
        {
            return *std::find_if(order.crbegin(), order.crend(),
                                 [&](const potential_projection& pp) { return pp.multiset != bound_m; });
        }
    }
    /**
     * A getter for the potential projection bound given a multiset charge configuration specification, which is the
     * first potential projection in the ordering when traversing either from below or from above, that matches its
     * multiset charge configuration to the argument.
     *
     * @tparam bound The type of bound to obtain.
     * @param m_conf The multiset charge configuration to match.
     * @return The potential projection that forms the requested bound on the subset of the potential projection order
     * of potential projections that match their multiset charge configuration to the argument.
     */
    template <bound_direction bound>
    [[nodiscard]] const potential_projection& get_pot_proj_for_m_conf(const uint64_t m_conf) const noexcept
    {
        if constexpr (bound == bound_direction::LOWER)
        {
            return *std::find_if(order.cbegin(), order.cend(),
                                 [&](const potential_projection& pp) { return pp.multiset == m_conf; });
        }
        else if constexpr (bound == bound_direction::UPPER)
        {
            return *std::prev(std::find_if(order.crbegin(), order.crend(),
                                           [&](const potential_projection& pp) { return pp.multiset == m_conf; })
                                  .base(),
                              1);
        }
    }
    /**
     * Removes all occurrences of potential projections that match their multiset charge configuration to the argument.
     *
     * @param m_conf The multiset charge configuration of which all occurrences must be removed.
     */
    void remove_m_conf(const uint64_t m_conf) noexcept
    {
        for (pot_proj_order::const_iterator it = order.cbegin(); it != order.cend();)
        {
            it->multiset == m_conf ? it = order.erase(it) : ++it;
        }
    }
    /**
     * Adds a potential projection to the ordering, automatically placing it appropriately.
     *
     * @param pp Potential projection to add.
     */
    inline void add(const potential_projection& pp) noexcept
    {
        order.emplace(pp);
    }
};
/**
 * The type of a charge space is defined. It is a set of cluster charge states, forming the state space in our setting.
 */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
using sidb_cluster_charge_state_space = std::unordered_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;
#else
using sidb_cluster_charge_state_space = phmap::flat_hash_set<sidb_cluster_charge_state, sidb_cluster_charge_state>;
#endif
/**
 * This struct defines a hashing of shared pointers to clusters, used to facilitate quick access in a clustering.
 */
struct sidb_cluster_ptr_hash
{
    /**
     * The hashing operation is defined.
     *
     * @param c Shared pointer to a cluster to take the has of.
     * @return The hash computed over the the unique id associated with the cluster.
     */
    inline std::size_t operator()(const sidb_cluster_ptr& c) const noexcept
    {
        return std::hash<uint64_t>{}(get_unique_cluster_id(c));
    }
};
/**
 * A clustering is a set of disjoint clusters, ie., none share an SiDB.
 */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
using sidb_clustering = std::set<sidb_cluster_ptr>;
#else
using sidb_clustering = phmap::flat_hash_set<sidb_cluster_ptr, sidb_cluster_ptr_hash>;
#endif
/**
 * This struct defined the fully decorated cluster hierarchy type which follows the structure of a "general tree". It
 * contains multiple stores:
 * - a set of indices that correspond to the SiDBs as stored in a `charge_distribution_surface` object,
 * - a store of potential bounds for each SiDB in the cluster that give the fraction of the local electrostatic
 * potential that is received from outside the cluster, and
 * - a store containing the potential projection orders onto each SiDB, inside and outside the cluster.
 */
struct sidb_cluster
{
    using uid_t   = uint64_t;
    using sidb_ix = uint64_t;
    /**
     *
     */
    const uid_t uid;
    /**
     * The set of children of a cluster is a clustering.
     */
    sidb_clustering children;
    /**
     * Every cluster carries a pointer to its parent. For the top cluster, this is `nullptr`.
     */
    std::weak_ptr<sidb_cluster> parent{};

#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    std::set<sidb_ix>                             sidbs;
    std::map<sidb_ix, std::array<double, 2>>      recv_ext_pot_bounds{};
    std::map<sidb_ix, potential_projection_order> pot_projs{};
#else
    phmap::flat_hash_set<sidb_ix>                             sidbs;
    phmap::flat_hash_map<sidb_ix, std::array<double, 2>> received_ext_pot_bounds{};
    phmap::flat_hash_map<sidb_ix, potential_projection_order> pot_projs{};
#endif
    /**
     * The charge state space of the cluster.
     */
    sidb_cluster_charge_state_space charge_space{};
    /**
     * SiDB cluster hierarchy constructor.
     *
     * @param c Set of SiDB indices for the cluster to contain.
     * @param v A set of cluster hierarchies to set as the children of this cluster.
     * @param unique_id The unsigned integer to identify the cluster hierarchy uniquely with. For the case of a
     * singleton cluster, the unique identifier is set to be the index of the single SiDB it contains.
     */
#ifdef DEBUG_SIDB_CLUSTER_HIERARCHY
    explicit sidb_cluster(std::set<sidb_ix> c, sidb_clustering x, uid_t unique_id) noexcept :
#else
    explicit sidb_cluster(phmap::flat_hash_set<sidb_ix> c, sidb_clustering x, uid_t unique_id) noexcept :
#endif
            uid{x.empty() ? *c.cbegin() : unique_id},
            children{std::move(x)},
            sidbs{std::move(c)}
    {}
    /**
     * This function returns a shared pointer to the parent of this cluster.
     *
     * @return A shared pointer to the parent of this cluster
     */
    inline sidb_cluster_ptr get_parent() const
    {
        return parent.lock();
    }
    /**
     * This function initializes the charge space of a singleton cluster corresponding with the given simulation base,
     * and sets the initial bounds on the potential received from outside the cluster as the local potential bounds. The
     * local potential bounds do not include the local external potential at the SiDB in the singleton, as this is
     * passed separately.
     *
     * @param loc_pot_min The minimum local potential for the SiDB in the singleton cluster.
     * @param loc_pot_max The maximum local potential for the SiDB in the singleton cluster.
     * @param loc_ext_pot The local external potential at the SiDB in the singleton cluster. Specifically, this is the
     * sum of the local defect potential and the local external potential.
     * @param base The simulation base.
     * @param self_ptr Shared pointer to itself.
     */
    void initialize_singleton_cluster_charge_space(const double loc_pot_min, const double loc_pot_max,
                                                   const double loc_ext_pot, const uint8_t base,
                                                   const sidb_cluster_ptr& self_ptr) noexcept
    {
        assert(sidbs.size() == 1);

        const uint64_t ix = *sidbs.cbegin();

        for (const sidb_charge_state cs : base == 2 ? SIDB_CHARGE_STATES_BASE_2 : SIDB_CHARGE_STATES_BASE_3)
        {
            charge_space.emplace(sidb_cluster_charge_state{self_ptr, cs, loc_ext_pot});
        }

        received_ext_pot_bounds[ix][static_cast<uint8_t>(bound_direction::LOWER)] = loc_pot_min;
        received_ext_pot_bounds[ix][static_cast<uint8_t>(bound_direction::UPPER)] = loc_pot_max;
    }
    /**
     * The bound on the potential an SiDB receives from outside this cluster is returned.
     *
     * @tparam bound The potential bound that is requested.
     * @param i The SiDB index for which the potential bound is requested.
     * @return The potential bound that is stored.
     */
    template <bound_direction bound>
    constexpr inline double get_received_ext_pot_bound(const sidb_ix i) noexcept
    {
        return received_ext_pot_bounds.at(i).at(static_cast<uint8_t>(bound));
    }
    /**
     * The bound on the potential an SiDB receives from outside this cluster is set.
     *
     * @tparam bound The potential bound that is set.
     * @param i The SiDB index for which the potential bound is set.
     * @param new_bound The new potential bound that is set.
     */
    template <bound_direction bound>
    constexpr inline void set_received_ext_pot_bound(const sidb_ix i, const double new_bound) noexcept
    {
        received_ext_pot_bounds[i][static_cast<uint8_t>(bound)] = new_bound;
    }
    /**
     * The bound on the potential an SiDB receives from outside this cluster is updated.
     *
     * @tparam bound The potential bound that is updated.
     * @param i The SiDB index for which the potential bound is updated.
     * @param diff The difference with which the potential bound is updated.
     */
    template <bound_direction bound>
    constexpr inline void update_received_ext_pot_bound(const sidb_ix i, const double diff) noexcept
    {
        received_ext_pot_bounds[i][static_cast<uint8_t>(bound)] += diff;
    }
    /**
     * Function to return the number of SiDBs contained in the cluster.
     *
     * @return The number of SiDBs contained in the cluster.
     */
    inline uint64_t num_sidbs() const noexcept
    {
        return sidbs.size();
    }
    /**
     * Equality operation on cluster hierarchies. Checks the respective unique identifiers for equality.
     *
     * @param other Cluster (hierarchy) to compare to.
     * @return `true` if and only if the unique identifiers match.
     */
    constexpr inline bool operator==(const sidb_cluster& other) const noexcept
    {
        return uid == other.uid;
    }
};
/**
 * Function to obtain the amount of SiDBs in the given cluster. Required for compilation due to mutual recursion.
 *
 * @param c Cluster of which the number of SiDBs it contains is requested.
 * @return The number of SiDBs in the given cluster.
 */
static inline uint64_t get_cluster_size(const sidb_cluster_ptr& c) noexcept
{
    return c->sidbs.size();
}
/**
 * Function to obtain the unique identifier of the given cluster. Required for compilation due to mutual recursion.
 *
 * @param c Cluster of which its unique identifier is requested.
 * @return The unique identifier of the given cluster.
 */
static inline uint64_t get_unique_cluster_id(const sidb_cluster_ptr& c) noexcept
{
    return c->uid;
}
/**
 * Function to obtain the SiDB index contained in a singleton cluster. Required for compilation due to mutual recursion.
 *
 * @param c Singleton cluster of which the single SiDB (index) it contains is requested.
 * @return The SiDB index contained in the given cluster. It is equal to the unique identifier of the cluster.
 */
static inline uint64_t get_singleton_sidb_ix(const sidb_cluster_ptr& c) noexcept
{
    assert(get_cluster_size(c) == 1);
    return get_unique_cluster_id(c);
}
/**
 * This function is used to obtain the cluster charge state compositions of the multiset charge configuration in a
 * projector state. The corresponding charge space element of the projector is found and its compositions are returned.
 *
 * @param pst Projector state of which the corresponding compositions are requested.
 * @return The compositions associated with the multiset charge configuration of the projecting cluster.
 */
static inline std::vector<sidb_cluster_state_composition>
get_projector_state_compositions(const sidb_cluster_projector_state& pst) noexcept
{
    return pst.cluster->charge_space.find(sidb_cluster_charge_state{pst.multiset_conf})->compositions;
}
/**
 * This recursive function is used to convert a binary cluster hierarchy, as for instance returned by
 * `sidb_cluster_hierarchy` function that uses ALGLIB's `clusterizer`. The returned structure includes parent pointers.
 *
 * @param n A node from a binary cluster hierarchy, as for instance returned by parsing ALGLIB's result.
 * @param uid Variable reference which is updated in each execution to ensure uniqueness.
 * @return A uniquely identified node in a decorated cluster hierarchy that follows the "general tree" structure.
 */
static sidb_cluster_ptr to_unique_sidb_cluster(const sidb_binary_cluster_hierarchy_node& n, uint64_t& uid)
{
    sidb_clustering children;

    for (uint8_t c = 0; c < 2; ++c)
    {
        if (n.sub.at(c))
        {
            children.insert(to_unique_sidb_cluster(*n.sub.at(c), uid));
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
    {}

    return parent;
}
/**
 * This function initiates the recursive procedure of converting a binary cluster hierarchy to our bespoke version.
 *
 * @param n A node from a binary cluster hierarchy, as for instance returned by parsing ALGLIB's result.
 * @return A uniquely identified node in a decorated cluster hierarchy that follows the "general tree" structure.
 */
inline sidb_cluster_ptr to_sidb_cluster(const sidb_binary_cluster_hierarchy_node& n)
{
    uint64_t uid = n.c.size();

    if (uid != 1)
    {
        return to_unique_sidb_cluster(n, uid);
    }

    // to avoid weird shared pointer deallocation behaviour, give a parent to a singleton cluster hierarchy
    const sidb_cluster_ptr parent = std::make_shared<sidb_cluster>(
        n.c, sidb_clustering{std::make_shared<sidb_cluster>(n.c, sidb_clustering{}, 0)}, 1);
    (*parent->children.cbegin())->parent = std::weak_ptr<sidb_cluster>(parent);

    return parent;
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_SIDB_CLUSTER_HIERARCHY_HPP
