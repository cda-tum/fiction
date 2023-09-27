//
// Created by marcel on 15.09.23.
//

#ifndef FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP
#define FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/algorithms/physical_design/design_sidb_gates.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/fcn_gate_library.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <array>

namespace fiction
{

class sidb_dynamic_gate_library : public fcn_gate_library<sidb_technology, 60, 46>  // width and height of a hexagon
{
  public:
    explicit sidb_dynamic_gate_library() = delete;

    template <typename GateLyt, typename CellLyt>
    [[nodiscard]] static fcn_gate set_up_gate(const GateLyt& lyt, const tile<GateLyt>& t,
                                              const sidb_surface<CellLyt>& sidb_surface)
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt must be a gate level layout.");
        static_assert(!has_siqad_coord_v<CellLyt>, "CellLyt has SiQAD coordinates.");

        const auto n = lyt.get_node(t);
        const auto f = lyt.node_function(n);
        const auto p = determine_port_routing(lyt, t);

        const design_sidb_gates_params params{sidb_simulation_parameters{2, -0.32},
                                              design_sidb_gates_params::design_sidb_gates_mode::RANDOM,
                                              {{15, 7, 0}, {25, 13, 0}},
                                              4,
                                              sidb_simulation_engine::QUICKEXACT};

        auto absolut_cell       = cell<CellLyt>{t.x * 45, t.y * (60 * 3 / 4), t.z};
        auto absolut_cell_siqad = fiction::siqad::to_siqad_coord(absolut_cell);

        auto layout        = sidb_defect_cell_clk_lyt_siqad{};
        auto update_layout = [&layout, &absolut_cell, &absolut_cell_siqad](const auto& cd)
        {
            if (euclidean_distance(CellLyt{}, cd.first, absolut_cell) < 30)
            {

                auto defect_pos_siqad    = fiction::siqad::to_siqad_coord(cd.first);
                auto shifted_defect_cell = defect_pos_siqad - absolut_cell_siqad;
                if (is_charged_defect(cd.second))
                {
                    if (cd.second.type == sidb_defect_type::DB)
                    {
                        layout.assign_sidb_defect(shifted_defect_cell,
                                                  sidb_defect{sidb_defect_type::DB, -1, 10.6, 5.9});
                    }
                    else if (cd.second.type == sidb_defect_type::SI_VACANCY)
                    {
                        layout.assign_sidb_defect(shifted_defect_cell,
                                                  sidb_defect{sidb_defect_type::SI_VACANCY, 1, 9.7, 2.1});
                    }
                }
                else
                {
                    layout.assign_sidb_defect(shifted_defect_cell, cd.second);
                }
            }
        };

