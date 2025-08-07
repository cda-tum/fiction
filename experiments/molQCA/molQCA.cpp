#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/exact.hpp"
#include "fiction/io/write_qll_layout.hpp"
#include "fiction/io/write_svg_layout.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/molecular_qca_library.hpp"
#include "fiction/technology/qca_one_library.hpp"
#include "fiction/utils/debug/network_writer.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/exact.hpp>
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

    const auto and2   = ntk.create_and(x1, x2);
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

    const auto or2   = ntk.create_or(x1, x2);
    const auto not_or = ntk.create_not(or2);

    ntk.create_po(not_or, "nor");

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
uint64_t critical_path_length_grid(const Lyt& lyt)
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    using tile_t = fiction::tile<Lyt>;

    // reachability cache: tile  ➜  1 = already explored
    phmap::flat_hash_set<tile_t> visited;

    uint64_t crit_len = 0;

    /* ---------- 1. iterate over every primary output ---------- */
    lyt.foreach_po(
        [&](const auto& po)
        {
            // BFS queue holds <current tile , accumulated geometric length>
            std::queue<std::pair<tile_t, uint64_t>> q;
            q.emplace(po, 0);
            visited.insert(static_cast<tile_t>(po));

            /* ---------- 2. reverse walk until we hit the driving PI ---------- */
            while (!q.empty())
            {
                auto [t, len] = q.front();
                q.pop();

                /* 2a. PI reached → update candidate for critical path length */
                if (lyt.is_pi_tile(t))
                {
                    crit_len = std::max(crit_len, len);
                    continue;  // done with this branch
                }

                /* 2b. enqueue all fan-ins that have not been explored yet */
                for (const tile_t& src : lyt.incoming_data_flow(t))
                {
                    if (visited.insert(src).second)  // 1st time we see this tile?
                    {
                        // add Manhattan distance from t to src (|Δx|+|Δy|) to current length
                        const uint64_t step = std::abs(int(src.x) - int(t.x)) + std::abs(int(src.y) - int(t.y));

                        q.emplace(src, len + step);
                    }
                }
            }
        });

    return crit_len + 1;  // ← critical path in *tiles*
}

template <typename Lyt>
auto path_lengths_grid(const Lyt& lyt)
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    using tile_t = fiction::tile<Lyt>;
    using node_t = mockturtle::node<Lyt>;

    // for each output on vector entry with
    //   first:  the length of the longest path,
    //   second: a pair of vectors: {PI node IDs}, {corresponding path lengths}
    std::vector<std::pair<uint64_t, std::pair<std::vector<node_t>, std::vector<uint64_t>>>> paths_per_output;

    // Iterate over every primary output
    lyt.foreach_po(
        [&](const auto& po)
        {
            tile_t out_tile = static_cast<tile_t>(po);
            node_t po_node  = static_cast<node_t>(po);

            // Track visited tiles
            phmap::flat_hash_set<tile_t> visited;

            // BFS queue: <current tile, accumulated distance>
            std::queue<std::pair<tile_t, uint64_t>> q;
            q.emplace(out_tile, 0);
            visited.insert(out_tile);

            // Temporary storage of PI nodes and their path lengths
            std::vector<node_t>   pi_nodes;
            std::vector<uint64_t> pi_lengths;

            // Reverse-walk until reaching all PIs
            while (!q.empty())
            {
                auto [t, len] = q.front();
                q.pop();

                if (lyt.is_pi_tile(t))
                {
                    // Record this PI's node ID and length (+1 counts the PI tile)
                    pi_nodes.push_back(lyt.get_node(t));
                    pi_lengths.push_back(len + 1);
                    continue;
                }

                // Explore all fan-in tiles
                for (const tile_t& src : lyt.incoming_data_flow(t))
                {
                    if (visited.insert(src).second)
                    {
                        uint64_t step = std::abs(int(src.x) - int(t.x)) + std::abs(int(src.y) - int(t.y));
                        q.emplace(src, len + step);
                    }
                }
            }

            // Determine the longest path length
            uint64_t max_len = 0;
            if (!pi_lengths.empty())
            {
                max_len = *std::max_element(pi_lengths.begin(), pi_lengths.end());
            }

            // Store for this output
            paths_per_output.push_back({max_len, {std::move(pi_nodes), std::move(pi_lengths)}});
        });

    return paths_per_output;
}

