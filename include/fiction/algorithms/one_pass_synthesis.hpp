//
// Created by marcel on 09.04.20.
//

#ifndef FICTION_ONE_PASS_SYNTHESIS_HPP
#define FICTION_ONE_PASS_SYNTHESIS_HPP

#include "iter/aspect_ratio_iterator.hpp"
#include "utils/mugen_info.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>
#include <mockturtle/utils/stopwatch.hpp>

// pybind11 has quite some warnings in its code; let's silence them a little
#pragma GCC diagnostic push  // GCC
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma warning(push, 0)  // MSVC
#include <pybind11/embed.h>
#pragma GCC diagnostic pop  // GCC
#pragma warning(pop)        // MSVC

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

struct one_pass_synthesis_params
{
    /**
     * Clocking scheme to be used.
     */
    std::shared_ptr<clocking_scheme<coord_t>> scheme = nullptr;
    /**
     * Number of tiles to use.
     */
    uint16_t upper_bound = std::numeric_limits<uint16_t>::max();
    /**
     * Use just one fixed tile size.
     */
    uint16_t fixed_size = 0ul;
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
     * Flag to indicate that designated wires should be routed to balance I/O port paths.
     */
    bool io_ports = true;  // TODO thus far, io_ports have to be set to true
    /**
     * Number of threads to use for exploring the possible dimensions.
     */
    std::size_t num_threads = 1ul;
    /**
     * Sets a timeout in seconds for the solving process, where 0 allows for unlimited time.
     */
    uint32_t timeout = 0;
    /**
     * Name of the resulting network.
     */
    std::string name{};
};

struct one_pass_synthesis_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    uint64_t num_gates{0ull}, num_wires{0ull};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates  = {}\n", num_gates);
        out << fmt::format("[i] num. wires  = {}\n", num_wires);
    }
};

