#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/exact.hpp"
#include "fiction/io/write_qll_layout.hpp"
#include "fiction/io/write_svg_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/molecular_qca_library.hpp"
#include "fiction/technology/qca_one_library.hpp"
#include "fiction/utils/debug/network_writer.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/exact.hpp>
#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>  // graph-oriented layout design algorithm
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/network_reader.hpp>                                          // read networks from files
#include <fiction/types.hpp>                                                      // types suitable for the FCN domain

#include <fmt/format.h>                    // output formatting
#include <mockturtle/utils/stopwatch.hpp>  // runtime measurements

#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>

template <typename Ntk>
mockturtle::names_view<Ntk> and3()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");

    const auto and2 = ntk.create_and(x1, x2);
    const auto and3 = ntk.create_and(and2, x3);

    ntk.create_po(and3, "and3");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> and4()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");
    const auto x4 = ntk.create_pi("d");

    const auto and2 = ntk.create_and(x1, x2);
    const auto and3 = ntk.create_and(x3, x4);
    const auto and4 = ntk.create_and(and2, and3);

    ntk.create_po(and4, "and4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> aoi21()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");

    const auto and2  = ntk.create_and(x1, x2);
    const auto ao2   = ntk.create_or(and2, x3);
    const auto aoi21 = ntk.create_not(ao2);

    ntk.create_po(aoi21, "aoi21");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> aoi22()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");
    const auto x4 = ntk.create_pi("d");

    const auto and2  = ntk.create_and(x1, x2);
    const auto and3  = ntk.create_and(x3, x4);
    const auto ao2   = ntk.create_or(and2, and3);
    const auto aoi22 = ntk.create_not(ao2);

    ntk.create_po(aoi22, "aoi22");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> oai21()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");

    const auto and2  = ntk.create_or(x1, x2);
    const auto oa2   = ntk.create_and(and2, x3);
    const auto oai21 = ntk.create_not(oa2);

    ntk.create_po(oai21, "oai21");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> oai22()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("c");
    const auto x4 = ntk.create_pi("d");

    const auto and2  = ntk.create_or(x1, x2);
    const auto and3  = ntk.create_or(x3, x4);
    const auto oa2   = ntk.create_and(and2, and3);
    const auto oai22 = ntk.create_not(oa2);

    ntk.create_po(oai22, "oai22");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> xor2()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("a");

    const auto and2    = ntk.create_and(x1, x2);
    const auto not_and = ntk.create_not(and2);
    const auto or2     = ntk.create_or(x2, x3);
    const auto xor2    = ntk.create_and(or2, not_and);

    // outputs
    ntk.create_po(xor2, "xor2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> comparator1bit()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("a");

    const auto not_x1 = ntk.create_not(x1);
    const auto not_x2 = ntk.create_not(x2);

    const auto a_smaller_b = ntk.create_and(not_x1, x2);
    const auto b_smaller_a = ntk.create_and(not_x2, x3);
    const auto or2         = ntk.create_or(a_smaller_b, b_smaller_a);
    const auto a_equal_b   = ntk.create_not(or2);

    // outputs
    ntk.create_po(a_smaller_b, "a_smaller_b");
    ntk.create_po(b_smaller_a, "b_smaller_a");
    ntk.create_po(a_equal_b, "a_equal_b");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> half_adder()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("a");

    const auto carry   = ntk.create_and(x1, x2);
    const auto not_and = ntk.create_not(carry);
    const auto a_or_b  = ntk.create_or(x2, x3);
    const auto sum     = ntk.create_and(a_or_b, not_and);

    // outputs
    ntk.create_po(sum, "sum");
    ntk.create_po(carry, "carry");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nand()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");

    const auto and2    = ntk.create_and(x1, x2);
    const auto not_and = ntk.create_not(and2);

    ntk.create_po(not_and, "nand");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nor()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");

    const auto or2    = ntk.create_or(x1, x2);
    const auto not_or = ntk.create_not(or2);

    ntk.create_po(not_or, "nor");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> c17()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("2");
    const auto x2 = ntk.create_pi("3");
    const auto x3 = ntk.create_pi("4");
    const auto x4 = ntk.create_pi("4_c");
    const auto x5 = ntk.create_pi("5");
    const auto x6 = ntk.create_pi("6");

    const auto a1  = ntk.create_and(x4, x1);
    const auto a2  = ntk.create_and(x3, x5);
    const auto na2 = ntk.create_not(a2);
    const auto a3  = ntk.create_and(na2, x2);
    const auto a4  = ntk.create_and(na2, x6);
    const auto o1  = ntk.create_or(a3, a4);
    const auto o2  = ntk.create_or(a1, a3);

    ntk.create_po(o1, "po1");
    ntk.create_po(o2, "po2");

    return ntk;
}