/**
 * Compute per-PI slack amounts given per-PO path data.
 * @param paths_per_output  A vector per-output, where each entry holds:
 *   - first:  u_x = the (original) max_path length for PO x
 *   - second: a pair of:
 *       * vector<node_t>    pi_ids = the PI node IDs feeding PO x
 *       * vector<uint64_t>  lens   = the corresponding path lengths ℓ_{i,x}
 * @return  map from each PI node to its computed slack s_i
 */
template <typename Lyt>
std::unordered_map<mockturtle::node<Lyt>, uint64_t> compute_slacks(
    const std::vector<std::pair<uint64_t, std::pair<std::vector<mockturtle::node<Lyt>>, std::vector<uint64_t>>>>&
        paths_per_output)
{
    const size_t P = paths_per_output.size();

    // u[x] = current adjusted max_path for output x
    std::vector<uint64_t> u(P);
    // per-PO PI lists and lengths
    std::vector<std::vector<mockturtle::node<Lyt>>> pi_ids(P);
    std::vector<std::vector<uint64_t>>              lens(P);

    // initialize from input
    for (size_t x = 0; x < P; ++x)
    {
        u[x]      = paths_per_output[x].first;
        pi_ids[x] = paths_per_output[x].second.first;
        lens[x]   = paths_per_output[x].second.second;
    }

    // slack map: s[i] = current best guess for PI i
    std::unordered_map<mockturtle::node<Lyt>, uint64_t> s;

    // initial slack estimates
    for (size_t x = 0; x < P; ++x)
    {
        for (size_t j = 0; j < pi_ids[x].size(); ++j)
        {
            auto     i     = pi_ids[x][j];
            uint64_t l     = lens[x][j];
            uint64_t slack = (u[x] >= l ? u[x] - l : 0);
            auto     it    = s.find(i);
            if (it == s.end() || slack > it->second)
                s[i] = slack;
        }
    }

    // iterative fixpoint
    bool changed = true;
    while (changed)
    {
        changed = false;

        // A) adjust each u[x]
        for (size_t x = 0; x < P; ++x)
        {
            uint64_t new_u = 0;
            for (size_t j = 0; j < pi_ids[x].size(); ++j)
            {
                auto     i    = pi_ids[x][j];
                uint64_t l    = lens[x][j];
                uint64_t cand = s[i] + l;
                new_u         = std::max(new_u, cand);
            }
            if (new_u > u[x])
            {
                u[x]    = new_u;
                changed = true;
            }
        }

        // B) adjust each s[i]
        for (size_t x = 0; x < P; ++x)
        {
            for (size_t j = 0; j < pi_ids[x].size(); ++j)
            {
                auto     i     = pi_ids[x][j];
                uint64_t l     = lens[x][j];
                uint64_t slack = (u[x] >= l ? u[x] - l : 0);
                auto     it    = s.find(i);
                if (it == s.end() || slack > it->second)
                {
                    s[i]    = slack;
                    changed = true;
                }
            }
        }
    }

    return s;
}

template <typename Lyt>
std::pair<uint64_t, uint64_t>
compute_layout_extension(const Lyt& gate_lyt, const std::unordered_map<mockturtle::node<Lyt>, uint64_t>& slacks,
                         std::unordered_map<mockturtle::node<Lyt>, uint64_t>& border_properties)
{
    uint64_t              max_x_ext    = 0;
    uint64_t              max_y_ext    = 0;
    uint64_t              corner_slack = 0;
    mockturtle::node<Lyt> corner_node;
    bool                  has_corner = false;

    // First pass: handle pure-border PIs and capture the corner PI
    for (const auto& entry : slacks)
    {
        auto     pi_node = entry.first;
        uint64_t slack   = entry.second;

        auto tile     = gate_lyt.get_tile(pi_node);
        bool at_north = gate_lyt.is_at_northern_border(tile);
        bool at_west  = gate_lyt.is_at_western_border(tile);

        if (at_north && !at_west)
        {
            max_y_ext                  = std::max(max_y_ext, slack);
            border_properties[pi_node] = 0;
        }
        else if (at_west && !at_north)
        {
            max_x_ext                  = std::max(max_x_ext, slack);
            border_properties[pi_node] = 1;
        }
        else if (at_north && at_west)
        {
            corner_slack = slack;
            has_corner   = true;
            corner_node  = pi_node;
        }
    }

    // Second pass: decide where to apply the corner PI’s slack
    if (has_corner)
    {
        uint64_t dx = (corner_slack > max_x_ext) ? (corner_slack - max_x_ext) : 0;
        uint64_t dy = (corner_slack > max_y_ext) ? (corner_slack - max_y_ext) : 0;

        if (dx < dy)
        {
            max_x_ext                      = std::max(max_x_ext, corner_slack);
            border_properties[corner_node] = 1;
        }
        else
        {
            max_y_ext                      = std::max(max_y_ext, corner_slack);
            border_properties[corner_node] = 0;
        }
    }

    return {max_x_ext, max_y_ext};
}

