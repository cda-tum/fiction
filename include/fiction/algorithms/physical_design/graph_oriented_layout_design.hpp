//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
#define FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/network_utils.hpp"
#include "fiction/utils/placement_utils.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <fmt/core.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/immutable_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Parameters for the graph-oriented layout design algorithm.
 */
struct graph_oriented_layout_design_params
{
    /**
     * High effort mode.
     */
    bool high_effort = false;
    /**
     * Return first found layout.
     */
    bool return_first = false;
    /**
     * Timeout limit (in ms).
     */
    std::optional<uint64_t> timeout = std::nullopt;
    /**
     * Verbosity.
     */
    bool verbose = false;
};
/**
 * This struct stores statistics about the graph-oriented layout design process.
 */
struct graph_oriented_layout_design_stats
{
    /**
     * Runtime of the graph-oriented layout design process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{
/**
 * Alias for a vector of tiles in a given layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using coord_vec_type = std::vector<fiction::tile<Lyt>>;
/**
 * This struct defines a hash function for a nested vector of layout tiles.
 * It calculates a combined hash value for a vector of tiles based on the coordinates of each tile.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
struct nested_vector_hash
{
    /**
     * Computes a hash value for a vector of `fiction::tile` objects.
     *
     * @param vec The vector of tiles to be hashed.
     * @return A combined hash value for the vector of tiles.
     */
    size_t operator()(const coord_vec_type<Lyt>& vec) const
    {
        size_t       hash  = 0;
        const size_t prime = 0x9e3779b9;
        for (const auto& tile : vec)
        {
            hash ^= std::hash<uint64_t>{}(tile.x) + prime + (hash << 6u) + (hash >> 2u);
            hash ^= std::hash<uint64_t>{}(tile.y) + prime + (hash << 6u) + (hash >> 2u);
            hash ^= std::hash<uint64_t>{}(tile.z) + prime + (hash << 6u) + (hash >> 2u);
        }
        return hash;
    }
};
/**
 * A priority queue class for managing elements with associated priorities.
 * The elements are stored in a priority queue, with the highest priority elements being
 * retrieved first.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
class priority_queue
{
    // Counter to keep track of insertion order
    size_t counter = 0;

    // Priority queue with tuples of (priority, insertion_order, element)
    using queue_element = std::tuple<double, size_t, coord_vec_type<Lyt>>;
    std::priority_queue<queue_element, std::vector<queue_element>, std::greater<queue_element>> elements;

  public:
    /**
     * Checks if the priority queue is empty.
     *
     * @return True if the priority queue is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const
    {
        return elements.empty();
    }

    /**
     * Adds an element to the priority queue with a given priority.
     *
     * @param item The element to be added.
     * @param priority The priority of the element.
     */
    void put(const coord_vec_type<Lyt>& item, double priority)
    {
        elements.emplace(priority, counter++, item);
    }

    /**
     * Retrieves and removes the element with the highest priority from the queue.
     *
     * @return The element with the highest priority.
     */
    coord_vec_type<Lyt> get()
    {
        coord_vec_type<Lyt> item = std::get<2>(elements.top());
        elements.pop();
        return item;
    }
};
/**
 * Alias for a distance functor on 2DDWave-clocked layouts.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using dist = twoddwave_distance_functor<Lyt, uint64_t>;
/**
 * Alias for a cost functor with unit cost.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using cost = unit_cost_functor<Lyt, uint8_t>;
/**
 * Parameters for the A* algorithm with wire-crossings enabled.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
const a_star_params A_STAR_PARAMS{true};
/**
 * Alias for a dictionary that maps nodes from a mockturtle network to signals in a layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Type of the mockturtle network.
 */
template <typename Lyt, typename Ntk>
using node_dict_type = mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>;
/**
 * Alias for a priority queue containing vertices of the search space graph.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using frontiers_type = detail::priority_queue<Lyt>;
/**
 * Alias for an unordered map that maps a vertex in the search space graph to its accumulated cost value.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using cost_so_far_type = std::unordered_map<coord_vec_type<Lyt>, double, detail::nested_vector_hash<Lyt>>;
/**
 * This struct holds two boolean values indicating the allowed positions for PIs.
 */
struct pi_locations
{
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the top.
     */
    bool top;
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the left.
     */
    bool left;
};
/**
 * A structure representing a search space graph.
 *
 * This struct encapsulates all the necessary data for managing a search space graph
 * during the graph-oriented layout design process. It holds the current vertex,
 * network, nodes to be placed, and other relevant information.
 *
 * @tparam Lyt The layout type.
 * @tparam Ntk The network type.
 */
template <typename Lyt>
struct search_space_graph
{
    /**
     * The current vertex in the search space graph.
     */
    coord_vec_type<Lyt> current_vertex;
    /**
     * The network associated with this search space graph.
     */
    fiction::tec_nt network;
    /**
     * Topological list of nodes to be placed in the layout.
     */
    std::vector<mockturtle::node<fiction::tec_nt>> nodes_to_place;
    /**
     * Flags indicating if primary inputs (PIs) can be placed at the top or left.
     */
    pi_locations pi_locs = {false, false};
    /**
     * Flag indicating if this graph's frontier is active.
     */
    bool frontier_flag = false;
    /**
     * The cost so far for reaching each vertex in the layout.
     */
    cost_so_far_type<Lyt> cost_so_far;
    /**
     * The maximum number of positions to be considered for expansions.
     */
    uint64_t num_expansions = 4;
    /**
     * Priority queue containing vertices of the search space graph.
     */
    frontiers_type<Lyt> frontier;
};
/**
 * @struct TimeoutSettings
 * @brief A structure to encapsulate the timeout settings based on given parameters.
 *
 * This struct holds a single `timeout` value which is determined based on the
 * provided optional timeout value and a boolean indicating if high effort is required.
 */
