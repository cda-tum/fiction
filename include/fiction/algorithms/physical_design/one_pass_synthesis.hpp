//
// Created by marcel on 09.04.20.
//

#ifndef FICTION_ONE_PASS_SYNTHESIS_HPP
#define FICTION_ONE_PASS_SYNTHESIS_HPP

#if (MUGEN)

#include "fiction/algorithms/iter/aspect_ratio_iterator.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"
#include "utils/mugen_info.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// pybind11 has quite some warnings in its code; let's silence them a little
#pragma GCC diagnostic push  // GCC
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wrange-loop-analysis"
#pragma warning(push, 0)  // MSVC
#include <pybind11/cast.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pytypes.h>
#pragma GCC diagnostic pop  // GCC
#pragma warning(pop)        // MSVC

namespace fiction
{

/**
 * Parameters for the one-pass synthesis algorithm.
 */
struct one_pass_synthesis_params
{
    /**
     * Clocking scheme to be used.
     */
    std::string scheme = "2DDWave";
    /**
     * Number of tiles to use as an upper bound in x direction.
     */
    uint16_t upper_bound_x = std::numeric_limits<uint16_t>::max();
    /**
     * Number of tiles to use as an upper bound in y direction.
     */
    uint16_t upper_bound_y = std::numeric_limits<uint16_t>::max();
    /**
     * Investigate only aspect ratios with the number of tiles given as upper bound.
     */
    bool fixed_size = false;
    /**
     * Enable the use of wire elements.
     */
    bool enable_wires = false;
    /**
     * Enable the use of NOT gates.
     */
    bool enable_not = false;
    /**
     * Enable the use of AND gates.
     */
    bool enable_and = false;
    /**
     * Enable the use of OR gates.
     */
    bool enable_or = false;
    /**
     * Enable the use of MAJ gates.
     */
    bool enable_maj = false;
    /**
     * Flag to indicate that crossings should be used.
     */
    bool crossings = false;
    /**
     * Flag to indicate that I/Os should be realized by designated wire segments (preferred).
     */
    bool io_pins = true;  // TODO thus far, io_ports have to be set to true
#if !defined(__APPLE__)
    /**
     * Number of threads to use for exploring the possible aspect ratios.
     *
     * @note This is an unstable beta feature.
     */
    std::size_t num_threads = 1ul;
#endif
    /**
     * Sets a timeout in seconds for the solving process, where 0 allows for unlimited time.
     */
    uint32_t timeout = 0u;
    /**
     * Name of the resulting network.
     */
    std::string name{};
};

struct one_pass_synthesis_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    uint64_t num_gates{0ull}, num_wires{0ull}, num_crossings{0ull};

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
 * A Python interpreter instance that is necessary to call Mugen, a library written in Python. This instance is
 * scoped and only need to exist. No operations are to be performed on this object. It handles creation and proper
 * destruction of all Python objects used during this session and deals with the CPython API.
 */
inline static const pybind11::scoped_interpreter INSTANCE{};

// suppress warning 'declared with greater visibility than the type of its field'
#pragma GCC visibility push(hidden)
/**
 * Sub-class to handle interaction with the Python code Mugen as well as some house-keeping.
 */
template <typename Lyt, typename TT>
class mugen_handler
{
  public:
    /**
     * Standard constructor.
     *
     * @param spec The Boolean functions to synthesize.
     * @param sketch Reference to an empty layout that serves as a floor plan for S&P&R by Mugen.
     * @param p The configurations to respect in the SAT instance generation process.
     */
    mugen_handler(const std::vector<TT>& spec, Lyt& sketch, one_pass_synthesis_params p) :
            tts{spec},
            num_pis{spec[0].num_vars()},  // since all tts have to have the same number of variables
            lyt{sketch},
            ps{std::move(p)},  // need a copy because timeout will be altered
            pi_list(num_pis),
            mugen{pybind11::module::import("mugen")}
    {}
    /**
     * Evaluates a given aspect ratio regarding the stored configurations whether it can be skipped, i.e., does not
     * need to be explored by Mugen. The better this function is, the more UNSAT instances can be skipped without
     * losing the optimality guarantee. This function should never be overly restrictive!
     *
     * @param ratio Aspect ratio to evaluate.
     * @return `true` iff ratio can safely be skipped because it is UNSAT anyways.
     */
    [[nodiscard]] bool skippable(const aspect_ratio_t<Lyt>& ratio) const noexcept
    {
        // skip aspect ratios that extend beyond the specified upper bounds
        if (ratio.x >= ps.upper_bound_x || ratio.y >= ps.upper_bound_y)
        {
            return true;
        }
        // OPEN clocking optimization: rotated aspect ratios don't need to be explored
        if (lyt.is_clocking_scheme(clock_name::OPEN))
        {
            if (ratio.x != ratio.y && ratio.x == lyt.y() && ratio.y == lyt.x())
            {
                return true;
            }
        }
        // more conditions go here

        return false;
    }
    /**
     * Resizes the layout.
     *
     * @param ratio Current aspect ratio to work on.
     */
    void update_aspect_ratio(const aspect_ratio_t<Lyt>& ratio) noexcept
    {
        lyt.resize(ratio);
    }
    /**
     * Sets the timeout to the given value.
     *
     * @param timeout New timeout value.
     */
    void update_timeout(const uint32_t timeout) noexcept
    {
        ps.timeout = timeout;
    }
    /**
     * Passes the current scheme_graph to Mugen and synthesizes it. If there is an implementation on this graph
     * realizing the specification, this function returns true.
     *
     * @return `true` iff the instance generated for the current configuration is SAT.
     */
    bool is_satisfiable()
    {
        namespace py = pybind11;
        using namespace py::literals;

        const auto scheme_graph = generate_scheme_graph();

        // Mugen modifies its parameters, therefore, a copy is kept
        const auto py_spec = as_py_lists(tts);

        const auto nets = scheme_graph.attr("synthesize")(scheme_graph, py_spec);
        for (auto net_it = nets.begin(); net_it != nets.end(); ++net_it)
        {
            if (net_it->is_none())
            {
                return false;
            }

            to_gate_layout(*net_it);

            return true;
        }

        return false;
    }