/*template <typename Ntk>
mockturtle::names_view<Ntk> full_adder()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("cin");
    const auto x2 = ntk.create_pi("x");
    const auto x3 = ntk.create_pi("y");
    const auto x4 = ntk.create_pi("x");
    const auto x5 = ntk.create_pi("cin");
    const auto x6 = ntk.create_pi("x");
    const auto x7 = ntk.create_pi("y");

    const auto and2   = ntk.create_and(x2, x3);
    const auto and_inv = ntk.create_not(and2);
    const auto or2  = ntk.create_or(x3, x4);
    const auto xor2     = ntk.create_and(and_inv, or2);

    const auto sum = ntk.create_xor(xor2, x1);
    const auto n1 = ntk.create_and(x2, x3);

    const auto w3 = ntk.create_and(xor2, x5);
    const auto w1 = ntk.create_and(x6, x7);
    const auto cout = ntk.create_or(w3, w1);

    // outputs
    ntk.create_po(sum, "sum");
    ntk.create_po(cout, "carry");

    return ntk;
}*/

template <typename Ntk>
Ntk read_ntk(const std::string& name)
{
    fmt::print("[i] processing {}\n", name);

    std::ostringstream os{};

    fiction::network_reader<std::shared_ptr<Ntk>> reader{fiction_experiments::benchmark_path(name), os};

    const auto nets = reader.get_networks();

    return *nets.front();
}

/**
 * Critical path length and throughput storage struct.
 */
struct cp_and_tp
{
    /**
     * Length of the critical path in tiles.
     */
    uint64_t critical_path_length{0ull};
    /**
     * Throughput of the layout in clock cycles as \f$\frac{1}{x}\f$ where only \f$x\f$ is stored.
     */
    uint64_t throughput{0ull};
};

template <typename Lyt>
void move_nodes(Lyt& gate_lyt, fiction::tile<Lyt> tile,
                std::unordered_map<uint64_t, std::vector<mockturtle::signal<Lyt>>>& relations, uint64_t dx, uint64_t dy,
                bool dz = false)
{
    auto node = gate_lyt.get_node(tile);

    tile.x += dx;
    tile.y += dy;

    auto old_sig = gate_lyt.make_signal(node);
    auto new_sig = gate_lyt.move_node(node, tile);

    for (auto& [_, fanins] : relations)
    {
        std::replace(fanins.begin(), fanins.end(), old_sig, new_sig);
    }
}