struct timeout_settings
{
    uint64_t timeout;

    /**
     * @brief Constructs a TimeoutSettings object.
     *
     * This constructor initializes the `timeout` member. If `timeout_opt` has a value,
     * it uses that value for the timeout. If `timeout_opt` does not have a value,
     * it sets the timeout to 1000000 milliseconds (1000 seconds) if `high_effort` is true,
     * or to 10000 milliseconds (10 seconds) if `high_effort` is false.
     *
     * @param timeout_opt An optional unsigned value representing the desired timeout in microseconds.
     * @param high_effort A boolean indicating whether a high effort (longer timeout) is required.
     */
    timeout_settings(const std::optional<unsigned>& timeout_opt, bool high_effort)
    {
        timeout = timeout_opt.value_or(high_effort ? 1000000u : 10000u);
    }
};
/**
 * @brief Custom view class derived from mockturtle::topo_view.
 *
 * This class inherits from mockturtle::topo_view and overrides certain functions
 * to provide custom behavior. The topological order is generated from COs to CIs.
 */
template <class Ntk>
class topo_view_co_to_ci : public mockturtle::immutable_view<Ntk>
{
  public:
    using node   = typename Ntk::node;
    using signal = typename Ntk::signal;

    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    topo_view_co_to_ci(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk)
    {
        update_topo();
    }
    /*! \brief Reimplementation of `size`. */
    auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }
    /*! \brief Reimplementation of `num_gates`. */
    auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }
    /*! \brief Reimplementation of `node_to_index`. */
    uint32_t node_to_index(node const& n) const
    {
        return std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n));
    }
    /*! \brief Reimplementation of `index_to_node`. */
    node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }
    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end(), fn);
    }
    /*! \brief Reimplementation of `foreach_gate`. */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.begin() + offset, topo_order.end(), fn);
    }
    /*! \brief Implementation of `foreach_gate` in reverse topological order. */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend() - offset, fn);
    }
    void update_topo()
    {
        this->incr_trav_id();
        this->incr_trav_id();
        topo_order.reserve(this->size());

        /* constants and PIs */
        const auto c0 = this->get_node(this->get_constant(false));
        topo_order.push_back(c0);
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
        }

        Ntk::foreach_co([this](auto f) { create_topo_rec(this->get_node(f)); });
    }

  private:
    void create_topo_rec(node const& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
        {
            return;
        }

        /* ensure that the node is not temporarily marked */
        assert(this->visited(n) != this->trav_id() - 1);

        /* mark node temporarily */
        this->set_visited(n, this->trav_id() - 1);

        /* mark children */
        this->foreach_fanin(n, [this](signal const& f) { create_topo_rec(this->get_node(f)); });

        /* mark node n permanently */
        this->set_visited(n, this->trav_id());

        /* visit node */
        topo_order.push_back(n);
    }

    std::vector<node> topo_order;
};
/**
 * @brief Custom view class derived from mockturtle::topo_view.
 *
 * This class inherits from mockturtle::topo_view and overrides certain functions
 * to provide custom behavior. The topological order is generated from CIs to COs.
 */
template <class Ntk>
class topo_view_ci_to_co : public mockturtle::immutable_view<Ntk>
{
  public:
    // Use the base class constructor
    using node   = typename Ntk::node;
    using signal = typename Ntk::signal;
    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    topo_view_ci_to_co(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk)
    {
        update_topo();
    }
    /*! \brief Reimplementation of `size`. */
    auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }
    /*! \brief Reimplementation of `num_gates`. */
    auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }
    /*! \brief Reimplementation of `node_to_index`. */
    uint32_t node_to_index(node const& n) const
    {
        return std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n));
    }
    /*! \brief Reimplementation of `index_to_node`. */
    node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }
    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end(), fn);
    }
    /*! \brief Reimplementation of `foreach_gate`. */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.begin() + offset, topo_order.end(), fn);
    }
    /*! \brief Implementation of `foreach_gate` in reverse topological order. */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend() - offset, fn);
    }

    void update_topo()
    {
        this->incr_trav_id();
        this->incr_trav_id();
        topo_order.reserve(this->size());

        /* constants and PIs */
        const auto c0 = this->get_node(this->get_constant(false));
        topo_order.push_back(c0);
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
        }

        Ntk::foreach_ci([this](auto n) { create_topo_rec(n); });
    }

  private:
    void create_topo_rec(node const& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
        {
            return;
        }

        bool not_visited = false;

        this->foreach_fanin(n,
                            [this, &not_visited](signal const& f)
                            {
                                if (this->visited(this->get_node(f)) != this->trav_id())
                                {
                                    not_visited = true;
                                };
                            });

        if (not_visited)
        {
            return;
        }

        /* mark node n permanently */
        this->set_visited(n, this->trav_id());

        /* visit node */
        topo_order.push_back(n);

        /* mark children */
        this->foreach_fanout(n, [this](node const& n) { create_topo_rec(n); });
    }

    std::vector<node> topo_order;
};
/**
 * Implementation of the graph-oriented layout design algorithm.
 * This class handles the initialization and execution of the algorithm.
 *
 * @tparam Plyt Cartesian gate-level layout type.
 * @tparam Pntk Network type.
 */