  private:
    /**
     * The Boolean functions to synthesize.
     */
    const std::vector<TT>& tts;
    /**
     * Number of primary inputs according to spec.
     */
    const uint64_t num_pis;
    /**
     * The sketch that later contains the layout generated from a model.
     */
    Lyt& lyt;
    /**
     * Configurations specifying layout restrictions. Used in instance generation among other places.
     */
    one_pass_synthesis_params ps;
    /**
     * Pre-allocate PIs to preserve their order.
     */
    std::vector<mockturtle::node<Lyt>> pi_list;
    /**
     * The Python module named Mugen.
     */
    pybind11::module mugen;
    /**
     * Mugen node --> Lyt signal
     */
    std::map<pybind11::handle, mockturtle::signal<Lyt>> py_n_map{};
    /**
     * (crossing node, outgoing node) --> incoming node
     */
    std::map<std::pair<pybind11::handle, pybind11::handle>, pybind11::handle> crossing_map{};
    /**
     * Converts a vector of truth tables into a list of lists, i.e., Python data types.
     *
     * @param spec Truth tables.
     * @return Python list of lists representing the truth tables.
     */
    [[nodiscard]] pybind11::list as_py_lists(const std::vector<TT>& spec) const
    {
        namespace py = pybind11;

        py::list spec_py_list{};

        for (const auto& tt : spec)
        {
            py::list tt_py_list{};

            const auto binary_tt_string = kitty::to_binary(tt);

            // reverse order according to Mugen's API
            for (auto it = std::crbegin(binary_tt_string); it != std::crend(binary_tt_string); ++it)
            {
                tt_py_list.append(py::int_(*it == '0' ? 0 : 1));
            }

            spec_py_list.append(tt_py_list);
        }

        return spec_py_list;
    }
    /**
     * Generates a scheme_graph object that serves as a blueprint for the synthesis task. It works as an imprint of
     * the layout that is to be created by modeling the data flow connections of the used clocking scheme.
     *
     * @return A scheme_graph object representing the clocking scheme in its aspect ratio and data flow.
     */
    pybind11::object generate_scheme_graph()
    {
        namespace py = pybind11;
        using namespace py::literals;

        const auto scheme_graph = mugen.attr("scheme_graph");
        scheme_graph.attr("__init__")(scheme_graph, "shape"_a = py::make_tuple(lyt.x() + 1, lyt.y() + 1),
                                      "enable_wire"_a = ps.enable_wires, "enable_not"_a = ps.enable_not,
                                      "enable_and"_a = ps.enable_and, "enable_or"_a = ps.enable_or,
                                      "enable_maj"_a = ps.enable_maj, "enable_crossings"_a = ps.crossings,
                                      "designated_pi"_a = ps.io_pins, "designated_po"_a = ps.io_pins,
#if !defined(__APPLE__)
                                      "nr_threads"_a = ps.num_threads,
#endif
                                      "timeout"_a = ps.timeout);

        lyt.foreach_ground_tile(
            [this, &scheme_graph](const auto& t)
            {
                lyt.foreach_outgoing_clocked_zone(t,
                                                  [&scheme_graph, &t](const auto& at)
                                                  {
                                                      scheme_graph.attr("add_virtual_edge")(scheme_graph,
                                                                                            py::make_tuple(t.x, t.y),
                                                                                            py::make_tuple(at.x, at.y));
                                                  });
            });

        return scheme_graph;
    }