template <typename Lyt>
auto compute_slacks_and_layout_extension(Lyt& gate_lyt)
{
    using node = mockturtle::node<Lyt>;

    // Save orientations of POs and PIs
    std::unordered_map<node, uint8_t> border_properties;

    // Handle PIs
    std::unordered_map<node, uint64_t> slack_pis{};
    uint64_t                           x_slack_pi   = 0;
    uint64_t                           y_slack_pi   = 0;
    uint64_t                           min_diagonal = 0;

    // First pass: find the minimum diagonal distance among all PIs
    gate_lyt.foreach_pi(
        [&min_diagonal](const auto& pi)
        {
            const auto pi_tile = static_cast<fiction::tile<Lyt>>(pi);
            const auto diag    = static_cast<uint64_t>(pi_tile.x + pi_tile.y);
            min_diagonal       = std::min(min_diagonal, diag);
        });

    // Second pass: compute slacks and per-node slack values for PIs
    gate_lyt.foreach_pi(
        [&gate_lyt, &slack_pis, &x_slack_pi, &y_slack_pi, &min_diagonal, &border_properties](const auto& pi)
        {
            const auto pi_tile = gate_lyt.get_tile(pi);
            const auto diag    = static_cast<uint64_t>(pi_tile.x + pi_tile.y);
            const auto slack   = diag - min_diagonal;

            slack_pis[pi] = slack;

            if (gate_lyt.is_at_western_border(pi_tile))
            {
                x_slack_pi            = std::max(x_slack_pi, slack);
                border_properties[pi] = 0;
            }
            else if (gate_lyt.is_at_northern_border(pi_tile))
            {
                y_slack_pi            = std::max(y_slack_pi, slack);
                border_properties[pi] = 1;
            }
        });

    // Handle POs
    std::unordered_map<node, uint64_t> slack_pos{};
    uint64_t                           x_slack_po   = 0;
    uint64_t                           y_slack_po   = 0;
    uint64_t                           max_diagonal = 0;

    // First pass: find the maximum diagonal distance among all POs
    gate_lyt.foreach_po(
        [&max_diagonal](const auto& po)
        {
            const auto po_tile = static_cast<fiction::tile<Lyt>>(po);
            const auto diag    = static_cast<uint64_t>(po_tile.x + po_tile.y);
            max_diagonal       = std::max(max_diagonal, diag);
        });

    // Second pass: compute slacks and per-node slack values for POs
    gate_lyt.foreach_po(
        [&gate_lyt, &slack_pos, &max_diagonal, &x_slack_po, &y_slack_po, &border_properties](const auto& po)
        {
            const auto po_node = gate_lyt.get_node(po);
            const auto po_tile = static_cast<fiction::tile<Lyt>>(po);
            const auto diag    = static_cast<uint64_t>(po_tile.x + po_tile.y);
            const auto slack   = max_diagonal - diag;

            slack_pos[po_node] = slack;

            if (gate_lyt.is_at_eastern_border(po_tile))
            {
                x_slack_po                 = std::max(x_slack_po, slack);
                border_properties[po_node] = 2;
            }
            else if (gate_lyt.is_at_southern_border(po_tile))
            {
                y_slack_po                 = std::max(y_slack_po, slack);
                border_properties[po_node] = 3;
            }
        });

    const auto x_extension = static_cast<uint32_t>(x_slack_pi + x_slack_po);
    const auto y_extension = static_cast<uint32_t>(y_slack_pi + y_slack_po);

    return std::tuple{slack_pis,  slack_pos,   x_slack_pi,  y_slack_pi,       x_slack_po,
                      y_slack_po, x_extension, y_extension, border_properties};
}

template <typename Lyt>
void move_and_extend_layout(Lyt& gate_lyt, uint64_t x_extension, uint64_t y_extension, uint64_t x_slack_pi,
                            uint64_t y_slack_pi)
{
    // first save the old relations
    std::unordered_map<uint64_t, std::vector<mockturtle::signal<Lyt>>> fanin_relations;
    gate_lyt.foreach_node(
        [&](const auto& n)
        {
            std::vector<mockturtle::signal<Lyt>> fanins;
            gate_lyt.foreach_fanin(n, [&fanins](const auto fi) { fanins.push_back(fi); });
            fanin_relations[n] = fanins;
        });

    for (auto x = static_cast<int64_t>(gate_lyt.x()); x >= 0; --x)
    {
        for (auto y = static_cast<int64_t>(gate_lyt.y()); y >= 0; --y)
        {
            fiction::tile<Lyt> tile = {x, y};

            move_nodes(gate_lyt, tile, fanin_relations, x_slack_pi, y_slack_pi);

            ++tile.z;
            if (gate_lyt.get_node(tile))
            {
                move_nodes(gate_lyt, tile, fanin_relations, x_slack_pi, y_slack_pi, true);
            }
        }
    }

    for (const auto& entry : fanin_relations)
    {
        const auto node = entry.first;
        for (const auto& fi : entry.second)
        {
            gate_lyt.connect(fi, node);
        }
    }
    gate_lyt.resize({gate_lyt.x() + x_extension, gate_lyt.y() + y_extension, gate_lyt.z()});
}

