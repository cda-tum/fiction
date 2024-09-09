//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
#define FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/algorithms/physical_design/post_layout_optimization.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/network_utils.hpp"
#include "fiction/utils/placement_utils.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/immutable_view.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
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
     * The `effort_mode` enum defines different levels of computational effort for generating and exploring search space
     * graphs for during the graph-oriented layout design process. Each mode varies in the number of search space graphs
     * generated and the strategies employed, balancing between runtime efficiency and the likelihood of finding optimal
     * solutions.
     */
    enum effort_mode : std::uint8_t
    {
        /**
         * LOW_EFFORT mode generates 3 search space graphs. This option minimizes runtime but may not always yield the
         * optimal results.
         */
        LOW_EFFORT,

        /**
         * HIGH_EFFORT mode generates 12 search space graphs using various fanout substitution strategies, PI
         * placements, and other parameters. This wider exploration increases the chance of finding optimal layouts but
         * also extends runtime. When a solution is found in any graph, its cost is used to prune the remaining graphs.
         */
        HIGH_EFFORT,

        /**
         * HIGHEST_EFFORT mode builds upon HIGH_EFFORT by duplicating the 12 search space graphs for different cost
         * objectives. If the cost objective involves layout area, number of crossings, number of wire segments, or a
         * combination of area and crossings, a total of 48 search space graphs are generated. For a custom cost
         * objective, an additional 12 graphs are created, resulting in 60 graphs in total. This mode provides the best
         * guarantee of finding optimal solutions but significantly increases runtime.
         */
        HIGHEST_EFFORT
    };

    /**
     * The effort mode used. Defaults to HIGH_EFFORT.
     */
    effort_mode mode = HIGH_EFFORT;
    /**
     * Number of expansions for each vertex that should be explored. For each partial layout, `num_vertex_expansions`
     * positions will be checked for the next node/gate to be placed. A lower value requires less runtime, but the
     * layout might have a larger area or it could also lead to no solution being found. A higher value might lead to
     * better solutions, but also requires more runtime. Defaults to 4 expansions for each vertex.
     *
     */
    uint64_t num_vertex_expansions = 4u;
    /**
     * Return the first found layout, which might still have a high cost but can be found fast.
     */
    bool return_first = false;
    /**
     * Timeout limit (in ms).
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
    /**
     * Disable the creation of crossings during layout generation. If set to true, gates will only be placed if a
     * crossing-free wiring is found. Defaults to false.
     */
    bool planar = false;
    /**
     * The `cost_objective` enum defines various cost objectives that can be used in the graph-oriented layout design
     * process. Each cost objective represents a different metric used to expand a vertex in the search space graph.
     */
    enum cost_objective : std::uint8_t
    {
        /**
         * AREA: Optimizes for the total area of the layout, aiming to minimize the space required for the design.
         */
        AREA,
        /**
         * WIRES: Optimizes for the number of wire segments in the layout, reducing the delay and increasing throughput.
         */
        WIRES,
        /**
         * CROSSINGS: Optimizes for the number of wire crossings in the layout.
         */
        CROSSINGS,
        /**
         * ACP (Area-Crossings Product): Optimizes for a combination of layout area and the number of crossings.
         */
        ACP,
        /**
         * CUSTOM: Allows for a user-defined cost objective, enabling optimization based on specific criteria outside
         * the predefined options.
         */
        CUSTOM
    };
    /**
     * The cost objective used. Defaults to AREA
     */
    cost_objective cost = AREA;
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
    mockturtle::stopwatch<>::duration time_total{};
    /**
     * Layout width.
     */
    uint64_t x_size{0ull};
    /**
     * Layout height.
     */
    uint64_t y_size{0ull};
    /**
     * Number of gates.
     */
    uint64_t num_gates{0ull};
    /**
     * Number of wires.
     */
    uint64_t num_wires{0ull};
    /**
     * Number of crossings.
     */
    uint64_t num_crossings{0ull};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time      = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size     = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates      = {}\n", num_gates);
        out << fmt::format("[i] num. wires      = {}\n", num_wires);
        out << fmt::format("[i] num. crossings  = {}\n", num_crossings);
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
using coord_vec_type = std::vector<tile<Lyt>>;
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
     * Computes a hash value for a vector of `tile` objects.
     *
     * @param vec The vector of tiles to be hashed.
     * @return A combined hash value for the vector of tiles.
     */
    std::size_t operator()(const coord_vec_type<Lyt>& vec) const
    {
        std::size_t       hash  = 0ul;
        const std::size_t prime = 0x9e3779b9;
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

  private:
    /**
     * Counter to keep track of the insertion order of elements.
     */
    std::size_t counter = 0ul;
    /**
     * Tuple containing the priority, counter, and element.
     */
    using queue_element = std::tuple<double, std::size_t, coord_vec_type<Lyt>>;
    /**
     * Priority queue containing elements with associated priorities.
     */
    std::priority_queue<queue_element, std::vector<queue_element>, std::greater<queue_element>> elements;
};
/**
 * Alias for a dictionary that maps nodes from a mockturtle network to signals in a layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Type of the mockturtle network.
 */
template <typename Lyt, typename Ntk>
using node_dict_type = mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>;
/**
 * This enum class indicates the allowed positions for PIs.
 */
enum class pi_locations : std::uint8_t
{
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the top.
     */
    TOP,
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the left.
     */
    LEFT,
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the top and at the left.
     */
    TOP_AND_LEFT
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
    coord_vec_type<Lyt> current_vertex{};
    /**
     * The network associated with this search space graph.
     */
    tec_nt network;
    /**
     * Topological list of nodes to be placed in the layout.
     */
    std::vector<mockturtle::node<tec_nt>> nodes_to_place;
    /**
     * Enum indicating if primary inputs (PIs) can be placed at the top or left.
     */
    pi_locations pi_locs = pi_locations::TOP_AND_LEFT;
    /**
     * Flag indicating if this graph's frontier is active.
     */
    bool frontier_flag = true;
    /**
     * The cost so far for reaching each vertex in the layout.
     */
    std::unordered_map<coord_vec_type<Lyt>, double, detail::nested_vector_hash<Lyt>> cost_so_far{};
    /**
     * The maximum number of positions to be considered for expansions.
     */
    uint64_t num_expansions = 4u;
    /**
     * Priority queue containing vertices of the search space graph.
     */
    detail::priority_queue<Lyt> frontier{};
    /**
     * Create planar layouts.
     */
    bool planar = false;
    /**
     * The cost objective used to expand a vertex in the search space graph.
     */
    graph_oriented_layout_design_params::cost_objective cost =
        graph_oriented_layout_design_params::cost_objective::AREA;
};
/**
 * @brief Custom view class derived from mockturtle::topo_view.
 *
 * This class inherits from mockturtle::topo_view and overrides certain functions
 * to provide custom behavior. The topological order is generated from COs to CIs.
 */