    void initialize_pis()
    {
        // a little hacky: place them all at position {0, 0} so that they can be fetched to be stored as nodes
        // instead of as signals to not lose them as soon as their tile is overridden
        for (auto i = 0ul; i < num_pis; ++i)
        {
            pi_list[i] = lyt.get_node(lyt.create_pi(fmt::format("pi{}", i), {0, 0}));
        }
        // finally, remove the latest created PI again (which has overridden all others) from the layout
        lyt.move_node(pi_list[num_pis - 1], {});
    }

    // returns an iterator that points to the first non-PI node of the given list of nodes
    auto get_node_begin_iterator(const pybind11::handle& nodes) const
    {
        // set up the iterator to skip the PIs
        auto pi_it_end = nodes.begin();
        // use std::advance because there is no 'operator+' overload
        std::advance(pi_it_end, static_cast<std::iterator_traits<decltype(pi_it_end)>::difference_type>(num_pis));

        return pi_it_end;
    }

    [[nodiscard]] bool is_empty(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("EMPTY"));
    }

    [[nodiscard]] bool is_wire(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("WIRE"));
    }

    [[nodiscard]] bool is_crossing(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("CROSS"));
    }

    [[nodiscard]] bool is_negation(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("NOT"));
    }

    [[nodiscard]] bool is_conjunction(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("AND"));
    }

    [[nodiscard]] bool is_disjunction(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("OR"));
    }

    [[nodiscard]] bool is_majority(const pybind11::handle& node) const
    {
        return pybind11::str(node.attr("gate_type")).equal(pybind11::str("MAJ"));
    }

    [[nodiscard]] bool is_pi(const pybind11::handle& node) const
    {
        return pybind11::bool_(node.attr("is_pi"));
    }

    [[nodiscard]] bool is_po(const pybind11::handle& node) const
    {
        return pybind11::bool_(node.attr("is_po"));
    }

    [[nodiscard]] uint64_t get_pi_id(const pybind11::handle& node) const
    {
        return static_cast<uint64_t>(pybind11::int_(node.attr("coords")));
    }

    std::optional<mockturtle::node<Lyt>> has_pi_fanin(const pybind11::handle& node)
    {
        const auto fanin = node.attr("fanin");
        for (auto fanin_it = fanin.begin(); fanin_it != fanin.end(); ++fanin_it)
        {
            const auto fanin_n = fanin[*fanin_it];
            if (is_pi(fanin_n))
            {
                return pi_list[get_pi_id(fanin_n)];
            }
        }

        return std::nullopt;
    }

    // extract a Python tuple representing coordinates from a node and converts it to a tile
    [[nodiscard]] tile<Lyt> get_tile(const pybind11::handle& node) const
    {
        const auto coords = pybind11::tuple(node.attr("coords"));
        return {pybind11::int_(coords[0]), pybind11::int_(coords[1])};
    }

    // stores a (crossing, fanout) --> fanin relation for all fanins of the given crossing node
    void set_up_crossing(const pybind11::handle& net, const pybind11::handle& cross_node)
    {
        // Mugen's function to determine cardinal directions of two coordinates
        const auto get_direction = mugen.attr("get_direction");
        // Mugen's function to access a coordinate in a certain direction
        const auto get_coords_in_direction = mugen.attr("get_coords_in_direction");
        // map from coordinates to nodes
        const auto node_map = net.attr("node_map");
        // position of the crossing node
        const auto cross_coords = cross_node.attr("coords");
        // cross_node's direction map which maps fanin to fanout directions
        const auto dir_map = cross_node.attr("dir_map");
        // handle to the list of cross_node's fanins
        const auto fanin = cross_node.attr("fanin");
        // for each of cross_node's fanins
        for (auto fanin_it = fanin.begin(); fanin_it != fanin.end(); ++fanin_it)
        {
            // the fanin
            const auto fanin_n = fanin[*fanin_it];

            // fanin --> crossing direction
            const auto fanin_dir = get_direction(cross_node.attr("coords"), fanin_n.attr("coords"))[pybind11::int_(0)];
            // crossing --> fanout direction
            const auto fanout_dir = dir_map[fanin_dir];

            // the fanout node
            const auto fanout_n = node_map[get_coords_in_direction(cross_coords, fanout_dir)];

            // store (crossing, fanout) --> fanin
            crossing_map[std::make_pair(cross_node, fanout_n)] = fanin_n;
        }
    }

    std::vector<mockturtle::signal<Lyt>> get_fanins(const pybind11::handle& node)
    {
        std::vector<mockturtle::signal<Lyt>> fanins{};

        const auto fanin = node.attr("fanin");
        for (auto fanin_it = fanin.begin(); fanin_it != fanin.end(); ++fanin_it)
        {
            // the fanin node
            const auto fanin_n = fanin[*fanin_it];

            // skip PI nodes
            if (is_pi(fanin_n))
            {
                continue;
            }

            // if fanin is a crossing node, use the crossing map to trace paths
            if (is_crossing(fanin_n))
            {
                // access the crossing's fanin via the given node in the crossing map to reconstruct the path
                const auto c_fanin_n = crossing_map.at(std::make_pair(fanin_n, node));

                // get the tile where the crossing node is located
                auto fanin_n_t = get_tile(fanin_n);
                // switch to second layer if ground is already occupied
                if (!lyt.is_empty_tile(fanin_n_t))
                {
                    fanin_n_t = lyt.above(fanin_n_t);
                }

                mockturtle::signal<Lyt> fanin_signal{};

                // if the c_fanin_n is a crossing itself
                if (is_crossing(c_fanin_n))
                {
                    const auto c_fanin_n_t = get_tile(c_fanin_n);
                    // traverse the crossing path recursively
                    for (const auto& c_fanin_s : get_fanins(fanin_n))
                    {
                        // match the obtained fanins against c_fanin_n's position
                        if (const auto c_fanin_st = static_cast<tile<Lyt>>(c_fanin_s);
                            c_fanin_st == c_fanin_n_t || c_fanin_st == lyt.above(c_fanin_n_t))
                        {
                            fanin_signal = c_fanin_s;
                            break;
                        }
                    }
                }
                else
                {
                    // access py_n_map for the fanin signal
                    fanin_signal = py_n_map.at(c_fanin_n);
                }

                // create crossing wire and connect it to its fanin if it exists
                const auto cross_wire = lyt.create_buf(fanin_signal, fanin_n_t);

                fanins.push_back(cross_wire);
            }
            // otherwise, simply add the fanin signal to the list of fanins
            else
            {
                fanins.push_back(py_n_map.at(fanin_n));
            }
        }

        return fanins;
    }

    void place_nodes(const pybind11::handle& net)
    {
        // list of all nodes; first num_pi nodes are primary inputs
        const auto nodes = net.attr("nodes");
        // first iteration: iterate over all nodes to reserve their positions on the layout without assigning their
        // incoming signals yet
        for (auto node_it = get_node_begin_iterator(nodes); node_it != nodes.end(); ++node_it)
        {
            // the node
            const auto node = *node_it;

            // its position on the layout
            const auto node_pos = get_tile(node);

            // if node has no gate assigned
            if (is_empty(node))
            {
                continue;
            }
            // if node is a wire segment
            if (is_wire(node))
            {
                // if it is a primary input pin
                if (const auto pi = has_pi_fanin(node);
                    pi.has_value())  // PIs are nodes, i.e., potential fan-ins of the current node
                {
                    py_n_map[node] = lyt.move_node(*pi, node_pos);
                }
                // if it is a primary output pin
                else if (is_po(node))  // PO is simply a label of a node, i.e., not a successor node with that attribute
                {
                    continue;
                }
                // normal wire
                else
                {
                    py_n_map[node] = lyt.create_buf({}, node_pos);
                }
            }
            // if node represents a wire-crossing
            else if (is_crossing(node))
            {
                // corner case: since crossing nodes represent two wire segments each, their fanins are handled with a
                // dedicated map that unambiguously assigns the fanins for each fanout
                set_up_crossing(net, node);
            }
            // if node is a negation
            else if (is_negation(node))
            {
                py_n_map[node] = lyt.create_not({}, node_pos);
            }
            // if node is a conjunction
            else if (is_conjunction(node))
            {
                py_n_map[node] = lyt.create_and({}, {}, node_pos);
            }
            // if node is a disjunction
            else if (is_disjunction(node))
            {
                py_n_map[node] = lyt.create_or({}, {}, node_pos);
            }
            // if node is a majority
            else if (is_majority(node))
            {
                py_n_map[node] = lyt.create_maj({}, {}, {}, node_pos);
            }
        }
    }

    void establish_connections(const pybind11::handle& net)
    {
        // nodes can be present multiple times in the 'nodes' list, this set makes sure to visit them exactly once
        std::unordered_set<mockturtle::node<Lyt>> nodes_in_place{};

        // list of all nodes; first num_pi nodes are primary inputs
        const auto nodes = net.attr("nodes");
        // second iteration: draw connections between the placed gates
        for (auto node_it = get_node_begin_iterator(nodes); node_it != nodes.end(); ++node_it)
        {
            // the python node object
            const auto py_node = *node_it;

            // the layout node
            const auto lyt_node = lyt.get_node(py_n_map[py_node]);
            // crossings are a special case because they are handled on-the-fly in the get_fanins function
            if (is_crossing(py_node))
            {
                continue;
            }
            // POs are also skipped and only retrieved at the very end
            if (is_wire(py_node) && is_po(py_node))
            {
                continue;
            }

            // its position on the layout
            const auto node_pos = lyt.get_tile(lyt_node);

            // skip empty tiles
            if (lyt.is_empty_tile(node_pos))
            {
                continue;
            }
            // skip nodes already in place
            if (nodes_in_place.count(lyt_node) > 0)
            {
                continue;
            }

            // children (incoming signals) of the layout node
            const auto fanins = get_fanins(py_node);

            // the node is not moved, but its children are updated
            lyt.move_node(lyt_node, node_pos, fanins);

            nodes_in_place.insert(lyt_node);
        }
    }

    void initialize_pos(const pybind11::handle& net)
    {
        // primary output counter
        auto oc = 0u;
        // third iteration: retrieve primary outputs for PO order
        const auto po_map = net.attr("po_map");
        for (auto po_it = po_map.begin(); po_it != po_map.end(); ++po_it)
        {
            // the po node; for some reason, pybind11 seems to not like lists of tuples; it could not infer the type
            // of *po_it itself
            const auto po = pybind11::reinterpret_borrow<pybind11::tuple>(*po_it)[0];
            // the tile po is located on
            const auto po_pos = get_tile(po);
            // po's fanins
            const auto po_fanins = get_fanins(po);
            // a PO should have only a single fanin
            assert(po_fanins.size() == 1);
            // create the primary output on the layout
            lyt.create_po(po_fanins[0], fmt::format("po{}", oc++), po_pos);
        }
    }
    /**
     * Extracts a Lyt from the network synthesized by Mugen.
     *
     * @param net Synthesis result returned by Mugen.
     */
    void to_gate_layout(const pybind11::handle& net)
    {
        namespace py = pybind11;
        using namespace py::literals;

        initialize_pis();

        place_nodes(net);

        establish_connections(net);

        initialize_pos(net);
    }
};