template <typename Lyt>
void move_and_reconnect_pis_and_pos(Lyt& gate_lyt, std::unordered_map<mockturtle::node<Lyt>, uint64_t> slack_pis,
                                    std::unordered_map<mockturtle::node<Lyt>, uint64_t> slack_pos,
                                    std::unordered_map<mockturtle::node<Lyt>, uint8_t>  border_properties)
{
    // Handle Pis
    for (const auto& entry : slack_pis)
    {
        const auto pi_node = entry.first;
        const auto slack   = entry.second;

        if (slack == 0)
        {
            continue;
        }

        // move this PI north and wire south to old position
        if (border_properties[pi_node] == 1)
        {
            auto t     = gate_lyt.get_tile(pi_node);
            auto new_t = t;
            new_t.y -= slack;
            gate_lyt.move_node(pi_node, new_t);
            ++t.y;
            fiction::detail::wire_south(gate_lyt, new_t, t);
        }
        // move this PI west and wire east to old position
        else if (border_properties[pi_node] == 0)
        {
            auto t     = gate_lyt.get_tile(pi_node);
            auto new_t = t;
            new_t.x -= slack;
            gate_lyt.move_node(pi_node, new_t);
            ++t.x;
            fiction::detail::wire_east(gate_lyt, new_t, t);
        }
        if (gate_lyt.is_dead(pi_node))
        {
            gate_lyt.revive_from_dead(pi_node);
        }
    }

    // handle pos
    for (const auto& entry : slack_pos)
    {
        const auto po_node = entry.first;
        const auto slack   = entry.second;

        if (slack == 0)
        {
            continue;
        }

        std::vector<mockturtle::signal<Lyt>> fanin{};
        gate_lyt.foreach_fanin(po_node, [&fanin](auto const& fi) { fanin.push_back(fi); });

        // move this PO south and wire south to new position
        if (border_properties[po_node] == 3)
        {
            auto t     = gate_lyt.get_tile(po_node);
            auto new_t = t;
            new_t.y += slack;
            gate_lyt.move_node(po_node, new_t);
            auto sig_buf  = gate_lyt.create_buf(fanin[0], t);
            auto buf_tile = gate_lyt.get_tile(gate_lyt.get_node(sig_buf));
            auto sig      = fiction::detail::wire_south(gate_lyt, buf_tile, new_t);
            gate_lyt.connect(sig, po_node);
        }
        // move this PO east and wire east to new position
        else if (border_properties[po_node] == 2)
        {
            auto t     = gate_lyt.get_tile(po_node);
            auto new_t = t;
            new_t.x += slack;
            gate_lyt.move_node(po_node, new_t);
            auto sig_buf  = gate_lyt.create_buf(fanin[0], t);
            auto buf_tile = gate_lyt.get_tile(gate_lyt.get_node(sig_buf));
            auto sig      = fiction::detail::wire_east(gate_lyt, t, new_t);
            gate_lyt.connect(sig, po_node);
        }
    }
}

template <typename Lyt>
void synchronize_pis_pos(Lyt& gate_lyt)
{
    // Returns [slack_pis, slack_pos, x_slack_pi, y_slack_pi, x_slack_po, y_slack_po, x_extension, y_extension,
    // border_properties]
    auto value_container = compute_slacks_and_layout_extension(gate_lyt);
    move_and_extend_layout(gate_lyt, std::get<6>(value_container), std::get<7>(value_container),
                           std::get<2>(value_container), std::get<3>(value_container));
    move_and_reconnect_pis_and_pos(gate_lyt, std::get<0>(value_container), std::get<1>(value_container),
                                   std::get<8>(value_container));
}