template <typename Plyt, typename Pntk>
class graph_oriented_layout_design_impl
{
  public:
    /**
     * Constructor for the graph-oriented layout design algorithm.
     *
     * @param src The source network to be placed.
     * @param p The parameters for the graph-enhanced layout search algorithm.
     * @param st The statistics object to record execution details.
     */
    graph_oriented_layout_design_impl(const Pntk& src, const graph_oriented_layout_design_params& p,
                                      graph_oriented_layout_design_stats& st) :
            ntk{mockturtle::names_view<Pntk>(src)},
            ps{p},
            pst{st},
            high_effort{ps.high_effort},
            verbose{ps.verbose},
            timeout{timeout_settings(ps.timeout, ps.high_effort).timeout},
            start{std::chrono::high_resolution_clock::now()},
            num_search_space_graphs{ps.high_effort ? 12u : 2u}
    {}
    /**
     * Executes the graph-oriented layout design algorithm and returns the best found layout.
     *
     * @return The best layout found by the algorithm.
     */
    Plyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        // Initialize empty layout
        Plyt lyt{{}, twoddwave_clocking<Plyt>()};
        auto layout = fiction::obstruction_layout<Plyt>(lyt);
        using Lyt   = decltype(layout);

        // Initialize layout to keep track of current best solution
        Plyt best_lyt{{}, twoddwave_clocking<Plyt>()};

        // Initialize search space graphs
        std::vector<search_space_graph<Lyt>> search_space_graphs(num_search_space_graphs);

        // Initial setup for networks and nodes to place
        initialize_networks_and_nodes_to_place<Lyt, Pntk>(ntk, search_space_graphs);

        // Initial setup for PIs and cost
        initialize_pis_cost_and_num_expansions(search_space_graphs);

        // Initial setup for frontiers flags
        initialize_frontiers_flags(search_space_graphs);

        bool timeout_limit_reached = false;
        // Main A* loop
        while (!timeout_limit_reached)
        {
            for (auto& search_space_graph : search_space_graphs)
            {
                if (search_space_graph.frontier_flag)
                {
                    count++;
                    auto expansion = expand<Plyt, Lyt, fiction::tec_nt>(search_space_graph);
                    if (expansion.second)
                    {
                        best_lyt = *expansion.second;
                        restore_names(search_space_graph.network, best_lyt);
                        if (ps.return_first)
                        {
                            return best_lyt;
                        }
                    }
                    for (const auto& [next, cost] : expansion.first)
                    {
                        if (search_space_graph.cost_so_far.find(next) == search_space_graph.cost_so_far.end() ||
                            cost < search_space_graph.cost_so_far[next])
                        {
                            search_space_graph.cost_so_far[next] = cost;
                            double priority                      = cost;
                            search_space_graph.frontier.put(next, priority);
                        }
                    }
                }
            }
            for (auto& graph : search_space_graphs)
            {
                if (graph.frontier_flag)
                {
                    if (!graph.frontier.empty())
                    {
                        graph.current_vertex = graph.frontier.get();
                    }
                    else
                    {
                        graph.frontier_flag = false;
                    }
                }
            }
            timeout_limit_reached = std::none_of(search_space_graphs.begin(), search_space_graphs.end(),
                                                 [](const search_space_graph<decltype(layout)>& search_space_graph)
                                                 { return search_space_graph.frontier_flag; });

            const auto end = std::chrono::high_resolution_clock::now();
            const auto duration_ms =
                static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            if (duration_ms >= timeout)
            {
                timeout_limit_reached = true;
            }
        }