template <typename Lyt, typename TT>
class one_pass_synthesis_impl
{
  public:
    one_pass_synthesis_impl(const std::vector<TT>& spec, const one_pass_synthesis_params& p,
                            one_pass_synthesis_stats& st) :
            tts{spec},
            ps{p},
            pst{st},
            ari{ps.fixed_size ? static_cast<uint64_t>(ps.upper_bound_x * ps.upper_bound_y) : 0u}
    {}

    std::optional<Lyt> run()
    {
        // test for proper installation of all required Python libraries
        if (!test_dependencies())
        {
            return std::nullopt;
        }

        // empty layout with an initial size of 1 x 1 tiles
        Lyt layout{{0, 0}, *get_clocking_scheme<Lyt>(ps.scheme)};

        // handler for the Python interaction
        mugen_handler handler{tts, layout, ps};

        for (; ari <= static_cast<uint64_t>(ps.upper_bound_x) * static_cast<uint64_t>(ps.upper_bound_y);
             ++ari)  // <= to prevent overflow
        {

#if (PROGRESS_BARS)
            mockturtle::progress_bar bar("[i] examining layout aspect ratios: {:>2} × {:<2}");
#endif

            const auto aspect_ratio = typename Lyt::aspect_ratio{(*ari).x, (*ari).y, ps.crossings ? 1 : 0};

            if (handler.skippable(aspect_ratio))
            {
                continue;
            }

#if (PROGRESS_BARS)
            bar(aspect_ratio.x + 1, aspect_ratio.y + 1);
#endif

            handler.update_aspect_ratio(aspect_ratio);

            try
            {
                const auto sat =
                    mockturtle::call_with_stopwatch(pst.time_total, [&handler] { return handler.is_satisfiable(); });

                if (sat)  // solution found
                {
                    // statistical information
                    pst.x_size        = layout.x() + 1;
                    pst.y_size        = layout.y() + 1;
                    pst.num_gates     = layout.num_gates();
                    pst.num_wires     = layout.num_wires();
                    pst.num_crossings = layout.num_crossings();

                    // restore layout name
                    if constexpr (has_set_layout_name_v<Lyt>)
                    {
                        layout.set_layout_name(ps.name);
                    }

                    return layout;
                }
                // update timeout and retry
                if (ps.timeout)
                {
                    update_timeout(handler, pst.time_total);
                }
            }
            catch (const pybind11::error_already_set& e)
            {
                // timeout reached
                if (e.matches(PyExc_TimeoutError))
                {
                    return std::nullopt;
                }

                // unexpected error
                std::cout << "[e] something unexpected happened in Python; this needs investigation" << std::endl;
                throw;
            }
            // unexpected exception
            catch (...)
            {
                std::cout << "[e] something unexpected happened; this needs investigation" << std::endl;
                throw;
            }
        }

        return std::nullopt;
    }