namespace detail
{

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
     * @param lyt Reference to an empty layout that serves as a floor plan for S&P&R by Mugen.
     * @param p The configurations to respect in the SAT instance generation process.
     */
    mugen_handler(const std::vector<TT>& spec, Lyt& sketch, one_pass_synthesis_params p) noexcept :
            tts{spec},
            num_pis{spec[0].num_vars()},  // since all tts have to have the same number of variables
            num_pos{spec.size()},         // since all tts have to have the same number of variables
            lyt{sketch},
            ps{std::move(p)},  // need a copy because timeout will be altered
            mugen{pybind11::module::import("mugen")}
    {}
    /**
     * Evaluates a given aspect ratio regarding the stored configurations whether it can be skipped, i.e., does not
     * need to be explored by Mugen. The better this function is, the more UNSAT instances can be skipped without
     * losing the optimality guarantee. This function should never be overly restrictive!
     *
     * @param ratio Aspect ratio to evaluate.
     * @return True iff ratio can safely be skipped because it is UNSAT anyways.
     */
    [[nodiscard]] bool skippable(const aspect_ratio<Lyt>& ratio) const noexcept
    {
        // OPEN clocking optimization: rotated aspect ratios don't need to be explored
        if (lyt.is_clocking_scheme(clock_name::open3) || lyt.is_clocking_scheme(clock_name::open4))
        {
            if (ratio.x != ratio.y && ratio.x == lyt.y() && ratio.y == lyt.x())
                return true;
        }
        // TODO more conditions go here

        return false;
    }
    /**
     * Resizes the layout.
     *
     * @param ratio Current aspect ratio to work on.
     */
    void update_aspect_ratio(const aspect_ratio<Lyt>& ratio) noexcept
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
     * @return true iff the instance generated for the current configuration is SAT.
     */
    bool is_satisfiable()
    {
        namespace py = pybind11;
        using namespace py::literals;

        auto scheme_graph = generate_scheme_graph();

        scheme_graph.attr("to_png")(scheme_graph, "mugen_scheme");

        // Mugen modifies its parameters, therefore, a copy is kept
        auto py_spec = as_py_lists(tts);

        auto nets = scheme_graph.attr("synthesize")(scheme_graph, py_spec);
        for (auto net_it = nets.begin(); net_it != nets.end(); ++net_it)
        {
            if (net_it->is_none())
                return false;

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
     * Number of primary inputs and primary outputs according to spec respectively.
     */
    const uint64_t num_pis, num_pos;
    /**
     * The sketch that later contains the layout generated from a model.
     */
    Lyt& lyt;
    /**
     * Configurations specifying layout restrictions. Used in instance generation among other places.
     */
    one_pass_synthesis_params ps;
    /**
     * The Python module named Mugen.
     */
    pybind11::module mugen;
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
            auto tt_py_list = py::list();

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

        auto scheme_graph = mugen.attr("scheme_graph");
        scheme_graph.attr("__init__")(
            scheme_graph, "shape"_a = py::make_tuple(lyt.x() + 1, lyt.y() + 1), "enable_wire"_a = ps.enable_wires,
            "enable_not"_a = ps.enable_not, "enable_and"_a = ps.enable_and, "enable_or"_a = ps.enable_or,
            "enable_maj"_a = ps.enable_maj, "enable_crossings"_a = ps.crossings, "designated_pi"_a = ps.io_ports,
            "designated_po"_a = ps.io_ports, "nr_threads"_a = ps.num_threads, "timeout"_a = ps.timeout);

        lyt.foreach_ground_tile(
            [this, &scheme_graph](const auto& t)
            {
                lyt.foreach_outgoing_clocked_zone(t,
                                                  [&scheme_graph, &t](const auto& at) {
                                                      scheme_graph.attr("add_virtual_edge")(scheme_graph,
                                                                                            py::make_tuple(t.x, t.y),
                                                                                            py::make_tuple(at.x, at.y));
                                                  });
            });

        return scheme_graph;
    }
    /**
     * Extracts a Lyt from the network synthesized by Mugen.
     *
     * @param net Synthesis result returned by Mugen.
     */
    void to_gate_layout(pybind11::handle net) const
    {
        namespace py = pybind11;
        using namespace py::literals;

        using py_node_map = std::map<py::handle, mockturtle::signal<Lyt>>;

        py_node_map py_n_map{};

        // (crossing node, outgoing node) --> incoming node
        using crossing_map = std::map<std::pair<py::handle, py::handle>, py::handle>;
        crossing_map c_map{};

        // returns an iterator that points to the first non-PI node of the given list of nodes
        const auto get_node_begin_iterator = [this, &net](const auto& nodes)
        {
            // set up the iterator to skip the PIs
            auto pi_it_end = nodes.begin();  // use std::advance because there is no 'operator+' overload
            std::advance(pi_it_end, num_pis + 1);

            return pi_it_end;
        };

        // returns whether a node is empty
        const auto is_empty = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("EMPTY")); };
        // returns whether a node is a wire
        const auto is_wire = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("WIRE")); };
        // returns whether a node is a crossing
        const auto is_crossing = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("CROSS")); };
        // returns whether a node is a negation
        const auto is_negation = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("NOT")); };
        // returns whether a node is a conjunction
        const auto is_conjunction = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("AND")); };
        // returns whether a node is a disjunction
        const auto is_disjunction = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("OR")); };
        // returns whether a node is a majority
        const auto is_majority = [](const auto& node) -> bool
        { return py::str(node.attr("gate_type")).equal(py::str("MAJ")); };

        // checks if the given node is PI
        const auto is_pi = [](const auto& node) -> bool { return py::bool_(node.attr("is_pi")); };
        // checks if the given node is PO
        const auto is_po = [](const auto& node) -> bool { return py::bool_(node.attr("is_po")); };

        // returns the ID of a PI node
        const auto get_pi_id = [](const auto& node) { return py::int_(node.attr("coords")); };
        // returns the ID of a PO node
        const auto get_po_id = [&net](const auto& node)
        {
            auto index = 0u;

            const auto po_map = net.attr("po_map");
            for (auto po_it = po_map.begin(); po_it != po_map.end(); ++po_it, ++index)
            {
                // the po node; for some reason, pybind11 seems to not like lists of tuples; it could not infer the type
                // of *po_it itself
                const auto po = py::reinterpret_borrow<py::tuple>(*po_it)[0];
                if (node.is(po))
                {
                    // return po's index
                    return index;
                }
            }

            // fix compiler warning
            return 0u;
        };

        // extract a Python tuple representing coordinates from a node and converts it to a tile
        const auto get_tile = [](const auto& node, const bool second_layer = false) -> tile<Lyt>
        {
            const auto tuple = py::tuple(node.attr("coords"));
            return {py::int_(tuple[0]), py::int_(tuple[1]), second_layer ? 1 : 0};
        };

        // stores a (crossing, fanout) --> fanin relation for all fanins of the given crossing node
        const auto set_up_crossing = [this, &net, &c_map](const auto& cross_node) -> void
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
                const auto fanin_dir = get_direction(cross_node.attr("coords"), fanin_n.attr("coords"))[py::int_(0)];
                // direction of the associated fanout
                const auto fanout_dir = dir_map[fanin_dir];

                // the fanout node
                const auto fanout_n = node_map[get_coords_in_direction(cross_coords, fanout_dir)];

                // store (crossing, fanout) --> fanin
                c_map[std::make_pair(cross_node, fanout_n)] = fanin_n;
            }
        };

        const std::function<std::vector<mockturtle::signal<Lyt>>(const py::handle&)> get_fanins =
            [this, &is_crossing, &is_pi, &c_map, &get_tile, &py_n_map,
             &get_fanins](const auto& node) -> std::vector<mockturtle::signal<Lyt>>
        {
            std::vector<mockturtle::signal<Lyt>> fanins{};

            auto fanin = node.attr("fanin");
            for (auto fanin_it = fanin.begin(); fanin_it != fanin.end(); ++fanin_it)
            {
                auto fanin_n = fanin[*fanin_it];

                // skip PI nodes
                if (is_pi(fanin_n))
                {
                    continue;
                }

                // if fanin is a crossing node, use the crossing map to trace paths
                if (is_crossing(fanin_n))
                {
                    // access its fanin via the given node in the crossing map to reconstruct the path
                    const auto c_fanin_n = c_map[std::make_pair(fanin_n, node)];

                    // get the tile where the crossing node is located
                    auto cross_pos = get_tile(fanin_n);
                    // switch to second layer if ground is already occupied
                    cross_pos.z = lyt.is_empty_tile(cross_pos) ? 0 : 1;

                    mockturtle::signal<Lyt> fanin_signal{};
                    // if the fanin node has been set up already (i.e. if it is not a crossing itself)
                    if (py_n_map.count(c_fanin_n) > 0)  // TODO iterator map access
                    {
                        // find its signal in the py_n_map
                        fanin_signal = py_n_map[c_fanin_n];
                    }
                    else
                    {
                        fanin_signal = get_fanins(fanin_n)[0];
                    }

                    // create crossing wire and connect it to its fanin if it exists
                    const auto cross_wire = lyt.create_buf(fanin_signal, cross_pos);

                    fanins.push_back(cross_wire);
                }
                // otherwise, simply add the fanin signal to the list of fanins
                else
                {
                    fanins.push_back(py_n_map.at(fanin_n));
                }
            }

            return fanins;
        };

        // pre-allocate PIs and POs to preserve their order
        std::vector<mockturtle::node<Lyt>> pi_list(num_pis), po_list(num_pos);
        // a little hacky: place them all at position {0, 0} so that they can be fetched to be stored as nodes
        // instead of as signals to not lose them as soon as their tile is overridden
        for (auto i = 0ul; i < num_pis; ++i)
        {
            pi_list[i] = lyt.get_node(lyt.create_pi(fmt::format("pi{}", i), {0, 0}));
        }
        // finally, remove the latest created PI again (which has overridden all others) from the layout
        lyt.move_node(pi_list[num_pis - 1], {});
        // do the same for the POs
        for (auto i = 0ul; i < num_pos; ++i)
        {
            po_list[i] = lyt.get_node(lyt.create_po({}, fmt::format("po{}", i), {0, 0}));
        }
        // finally, remove the latest created PO again (which has overridden all others) from the layout
        lyt.move_node(po_list[num_pos - 1], {});

        // checks whether a node has a PI as fan-in
        const auto has_pi_fanin = [&is_pi, &get_pi_id,
                                   &pi_list](const auto& node) -> std::optional<mockturtle::node<Lyt>>
        {
            auto fanin = node.attr("fanin");
            for (auto fanin_it = fanin.begin(); fanin_it != fanin.end(); ++fanin_it)
            {
                auto fanin_n = fanin[*fanin_it];
                if (is_pi(fanin_n))
                {
                    return pi_list[get_pi_id(fanin_n)];
                }
            }

            return std::nullopt;
        };

        // list of all nodes; first num_pi nodes are primary inputs
        auto nodes = net.attr("nodes");

        net.attr("to_png")("mugen_net");

        // iterate over all nodes to reserve their positions on the layout without assigning their incoming signals yet
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
            else if (is_wire(node))
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
                    py_n_map[node] = lyt.move_node(po_list[get_po_id(node)], node_pos);

                    // TODO no reservation of POs create them after second iteration
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
                set_up_crossing(node);
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

        // second iteration: draw connections between the gates
        for (auto node_it = get_node_begin_iterator(nodes); node_it != nodes.end(); ++node_it)
        {
            // the python node object
            const auto py_node = *node_it;

            // the layout node
            auto lyt_node = lyt.get_node(py_n_map[py_node]);
            // crossings are a special case because they are handled on-the-fly in the get_fanins function
            if (is_crossing(py_node))
            {
                continue;
            }
            else
            {
                // its position on the layout
                const auto node_pos = lyt.get_tile(lyt_node);

                // skip empty tiles
                if (lyt.is_empty_tile(node_pos))
                {
                    continue;
                }

                // children (incoming signals) of the layout node
                const auto fanins = get_fanins(py_node);

                print_gate_level_layout(std::cout, lyt);

                // the node is not moved, but its children are updated
                lyt.move_node(lyt_node, node_pos, fanins);
            }
        }
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
            pst{st}
    {
        // add Mugen's path to Python's sys.path module scope
        pybind11::module::import("sys").attr("path").attr("append")(MUGEN_PATH);
    }

    std::optional<Lyt> run()
    {
        // test for proper installation of all required Python libraries
        if (!test_dependencies())
        {
            return std::nullopt;
        }

        // empty layout with an initial size of 1 x 1 tiles
        Lyt layout{{0, 0}, *ps.scheme};

        // handler for the Python interaction
        mugen_handler handler{tts, layout, ps};

        for (; ari <= ps.upper_bound; ++ari)  // <= to prevent overflow
        {

#if (PROGRESS_BARS)
            mockturtle::progress_bar bar("[i] examining layout dimensions: {:>2} × {:<2}");
#endif

            const auto aspect_ratio = typename Lyt::aspect_ratio{(*ari).x, (*ari).y, ps.crossings ? 1 : 0};

            if (handler.skippable(aspect_ratio))
                continue;

#if (PROGRESS_BARS)
            bar(aspect_ratio.x + 1, aspect_ratio.y + 1);
#endif

            handler.update_aspect_ratio(aspect_ratio);

            //            try
            //            {
            const auto sat =
                mockturtle::call_with_stopwatch(pst.time_total, [&handler] { return handler.is_satisfiable(); });

            if (sat)  // solution found
            {
                // statistical information
                pst.x_size    = layout.x() + 1;
                pst.y_size    = layout.y() + 1;
                pst.num_gates = layout.num_gates();
                pst.num_wires = layout.num_wires();

                return layout;
            }
            else  // update timeout and retry
            {
                if (ps.timeout)
                    update_timeout(handler, pst.time_total);
            }
            //            }
            //            // timeout reached
            //            catch (...)
            //            {
            //                return std::nullopt;
            //            }
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
    aspect_ratio_iterator<aspect_ratio<Lyt>> ari{5};
    /**
     * A Python interpreter instance that is necessary to call Mugen, a library written in Python. This instance is
     * scoped and only need to exist. No operations are to be performed on this object. It handles creation and proper
     * destruction of all Python objects used during this session and deals with the CPython API.
     */
    const pybind11::scoped_interpreter instance{};

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

    //    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ntk)> node2pos;
    /**
     * Tests whether all needed dependencies have been installed and can be accessed via Python.
     *
     * @return true iff all dependencies are met.
     */
    [[nodiscard]] bool test_dependencies() const
    {
        namespace py = pybind11;
        using namespace py::literals;

        // test for graphviz
        try
        {
            py::exec("from graphviz import *");
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

        return true;
    }
    /**
     * Calculates the time left for solving by subtracting the time passed from the configured timeout and updates
     * Mugen's timeout accordingly.
     *
     * @param handler Handler whose timeout is to be updated.
     * @param time Time passed since beginning of the solving process.
     */
    void update_timeout(mugen_handler<Lyt, TT>& handler, mockturtle::stopwatch<>::duration time) const noexcept
    {
        auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(time).count();
        // remaining time must be 1 because 0 means unlimited time
        auto time_left = (ps.timeout - time_elapsed > 0 ? static_cast<uint32_t>(ps.timeout - time_elapsed) : 1u);

        handler.update_timeout(time_left);
    }
};

#pragma GCC visibility pop

}  // namespace detail

/**
 * An exact approach combining logic synthesis and physical design using SAT solving. This class provides the interface
 * via which the technique can be called from the CLI. The instance generation happens in its sub-class mugen_handler.
 *
 * Mugen is a framework for one-pass synthesis of FCN circuit layouts developed by Winston Haaswijk. It can be found
 * on GitHub: https://github.com/whaaswijk/mugen
 *
 * Using iterative SAT calls, an optimal synthesis & placement & routing for a given specification will be found.
 * Starting with n, each possible layout dimension in n tiles will be examined by factorization and tested for
 * realizability using the SAT solver glucose. When no upper bound is given, this approach will run until it finds a
 * solution to the synthesis & placement & routing problem instance under all given constraints. Note that there are
 * combinations of constraints for which no valid solution under the given parameters might exist.
 *
 * Since Mugen is written in Python3, fiction uses pybind11 for interoperability. This can lead to performance and
 * integration issues. Make sure to follow all steps given in the README carefully if you decide to run this code.
 *
 * This approach is still experimental and is, therefore, excluded from CLI compilation by default.
 */
template <typename Lyt, typename TT>
std::optional<Lyt> one_pass_synthesis(const std::vector<TT>& tts, one_pass_synthesis_params ps = {},
                                      one_pass_synthesis_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    // tts cannot be empty
    assert(!tts.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(tts.begin(), tts.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == tts.end());

    one_pass_synthesis_stats                 st{};
    detail::one_pass_synthesis_impl<Lyt, TT> p{tts, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

template <typename Lyt, typename Ntk>
std::optional<Lyt> one_pass_synthesis(const Ntk& ntk, one_pass_synthesis_params ps = {},
                                      one_pass_synthesis_stats* pst = nullptr)
{
    static_assert(
        mockturtle::is_network_type_v<Ntk>,
        "Ntk is not a network type");  // Ntk is simulated anyways, therefore, this is the only relevant check here

    // might throw an std::bad_alloc exception if ntk has too many inputs
    const auto tts = mockturtle::simulate<kitty::dynamic_truth_table>(
        ntk, mockturtle::default_simulator<kitty::dynamic_truth_table>(static_cast<unsigned>(ntk.num_pis())));

    return one_pass_synthesis<Lyt>(tts, ps, pst);
}

}  // namespace fiction

#endif  // FICTION_ONE_PASS_SYNTHESIS_HPP