template <typename Lyt>
void move_nodes(Lyt& gate_lyt, fiction::tile<Lyt> tile,
                std::unordered_map<uint64_t, std::vector<mockturtle::signal<Lyt>>>& relations, uint64_t dx, uint64_t dy, bool dz = false)
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
std::unordered_map<mockturtle::node<Lyt>, uint8_t> get_po_border_orientation_map(const Lyt& gate_lyt)
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt must be a gate-level layout");

    std::unordered_map<mockturtle::node<Lyt>, uint8_t> po_orientation;

    gate_lyt.foreach_po([&](const auto& po)
                        {
                            const auto po_node = gate_lyt.get_node(po);
                            const auto po_tile = static_cast<fiction::tile<Lyt>>(po);

                            if (gate_lyt.is_at_eastern_border(po_tile))
                            {
                                po_orientation[po_node] = 0;  // East
                            }
                            else if (gate_lyt.is_at_southern_border(po_tile))
                            {
                                po_orientation[po_node] = 1;  // South
                            }
                            else
                            {
                                // Optional: warn or assign an invalid marker if neither border applies
                                // po_orientation[po_node] = 255;
                            }
                        });

    return po_orientation;
}

template <typename Lyt>
void synchronize_pis_pos(Lyt& gate_lyt)
{
    const auto                                          paths  = path_lengths_grid(gate_lyt);
    auto                                                slacks = compute_slacks<Lyt>(paths);
    auto                                                po_orientation = get_po_border_orientation_map(gate_lyt);
    std::unordered_map<mockturtle::node<Lyt>, uint64_t> border_properties;
    const auto extension   = compute_layout_extension(gate_lyt, slacks, border_properties);
    const auto x_extension = extension.first;
    const auto y_extension = extension.second;

    const auto orig_x = gate_lyt.x();
    const auto orig_y = gate_lyt.y();

    // first save the old relations
    std::unordered_map<uint64_t, std::vector<mockturtle::signal<Lyt>>> fanin_relations;
    gate_lyt.foreach_node(
        [&](const auto& n)
        {
            std::vector<mockturtle::signal<Lyt>> fanins;
            gate_lyt.foreach_fanin(n, [&fanins](const auto fi) {
                                       fanins.push_back(fi);
                                   });
            fanin_relations[n] = fanins;
        });

    for (auto x = static_cast<int64_t>(orig_x); x >= 0; --x)
    {
        for (auto y = static_cast<int64_t>(orig_y); y >= 0; --y)
        {
            fiction::tile<Lyt> tile = {x, y};

            move_nodes(gate_lyt, tile, fanin_relations, x_extension, y_extension);

            ++tile.z;
            if (gate_lyt.get_node(tile))
            {
                move_nodes(gate_lyt, tile, fanin_relations, x_extension, y_extension, true);
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

    for (const auto& entry : slacks)
    {
        const auto pi_node = entry.first;
        const auto slack   = entry.second;

        std::vector<mockturtle::node<Lyt>> fanouts{};
        gate_lyt.foreach_fanout(pi_node, [&fanouts](const auto& fo) { fanouts.push_back(fo); });
        // wire this PI north
        if (border_properties[pi_node] == 0)
        {
            auto t     = gate_lyt.get_tile(pi_node);
            auto new_t = t;
            new_t.y -= slack;
            gate_lyt.move_node(pi_node, new_t);
            ++t.y;
            fiction::detail::wire_south(gate_lyt, new_t, t);
        }
        // wire this PI west
        else if (border_properties[pi_node] == 1)
        {
            auto t     = gate_lyt.get_tile(pi_node);
            auto new_t = t;
            new_t.x -= slack;
            gate_lyt.move_node(pi_node, new_t);
            ++t.x;
            fiction::detail::wire_east(gate_lyt, new_t, t);
        }
        else
        {
            assert(false);
        }
    }

    // also synchronize the POs
    const auto longest_path =
        std::max_element(paths.begin(), paths.end(), [](const auto& a, const auto& b) { return a.first < b.first; })
            ->first;

    uint64_t x_slack_po = 0;
    uint64_t y_slack_po = 0;
    uint64_t max_diagonal = 0;
    gate_lyt.foreach_po([&gate_lyt, &max_diagonal](const auto& po)
                        {
                            auto po_tile = static_cast<fiction::tile<Lyt>>(po);
                            max_diagonal = std::max(max_diagonal, static_cast<uint64_t>(po_tile.x + po_tile.y));
                        });

    gate_lyt.foreach_po([&gate_lyt, &max_diagonal, &po_orientation, &x_slack_po, &y_slack_po](const auto& po)
                        {
                            const auto po_node = gate_lyt.get_node(po);
                            auto po_tile = static_cast<fiction::tile<Lyt>>(po);
                            uint64_t po_diagonal = static_cast<uint64_t>(po_tile.x + po_tile.y);
                            const auto slack = max_diagonal - po_diagonal;

                            if (slack == 0)
                            {
                                return;
                            }

                            std::vector<mockturtle::node<Lyt>> fanins{};
                            gate_lyt.foreach_fanin(po_node, [&gate_lyt, &fanins](const auto& fi) { fanins.push_back(gate_lyt.get_node(fi)); });

                            if (po_orientation[po_node] == 0)
                            {
                                x_slack_po = std::max(x_slack_po, slack);
                                auto new_t = po_tile;
                                new_t.x += slack;
                                gate_lyt.move_node(po_node, new_t);

                                const auto fanin_tile = gate_lyt.get_tile(fanins[0]);
                                if(fanin_tile.x < po_tile.x)
                                {
                                    ++po_tile.x;
                                    fiction::detail::wire_east(gate_lyt, fanin_tile, po_tile);
                                    --po_tile.x;
                                }
                                else
                                {
                                    ++po_tile.y;
                                    fiction::detail::wire_south(gate_lyt, fanin_tile, po_tile);
                                    --po_tile.y;
                                }
                                const auto sig = fiction::detail::wire_east(gate_lyt, po_tile, new_t);

                                gate_lyt.connect(sig, po_node);
                            }
                            else if (po_orientation[po_node] == 1)
                            {
                                y_slack_po = std::max(y_slack_po, slack);
                                auto new_t = po_tile;
                                new_t.y += slack;
                                gate_lyt.move_node(po_node, new_t);

                                const auto fanin_tile = gate_lyt.get_tile(fanins[0]);
                                if(fanin_tile.x < po_tile.x)
                                {
                                    ++po_tile.x;
                                    fiction::detail::wire_east(gate_lyt, fanin_tile, po_tile);
                                    --po_tile.x;
                                }
                                else
                                {
                                    ++po_tile.y;
                                    fiction::detail::wire_south(gate_lyt, fanin_tile, po_tile);
                                    --po_tile.y;
                                }
                                const auto sig = fiction::detail::wire_south(gate_lyt, po_tile, new_t);

                                gate_lyt.connect(sig, po_node);
                            }
                        });

    gate_lyt.resize({gate_lyt.x() + x_extension + x_slack_po, gate_lyt.y() + y_extension + y_slack_po, gate_lyt.z()});
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

    static constexpr const uint64_t bench_select = fiction_experiments::mux21;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        const auto benchmark_network = read_ntk<fiction::tec_nt>(benchmark);

        std::string bench_name = "mux";
        //const auto benchmark_network = mux21<fiction::technology_network>();

        fiction::debug::write_dot_network(benchmark_network);

        fiction::exact_physical_design_params ps_exact{};
        ps_exact.straight_inverters = true;
        ps_exact.scheme             = "2DDWave";
        ps_exact.border_io          = true;
        ps_exact.crossings          = false;
        auto gate_layout_opt        = fiction::exact<gate_layout>(benchmark_network, ps_exact);

        std::cout << "Exact finished\n";

        if (!gate_layout_opt)
        {
            throw std::runtime_error("layout design failed");
        }

        std::cout << "Start applying library\n";

        auto& gate_lyt = *gate_layout_opt;

        fiction::debug::write_dot_layout(gate_lyt, "gate_lyt");

        synchronize_pis_pos(gate_lyt);

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