  private:
    const std::vector<TT> tts;

    one_pass_synthesis_params ps;
    one_pass_synthesis_stats& pst;

    /**
     * Factorizes a number of layout tiles into all possible aspect ratios for iteration.
     */
    aspect_ratio_iterator<aspect_ratio<Lyt>> ari{0};

    class pysat_version_mismatch_exception : public std::exception
    {
      public:
        explicit pysat_version_mismatch_exception(std::string v) : std::exception(), version{std::move(v)} {}

        [[nodiscard]] std::string detected() const
        {
            return version;
        }

      private:
        const std::string version;
    };

    /**
     * Tests whether all needed dependencies have been installed and can be accessed via Python.
     *
     * @return `true` iff all dependencies are met.
     */
    [[nodiscard]] bool test_dependencies() const
    {
        namespace py = pybind11;
        using namespace py::literals;

        // test for graphviz
        try
        {
            py::exec("import graphviz");
        }
        catch (...)
        {
            std::cout << "[e] Python module 'graphviz' could not be detected" << std::endl;
            return false;
        }

        // test for PySAT
        try
        {
            py::exec("from pysat.solvers import Glucose3");
            py::exec("from pysat.card import *");
        }
        catch (...)
        {
            std::cout << "[e] Python module 'PySAT' could not be detected" << std::endl;
            return false;
        }
        try
        {
            const auto installed_pysat_version = py::str(py::module::import("pysat").attr("__version__"));

            if (!installed_pysat_version.equal(py::str(REQUIRED_PYSAT_VERSION)))
            {
                throw pysat_version_mismatch_exception(installed_pysat_version);
            }
        }
        catch (const pysat_version_mismatch_exception& e)
        {
            std::cout << fmt::format("[e] 'PySAT' version '{}' was detected, but version '{}' is specifically needed",
                                     e.detected(), REQUIRED_PYSAT_VERSION)
                      << std::endl;
            return false;
        }

        // test for wrapt_timeout_decorator
        try
        {
            py::exec("from wrapt_timeout_decorator import *");
        }
        catch (...)
        {
            std::cout << "[e] Python module 'wrapt_timeout_decorator' could not be detected" << std::endl;
            return false;
        }

        // test for Mugen
        try
        {
            // add Mugen's path to Python's sys.path module scope
            pybind11::module::import("sys").attr("path").attr("append")(MUGEN_PATH);
            pybind11::module::import("mugen");
        }
        catch (...)
        {
            std::cout << "[e] The 'mugen' library could not be detected; it might have been moved" << std::endl;
        }

        return true;
    }
    /**
     * Calculates the time left for solving by subtracting the time passed from the configured timeout and updates
     * Mugen's timeout accordingly.
     *
     * @param handler Handler whose timeout is to be updated.
     * @param time Time passed since beginning of the solving process.
     */
    void update_timeout(mugen_handler<Lyt, TT>& handler, const mockturtle::stopwatch<>::duration time) const noexcept
    {
        const auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(time).count();
        // remaining time must be 1 because 0 means unlimited time
        const auto time_left = (ps.timeout - time_elapsed > 0 ? static_cast<uint32_t>(ps.timeout - time_elapsed) : 1u);

        handler.update_timeout(time_left);
    }
};