int main()  // NOLINT
{
    using gate_lyt =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, double, float, std::string>
        optimization_exp{"optimization",
                         "benchmark",
                         "inputs",
                         "outputs",
                         "initial nodes",
                         "ortho layout width (in tiles)",
                         "ortho layout height (in tiles)",
                         "ortho layout area (in tiles)",
                         "optimized layout width (in tiles)",
                         "optimized layout height (in tiles)",
                         "optimized layout area (in tiles)",
                         "gates",
                         "wires",
                         "critical path",
                         "throughput",
                         "runtime ortho (in sec)",
                         "runtime optimization (in sec)",
                         "improvement (%)",
                         "equivalent"};

    // stats
    using gate_layout = fiction::gate_level_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    using gate_layout_gold =
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>;

    static constexpr const uint64_t bench_select = fiction_experiments::c17;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        // auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        std::string bench_name        = "half_adder";
        auto        benchmark_network = half_adder<fiction::tec_nt>();

        fiction::debug::write_dot_network(benchmark_network);

        std::cout << "Layout started\n";
        /*fiction::exact_physical_design_params ps_exact{};
        ps_exact.straight_inverters = true;
        ps_exact.scheme             = "2DDWave";
        ps_exact.border_io          = true;
        ps_exact.crossings          = false;
        auto gate_layout_opt        = fiction::exact<gate_layout>(benchmark_network, ps_exact);*/

        fiction::graph_oriented_layout_design_stats  graph_oriented_layout_design_stats{};
        fiction::graph_oriented_layout_design_params graph_oriented_layout_design_params{};
        graph_oriented_layout_design_params.mode =
            fiction::graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        graph_oriented_layout_design_params.return_first          = true;
        graph_oriented_layout_design_params.enable_multithreading = true;
        graph_oriented_layout_design_params.planar                = true;

        auto gate_layout_opt = fiction::graph_oriented_layout_design<gate_layout, fiction::tec_nt>(
            benchmark_network, graph_oriented_layout_design_params, &graph_oriented_layout_design_stats);

        std::cout << "Layout finished\n";

        if (!gate_layout_opt)
        {
            throw std::runtime_error("layout design failed");
        }

        std::cout << "Start applying library\n";

        auto& gate_lyt = *gate_layout_opt;

        fiction::debug::write_dot_layout(gate_lyt, "gate_lyt");

        synchronize_pis_pos(gate_lyt);
        // synchronize_pis_pos_old(gate_lyt);

        fiction::debug::write_dot_layout(gate_lyt, "gate_lyt_moved");

        // write the QCA ONE version
        using qca_cell_clk_lyt =
            fiction::cell_level_layout<fiction::qca_technology,
                                       fiction::synchronization_element_layout  // se_layouts have only been
                                                                                // investigated for QCA technologies
                                       <fiction::clocked_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

        const auto layout = fiction::apply_gate_library<fiction::qca_cell_clk_lyt, fiction::qca_one_library>(gate_lyt);

        std::string filename_qca = bench_name + "_qcaone.svg";

        fiction::write_qca_layout_svg(layout, filename_qca);

        // fiction::write_qll_layout(layout, "mux_exact_qcaone.qll");

        // write the molQCA version
        using mol_qca_cell_clk_lyt =
            fiction::cell_level_layout<fiction::mol_qca_technology,
                                       fiction::synchronization_element_layout  // se_layouts have only been
                                                                                // investigated for QCA technologies
                                       <fiction::clocked_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

        const auto layout_mol_qca =
            fiction::apply_gate_library<mol_qca_cell_clk_lyt, fiction::molecular_qca_library>(gate_lyt);

        fiction::debug::write_dot_network(benchmark_network);

        std::string filename_mol_qca_svg = bench_name + "_molqca.svg";

        fiction::write_mol_qca_layout_svg(layout_mol_qca, filename_mol_qca_svg);

        std::string filename_mol_qca_qll = bench_name + "_molqca.qll";

        fiction::write_qll_layout(layout_mol_qca, filename_mol_qca_qll);
    }

    return EXIT_SUCCESS;
}