template <typename Ntk>
class topo_view_co_to_ci : public mockturtle::immutable_view<Ntk>
{
  public:
    using node   = typename Ntk::node;
    using signal = typename Ntk::signal;

    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    explicit topo_view_co_to_ci(const Ntk& ntk) : mockturtle::immutable_view<Ntk>(ntk)
    {
        update_topo();
    }
    /*! \brief Reimplementation of `size`. */
    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }
    /*! \brief Reimplementation of `num_gates`. */
    [[nodiscard]] auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }
    /*! \brief Reimplementation of `node_to_index`. */
    [[nodiscard]] uint32_t node_to_index(const node& n) const
    {
        return std::distance(std::cbegin(topo_order), std::find(std::cbegin(topo_order), std::cend(topo_order), n));
    }
    /*! \brief Reimplementation of `index_to_node`. */
    [[nodiscard]] node index_to_node(const uint32_t index) const
    {
        return topo_order.at(index);
    }
    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.cbegin(), topo_order.cend(), std::forward<Fn>(fn));
    }
    /*! \brief Reimplementation of `foreach_gate`. */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        const uint32_t offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.cbegin() + offset, topo_order.cend(), std::forward<Fn>(fn));
    }
    /*! \brief Implementation of `foreach_gate` in reverse topological order. */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        const uint32_t offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.crbegin(), topo_order.crend() - offset, std::forward<Fn>(fn));
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
template <typename Ntk>
class topo_view_ci_to_co : public mockturtle::immutable_view<Ntk>
{
  public:
    // use the base class constructor
    using node   = typename Ntk::node;
    using signal = typename Ntk::signal;
    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    explicit topo_view_ci_to_co(const Ntk& ntk) : mockturtle::immutable_view<Ntk>(ntk)
    {
        update_topo();
    }
    /*! \brief Reimplementation of `size`. */
    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }
    /*! \brief Reimplementation of `num_gates`. */
    [[nodiscard]] auto num_gates() const
    {
        const uint32_t offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }
    /*! \brief Reimplementation of `node_to_index`. */
    [[nodiscard]] uint32_t node_to_index(const node& n) const
    {
        return std::distance(std::cbegin(topo_order), std::find(std::cbegin(topo_order), std::cend(topo_order), n));
    }
    /*! \brief Reimplementation of `index_to_node`. */
    [[nodiscard]] node index_to_node(const uint32_t index) const
    {
        return topo_order.at(index);
    }
    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.cbegin(), topo_order.cend(), std::forward<Fn>(fn));
    }
    /*! \brief Reimplementation of `foreach_gate`. */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        const uint32_t offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.cbegin() + offset, topo_order.cend(), std::forward<Fn>(fn));
    }
    /*! \brief Implementation of `foreach_gate` in reverse topological order. */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        const uint32_t offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.crbegin(), topo_order.crend() - offset, std::forward<Fn>(fn));
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

        Ntk::foreach_ci([this](const auto& n) { create_topo_rec(n); });
    }

  private:
    void create_topo_rec(const node& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
        {
            return;
        }

        bool not_visited = false;

        this->foreach_fanin(n,
                            [this, &not_visited](const signal& f)
                            {
                                if (this->visited(this->get_node(f)) != this->trav_id())
                                {
                                    not_visited = true;
                                }
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
        this->foreach_fanout(n, [this](const auto& fo) { create_topo_rec(fo); });
    }

    std::vector<node> topo_order;
};
/**
 * When checking for possible paths on a layout between two tiles SRC and DEST, one of them could also be the new tile
 * for the next gate to be placed and it therefore has to be checked if said tile is still empty
 */
enum class new_gate_location : std::uint8_t
{
    /**
     * Do not check any tiles.
     */
    NONE,
    /**
     * Check if the source tile is empty.
     */
    SRC,
    /**
     * Check if the destination tile is empty.
     */
    DEST
};
/**
 * Struct to hold information necessary for gate placement during layout generation for one vertex.
 *
 * @tparam ObstrLyt The type of the layout.
 */
template <typename ObstrLyt>
struct placement_info
{
    /**
     * The index of the current node being placed.
     */
    uint64_t current_node;
    /**
     * The index of the current primary output.
     */
    uint64_t current_po;
    /**
     * Mapping of nodes to their positions in the layout.
     */
    node_dict_type<ObstrLyt, tec_nt> node2pos;
    /**
     * Mapping of primary input nodes to layout nodes.
     */
    mockturtle::node_map<mockturtle::node<ObstrLyt>, tec_nt> pi2node;
};
/**
 * Implementation of the graph-oriented layout design algorithm.
 * This class handles the initialization and execution of the algorithm.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 */
template <typename Lyt, typename Ntk>
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
    graph_oriented_layout_design_impl(const Ntk& src, const graph_oriented_layout_design_params& p,
                                      graph_oriented_layout_design_stats& st) :
            ntk{convert_network<tec_nt>(src)},
            ps{p},
            pst{st},
            timeout{ps.timeout},
            start{std::chrono::high_resolution_clock::now()},
            num_search_space_graphs{calculate_num_search_space_graphs(ps.mode, ps.cost)},
            ssg_vec(num_search_space_graphs)
    {
        ntk.substitute_po_signals();
    }
    /**
     * Executes the graph-oriented layout design algorithm and returns the best found layout.
     *
     * @return The best layout found by the algorithm.
     */
    std::optional<Lyt> run() noexcept
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        // initialize layout to keep track of current best solution
        Lyt best_lyt{{}, twoddwave_clocking<Lyt>()};

        // initialize search space graphs
        initialize();

        // check if a timeout was set
        bool timeout_set = (timeout != std::numeric_limits<uint64_t>::max());

        // if no timeout was set and high-effort mode is disabled, set timeout to 10s
        if (!timeout_set && (ps.mode == graph_oriented_layout_design_params::effort_mode::LOW_EFFORT))
        {
            timeout = 10000u;
        }
        bool timeout_limit_reached = false;

        // main A* loop
        while (!timeout_limit_reached)
        {
            for (auto& ssg : ssg_vec)
            {
                if (ssg.frontier_flag)
                {
                    num_evaluated_paths++;
                    const auto expansion = expand(ssg);
                    if (expansion.second)
                    {
                        best_lyt = *expansion.second;
                        restore_names(ssg.network, best_lyt);

                        // statistical information
                        pst.x_size        = best_lyt.x() + 1;
                        pst.y_size        = best_lyt.y() + 1;
                        pst.num_gates     = best_lyt.num_gates();
                        pst.num_wires     = best_lyt.num_wires();
                        pst.num_crossings = best_lyt.num_crossings();

                        if (ps.return_first)
                        {
                            return best_lyt;
                        }
                    }
                    for (const auto& [next, cost] : expansion.first)
                    {
                        if (ssg.cost_so_far.find(next) == ssg.cost_so_far.cend() || cost < ssg.cost_so_far[next])
                        {
                            ssg.cost_so_far[next] = cost;
                            double priority       = cost;
                            ssg.frontier.put(next, priority);
                        }
                    }
                }
            }
            for (auto& ssg : ssg_vec)
            {
                if (ssg.frontier_flag)
                {
                    if (!ssg.frontier.empty())
                    {
                        ssg.current_vertex = ssg.frontier.get();
                    }
                    else
                    {
                        ssg.frontier_flag = false;
                    }
                }
            }

            timeout_limit_reached =
                std::none_of(ssg_vec.cbegin(), ssg_vec.cend(), [](const auto& ssg) { return ssg.frontier_flag; });

            const auto end = std::chrono::high_resolution_clock::now();
            const auto duration_ms =
                static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

            if (duration_ms >= timeout)
            {
                // Terminate the algorithm if the specified timeout was set or a solution was found in low-effort mode
                if (timeout_set || (ps.mode == graph_oriented_layout_design_params::effort_mode::LOW_EFFORT &&
                                    found_solution(ps.cost)))
                {
                    timeout_limit_reached = true;
                }
            }
        }

        // check if any layout was found
        if (found_solution(ps.cost))
        {
            return best_lyt;
        }
        return std::nullopt;
    }

  private:
    /**
     * Alias for an obstruction layout based on the given layout type.
     */
    using ObstrLyt = obstruction_layout<Lyt>;
    /**
     * The network to be placed and routed.
     */
    tec_nt ntk;
    /**
     * Parameters.
     */
    graph_oriented_layout_design_params ps;
    /**
     * Statistics.
     */
    graph_oriented_layout_design_stats& pst;
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
     * Vector of search space graphs.
     */
    std::vector<search_space_graph<ObstrLyt>> ssg_vec;
    /**
     * Count evaluated paths in the search space graphs.
     */
    uint64_t num_evaluated_paths{0ul};
    /**
     * Keep track of the maximum number of placed nodes.
     */
    uint64_t max_placed_nodes{0ul};
    /**
     * The current best solution with respect to area, initialized to the maximum possible value.
     * This value will be updated as better solutions are found.
     */
    uint64_t best_area_solution = std::numeric_limits<uint64_t>::max();
    /**
     * The current best solution with respect to the number of wire segments, initialized to the maximum possible value.
     * This value will be updated as better solutions are found.
     */
    uint64_t best_wire_solution = std::numeric_limits<uint64_t>::max();
    /**
     * The current best solution with respect to the number of crossings, initialized to the maximum possible
     * value. This value will be updated as better solutions are found.
     */
    uint64_t best_crossing_solution = std::numeric_limits<uint64_t>::max();
    /**
     * The current best solution with respect to the area-crossing product (ACP), initialized to the maximum possible
     * value. This value will be updated as better solutions are found.
     */
    uint64_t best_acp_solution = std::numeric_limits<uint64_t>::max();
    /**
     * The current best solution with respect to a custom cost objective, initialized to the maximum possible value.
     * This value will be updated as better solutions are found.
     */
    uint64_t best_custom_solution = std::numeric_limits<uint64_t>::max();
    /**
     * Current best solution w.r.t. area after relocating POs.
     */
    uint64_t best_optimized_solution{std::numeric_limits<uint64_t>::max()};
    /**
     * Flag indicating if an initial solution has been found with the layout area as cost objective.
     * When set to `true`, subsequent search space graphs with the layout area as cost objective can be pruned.
     */
    bool improve_area_solution = false;
    /**
     * Flag indicating if an initial solution has been found with the number of wire segments as cost objective.
     * When set to `true`, subsequent search space graphs with the number of wire segments as cost objective can be
     * pruned.
     */
    bool improve_wire_solution = false;
    /**
     * Flag indicating if an initial solution has been found with the number of crossings as cost objective.
     * When set to `true`, subsequent search space graphs with the number of crossings as cost objective can be pruned.
     */
    bool improve_crossing_solution = false;
    /**
     * Flag indicating if an initial solution has been found with the area-crossings product as cost objective.
     * When set to `true`, subsequent search space graphs with the area-crossing product as cost objective can be
     * pruned.
     */
    bool improve_acp_solution = false;
    /**
     * Flag indicating if an initial solution has been found with a custom cost objective.
     * When set to `true`, subsequent search space graphs with a custom cost objective can be pruned.
     */
    bool improve_custom_solution = false;

    /**
     * Determines the number of search space graphs to generate based on the selected effort mode and cost objective.
     *
     * @param mode The effort mode chosen for the layout design, determining the level of computational effort.
     * @param cost The cost that specifies the optimization objective for the layout design.
     * @return The number of search space graphs to be generated.
     */
    std::uint32_t calculate_num_search_space_graphs(graph_oriented_layout_design_params::effort_mode    mode,
                                                    graph_oriented_layout_design_params::cost_objective cost) const
    {
        if (mode == graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT)
        {
            return (cost == graph_oriented_layout_design_params::cost_objective::CUSTOM) ? 60u : 48u;
        }
        return (mode == graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT) ? 12u : 3u;
    }
    /**
     * Checks if a solution has been found for the specified cost objective.
     *
     * @param cost The cost objective used.
     * @return True if a solution was found for the given cost objective; otherwise, false.
     */
    bool found_solution(graph_oriented_layout_design_params::cost_objective cost) const
    {
        switch (cost)
        {
            case graph_oriented_layout_design_params::cost_objective::AREA: return improve_area_solution;
            case graph_oriented_layout_design_params::cost_objective::WIRES: return improve_wire_solution;
            case graph_oriented_layout_design_params::cost_objective::CROSSINGS: return improve_crossing_solution;
            case graph_oriented_layout_design_params::cost_objective::ACP: return improve_acp_solution;
            case graph_oriented_layout_design_params::cost_objective::CUSTOM: return improve_custom_solution;
            default: return false;
        }
    }
    /**
     * Checks if there is a path between the source and destination tiles in the given layout.
     *
     * @param layout The layout to be checked.
     * @param src The source tile.
     * @param dest The destination tile.
     * @param new_gate_loc Enum indicating if the src or dest have to host a new gate and therefore have to be empty.
     * Defaults to `new_gate_location::NONE`.
     * @param planar Only consider crossing-free paths.
     * @return A path from `src` to `dest` if one exists.
     */
    [[nodiscard]] layout_coordinate_path<ObstrLyt>
    check_path(const ObstrLyt& layout, const tile<ObstrLyt>& src, const tile<ObstrLyt>& dest,
               const new_gate_location new_gate_loc = new_gate_location::NONE, const bool planar = false) noexcept
    {
        const bool src_is_new_pos  = (new_gate_loc == new_gate_location::SRC);
        const bool dest_is_new_pos = (new_gate_loc == new_gate_location::DEST);

        if ((layout.is_empty_tile(src) && src_is_new_pos) || (layout.is_empty_tile(dest) && dest_is_new_pos) ||
            (new_gate_loc == new_gate_location::NONE))
        {
            using dist = twoddwave_distance_functor<ObstrLyt, uint64_t>;
            using cost = unit_cost_functor<ObstrLyt, uint8_t>;

            static a_star_params a_star_crossing_params{};
            a_star_crossing_params.crossings = !planar;

            return a_star<layout_coordinate_path<ObstrLyt>>(layout, {src, dest}, dist(), cost(),
                                                            a_star_crossing_params);
        }

        return {};
    }
    /**
     * Retrieves the possible positions for Primary Inputs (PIs) in the given layout based on the specified
     * criteria of positioning at the top or left side, with a limit on the number of possible positions.
     *
     * @param layout The layout in which to find the possible positions for PIs.
     * @param pi_locs Struct indicating if PIs are allowed at the top or left side of the layout.
     * @param num_expansions The maximum number of positions to be returned (is doubled for PIs).
     * @param planar Only consider crossing-free paths.
     * @return A vector of tiles representing the possible positions for PIs.
     */
    [[nodiscard]] coord_vec_type<ObstrLyt> get_possible_positions_pis(ObstrLyt& layout, const pi_locations& pi_locs,
                                                                      const uint64_t num_expansions,
                                                                      const bool     planar = false) noexcept
    {
        uint64_t count_expansions = 0ul;

        coord_vec_type<ObstrLyt> possible_positions{};

        layout.resize({layout.x() + 1, layout.y() + 1, 1});
        const tile<ObstrLyt> drain{layout.x(), layout.y(), 0};

        // check if a path from the input to the drain exists
        const auto check_tile = [&](const uint64_t x, const uint64_t y) noexcept
        {
            const tile<ObstrLyt> tile{x, y, 0};
            if (!check_path(layout, tile, drain, new_gate_location::SRC, planar).empty())
            {
                count_expansions++;
                possible_positions.push_back(tile);
            }
        };

        uint64_t max_iterations = 0;

        if (pi_locs == pi_locations::TOP_AND_LEFT)
        {
            max_iterations = std::max(layout.x(), layout.y());
        }
        else if (pi_locs == pi_locations::TOP)
        {
            max_iterations = layout.x();
        }
        else
        {
            max_iterations = layout.y();
        }

        const uint64_t expansion_limit = (pi_locs == pi_locations::TOP_AND_LEFT) ? 2 * num_expansions : num_expansions;
        possible_positions.reserve(expansion_limit);

        for (uint64_t k = 0ul; k < max_iterations; k++)
        {
            if (((pi_locs == pi_locations::TOP) || (pi_locs == pi_locations::TOP_AND_LEFT)) && k < layout.x())
            {
                check_tile(k, 0);
            }
            if (((pi_locs == pi_locations::LEFT) || (pi_locs == pi_locations::TOP_AND_LEFT)) && k < layout.y())
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
     * @param layout The layout in which to find the possible positions for POs.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param fc A vector of nodes that precede the PO nodes.
     * @param planar Only consider crossing-free paths.
     * @return A vector of tiles representing the possible positions for POs.
     */
    [[nodiscard]] coord_vec_type<ObstrLyt> get_possible_positions_pos(const ObstrLyt&                 layout,
                                                                      const placement_info<ObstrLyt>& place_info,
                                                                      const fanin_container<tec_nt>&  fc,
                                                                      const bool planar = false) noexcept
    {
        coord_vec_type<ObstrLyt> possible_positions{};

        const auto& pre             = fc.fanin_nodes[0];
        const auto  pre_t           = static_cast<tile<ObstrLyt>>(place_info.node2pos[pre]);
        const auto  expansion_limit = std::max(layout.x() - pre_t.x, layout.y() - pre_t.y);

        possible_positions.reserve(expansion_limit);

        // check if path from previous tile to PO exists
        auto check_tile = [&](const uint64_t x, const uint64_t y)
        {
            const tile<ObstrLyt> tile{x, y, 0};
            if (!check_path(layout, pre_t, tile, new_gate_location::DEST, planar).empty())
            {
                possible_positions.push_back(tile);
            }
        };

        for (uint64_t k = 0ul; k <= expansion_limit; ++k)
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
     * @param layout The layout in which to find the possible positions for a single fan-in node.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param num_expansions The maximum number of positions to be returned.
     * @param fc A vector of nodes that precede the single fanin node.
     * @param planar Only consider crossing-free paths.
     * @return A vector of tiles representing the possible positions for a single fan-in node.
     */
    [[nodiscard]] coord_vec_type<ObstrLyt>
    get_possible_positions_single_fanin(ObstrLyt& layout, const placement_info<ObstrLyt>& place_info,
                                        const uint64_t num_expansions, const fanin_container<tec_nt>& fc,
                                        const bool planar = false) noexcept
    {
        coord_vec_type<ObstrLyt> possible_positions{};
        possible_positions.reserve(num_expansions);

        uint64_t count_expansions = 0ul;

        const auto& pre   = fc.fanin_nodes[0];
        const auto  pre_t = static_cast<tile<ObstrLyt>>(place_info.node2pos[pre]);

        // check if path from previous tile to new tile and from new tile to drain exist
        const auto check_tile = [&](const uint64_t x, const uint64_t y) noexcept
        {
            const tile<ObstrLyt> new_pos{pre_t.x + x, pre_t.y + y, 0};

            if (!check_path(layout, pre_t, new_pos, new_gate_location::DEST, planar).empty())
            {
                layout.resize({layout.x() + 1, layout.y() + 1, 1});
                const tile<ObstrLyt> drain{layout.x(), layout.y(), 0};

                if (!check_path(layout, new_pos, drain, new_gate_location::SRC, planar).empty())
                {
                    possible_positions.push_back(new_pos);
                    count_expansions++;
                }

                layout.resize({layout.x() - 1, layout.y() - 1, 1});
            }
        };

        // iterate diagonally
        for (uint64_t k = 0ul; k < layout.x() + layout.y() + 1; ++k)
        {
            for (uint64_t x = 0ul; x < k + 1; ++x)
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
     * @param layout The layout in which to find the possible positions for a double fan-in node.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param num_expansions The maximum number of positions to be returned.
     * @param fc A vector of nodes that precede the double fanin node.
     * @param planar Only consider crossing-free paths.
     * @return A vector of tiles representing the possible positions for a double fan-in node.
     */
    [[nodiscard]] coord_vec_type<ObstrLyt>
    get_possible_positions_double_fanin(ObstrLyt& layout, const placement_info<ObstrLyt>& place_info,
                                        const uint64_t num_expansions, const fanin_container<tec_nt>& fc,
                                        const bool planar = false) noexcept
    {
        coord_vec_type<ObstrLyt> possible_positions{};
        possible_positions.reserve(num_expansions);
        uint64_t count_expansions = 0ul;

        const auto& pre1 = fc.fanin_nodes[0];
        const auto& pre2 = fc.fanin_nodes[1];

        const auto pre1_t = static_cast<tile<ObstrLyt>>(place_info.node2pos[pre1]);
        const auto pre2_t = static_cast<tile<ObstrLyt>>(place_info.node2pos[pre2]);

        const auto min_x = std::max(pre1_t.x, pre2_t.x) + (pre1_t.x == pre2_t.x ? 1 : 0);
        const auto min_y = std::max(pre1_t.y, pre2_t.y) + (pre1_t.y == pre2_t.y ? 1 : 0);

        // check if path from previous tiles to new tile and from new tile to drain exist
        auto check_tile = [&](uint64_t x, uint64_t y)
        {
            const tile<ObstrLyt> new_pos{min_x + x, min_y + y, 0};

            const auto path = check_path(layout, pre1_t, new_pos, new_gate_location::DEST, planar);
            if (!path.empty())
            {
                for (const auto& el : path)
                {
                    layout.obstruct_coordinate(el);
                }

                if (!check_path(layout, pre2_t, new_pos, new_gate_location::DEST, planar).empty())
                {
                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    const tile<ObstrLyt> drain{layout.x(), layout.y(), 0};

                    if (!check_path(layout, new_pos, drain, new_gate_location::SRC, planar).empty())
                    {
                        possible_positions.push_back(new_pos);
                        count_expansions++;
                    }

                    layout.resize({layout.x() - 1, layout.y() - 1, 1});
                }

                for (const auto& el : path)
                {
                    layout.clear_obstructed_coordinate(el);
                }
            }
        };

        // iterate diagonally
        for (uint64_t k = 0ul; k < layout.x() + layout.y() + 1; ++k)
        {
            for (uint64_t x = 0ul; x < k + 1; ++x)
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
     * @param layout The layout in which to find the possible positions.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param ssg The search space graph.
     * @return A vector of tiles representing the possible positions for the current node.
     */
    [[nodiscard]] coord_vec_type<ObstrLyt> get_possible_positions(ObstrLyt&                           layout,
                                                                  const search_space_graph<ObstrLyt>& ssg,
                                                                  const placement_info<ObstrLyt>& place_info) noexcept
    {
        const auto fc = fanins(ssg.network, ssg.nodes_to_place[place_info.current_node]);

        if (ssg.network.is_pi(ssg.nodes_to_place[place_info.current_node]))
        {
            return get_possible_positions_pis(layout, ssg.pi_locs, ssg.network.num_pis(), ssg.planar);
        }
        if (ssg.network.is_po(ssg.nodes_to_place[place_info.current_node]))
        {
            return get_possible_positions_pos(layout, place_info, fc, ssg.planar);
        }
        if (fc.fanin_nodes.size() == 1)
        {
            return get_possible_positions_single_fanin(layout, place_info, ssg.num_expansions, fc, ssg.planar);
        }

        return get_possible_positions_double_fanin(layout, place_info, ssg.num_expansions, fc, ssg.planar);
    }
    /**
     * Validates the given layout based on the nodes in the network and their mappings in the node dictionary.
     * It checks if the placement of nodes in the layout is possible and ensures there are valid paths from each tile to
     * the drain.
     *
     * @param layout The layout to be validated.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param ssg The search space graph.
     */
    [[nodiscard]] bool valid_layout(ObstrLyt& layout, const search_space_graph<ObstrLyt>& ssg,
                                    const placement_info<ObstrLyt>& place_info) noexcept
    {
        const auto check_tile = [&](const auto& t) noexcept
        {
            layout.resize({layout.x() + 1, layout.y() + 1, 1});
            const tile<ObstrLyt> drain{layout.x(), layout.y(), 0};

            const bool path_exists = !check_path(layout, t, drain, new_gate_location::DEST, ssg.planar).empty();
            layout.resize({layout.x() - 1, layout.y() - 1, 1});

            return path_exists;
        };

        const auto is_empty_tile_or_crossable = [&](const auto& t) noexcept
        {
            return layout.is_empty_tile(t) ||
                   (layout.is_empty_tile({t.x, t.y, 1}) && !layout.is_obstructed_coordinate({t.x, t.y, 1}));
        };

        for (uint64_t node = 0ul; node < place_info.current_node; node++)
        {
            const auto layout_tile = static_cast<tile<ObstrLyt>>(place_info.node2pos[ssg.nodes_to_place[node]]);
            const bool no_fanout_and_not_po =
                !layout.is_po_tile(layout_tile) && (layout.fanout_size(layout.get_node(layout_tile)) == 0);
            const bool one_dangling_fanout = (layout.fanout_size(layout.get_node(layout_tile)) == 1) &&
                                             ssg.network.is_fanout(ssg.nodes_to_place[node]);

            if (no_fanout_and_not_po || one_dangling_fanout)
            {
                if (!check_tile(layout_tile))
                {
                    return false;
                }
            }

            const bool two_dangling_fanouts = (layout.fanout_size(layout.get_node(layout_tile)) == 0) &&
                                              ssg.network.is_fanout(ssg.nodes_to_place[node]);

            if (two_dangling_fanouts)
            {
                tile<ObstrLyt> right_tile{layout_tile.x + 1, layout_tile.y, 0};
                tile<ObstrLyt> bottom_tile{layout_tile.x, layout_tile.y + 1, 0};

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
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param fc A vector of nodes that precede the single fanin node.
     * @param planar Only consider crossing-free paths.
     */
    void route_single_input_node(const tile<ObstrLyt>& position, ObstrLyt& layout,
                                 node_dict_type<ObstrLyt, tec_nt>& node2pos, const fanin_container<tec_nt>& fc,
                                 const bool planar = false) noexcept
    {
        const auto& pre   = fc.fanin_nodes[0];
        const auto  pre_t = static_cast<tile<ObstrLyt>>(node2pos[pre]);

        layout.move_node(layout.get_node(position), position, {});

        const auto path = check_path(layout, pre_t, position, new_gate_location::NONE, planar);
        assert(!path.empty());

        route_path(layout, path);

        for (const auto& el : path)
        {
            layout.obstruct_coordinate(el);
        }
    }
    /**
     * Places a node with two inputs in the layout and routes it.
     *
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param node2pos A dictionary mapping nodes from the network to signals in the layout.
     * @param fc A vector of nodes that precede the double fanin node.
     * @param planar Only consider crossing-free paths.
     */
    void route_double_input_node(const tile<ObstrLyt>& position, ObstrLyt& layout,
                                 node_dict_type<ObstrLyt, tec_nt>& node2pos, const fanin_container<tec_nt>& fc,
                                 const bool planar = false) noexcept
    {
        const auto& pre1 = fc.fanin_nodes[0];
        const auto& pre2 = fc.fanin_nodes[1];

        const auto pre1_t = static_cast<tile<ObstrLyt>>(node2pos[pre1]);
        const auto pre2_t = static_cast<tile<ObstrLyt>>(node2pos[pre2]);

        layout.move_node(layout.get_node(position), position, {});

        const auto path_1 = check_path(layout, pre1_t, position, new_gate_location::NONE, planar);
        assert(!path_1.empty());

        for (const auto& el : path_1)
        {
            layout.obstruct_coordinate(el);
        }

        const auto path_2 = check_path(layout, pre2_t, position, new_gate_location::NONE, planar);
        assert(!path_2.empty());

        for (const auto& el : path_2)
        {
            layout.obstruct_coordinate(el);
        }

        route_path(layout, path_1);
        route_path(layout, path_2);
    }
    /**
     * Executes a single placement step in the layout for the given network node. It determines the type of the node,
     * places it accordingly, and checks if a solution was found.
     *
     * @param position The tile representing the position for placement.
     * @param layout The layout in which to place the node.
     * @param place_info The placement context containing current node, primary output index, node to position mapping,
     * and PI to node mapping.
     * @param ssg The search space graph.
     * @return A boolean indicating if a solution was found.
     */
    [[nodiscard]] bool place_and_route(const tile<ObstrLyt>& position, ObstrLyt& layout,
                                       search_space_graph<ObstrLyt>& ssg, placement_info<ObstrLyt>& place_info) noexcept
    {
        // vector to store preceding nodes
        const auto fc = fanins(ssg.network, ssg.nodes_to_place[place_info.current_node]);

        if (ssg.network.is_pi(ssg.nodes_to_place[place_info.current_node]))
        {
            // place primary input node
            place_info.node2pos[ssg.nodes_to_place[place_info.current_node]] =
                layout.move_node(place_info.pi2node[ssg.nodes_to_place[place_info.current_node]], position);
        }
        else if (fc.fanin_nodes.size() == 1)
        {
            const auto& pre = fc.fanin_nodes[0];

            // place single input node
            if (ssg.network.is_po(ssg.nodes_to_place[place_info.current_node]))
            {
                place_info.node2pos[ssg.nodes_to_place[place_info.current_node]] =
                    layout.create_po(place_info.node2pos[pre], fmt::format("po{}", place_info.current_po++), position);
            }
            else
            {
                const auto pre_t = static_cast<tile<ObstrLyt>>(place_info.node2pos[pre]);
                auto       a     = static_cast<mockturtle::signal<ObstrLyt>>(pre_t);

                place_info.node2pos[ssg.nodes_to_place[place_info.current_node]] =
                    place(layout, position, ssg.network, ssg.nodes_to_place[place_info.current_node], a);
            }

            route_single_input_node(position, layout, place_info.node2pos, fc, ssg.planar);
        }
        else
        {
            // place double input node
            const auto& pre1 = fc.fanin_nodes[0];
            const auto& pre2 = fc.fanin_nodes[1];

            const auto a1 = static_cast<mockturtle::signal<ObstrLyt>>(pre1);
            const auto a2 = static_cast<mockturtle::signal<ObstrLyt>>(pre2);

            place_info.node2pos[ssg.nodes_to_place[place_info.current_node]] = place(
                layout, position, ssg.network, ssg.nodes_to_place[place_info.current_node], a1, a2, fc.constant_fanin);

            route_double_input_node(position, layout, place_info.node2pos, fc, ssg.planar);
        }

        place_info.current_node++;
        layout.obstruct_coordinate({position.x, position.y, 0});
        layout.obstruct_coordinate({position.x, position.y, 1});

        // check if solution was found and update max_placed_nodes
        const auto found_solution = (place_info.current_node == ssg.nodes_to_place.size());
        max_placed_nodes          = std::max(place_info.current_node, max_placed_nodes);

        return found_solution;
    }
    /**
     * Outputs placement information, including the current runtime, the number of evaluated paths in the search space
     * graphs and the layout dimensions.
     *
     * @param lyt Current layout.
     */
    void print_placement_info(const ObstrLyt& lyt) const
    {
        std::cout << "\n[i] Found improved solution:\n";

        // calculate the duration between start and end
        const auto end      = std::chrono::high_resolution_clock::now();
        const auto duration = end - start;

        // extract the duration components
        const auto us  = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;
        const auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        // output the elapsed time
        std::cout << fmt::format("[i]   Time taken:       {} s {} ms {} µs\n", sec, ms, us);
        std::cout << fmt::format("[i]   Evaluated paths:  {}\n", num_evaluated_paths);
        std::cout << fmt::format("[i]   Layout dimension: {} × {} = {}\n", lyt.x() + 1, lyt.y() + 1, lyt.area());
        std::cout << fmt::format("[i]   #Wires: {}\n", lyt.num_wires() - lyt.num_pis() - lyt.num_pos());
        std::cout << fmt::format("[i]   #Crossings: {}\n", lyt.num_crossings());
        std::cout << fmt::format("[i]   ACP: {}\n", lyt.area() * (lyt.num_crossings() + 1));
    }
    /**
     * Initializes the layout with minimum width
     *
     * @param min_layout_width The minimum width of the layout.
     * @param planar Create planar layouts with a depth of 0.
     * @return The initialized layout.
     */
    ObstrLyt initialize_layout(uint64_t min_layout_width, const bool planar = false)
    {
        const auto layout_depth = planar ? 0 : 1;
        Lyt        lyt{{min_layout_width - 1, 0, layout_depth}, twoddwave_clocking<Lyt>()};
        return obstruction_layout<Lyt>(lyt);
    }
    /**
     * Adjusts the layout size based on the last position.
     *
     * @param position The last position in the layout.
     * @param layout The layout to be adjusted.
     * @param ssg The search space graph.
     * @param place_info The placement information.
     */
    void adjust_layout_size(const tile<ObstrLyt>& position, ObstrLyt& layout, const search_space_graph<ObstrLyt>& ssg,
                            const placement_info<ObstrLyt>& place_info)
    {
        const auto max_layout_width  = ssg.nodes_to_place.size();
        const auto max_layout_height = ssg.nodes_to_place.size();

        if (position.x == layout.x() && layout.x() < (max_layout_width - 1) &&
            !ssg.network.is_po(ssg.nodes_to_place[place_info.current_node - 1]))
        {
            layout.resize({layout.x() + 1, layout.y(), layout.z()});
        }
        if (position.y == layout.y() && layout.y() < (max_layout_height - 1) &&
            !ssg.network.is_po(ssg.nodes_to_place[place_info.current_node - 1]))
        {
            layout.resize({layout.x(), layout.y() + 1, layout.z()});
        }
    }
    std::uint64_t calculate_cost(const Lyt& layout, graph_oriented_layout_design_params::cost_objective cost_function)
    {
        uint64_t cost = 0;
        if (cost_function == graph_oriented_layout_design_params::cost_objective::AREA)
        {
            const auto bb = bounding_box_2d(layout);
            cost          = static_cast<uint64_t>(bb.get_max().x + 1u) * static_cast<uint64_t>(bb.get_max().y + 1u);
        }
        else if (cost_function == graph_oriented_layout_design_params::cost_objective::WIRES)
        {
            cost = layout.num_wires() - layout.num_pis() - layout.num_pos();
        }
        else if (cost_function == graph_oriented_layout_design_params::cost_objective::CROSSINGS)
        {
            cost = layout.num_crossings();
        }
        else if (cost_function == graph_oriented_layout_design_params::cost_objective::ACP)
        {
            const auto bb = bounding_box_2d(layout);
            cost          = (layout.num_crossings() + 1) *
                   (static_cast<uint64_t>(bb.get_max().x + 1u) * static_cast<uint64_t>(bb.get_max().y + 1u));
        }
        else if (cost_function == graph_oriented_layout_design_params::cost_objective::CUSTOM)
        {
            // define your cost function here (has to be a positive integer of type uint64_t)
            cost = 1u;
        }
        return cost;
    }
    /**
     * Generates the next possible positions with their priorities based on the layout and search space graph.
     *
     * @param possible_positions A vector of possible positions to be considered.
     * @param layout The layout to be used.
     * @param ssg The search space graph.
     * @return A pair containing the next positions with their priorities and an optional layout.
     */
    std::pair<std::vector<std::pair<coord_vec_type<ObstrLyt>, double>>, std::optional<ObstrLyt>>
    generate_next_positions(const coord_vec_type<ObstrLyt>& possible_positions, ObstrLyt& layout,
                            const search_space_graph<ObstrLyt>& ssg)
    {
        std::vector<std::pair<coord_vec_type<ObstrLyt>, double>> next_positions;
        next_positions.reserve(2 * ssg.num_expansions);

        for (const auto& position : possible_positions)
        {
            auto new_sequence = ssg.current_vertex;
            new_sequence.push_back(position);

            const auto remaining_nodes_to_place =
                static_cast<double>(ssg.nodes_to_place.size() - (ssg.current_vertex.size() + 1));

            if (ssg.cost == graph_oriented_layout_design_params::cost_objective::AREA)
            {
                // current layout size
                const double cost1 = static_cast<double>(((std::max(layout.x() - 1, position.x) + 1) *
                                                          (std::max(layout.y() - 1, position.y) + 1))) /
                                     static_cast<double>((ssg.nodes_to_place.size() * ssg.nodes_to_place.size()));

                // position of last placed node
                const double cost2 = static_cast<double>(((position.x + 1) * (position.y + 1))) /
                                     static_cast<double>((ssg.nodes_to_place.size() * ssg.nodes_to_place.size()));

                double priority = remaining_nodes_to_place + cost1 + cost2;
                next_positions.push_back({new_sequence, priority});
            }
            else
            {
                uint64_t cost = calculate_cost(layout, ssg.cost);

                const double cost1 = static_cast<double>(cost) / static_cast<double>(1000 * ssg.nodes_to_place.size());

                double priority = remaining_nodes_to_place + cost1;

                next_positions.push_back({new_sequence, priority});
            }
        }

        return {next_positions, std::nullopt};
    }
    /**
     * Computes possible expansions and their priorities for the current vertex in the search space graph.
     * It handles placement of nodes, checks for valid paths, and finds potential next positions based on priorities.
     *
     * @param ssg The search space graph.
     * @return A pair containing a vector of next positions with their priorities and an optional layout.
     * If an improved solution is found, the layout is returned.
     * If the layout is invalid or no improvement is possible, std::nullopt is returned.
     */
    [[nodiscard]] std::pair<std::vector<std::pair<coord_vec_type<ObstrLyt>, double>>, std::optional<ObstrLyt>>
    expand(search_space_graph<ObstrLyt>& ssg) noexcept
    {
        const auto min_layout_width = ssg.network.num_pis();

        std::vector<std::pair<coord_vec_type<ObstrLyt>, double>> next_positions;
        next_positions.reserve(2 * ssg.num_expansions);

        auto layout = initialize_layout(min_layout_width, ssg.planar);

        auto                             pi2node = reserve_input_nodes(layout, ssg.network);
        node_dict_type<ObstrLyt, tec_nt> node2pos{ssg.network};
        placement_info<ObstrLyt>         place_info{0ul, 0ul, node2pos, pi2node};

        coord_vec_type<ObstrLyt> possible_positions{};
        possible_positions.reserve(2 * ssg.num_expansions);

        if (ssg.current_vertex.empty())
        {
            possible_positions = get_possible_positions(layout, ssg, place_info);
        }

        for (uint64_t idx = 0ul; idx < ssg.current_vertex.size(); ++idx)
        {
            const auto position = ssg.current_vertex[idx];

            bool found_solution = place_and_route(position, layout, ssg, place_info);

            uint64_t cost         = 0ul;
            uint64_t desired_cost = 0ul;

            bool     improve_solution = improve_custom_solution;
            uint64_t best_solution    = best_custom_solution;

            switch (ssg.cost)
            {
                case graph_oriented_layout_design_params::cost_objective::AREA:
                    improve_solution = improve_area_solution;
                    best_solution    = best_area_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::WIRES:
                    improve_solution = improve_wire_solution;
                    best_solution    = best_wire_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::CROSSINGS:
                    improve_solution = improve_crossing_solution;
                    best_solution    = best_crossing_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::ACP:
                    improve_solution = improve_acp_solution;
                    best_solution    = best_acp_solution;
                    break;
                default: break;
            }

            if (improve_solution)
            {
                cost = calculate_cost(layout, ssg.cost);
            }

            if (found_solution && (!improve_solution || cost <= best_solution))
            {
                cost = calculate_cost(layout, ssg.cost);

                switch (ssg.cost)
                {
                    case graph_oriented_layout_design_params::cost_objective::AREA:
                        improve_area_solution = true;
                        best_area_solution    = cost;
                        break;
                    case graph_oriented_layout_design_params::cost_objective::WIRES:
                        improve_wire_solution = true;
                        best_wire_solution    = cost;
                        break;
                    case graph_oriented_layout_design_params::cost_objective::CROSSINGS:
                        improve_crossing_solution = true;
                        best_crossing_solution    = cost;
                        break;
                    case graph_oriented_layout_design_params::cost_objective::ACP:
                        improve_acp_solution = true;
                        best_acp_solution    = cost;
                        break;
                    default:
                        improve_custom_solution = true;
                        best_custom_solution    = cost;
                        break;
                }

                fiction::post_layout_optimization_params plo_params{};
                plo_params.optimize_pos_only   = true;
                plo_params.planar_optimization = ssg.planar;

                fiction::post_layout_optimization(layout, plo_params);

                const auto bb_after_plo = fiction::bounding_box_2d(layout);
                layout.resize({bb_after_plo.get_max().x, bb_after_plo.get_max().y, layout.z()});

                desired_cost = calculate_cost(layout, ps.cost);

                if (desired_cost < best_optimized_solution)
                {
                    best_optimized_solution = desired_cost;

                    if (ps.verbose)
                    {
                        print_placement_info(layout);
                    }

                    return {{}, layout};
                }

                return {{}, std::nullopt};
            }

            improve_solution = improve_custom_solution;
            best_solution    = best_custom_solution;

            switch (ssg.cost)
            {
                case graph_oriented_layout_design_params::cost_objective::AREA:
                    improve_solution = improve_area_solution;
                    best_solution    = best_area_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::WIRES:
                    improve_solution = improve_wire_solution;
                    best_solution    = best_wire_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::CROSSINGS:
                    improve_solution = improve_crossing_solution;
                    best_solution    = best_crossing_solution;
                    break;
                case graph_oriented_layout_design_params::cost_objective::ACP:
                    improve_solution = improve_acp_solution;
                    best_solution    = best_acp_solution;
                    break;
                default: break;
            }
            if (improve_solution && cost >= best_solution)
            {
                return {{}, std::nullopt};
            }

            adjust_layout_size(position, layout, ssg, place_info);
            // check if it's the last position in the current vertex
            if (idx == (ssg.current_vertex.size() - 1))
            {
                if (!valid_layout(layout, ssg, place_info))
                {
                    return {{}, std::nullopt};
                }

                possible_positions = get_possible_positions(layout, ssg, place_info);
            }
        }

        return generate_next_positions(possible_positions, layout, ssg);
    }
    /**
     * Initializes the allowed positions for primary inputs (PIs), the cost for each search space graph and the maximum
     * number of expansions.
     */
    void initialize_pis_cost_and_num_expansions() noexcept
    {
        static constexpr std::array<pi_locations, 60> pi_locs = {
            {pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT,
             pi_locations::TOP, pi_locations::LEFT, pi_locations::TOP_AND_LEFT}};

        auto pi_loc_it = pi_locs.cbegin();
        for (auto& graph : ssg_vec)
        {
            if (pi_loc_it != pi_locs.cend())
            {
                graph.pi_locs = *pi_loc_it;
                ++pi_loc_it;
            }

            graph.cost_so_far[graph.current_vertex] = 0;
            graph.num_expansions                    = ps.num_vertex_expansions;
            graph.planar                            = ps.planar;
        }
    }
    /**
     * Initializes the networks and nodes to place.
     */
    void initialize_networks_and_nodes_to_place() noexcept
    {
        // prepare nodes to place for each topology view
        const auto prepare_nodes_to_place = [](auto& network, auto& nodes_to_place) noexcept
        {
            network.foreach_node(
                [&nodes_to_place, &network](const auto& n)
                {
                    if (!network.is_constant(n) && !network.is_po(n))
                    {
                        nodes_to_place.push_back(n);
                    }
                });

            network.foreach_co(
                [&](const auto& f)
                {
                    auto n = network.get_node(f);
                    nodes_to_place.push_back(n);
                });
        };

        fanout_substitution_params params{};
        params.strategy = fanout_substitution_params::substitution_strategy::BREADTH;
        mockturtle::fanout_view network_substituted_breadth{fanout_substitution<tec_nt>(ntk, params)};

        topo_view_co_to_ci network_breadth_co_to_ci{network_substituted_breadth};

        // initialize search space graphs with networks
        ssg_vec[0].network = network_breadth_co_to_ci;
        ssg_vec[1].network = network_breadth_co_to_ci;
        ssg_vec[2].network = network_breadth_co_to_ci;

        std::vector<mockturtle::node<decltype(network_breadth_co_to_ci)>> nodes_to_place_breadth_co_to_ci{};
        prepare_nodes_to_place(network_breadth_co_to_ci, nodes_to_place_breadth_co_to_ci);

        ssg_vec[0].nodes_to_place = nodes_to_place_breadth_co_to_ci;
        ssg_vec[1].nodes_to_place = nodes_to_place_breadth_co_to_ci;
        ssg_vec[2].nodes_to_place = nodes_to_place_breadth_co_to_ci;

        for (uint64_t i = 0ul; i < 3; ++i)
        {
            ssg_vec[i].cost = ps.cost;
        }

        if ((ps.mode == graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT) ||
            (ps.mode == graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT))
        {
            params.strategy = fanout_substitution_params::substitution_strategy::DEPTH;
            mockturtle::fanout_view network_substituted_depth{fanout_substitution<tec_nt>(ntk, params)};

            topo_view_ci_to_co network_breadth_ci_to_co{network_substituted_breadth};
            topo_view_co_to_ci network_depth_co_to_ci{network_substituted_depth};
            topo_view_ci_to_co network_depth_ci_to_co{network_substituted_depth};

            // initialize search space graphs with networks
            for (uint64_t i = 0ul; i < 3; ++i)
            {
                ssg_vec[0 + i].network = network_breadth_co_to_ci;
                ssg_vec[3 + i].network = network_breadth_ci_to_co;
                ssg_vec[6 + i].network = network_depth_co_to_ci;
                ssg_vec[9 + i].network = network_depth_ci_to_co;

                if (ps.mode == graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT)
                {
                    ssg_vec[12 + i].network = network_breadth_co_to_ci;
                    ssg_vec[15 + i].network = network_breadth_ci_to_co;
                    ssg_vec[18 + i].network = network_depth_co_to_ci;
                    ssg_vec[21 + i].network = network_depth_ci_to_co;

                    ssg_vec[24 + i].network = network_breadth_co_to_ci;
                    ssg_vec[27 + i].network = network_breadth_ci_to_co;
                    ssg_vec[30 + i].network = network_depth_co_to_ci;
                    ssg_vec[33 + i].network = network_depth_ci_to_co;

                    ssg_vec[36 + i].network = network_breadth_co_to_ci;
                    ssg_vec[39 + i].network = network_breadth_ci_to_co;
                    ssg_vec[42 + i].network = network_depth_co_to_ci;
                    ssg_vec[45 + i].network = network_depth_ci_to_co;

                    if (ps.cost == graph_oriented_layout_design_params::cost_objective::CUSTOM)
                    {
                        ssg_vec[48 + i].network = network_breadth_co_to_ci;
                        ssg_vec[51 + i].network = network_breadth_ci_to_co;
                        ssg_vec[54 + i].network = network_depth_co_to_ci;
                        ssg_vec[57 + i].network = network_depth_ci_to_co;
                    }
                }
            }

            std::vector<mockturtle::node<decltype(network_breadth_ci_to_co)>> nodes_to_place_breadth_ci_to_co{};
            std::vector<mockturtle::node<decltype(network_depth_co_to_ci)>>   nodes_to_place_depth_co_to_ci{};
            std::vector<mockturtle::node<decltype(network_depth_ci_to_co)>>   nodes_to_place_depth_ci_to_co{};

            prepare_nodes_to_place(network_breadth_ci_to_co, nodes_to_place_breadth_ci_to_co);
            prepare_nodes_to_place(network_depth_co_to_ci, nodes_to_place_depth_co_to_ci);
            prepare_nodes_to_place(network_depth_ci_to_co, nodes_to_place_depth_ci_to_co);

            // assign nodes to place to each search space graphs
            for (uint64_t i = 0ul; i < 3; ++i)
            {
                ssg_vec[0 + i].nodes_to_place = nodes_to_place_breadth_co_to_ci;
                ssg_vec[3 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                ssg_vec[6 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                ssg_vec[9 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;

                if (ps.mode == graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT)
                {
                    ssg_vec[12 + i].nodes_to_place = nodes_to_place_breadth_co_to_ci;
                    ssg_vec[15 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                    ssg_vec[18 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                    ssg_vec[21 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;

                    ssg_vec[24 + i].nodes_to_place = nodes_to_place_breadth_co_to_ci;
                    ssg_vec[27 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                    ssg_vec[30 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                    ssg_vec[33 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;

                    ssg_vec[36 + i].nodes_to_place = nodes_to_place_breadth_co_to_ci;
                    ssg_vec[39 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                    ssg_vec[42 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                    ssg_vec[45 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;

                    if (ps.cost == graph_oriented_layout_design_params::cost_objective::CUSTOM)
                    {
                        ssg_vec[48 + i].nodes_to_place = nodes_to_place_breadth_co_to_ci;
                        ssg_vec[51 + i].nodes_to_place = nodes_to_place_breadth_ci_to_co;
                        ssg_vec[54 + i].nodes_to_place = nodes_to_place_depth_co_to_ci;
                        ssg_vec[57 + i].nodes_to_place = nodes_to_place_depth_ci_to_co;
                    }
                }
            }

            if (ps.mode == graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT)
            {
                for (uint64_t i = 0ul; i < 12; ++i)
                {
                    ssg_vec[i].cost = ps.cost;
                }
            }
            else
            {
                for (uint64_t i = 0ul; i < 12; ++i)
                {
                    ssg_vec[i].cost = graph_oriented_layout_design_params::cost_objective::AREA;
                }
                for (uint64_t i = 12ul; i < 24; ++i)
                {
                    ssg_vec[i].cost = graph_oriented_layout_design_params::cost_objective::WIRES;
                }
                for (uint64_t i = 24ul; i < 36; ++i)
                {
                    ssg_vec[i].cost = graph_oriented_layout_design_params::cost_objective::CROSSINGS;
                }
                for (uint64_t i = 36ul; i < 48; ++i)
                {
                    ssg_vec[i].cost = graph_oriented_layout_design_params::cost_objective::ACP;
                }
                if (ps.cost == graph_oriented_layout_design_params::cost_objective::CUSTOM)
                {
                    for (uint64_t i = 48ul; i < 60; ++i)
                    {
                        ssg_vec[i].cost = graph_oriented_layout_design_params::cost_objective::CUSTOM;
                    }
                }
            }
        }
    }
    /**
     * Initialize the search space graphs.
     */
    void initialize() noexcept
    {
        // initial setup for networks and nodes to place
        initialize_networks_and_nodes_to_place();

        // initial setup for PIs and cost
        initialize_pis_cost_and_num_expansions();
    }
};

}  // namespace detail

/**
 * A scalable and efficient placement & routing approach based on spanning a search space graph of partial layouts and
 * finding a path to one of its leaves, i.e., a complete layout.
 *
 * The search space graph starts with an empty layout and then expands it based on where the first node in a topological
 * sort of the logic network can be placed. Based on the position of this first node, a cost is assigned to each
 * expansion based on the position of the placed node. The vertex with the lowest cost, which is the smallest layout
 * w.r.t. area, is then chosen for the next expansion. This iterative process continues until a leaf node is found,
 * which is a layout with all nodes placed. The algorithm then continues to backtrack through the search space graph to
 * find other complete layouts with lower cost.
 *
 * Exclusively generates 2DDWave-clocked layouts.
 *
 * This algorithm was proposed in \"A* is Born: Efficient and Scalable Physical Design for Field-coupled Nanocomputing\"
 * by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2024 (https://ieeexplore.ieee.org/document/10628808).
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param ntk The network to be placed and routed.
 * @param ps The parameters for the A* priority routing algorithm. Defaults to an empty parameter set.
 * @param pst A pointer to a statistics object to record execution details. Defaults to nullptr.
 * @return The smallest layout yielded by the graph-oriented layout design algorithm under the given parameters.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> graph_oriented_layout_design(Ntk& ntk, graph_oriented_layout_design_params ps = {},
                                                graph_oriented_layout_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a technology_network anyway, therefore,
                                                 // this is the only relevant check here

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, 2))
    {
        throw high_degree_fanin_exception();
    }

    graph_oriented_layout_design_stats                  st{};
    detail::graph_oriented_layout_design_impl<Lyt, Ntk> p{ntk, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