#pragma GCC visibility pop

}  // namespace detail

/**
 * A physical design approach combining logic synthesis and physical design into a single run instead of considering
 * them independently. To this end, SAT solving is utilized, which makes this approach an exact one but one that is
 * independent of prior logic network synthesis. Nevertheless, it does only find solutions for small specifications
 * because it does not scale.
 *
 * The algorithm was originally proposed in \"One-pass Synthesis for Field-coupled Nanocomputing Technologies\" by M.
 * Walter, W. Haaswijk, R. Wille, F. Sill Torres, and Rolf Drechsler in ASP-DAC 2021.
 *
 * Using iterative SAT calls, an optimal synthesis & placement & routing for a given specification will be found.
 * Starting with \f$n\f$, each possible layout aspect ratio in \f$n\f$ tiles will be examined by factorization and
 * tested for realizability using the SAT solver glucose. When no upper bound is given, this approach will run until it
 * finds a solution to the synthesis & placement & routing problem instance under all given constraints. Note that there
 * are combinations of constraints for which no valid solution under the given parameters might exist. It is, thus,
 * prudent to always provide a timeout limit.
 *
 * This implementation relies on Mugen, a framework for one-pass synthesis of FCN circuit layouts developed by Winston
 * Haaswijk. It can be found on GitHub: https://github.com/whaaswijk/mugen
 *
 * Since Mugen is written in Python3, fiction uses pybind11 for interoperability. This can lead to performance and
 * integration issues. Mugen requires the following Python3 packages to be installed:
 * - `graphviz`
 * - `python-sat`
 * - `wrapt_timeout_decorator`
 *
 * Due to the integration hassle, possible performance issues, and its experimental status this approach is excluded
 * from (CLI) compilation by default. To enable it, pass `-DFICTION_ENABLE_MUGEN=ON` to the cmake call.
 *
 * @tparam Lyt Gate-level layout type to generate.
 * @tparam TT Truth table type used as specification.
 * @param tts A vector of truth tables where table at index `i` specifies the Boolean function for output `i`.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `TT` implementing `tts` as an FCN circuit if one is found under the given
 * parameters; `std::nullopt`, otherwise.
 */