        std::string path = " ";
        try
        {
            if constexpr (fiction::has_is_fanout_v<GateLyt>)
            {
                if (lyt.is_fanout(n))
                {
                    if (lyt.fanout_size(n) == 2)
                    {
                        path = FANOUT_MAP.at(p);
                        std::cout << path << std::endl;
                        layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                            "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                            path);
                        // erase defects
                        sidb_surface.foreach_sidb_defect(update_layout);
                        const auto found_gate_layouts = design_sidb_gates(layout, create_fan_out_tt(), params);
                        std::cout << fmt::format("{} : defects", found_gate_layouts[0].num_defects()) << std::endl;
                        fiction::write_sqd_layout(found_gate_layouts[0],
                                                  fmt::format("{}/{}.sqd",
                                                              "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                              "experiments/defect_aware_physical_design/single_gates",
                                                              n));
                        const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                        std::cout << "finished" << std::endl;
                        return cell_list_to_gate<char>(list);
                    }
                }
            }
            if constexpr (fiction::has_is_buf_v<GateLyt>)
            {
                if (lyt.is_buf(n))
                {
                    if (lyt.is_ground_layer(t))
                    {
                        // crossing case
                        if (const auto at = lyt.above(t); (t != at) && lyt.is_wire_tile(at))
                        {
                            // two possible options: actual crossover and (parallel) hourglass wire
                            const auto pa = determine_port_routing(lyt, at);

                            path = CROSSING_MAP.at({p, pa});
                            if (path == "2o2o_hour.sqd")
                            {
                                path = "2i2o.sqd";
                                std::cout << path << std::endl;
                                layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                                    "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/"
                                    "skeleton_bestagons_with_tags/" +
                                    path);
                                sidb_surface.foreach_sidb_defect(update_layout);
                                const auto found_gate_layouts =
                                    design_sidb_gates(layout, create_double_wire_tt(), params);
                                std::cout << fmt::format("{} : defects", found_gate_layouts[0].num_defects())
                                          << std::endl;
                                fiction::write_sqd_layout(found_gate_layouts[0],
                                                          fmt::format("{}/{}.sqd",
                                                                      "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                                      "experiments/defect_aware_physical_design/single_gates",
                                                                      n));
                                const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                                std::cout << "finished hourglass" << std::endl;
                                return cell_list_to_gate<char>(list);
                            }
                            else if (path == "2o2o_cx.sqd")
                            {
                                path = "2i2o.sqd";
                                std::cout << path << std::endl;
                                layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                                    "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/"
                                    "skeleton_bestagons_with_tags/" +
                                    path);
                                sidb_surface.foreach_sidb_defect(update_layout);
                                const auto found_gate_layouts =
                                    design_sidb_gates(layout, create_crossing_wire_tt(), params);
                                std::cout << fmt::format("{} : defects", found_gate_layouts[0].num_defects())
                                          << std::endl;
                                fiction::write_sqd_layout(
                                    found_gate_layouts[0],
                                    fmt::format("{}/{}.sqd",
                                                "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                "experiments/defect_aware_physical_design/single_gates",
                                                n));
                                std::cout << "finished crossing" << std::endl;
                                const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                                return cell_list_to_gate<char>(list);
                            }
                        }

                        path = WIRE_MAP.at(p);
                        if (path == "empty")
                        {
                            return EMPTY_GATE;
                        }
                        std::cout << path << std::endl;
                        layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                            "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                            path);
                        sidb_surface.foreach_sidb_defect(update_layout);
                        std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                        const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                        const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                        fiction::write_sqd_layout(found_gate_layouts[0],
                                                  fmt::format("{}/{}.sqd",
                                                              "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                              "experiments/defect_aware_physical_design/single_gates",
                                                              n));
                        std::cout << "finished" << std::endl;
                        return cell_list_to_gate<char>(list);
                    }
                    return EMPTY_GATE;
                }
            }
            if constexpr (fiction::has_is_inv_v<GateLyt>)
            {
                if (lyt.is_inv(n))
                {
                    path = INVERTER_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (mockturtle::has_is_and_v<GateLyt>)
            {
                if (lyt.is_and(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (mockturtle::has_is_or_v<GateLyt>)
            {
                if (lyt.is_or(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_nand_v<GateLyt>)
            {
                if (lyt.is_nand(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_nor_v<GateLyt>)
            {
                if (lyt.is_nor(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (mockturtle::has_is_xor_v<GateLyt>)
            {
                if (lyt.is_xor(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_xnor_v<GateLyt>)
            {
                if (lyt.is_xnor(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_ge_v<GateLyt>)
            {
                if (lyt.is_ge(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_le_v<GateLyt>)
            {
                if (lyt.is_le(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list               = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_gt_v<GateLyt>)
            {
                if (lyt.is_gt(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
            if constexpr (fiction::has_is_lt_v<GateLyt>)
            {
                if (lyt.is_lt(n))
                {
                    path = CONJUNCTION_MAP.at(p);
                    std::cout << path << std::endl;
                    layout = read_sqd_layout<sidb_defect_cell_clk_lyt_siqad>(
                        "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/skeleton_bestagons_with_tags/" +
                        path);
                    sidb_surface.foreach_sidb_defect(update_layout);
                    std::cout << fmt::format("{} : defects", layout.num_defects()) << std::endl;
                    const auto found_gate_layouts = design_sidb_gates(layout, std::vector<tt>{f}, params);
                    fiction::write_sqd_layout(found_gate_layouts[0],
                                              fmt::format("{}/{}.sqd",
                                                          "/Users/jandrewniok/CLionProjects/fiction_fork/"
                                                          "experiments/defect_aware_physical_design/single_gates",
                                                          n));
                    const auto list = cell_level_layout_to_list(found_gate_layouts.front());
                    std::cout << "finished" << std::endl;
                    return cell_list_to_gate<char>(list);
                }
            }
        }
        catch (const std::out_of_range&)
        {
            throw unsupported_gate_orientation_exception(t, p);
        }
    }

  private:
    template <typename Lyt>
    static std::array<std::array<char, 60>, 46> cell_level_layout_to_list(const Lyt& lyt)
    {
        uint64_t                             counter = 0;
        std::array<std::array<char, 60>, 46> empty_array{};
        const auto                           all_cell = all_sidbs_in_spanned_area({-10, -1, 0}, {49, 21, 1});
        for (auto i = 0u; i < 46; i++)
        {
            std::array<char, 60> inner_array{};
            for (auto j = 0u; j < 60; j++)
            {
                if ((lyt.get_cell_type(all_cell[counter]) == Lyt::technology::cell_type::NORMAL) ||
                    (lyt.get_cell_type(all_cell[counter]) == Lyt::technology::cell_type::OUTPUT))
                {
                    if (all_cell[counter].y != 19 || all_cell[counter].z != 0)
                    {
                        inner_array[j] = 'x';
                    }
                    else
                    {
                        inner_array[j] = ' ';
                    }
                }
                else
                {
                    inner_array[j] = ' ';
                }
                counter += 1;
            }
            empty_array[i] = inner_array;
        }
        return empty_array;
    }

    template <typename Lyt>
    [[nodiscard]] static port_list<port_direction> determine_port_routing(const Lyt& lyt, const tile<Lyt>& t) noexcept
    {
        port_list<port_direction> p{};

        // determine incoming connector ports
        if (lyt.has_north_eastern_incoming_signal(t))
        {
            p.inp.emplace(port_direction::cardinal::NORTH_EAST);
        }
        if (lyt.has_north_western_incoming_signal(t))
        {
            p.inp.emplace(port_direction::cardinal::NORTH_WEST);
        }

        // determine outgoing connector ports
        if (lyt.has_south_eastern_outgoing_signal(t))
        {
            p.out.emplace(port_direction::cardinal::SOUTH_EAST);
        }
        if (lyt.has_south_western_outgoing_signal(t))
        {
            p.out.emplace(port_direction::cardinal::SOUTH_WEST);
        }

        // gates without connector ports

        // 1-input functions
        if (const auto n = lyt.get_node(t); lyt.is_pi(n) || lyt.is_po(n) || lyt.is_buf(n) || lyt.is_inv(n))
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(port_direction::cardinal::NORTH_WEST);
            }
            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(port_direction::cardinal::SOUTH_EAST);
            }
        }
        else  // 2-input functions
        {
            if (lyt.has_no_incoming_signal(t))
            {
                p.inp.emplace(port_direction::cardinal::NORTH_WEST);
                p.inp.emplace(port_direction::cardinal::NORTH_EAST);
            }
            if (lyt.has_no_outgoing_signal(t))
            {
                p.out.emplace(port_direction::cardinal::SOUTH_EAST);
            }
        }

        return p;
    };

    using port_gate_map = phmap::flat_hash_map<port_list<port_direction>, std::string>;
    using double_port_gate_map =
        phmap::flat_hash_map<std::pair<port_list<port_direction>, port_list<port_direction>>, std::string>;

    /**
     * Lookup table for wire mirroring. Maps ports to corresponding wires.
     */
    static inline const port_gate_map WIRE_MAP = {
        // primary inputs
        {{{}, {port_direction(port_direction::cardinal::SOUTH_WEST)}}, "1i1o_s.sqd"},
        {{{}, {port_direction(port_direction::cardinal::SOUTH_EAST)}}, "1i1o_d.sqd"},
        // primary outputs
        {{{port_direction(port_direction::cardinal::NORTH_WEST)}, {}}, "1i1o_d.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)}, {}}, "1i1o_sm.sqd"},
        // straight wire
        {{{port_direction(port_direction::cardinal::NORTH_WEST)},
          {port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i1o_s.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST)}},
         "1i1o_sm.sqd"},
        // diagonal wire
        {{{port_direction(port_direction::cardinal::NORTH_WEST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST)}},
         "1i1o_d.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i1o_dm.sqd"},
        // empty gate (for crossing layer)
        {{{}, {}}, "empty"},
    };
    /**
     * Lookup table for wire crossings and hourglass wires. Maps ports to corresponding crossovers.
     */
    static inline const double_port_gate_map CROSSING_MAP = {
        {{{{port_direction(port_direction::cardinal::NORTH_WEST)},
           {port_direction(port_direction::cardinal::SOUTH_WEST)}},
          {{port_direction(port_direction::cardinal::NORTH_EAST)},
           {port_direction(port_direction::cardinal::SOUTH_EAST)}}},
         "2o2o_hour.sqd"},
        {{{{port_direction(port_direction::cardinal::NORTH_EAST)},
           {port_direction(port_direction::cardinal::SOUTH_EAST)}},
          {{port_direction(port_direction::cardinal::NORTH_WEST)},
           {port_direction(port_direction::cardinal::SOUTH_WEST)}}},
         "2o2o_hour.sqd"},
        {{{{port_direction(port_direction::cardinal::NORTH_WEST)},
           {port_direction(port_direction::cardinal::SOUTH_EAST)}},
          {{port_direction(port_direction::cardinal::NORTH_EAST)},
           {port_direction(port_direction::cardinal::SOUTH_WEST)}}},
         "2o2o_cx.sqd"},
        {{{{port_direction(port_direction::cardinal::NORTH_EAST)},
           {port_direction(port_direction::cardinal::SOUTH_WEST)}},
          {{port_direction(port_direction::cardinal::NORTH_WEST)},
           {port_direction(port_direction::cardinal::SOUTH_EAST)}}},
         "2o2o_cx.sqd"},
    };
    /**
     * Lookup table for inverter mirroring. Maps ports to corresponding inverters.
     */
    static inline const port_gate_map INVERTER_MAP = {
        // straight inverters
        {{{port_direction(port_direction::cardinal::NORTH_WEST)},
          {port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i1o_s.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST)}},
         "1i1o_sm.sqd"},
        // diagonal inverters
        {{{port_direction(port_direction::cardinal::NORTH_WEST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST)}},
         "1i1o_d.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i1o_dm.sqd"},
        // without inputs
        {{{}, {port_direction(port_direction::cardinal::SOUTH_WEST)}}, "1i1o_s.sqd"},
        {{{}, {port_direction(port_direction::cardinal::SOUTH_EAST)}}, "1i1o_d.sqd"},
        // without outputs
        {{{port_direction(port_direction::cardinal::NORTH_WEST)}, {}}, "1i1o_d.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)}, {}}, "1i1o_sm.sqd"}};
    /**
     * Lookup table for conjunction mirroring. Maps ports to corresponding AND gates.
     */
    static inline const port_gate_map CONJUNCTION_MAP = {
        {{{port_direction(port_direction::cardinal::NORTH_WEST), port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST)}},
         "2i1o.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_WEST), port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "2i1o_m.sqd"}};
    /**
     * Lookup table for negated disjunction mirroring. Maps ports to corresponding NOR gates.
     */

    /**
     * Lookup table for fanout mirroring. Maps ports to corresponding fan-out gates.
     */
    static inline const port_gate_map FANOUT_MAP = {
        {{{port_direction(port_direction::cardinal::NORTH_WEST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST), port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i2o.sqd"},
        {{{port_direction(port_direction::cardinal::NORTH_EAST)},
          {port_direction(port_direction::cardinal::SOUTH_EAST), port_direction(port_direction::cardinal::SOUTH_WEST)}},
         "1i2o_m.sqd"}};
};

}  // namespace fiction

#endif  // FICTION_SIDB_DYNAMIC_GATE_LIBRARY_HPP
