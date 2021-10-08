//
// Created by marcel on 06.01.20.
//

#if (FICTION_Z3_SOLVER)

#ifndef FICTION_EXACT_HPP
#define FICTION_EXACT_HPP

#include "../algorithms/name_restoration.hpp"
#include "../io/print_layout.hpp"
#include "../layouts/clocking_scheme.hpp"
#include "../traits.hpp"
#include "../utils/debug/network_writer.hpp"
#include "fanout_substitution.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/progress_bar.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cstdint>
#include <optional>
#include <set>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct exact_physical_design_params
{
    /**
     * Clocking scheme to be used.
     */
    std::shared_ptr<clocking_scheme<coordinate<Lyt>>> scheme =
        std::make_shared<clocking_scheme<coordinate<Lyt>>>(twoddwave_4_clocking<Lyt>());
    /**
     * Number of tiles to use.
     */
    uint16_t upper_bound = std::numeric_limits<uint16_t>::max();
    /**
     * Use just one fixed tile size.
     */
    uint16_t fixed_size = 0ul;
    /**
     * Number of threads to use for exploring the possible dimensions.
     */
    std::size_t num_threads = 1ul;
    /**
     * Flag to indicate that crossings should be used.
     */
    bool crossings = false;
    /**
     * Flag to indicate that the number of used crossing tiles should be minimized.
     */
    bool minimize_wires = false;
    /**
     * Flag to indicate that the number of used crossing tiles should be minimized.
     */
    bool minimize_crossings = false;
    /**
     * Flag to indicate that designated wires should be routed to balance I/O port paths.
     */
    bool io_ports = false;
    /**
     * Flag to indicate that I/Os should be placed at the grid's border.
     */
    bool border_io = false;
    /**
     * Flag to indicate that a discrepancy in fan-in path lengths was allowed.
     */
    bool desynchronize = false;
    /**
     * Flag to indicate that artificial clock latch delays should be used to balance paths.
     */
    bool synchronization_elements = false;
    /**
     * Flag to indicate that only straight inverters should be used (instead of bend ones).
     */
    bool straight_inverters = false;
    /**
     * Sets a timeout in ms for the solving process. Standard is 4294967 seconds as defined by Z3.
     */
    unsigned timeout = 4294967u;
};

struct exact_physical_design_stats
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

template <typename Lyt, typename Ntk>
class exact_impl
{
  public:
    exact_impl(const Ntk& src, const exact_physical_design_params<Lyt>& p, exact_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<topology_network>>(src)}},
            ps{p},
            pst{st},
            node2pos{ntk}
    {}

    std::optional<Lyt> run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        Lyt layout{};

        // restore possibly set signal names
        restore_names(ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        return layout;
    }

  private:
    mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<topology_network>>> ntk;

    exact_physical_design_params<Lyt> ps;
    exact_physical_design_stats&      pst;

    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ntk)> node2pos;

    uint32_t po_counter{0};
};

}  // namespace detail

/**
 * A heuristic physical design approach based on orthogonal graph drawing. A slight modification of
 * Therese C. Biedl's improved algorithm for drawing of 3-graphs is used because the original
 * one works for undirected graphs only. Modification includes using directions of the logic network
 * directly instead of relabeling the edges according to its DFS tree, ordering the vertices
 * using jDFS instead of DFS, and adding an extra placement rule for nodes without predecessors.
 *
 * The algorithm works in linear time O(2|V| + |E|). Produced layout has a size of x * y, where
 * x + y = |V| - |PI| + 1. This is because each vertex leads to either one extra row or column
 * except for those without predecessors which create both.
 *
 * This is a proof of concept implementation for a scalable physical design approach for FCN.
 * It is not meant to be used for arranging fabricable circuits, as area is far from being optimal.
 *
 * May throw an 'high_degree_fanin_exception'.
 */
template <typename Lyt, typename Ntk>
std::optional<Lyt> exact(const Ntk& ntk, exact_physical_design_params<Lyt> ps = {}, exact_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyways, therefore,
                                                 // this is the only relevant check here

    exact_physical_design_stats  st{};
    detail::exact_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_ORTHOGONAL_HPP

#endif  // FICTION_Z3_SOLVER