template <typename Lyt, typename TT>
std::optional<Lyt> one_pass_synthesis(const std::vector<TT>& tts, one_pass_synthesis_params ps = {},
                                      one_pass_synthesis_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    if (ps.scheme == clock_name::OPEN || ps.scheme == clock_name::COLUMNAR || ps.scheme == clock_name::ROW)
    {
        throw unsupported_clocking_scheme_exception();
    }
    if (const auto clocking_scheme = get_clocking_scheme<Lyt>(ps.scheme); !clocking_scheme.has_value())
    {
        throw unsupported_clocking_scheme_exception();
    }
    else if (clocking_scheme->max_out_degree < 3 && ps.enable_maj)
    {
        ps.enable_maj = false;
        std::cout << fmt::format("[w] disabling MAJ gates as they are not supported by the {} clocking scheme",
                                 ps.scheme)
                  << std::endl;
    }

    // tts cannot be empty
    assert(!tts.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(tts.begin(), tts.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == tts.end());

    one_pass_synthesis_stats                 st{};
    detail::one_pass_synthesis_impl<Lyt, TT> p{tts, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}
/**
 * An overload of one_pass_synthesis above that utilizes a logic network as specification instead of a vector of truth
 * tables. It first generates truth tables from the given network and then calls the function above.
 *
 * This function might throw an `std::bad_alloc` exception if the provided logic network has too many inputs.
 *
 * @tparam Lyt Gate-level layout type to generate.
 * @tparam Ntk Logic network type used as specification.
 * @param ntk The network whose function is to be realized as an FCN circuit.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `TT` implementing `tts` as an FCN circuit if one is found under the given
 * parameters; `std::nullopt`, otherwise.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> one_pass_synthesis(const Ntk& ntk, const one_pass_synthesis_params& ps = {},
                                      one_pass_synthesis_stats* pst = nullptr)
{
    static_assert(
        mockturtle::is_network_type_v<Ntk>,
        "Ntk is not a network type");  // Ntk is simulated anyway, therefore, this is the only relevant check here

    // might throw an std::bad_alloc exception if ntk has too many inputs
    const auto tts = mockturtle::simulate<kitty::dynamic_truth_table>(
        ntk, mockturtle::default_simulator<kitty::dynamic_truth_table>{static_cast<unsigned>(ntk.num_pis())});

    auto lyt = one_pass_synthesis<Lyt>(tts, ps, pst);

    // if a layout was successfully generated, restore PI names
    if (lyt.has_value())
    {
        restore_network_name(ntk, *lyt);
        restore_input_names(ntk, *lyt);
        restore_output_names(ntk, *lyt);
    }

    return lyt;
}

}  // namespace fiction

#endif  // MUGEN

#endif  // FICTION_ONE_PASS_SYNTHESIS_HPP