        return best_lyt;
    }

  private:
    mockturtle::names_view<Pntk>        ntk;
    graph_oriented_layout_design_params ps;
    graph_oriented_layout_design_stats& pst;
    /**
     * High effort mode.
     */
    bool high_effort;
    /**
     * Be verbose.
     */
    bool verbose;
    /**
     * Timeout limit (in ms).
     */
    uint64_t timeout;
    /**
     * Start time.
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    /**
     * Number of search space graphs.
     */
    uint64_t num_search_space_graphs;
    /**
     * Count evaluated paths in the search space graphs.
     */
    uint64_t count{0};
    /**
     * Keep track of the maximum number of placed nodes.
     */
    uint64_t max_placed_nodes{0};
    /**
     * Current best solution w.r.t. area.
     */
    uint64_t best_solution{100000};
    /**
     * Flag indicating if initial solution was already found and other search space graphs should be pruned.
     */
    bool improv_mode{false};
    /**
     * Checks if there is a path between the source and destination tiles in the given layout.
     * Optionally returns the path if specified.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @param layout The layout to be checked.
     * @param src The source tile.
     * @param dest The destination tile.
     * @param src_is_new_pos Flag indicating if the source is a new position and should be checked if empty. Defaults to
     * false.
     * @param dest_is_new_pos Flag indicating if the destination is a new position and should be checked if empty.
     * Defaults to false.
     * @param return_path Flag indicating if the path should be returned. Defaults to false.
     * @return A pair where the first element is a boolean indicating if a path exists,
     * and the second element is an optional path (returned if `return_path` is true).
     */
    template <class Lyt>
    [[nodiscard]] std::pair<bool, std::optional<fiction::layout_coordinate_path<Lyt>>>
    check_path(const Lyt& layout, const fiction::tile<Lyt>& src, const fiction::tile<Lyt>& dest,
               const bool src_is_new_pos = false, const bool dest_is_new_pos = false,
               const bool return_path = false) noexcept
    {
        if ((layout.is_empty_tile(src) & src_is_new_pos) || (layout.is_empty_tile(dest) & dest_is_new_pos) ||
            (!src_is_new_pos && !dest_is_new_pos))
        {
            const auto path = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {src, dest}, dist<Lyt>(),
                                                                                    cost<Lyt>(), A_STAR_PARAMS<Lyt>);

            if (!path.empty())
            {
                if (return_path)
                {
                    return std::pair(true, path);
                }
                return std::pair(true, std::nullopt);
            }
        }
        return std::pair(false, std::nullopt);
    }
    /**
     * Retrieves the possible positions for Primary Inputs (PIs) in the given layout based on the specified
     * criteria of positioning at the top or left side, with a limit on the number of possible positions.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @param layout The layout in which to find the possible positions for PIs.
     * @param pi_locs Struct indicating if PIs are allowed at the top or left side of the layout.
     * @param num_expansions The maximum number of positions to be returned (is doubled for PIs).
     * @return A vector of tiles representing the possible positions for PIs.
     */
    template <class Lyt>
    [[nodiscard]] std::vector<fiction::tile<Lyt>> get_possible_positions_pis(Lyt& layout, const pi_locations pi_locs,
                                                                             const uint64_t num_expansions) noexcept
    {
        uint64_t                        count_expansions = 0;
        std::vector<fiction::tile<Lyt>> possible_positions{};

        layout.resize({layout.x() + 1, layout.y() + 1, 1});
        const fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};

        // Check if path from Input to drain exists
        auto check_tile = [&](uint64_t x, uint64_t y)
        {
            const fiction::tile<Lyt> tile{x, y, 0};
            if (check_path(layout, tile, drain, true, false, false).first)
            {
                count_expansions++;
                possible_positions.push_back(tile);
            }
        };

        const uint64_t max_iterations =
            (pi_locs.top && pi_locs.left) ? std::max(layout.x(), layout.y()) : (pi_locs.top ? layout.x() : layout.y());
        const uint64_t expansion_limit = (pi_locs.top && pi_locs.left) ? 2 * num_expansions : num_expansions;
        possible_positions.reserve(expansion_limit);

        for (uint64_t k = 0; k < max_iterations; k++)
        {
            if (pi_locs.top && k < layout.x())
            {
                check_tile(k, 0);
            }
            if (pi_locs.left && k < layout.y())
            {
                check_tile(0, k);
            }
            if (count_expansions >= expansion_limit)
            {
                layout.resize({layout.x() - 1, layout.y() - 1, 1});
                return possible_positions;
            }
        }

        layout.resize({layout.x() - 1, layout.y() - 1, 1});
        return possible_positions;
    }
    /**
     * Retrieves the possible positions for Primary Outputs (POs) in the given layout based on the positions
     * of the preceding nodes.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param layout The layout in which to find the possible positions for POs.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param fc A vector of nodes that precede the PO nodes.
     * @return A vector of tiles representing the possible positions for POs.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] std::vector<fiction::tile<Lyt>>
    get_possible_positions_pos(const Lyt& layout, node_dict_type<Lyt, Ntk>& node2pos,
                               const fanin_container<mockturtle::names_view<technology_network>>& fc) noexcept
    {
        std::vector<fiction::tile<Lyt>> possible_positions{};
        const auto&                     pre             = fc.fanin_nodes[0];
        const auto                      pre_t           = static_cast<tile<Lyt>>(node2pos[pre]);
        const auto                      expansion_limit = std::max(layout.x() - pre_t.x, layout.y() - pre_t.y);
        possible_positions.reserve(expansion_limit);

        // Check if path from previous tile to PO exists
        auto check_tile = [&](uint64_t x, uint64_t y)
        {
            const fiction::tile<Lyt> tile{x, y, 0};
            if (check_path(layout, pre_t, tile, false, true, false).first)
            {
                possible_positions.push_back(tile);
            }
        };

        for (uint64_t k = 0; k <= expansion_limit; ++k)
        {
            if (pre_t.x + k <= layout.x())
            {
                check_tile(pre_t.x + k, layout.y());
            }
            if (pre_t.y + k < layout.y())
            {
                check_tile(layout.x(), pre_t.y + k);
            }
        }

        return possible_positions;
    }
    /**
     * Retrieves the possible positions for a single fan-in node in the given layout, based on the positions
     * of preceding nodes and a specified number of expansions.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param layout The layout in which to find the possible positions for a single fan-in node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param num_expansions The maximum number of positions to be returned.
     * @param fc A vector of nodes that precede the single fanin node.
     * @return A vector of tiles representing the possible positions for a single fan-in node.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] std::vector<fiction::tile<Lyt>>
    get_possible_positions_single_fanin(Lyt& layout, node_dict_type<Lyt, Ntk>& node2pos, const uint64_t num_expansions,
                                        const fanin_container<mockturtle::names_view<technology_network>>& fc) noexcept
    {
        std::vector<fiction::tile<Lyt>> possible_positions{};
        possible_positions.reserve(num_expansions);
        uint64_t    count_expansions = 0;
        const auto& pre              = fc.fanin_nodes[0];
        const auto  pre_t            = static_cast<tile<Lyt>>(node2pos[pre]);

        // Check if path from previous tile to new tile and from new tile to drain exist
        auto check_tile = [&](uint64_t x, uint64_t y)
        {
            const fiction::tile<Lyt> new_pos{pre_t.x + x, pre_t.y + y, 0};

            if (check_path(layout, pre_t, new_pos, false, true, false).first)
            {
                layout.resize({layout.x() + 1, layout.y() + 1, 1});
                const fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};

                if (check_path(layout, new_pos, drain, true, false, false).first)
                {
                    possible_positions.push_back(new_pos);
                    count_expansions++;
                }
                layout.resize({layout.x() - 1, layout.y() - 1, 1});
            }
        };

        // Iterate diagonally
        for (uint64_t k = 0; k < layout.x() + layout.y() + 1; ++k)
        {
            for (uint64_t x = 0; x < k + 1; ++x)
            {
                const auto y = k - x;
                if ((pre_t.y + y) <= layout.y() && (pre_t.x + x) <= layout.x())
                {
                    check_tile(x, y);
                }
                if (count_expansions >= num_expansions)
                {
                    return possible_positions;
                }
            }
        }
        return possible_positions;
    }
    /**
     * Retrieves the possible positions for a double fan-in node in the given layout, based on the positions
     * of preceding nodes and a specified number of expansions.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param layout The layout in which to find the possible positions for a double fan-in node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param num_expansions The maximum number of positions to be returned.
     * @param fc A vector of nodes that precede the double fanin node.
     * @return A vector of tiles representing the possible positions for a double fan-in node.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] std::vector<fiction::tile<Lyt>>
    get_possible_positions_double_fanin(Lyt& layout, node_dict_type<Lyt, Ntk>& node2pos, const uint64_t num_expansions,
                                        const fanin_container<mockturtle::names_view<technology_network>>& fc) noexcept
    {
        std::vector<fiction::tile<Lyt>> possible_positions{};
        possible_positions.reserve(num_expansions);
        uint64_t count_expansions = 0;

        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]), pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

        const auto min_x = std::max(pre1_t.x, pre2_t.x) + (pre1_t.x == pre2_t.x ? 1 : 0);
        const auto min_y = std::max(pre1_t.y, pre2_t.y) + (pre1_t.y == pre2_t.y ? 1 : 0);

        // Check if path from previous tiles to new tile and from new tile to drain exist
        auto check_tile = [&](uint64_t x, uint64_t y)
        {
            const fiction::tile<Lyt> new_pos{min_x + x, min_y + y, 0};

            const auto path_1 = check_path(layout, pre1_t, new_pos, false, true, true);
            if (path_1.first)
            {
                for (const auto& el : *path_1.second)
                {
                    layout.obstruct_coordinate(el);
                }

                if (check_path(layout, pre2_t, new_pos, false, true, false).first)
                {
                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    const fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};

                    if (check_path(layout, new_pos, drain, true, false, false).first)
                    {
                        possible_positions.push_back(new_pos);
                        count_expansions++;
                    }

                    layout.resize({layout.x() - 1, layout.y() - 1, 1});
                }

                for (const auto& el : *path_1.second)
                {
                    layout.clear_obstructed_coordinate(el);
                }
            }
        };

        // Iterate diagonally
        for (uint64_t k = 0; k < layout.x() + layout.y() + 1; ++k)
        {
            for (uint64_t x = 0; x < k + 1; ++x)
            {
                const auto y = k - x;
                if ((min_y + y) <= layout.y() && (min_x + x) <= layout.x())
                {
                    check_tile(x, y);
                }
                if (count_expansions >= num_expansions)
                {
                    return possible_positions;
                }
            }
        }

        return possible_positions;
    }
    /**
     * Retrieves the possible positions for a given node in the layout based on its type and preceding nodes.
     * It determines the type of the node (PI, PO, single fan-in, double fan-in) and returns the corresponding
     * possible positions.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param layout The layout in which to find the possible positions.
     * @param search_space_graph The search space graph.
     * @param current_node The current node index in the nodes_to_place vector.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @return A vector of tiles representing the possible positions for the current node.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] std::vector<fiction::tile<Lyt>>
    get_possible_positions(Lyt& layout, const search_space_graph<Lyt>& search_space_graph, const uint64_t& current_node,
                           node_dict_type<Lyt, Ntk>& node2pos) noexcept
    {
        const auto fc = fanins(search_space_graph.network, search_space_graph.nodes_to_place[current_node]);

        if (search_space_graph.network.is_pi(search_space_graph.nodes_to_place[current_node]))
        {
            return get_possible_positions_pis(layout, search_space_graph.pi_locs, search_space_graph.network.num_pis());
        }
        if (search_space_graph.network.is_po(search_space_graph.nodes_to_place[current_node]))
        {
            return get_possible_positions_pos<Lyt, Ntk>(layout, node2pos, fc);
        }
        if (fc.fanin_nodes.size() == 1)
        {
            return get_possible_positions_single_fanin<Lyt, Ntk>(layout, node2pos, search_space_graph.num_expansions,
                                                                 fc);
        }
        if (fc.fanin_nodes.size() == 2)
        {
            return get_possible_positions_double_fanin<Lyt, Ntk>(layout, node2pos, search_space_graph.num_expansions,
                                                                 fc);
        }
    }
    /**
     * Validates the given layout based on the nodes in the network and their mappings in the node dictionary.
     * It checks if the placement of nodes in the layout is possible and ensures there are valid paths from each tile to
     * the drain.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param layout The layout to be validated.
     * @param network The network containing the nodes.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param placement_possible A boolean flag that will be set to false if placement is not possible.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] bool valid_layout(Lyt& layout, const Ntk& network, node_dict_type<Lyt, Ntk>& node2pos,
                                    std::vector<mockturtle::node<Ntk>>& nodes_to_place, uint64_t& current_node) noexcept
    {
        auto check_tile = [&](const fiction::tile<Lyt>& tile)
        {
            layout.resize({layout.x() + 1, layout.y() + 1, 1});
            const fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};
            const bool               path_exists = check_path(layout, tile, drain, false, true, false).first;
            layout.resize({layout.x() - 1, layout.y() - 1, 1});
            return path_exists;
        };

        auto is_empty_tile_or_crossable = [&](const fiction::tile<Lyt>& tile)
        {
            return layout.is_empty_tile(tile) ||
                   (layout.is_empty_tile({tile.x, tile.y, 1}) && !layout.is_obstructed_coordinate({tile.x, tile.y, 1}));
        };

        for (uint64_t node = 0; node < current_node; node++)
        {
            const auto layout_tile = static_cast<fiction::tile<Lyt>>(node2pos[nodes_to_place[node]]);
            const bool no_fanout_and_not_po =
                !layout.is_po_tile(layout_tile) && (layout.fanout_size(layout.get_node(layout_tile)) == 0);
            const bool one_dangling_fanout =
                (layout.fanout_size(layout.get_node(layout_tile)) == 1) && network.is_fanout(nodes_to_place[node]);

            if (no_fanout_and_not_po || one_dangling_fanout)
            {
                if (!check_tile(layout_tile))
                {
                    return false;
                }
            }

            const bool two_dangling_fanouts =
                (layout.fanout_size(layout.get_node(layout_tile)) == 0) && network.is_fanout(nodes_to_place[node]);

            if (two_dangling_fanouts)
            {
                fiction::tile<Lyt> right_tile{layout_tile.x + 1, layout_tile.y, 0};
                fiction::tile<Lyt> bottom_tile{layout_tile.x, layout_tile.y + 1, 0};

                if (!(is_empty_tile_or_crossable(right_tile) && is_empty_tile_or_crossable(bottom_tile)))
                {
                    return false;
                }
            }
        }

        return true;
    }
    /**
     * Places a node with a single input in the layout and routes it.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param fc A vector of nodes that precede the single fanin node.
     */
    template <typename Lyt, typename Ntk>
    void route_single_input_node(const fiction::tile<Lyt> position, Lyt& layout, node_dict_type<Lyt, Ntk>& node2pos,
                                 const fanin_container<mockturtle::names_view<technology_network>>& fc) noexcept
    {
        const auto& pre   = fc.fanin_nodes[0];
        const auto  pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

        layout.move_node(layout.get_node(position), position, {});

        const auto path = check_path(layout, pre_t, position, false, false, true);

        fiction::route_path(layout, *path.second);

        for (const auto& el : *path.second)
        {
            layout.obstruct_coordinate(el);
        }
    }
    /**
     * Places a node with two inputs in the layout and routes it.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param fc A vector of nodes that precede the double fanin node.
     */
    template <typename Lyt, typename Ntk>
    void route_double_input_node(const fiction::tile<Lyt> position, Lyt& layout, node_dict_type<Lyt, Ntk>& node2pos,
                                 const fanin_container<mockturtle::names_view<technology_network>>& fc) noexcept
    {
        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]), pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

        layout.move_node(layout.get_node(position), position, {});

        const auto path_1 = check_path(layout, pre1_t, position, false, false, true);

        for (const auto& el : *path_1.second)
        {
            layout.obstruct_coordinate(el);
        }

        const auto path_2 = check_path(layout, pre2_t, position, false, false, true);

        for (const auto& el : *path_2.second)
        {
            layout.obstruct_coordinate(el);
        }

        fiction::route_path(layout, *path_1.second);
        fiction::route_path(layout, *path_2.second);
    }
    /**
     * Executes a single placement step in the layout for the given network node. It determines the type of the node,
     * places it accordingly, and checks if a solution was found.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param network The network containing the nodes.
     * @param current_node The current node index in the nodes_to_place vector.
     * @param nodes_to_place A vector representing the nodes to be placed.
     * @param current_po The current primary output index.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param pi2node A mapping of primary input nodes to layout nodes.
     * @return A boolean indicating if a solution was found.
     * @throws std::runtime_error If the node type is invalid or not recognized.
     */
    template <typename Lyt, typename Ntk>
    [[nodiscard]] bool place_and_route(const fiction::tile<Lyt> position, Lyt& layout, const Ntk& network,
                                       uint64_t& current_node, std::vector<mockturtle::node<Ntk>>& nodes_to_place,
                                       uint64_t& current_po, node_dict_type<Lyt, Ntk>& node2pos,
                                       mockturtle::node_map<mockturtle::node<Lyt>, Ntk>& pi2node) noexcept
    {
        // Vector to store preceding nodes
        const auto fc = fanins(network, nodes_to_place[current_node]);

        if (network.is_pi(nodes_to_place[current_node]))
        {
            // Place primary input node
            node2pos[nodes_to_place[current_node]] = layout.move_node(pi2node[nodes_to_place[current_node]], position);
        }
        else if (fc.fanin_nodes.size() == 1)
        {
            const auto& pre = fc.fanin_nodes[0];
            // Place single input node
            if (network.is_po(nodes_to_place[current_node]))
            {
                node2pos[nodes_to_place[current_node]] =
                    layout.create_po(node2pos[pre], fmt::format("po{}", current_po++), position);
            }
            else
            {
                const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);
                auto       a     = static_cast<mockturtle::signal<Lyt>>(pre_t);

                node2pos[nodes_to_place[current_node]] =
                    place(layout, position, network, nodes_to_place[current_node], a);
            }

            route_single_input_node(position, layout, node2pos, fc);
        }
        else
        {
            // Place double input node
            const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];
            auto a1 = static_cast<mockturtle::signal<Lyt>>(pre1), a2 = static_cast<mockturtle::signal<Lyt>>(pre2);

            node2pos[nodes_to_place[current_node]] =
                place(layout, position, network, nodes_to_place[current_node], a1, a2, fc.constant_fanin);

            route_double_input_node(position, layout, node2pos, fc);
        }

        current_node += 1;
        layout.obstruct_coordinate({position.x, position.y, 0});
        layout.obstruct_coordinate({position.x, position.y, 1});

        // Check if solution was found and update max_placed_nodes
        const auto found_solution = (current_node == nodes_to_place.size());
        max_placed_nodes          = std::max(current_node, max_placed_nodes);

        return found_solution;
    }
    /**
     * Outputs placement information.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param lyt Current layout.
     * @param area Layout area (in tiles).
     */
    template <typename Lyt>
    void print_placement_info(const Lyt& lyt, const uint64_t area)
    {
        std::cout << "Found improved solution:\n";

        // Calculate the duration between start and end
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = end - start;

        // Extract the duration components
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        // Output the elapsed time
        std::cout << "Time taken: " << sec << " seconds, " << ms << " milliseconds, and " << us << " microseconds\n";
        std::cout << "Evaluated Paths: " << count << "\n";
        std::cout << "Layout Dimension: " << lyt.x() + 1 << " x " << lyt.y() + 1 << " = " << area << std::endl;
    }
    /**
     * Computes possible expansions and their priorities for the current vertex in the search space graph.
     * It handles placement of nodes, checks for valid paths, and finds potential next positions based on priorities.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param search_space_graph The search space graph.
     * @param verbose Flag indicating if verbose output is enabled.
     * @return A pair containing a vector of next positions with their priorities and an optional layout.
     * If an improved solution is found, the layout is returned.
     * If the layout is invalid or no improvement is possible, std::nullopt is returned.
     */
    template <typename OrigLyt, typename Lyt, typename Ntk>
    [[nodiscard]] std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>
    expand(search_space_graph<Lyt>& search_space_graph) noexcept
    {
        uint64_t current_po   = 0;
        uint64_t current_node = 0;

        const auto     min_layout_width  = search_space_graph.network.num_pis();
        const uint64_t min_layout_height = 1;
        const auto     max_layout_width  = search_space_graph.nodes_to_place.size();
        const auto     max_layout_height = search_space_graph.nodes_to_place.size();

        node_dict_type<Lyt, Ntk>                            node2pos{search_space_graph.network};
        std::vector<std::pair<coord_vec_type<Lyt>, double>> next_positions;
        next_positions.reserve(2 * search_space_graph.num_expansions);

        OrigLyt lyt{{min_layout_width - 1, min_layout_height - 1, 1}, twoddwave_clocking<Lyt>()};
        auto    layout  = fiction::obstruction_layout<OrigLyt>(lyt);
        auto    pi2node = reserve_input_nodes(layout, search_space_graph.network);

        bool                   found_solution = false;
        std::vector<tile<Lyt>> possible_positions{};
        possible_positions.reserve(2 * search_space_graph.num_expansions);

        if (search_space_graph.current_vertex.empty())
        {
            possible_positions = get_possible_positions<Lyt, Ntk>(layout, search_space_graph, current_node, node2pos);
        }

        for (uint64_t idx = 0; idx < search_space_graph.current_vertex.size(); ++idx)
        {
            const auto position = search_space_graph.current_vertex[idx];

            found_solution =
                place_and_route<Lyt, Ntk>(position, layout, search_space_graph.network, current_node,
                                          search_space_graph.nodes_to_place, current_po, node2pos, pi2node);

            uint64_t area = 0;
            if (improv_mode)
            {
                const auto bb = fiction::bounding_box_2d(layout);
                area          = (bb.get_x_size() + 1) * (bb.get_y_size() + 1);
            }

            if (found_solution && (!improv_mode || (area < best_solution)))
            {
                const auto bb = fiction::bounding_box_2d(layout);
                layout.resize({bb.get_x_size(), bb.get_y_size(), layout.z()});
                area = (layout.x() + 1) * (layout.y() + 1);
                if (verbose)
                {
                    print_placement_info<Lyt>(layout, area);
                }
                best_solution = area;
                improv_mode   = true;
                return {{}, layout};
            }

            if (improv_mode && area >= best_solution)
            {
                return {{}, std::nullopt};
            }

            // Expand layout dimensions if needed
            if (position.x == layout.x() && layout.x() < (max_layout_width - 1))
            {
                layout.resize({layout.x() + 1, layout.y(), layout.z()});
            }
            if (position.y == layout.y() && layout.y() < (max_layout_height - 1))
            {
                layout.resize({layout.x(), layout.y() + 1, layout.z()});
            }

            // Check if it's the last position in the current vertex
            if (idx == (search_space_graph.current_vertex.size() - 1))
            {
                if (!valid_layout<Lyt, Ntk>(layout, search_space_graph.network, node2pos,
                                            search_space_graph.nodes_to_place, current_node))
                {
                    return {{}, std::nullopt};
                }
                possible_positions =
                    get_possible_positions<Lyt, Ntk>(layout, search_space_graph, current_node, node2pos);
            }
        }

        // Generate next possible positions with their priorities
        for (const auto& position : possible_positions)
        {
            auto new_sequence = search_space_graph.current_vertex;
            new_sequence.push_back(position);
            const auto remaining_nodes_to_place = static_cast<double>(search_space_graph.nodes_to_place.size() -
                                                                      (search_space_graph.current_vertex.size() + 1));
            // Current layout size
            const double cost1 = static_cast<double>(((std::max(layout.x() - 1, position.x) + 1) *
                                                      (std::max(layout.y() - 1, position.y) + 1))) /
                                 static_cast<double>((max_layout_width * max_layout_height));
            // Position of last placed node
            const double cost2 = static_cast<double>(((position.x + 1) * (position.y + 1))) /
                                 static_cast<double>((max_layout_width * max_layout_height));

            double priority = remaining_nodes_to_place + cost1 + cost2;
            next_positions.push_back({new_sequence, priority});
        }

        return {next_positions, std::nullopt};
    }
    /**
     * Initializes the allowed positions for primary inputs (PIs), the cost for each search space graph and the maximum
     * number of expansions.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @param search_space_graphs The vector of search space graphs to be initialized.
     */
    template <typename Lyt>
    void initialize_pis_cost_and_num_expansions(std::vector<search_space_graph<Lyt>>& search_space_graphs) noexcept
    {
        constexpr std::array<pi_locations, 12> pi_locs = {{{true, false},
                                                           {false, true},
                                                           {true, true},
                                                           {true, false},
                                                           {false, true},
                                                           {true, true},
                                                           {true, false},
                                                           {false, true},
                                                           {true, true},
                                                           {true, false},
                                                           {false, true},
                                                           {true, true}}};

        for (uint64_t i = 0; i < search_space_graphs.size(); ++i)
        {
            search_space_graphs[i].pi_locs                                            = pi_locs[i];
            search_space_graphs[i].cost_so_far[search_space_graphs[i].current_vertex] = 0;
            search_space_graphs[i].num_expansions                                     = 4;
        }
    }
    /**
     * Initializes the networks and nodes to place.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @tparam Ntk Network type.
     * @param ntk The network to be placed and routed.
     * @param search_space_graphs The vector of search space graphs to be initialized.
     */
    template <typename Lyt, typename Ntk>
    void initialize_networks_and_nodes_to_place(Ntk&                                  pntk,
                                                std::vector<search_space_graph<Lyt>>& search_space_graphs) noexcept
    {
        // Prepare nodes to place for each topology view
        auto prepare_nodes_to_place = [](auto& network, auto& nodes_to_place)
        {
            network.foreach_node(
                [&nodes_to_place, &network](const auto& n)
                {
                    if (!network.is_constant(n))
                    {
                        nodes_to_place.push_back(n);
                    }
                });
        };

        // Convert network and apply fanout substitution
        auto tec_ntk = fiction::convert_network<fiction::tec_nt, Ntk>(pntk);
        tec_ntk.substitute_po_signals();

        fiction::fanout_substitution_params params{};
        params.strategy = fiction::fanout_substitution_params::substitution_strategy::BREADTH;
        mockturtle::fanout_view network_substituted_breadth{
            fanout_substitution<mockturtle::names_view<fiction::technology_network>>(tec_ntk, params)};

        topo_view_co_to_ci network_breadth_co_to_ci{network_substituted_breadth};

        // Initialize search space graphs with networks
        search_space_graphs[0].network = network_breadth_co_to_ci;
        search_space_graphs[1].network = network_breadth_co_to_ci;

        std::vector<mockturtle::node<decltype(network_breadth_co_to_ci)>> nodes_to_place_breadth_co_to_ci{};
        prepare_nodes_to_place(network_breadth_co_to_ci, nodes_to_place_breadth_co_to_ci);

        search_space_graphs[0].nodes_to_place = nodes_to_place_breadth_co_to_ci;
        search_space_graphs[1].nodes_to_place = nodes_to_place_breadth_co_to_ci;

        if (high_effort)
        {
            params.strategy = fiction::fanout_substitution_params::substitution_strategy::DEPTH;
            mockturtle::fanout_view network_substituted_depth{
                fanout_substitution<mockturtle::names_view<fiction::technology_network>>(tec_ntk, params)};

            topo_view_ci_to_co network_breadth_ci_to_co{network_substituted_breadth};
            topo_view_co_to_ci network_depth_co_to_ci{network_substituted_depth};
            topo_view_ci_to_co network_depth_ci_to_co{network_substituted_depth};

            // Initialize search space graphs with networks
            search_space_graphs[2].network = network_breadth_co_to_ci;
            for (uint64_t i = 0; i < 3; ++i)
            {
                search_space_graphs[3 + i].network = network_breadth_ci_to_co;
                search_space_graphs[6 + i].network = network_depth_co_to_ci;
                search_space_graphs[9 + i].network = network_depth_ci_to_co;
            }

            std::vector<mockturtle::node<decltype(network_breadth_ci_to_co)>> nodes_to_place_breadth_ci_to_co{};
            std::vector<mockturtle::node<decltype(network_depth_co_to_ci)>>   nodes_to_place_depth_co_to_ci{};
            std::vector<mockturtle::node<decltype(network_depth_ci_to_co)>>   nodes_to_place_depth_ci_to_co{};
            prepare_nodes_to_place(network_breadth_ci_to_co, nodes_to_place_breadth_ci_to_co);
            prepare_nodes_to_place(network_depth_co_to_ci, nodes_to_place_depth_co_to_ci);
            prepare_nodes_to_place(network_depth_ci_to_co, nodes_to_place_depth_ci_to_co);

            search_space_graphs[2].nodes_to_place = nodes_to_place_breadth_co_to_ci;
            // Assign nodes to place to each search space graphs
            for (uint64_t i = 0; i < 3; ++i)
            {
                search_space_graphs[3 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                search_space_graphs[6 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                search_space_graphs[9 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;
            }
        }
    }
    /**
     * Initializes the enabled search space graphs.
     *
     * @tparam Lyt Cartesian gate-level layout type.
     * @param search_space_graphs The vector of search space graphs to be initialized.
     */
    template <typename Lyt>
    void initialize_frontiers_flags(std::vector<search_space_graph<Lyt>>& search_space_graphs) noexcept
    {
        for (uint64_t i = 0; i < (high_effort ? 12 : 2); ++i)
        {
            search_space_graphs[i].frontier_flag = true;
        }
    }
};

}  // namespace detail
/**
 * A scalable and efficient placement & routing approach based on spanning a search space graph of partial layouts and
 * finding a path to one of its leafs, i.e., a complete layout.
 *
 * The search space graph start with an empty layout and then expands it based on where the first node in a topological
 * sort of the logic network can be placed. Based on the position of this first node, a cost is assigned to each
 * expansion based on the position of the placed node. The vertex with the least cost, which is the smallest layout
 * w.r.t. area, is then chosen for the next expansion. This iterative process continues until a leaf node is found,
 * which is a layout with all nodes placed. The algorithm then continues to backtrack through the search space graph to
 * find other complete layouts with less cost.
 *
 * Only works for 2DDWave-clocked layouts.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param ntk The network to be placed and routed.
 * @param ps The parameters for the A* priority routing algorithm. Defaults to an empty parameter set.
 * @param pst A pointer to a statistics object to record execution details. Defaults to nullptr.
 * @return The resulting layout after applying the A* priority routing algorithm.
 */
template <typename Lyt, typename Ntk>
Lyt graph_oriented_layout_design(Ntk& ntk, graph_oriented_layout_design_params ps = {},
                                 graph_oriented_layout_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here
    graph_oriented_layout_design_stats                  st{};
    detail::graph_oriented_layout_design_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
