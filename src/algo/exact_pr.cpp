//
// Created by marcel on 09.03.17.
//

#include "exact_pr.h"


exact_pr::exact_pr(logic_network_ptr ln, exact_pr_config&& config)
        :
        place_route(std::move(ln)),
        config{config},
        lower_bound{static_cast<unsigned>(network->vertex_count(config.io_ports))},
        solver{ctx}
{
    layout = std::make_shared<fcn_gate_layout>(std::move(*config.scheme), network);
    initialize_vcl_map();
    set_timeout(config.timeout);
}

place_route::pr_result exact_pr::perform_place_and_route()
{
    unsigned time_left = config.timeout;
    auto start = chrono::now(), round = chrono::now();

    for (auto i = config.fixed_size ? config.upper_bound : lower_bound; i <= config.upper_bound; ++i) // <= to prevent overflow
    {
        for (auto& dimension : factorize(i))
        {
            layout->resize(std::move(dimension));

            initialize_tv_map();
            initialize_te_map();
            initialize_tc_map();
            initialize_tp_map();

            if (!layout->is_regularly_clocked())
                initialize_tcl_map();

            if (config.artificial_latch && !config.path_discrepancy)
                initialize_tl_map();

            try
            {
                solver.push();
                generate_smt_instance();

                switch (solver.check())
                {
                    case z3::sat:
                    {
                        auto stop = chrono::now();
                        assign_layout();

                        return pr_result{true, nlohmann::json{{"runtime", calc_runtime(start, stop)}}};
                    }
                    default:
                        break;
                }
                solver.pop();

                // update timeout
                auto time_elapsed = calc_runtime(round, chrono::now());
                time_left = time_left - time_elapsed > 0u ? static_cast<unsigned>(time_left - time_elapsed) : 0u;

                if (!time_left)
                    throw z3::exception("timeout");

                set_timeout(time_left);
                round = chrono::now();
            }
            catch (const z3::exception&)
            {
                return pr_result{false, nlohmann::json{{"runtime", calc_runtime(start, chrono::now())}}};
            }
        }
    }

    return pr_result{false, nlohmann::json{{"runtime", calc_runtime(start, chrono::now())}}};
}

void exact_pr::set_timeout(const unsigned t)
{
    z3::params p{ctx};
    p.set("timeout", t);
    solver.set(p);
}

void exact_pr::initialize_tv_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        for (auto&& v : network->vertices(config.io_ports))
        {
            const auto v_i = network->index(v);
            z3_expr_proxy ep{ctx.bool_const(boost::str(boost::format("tv_%d_%d") % t_i % v_i).c_str())};
            tv_map.emplace(std::make_pair(t_i, v_i), ep);
        }
    }
}

void exact_pr::initialize_te_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        for (auto&& e : network->edges(config.io_ports))
        {
            const auto src = network->index(network->source(e)), tgt = network->index(network->target(e));
            z3_expr_proxy ep{ctx.bool_const(boost::str(boost::format("te_%d_(%d,%d)") % t_i % src % tgt).c_str())};
            te_map.emplace(std::make_pair(t_i, std::make_pair(src, tgt)), ep);
        }
    }
}

void exact_pr::initialize_tc_map()
{
    for (auto&& t1 : layout->ground_layer())
    {
        const auto t1_i = layout->index(t1);
        if (layout->is_regularly_clocked())
        {
            for (auto&& t2 : layout->outgoing_clocked_tiles(t1))
            {
                const auto t2_i = layout->index(t2);
                z3_expr_proxy ep{ctx.bool_const(boost::str(boost::format("tc_%d_%d") % t1_i % t2_i).c_str())};
                tc_map.emplace(std::make_pair(t1_i, t2_i), ep);
            }
        }
        else  // irregular clocking
        {
            for (auto&& t2 : layout->surrounding_2d(t1))
            {
                const auto t2_i = layout->index(t2);
                z3_expr_proxy ep{ctx.bool_const(boost::str(boost::format("tc_%d_%d") % t1_i % t2_i).c_str())};
                tc_map.emplace(std::make_pair(t1_i, t2_i), ep);
            }
        }
    }
}

void exact_pr::initialize_tp_map()
{
    for (auto&& t1 : layout->ground_layer())
    {
        const auto t1_i = layout->index(t1);
        for (auto&& t2 : layout->ground_layer())
        {
            const auto t2_i = layout->index(t2);
            z3_expr_proxy v{ctx.bool_const(boost::str(boost::format("tp_%d_%d") % t1_i % t2_i).c_str())};
            tp_map.emplace(std::make_pair(t1_i, t2_i), v);
        }
    }
}

void exact_pr::initialize_vcl_map()
{
    auto initialize = [this](const logic_vertex _v) -> void
    {
        const auto v_i = network->index(_v);
        z3_expr_proxy ep{ctx.real_const(boost::str(boost::format("vcl_%d") % v_i).c_str())};
        vcl_map.emplace(v_i, ep);
    };

    if (config.io_ports)
    {
        for (auto&& v : network->get_pis())
            initialize(v);
    }
    else
    {
        for (auto&& pi : network->get_pis())
        {
            for (auto&& v : network->adjacent_vertices(pi))
                initialize(v);
        }
    }
}

void exact_pr::initialize_tcl_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        z3_expr_proxy ep{ctx.real_const(boost::str(boost::format("tcl_%d") % t_i).c_str())};
        tcl_map.emplace(t_i, ep);
    }
}

void exact_pr::initialize_tl_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        z3_expr_proxy ep{ctx.int_const(boost::str(boost::format("tl_%d") % t_i).c_str())};
        tl_map.emplace(t_i, ep);
    }
}

z3::expr exact_pr::get_tv(const layout_tile& t, const logic_vertex v)
{
    return tv_map.at(std::make_pair(layout->index(t), network->index(v)))[0u];
}

z3::expr exact_pr::get_te(const layout_tile& t, const logic_edge& e)
{
    return te_map.at(std::make_pair(layout->index(t), std::make_pair(network->index(network->source(e)),
                                                                     network->index(network->target(e)))))[0u];
}

z3::expr exact_pr::get_tc(const layout_tile& t1, const layout_tile& t2)
{
    return tc_map.at(std::make_pair(layout->index(t1), layout->index(t2)))[0u];
}

z3::expr exact_pr::get_tp(const layout_tile& t1, const layout_tile& t2)
{
    return tp_map.at(std::make_pair(layout->index(t1), layout->index(t2)))[0u];
}

z3::expr exact_pr::get_vcl(const logic_vertex v)
{
    return vcl_map.at(network->index(v))[0u];
}

z3::expr exact_pr::get_tcl(const layout_tile& t)
{
    return tcl_map.at(layout->index(t))[0u];
}

z3::expr exact_pr::get_tl(const layout_tile& t)
{
    return tl_map.at(layout->index(t))[0u];
}

z3::expr exact_pr::mk_eq(const z3::expr_vector& v) const noexcept
{
    z3::expr_vector eq{v.ctx()};
    for (auto i : iter::range(1u, v.size()))
        eq.push_back(v[i - 1] == v[i]);

    return z3::mk_and(eq);
}

void exact_pr::tile_ite_counters(const logic_edge& e, z3::expr_vector& ve) noexcept
{
    z3::expr one  = ctx.real_val(1);
    z3::expr zero = ctx.real_val(0);

    z3::expr num_phases = ctx.real_val(layout->num_clocks());

    for (auto&& t : layout->ground_layer())
    {
        // an artificial latch variable counts as an extra 1 clock cycle (n clock phases)
        if (config.artificial_latch)
            ve.push_back(z3::ite(get_te(t, e), get_tl(t) * num_phases + one, zero));
        else
            ve.push_back(z3::ite(get_te(t, e), one, zero));
    }
}

void exact_pr::restrict_tile_elements()
{
    for (auto&& t : layout->ground_layer())
    {
        if (config.crossings)
        {
            z3::expr_vector tv{ctx};
            for (auto&& v : network->vertices(config.io_ports))
                tv.push_back(get_tv(t, v));

            solver.add(z3::atmost(tv, 1u));

            z3::expr_vector te{ctx};
            for (auto&& e : network->edges(config.io_ports))
                te.push_back(get_te(t, e));

            solver.add(z3::atmost(te, 2u));
        }
        else
        {
            z3::expr_vector ve{ctx};
            for (auto&& v : network->vertices(config.io_ports))
                ve.push_back(get_tv(t, v));

            for (auto&& e : network->edges(config.io_ports))
                ve.push_back(get_te(t, e));

            solver.add(z3::atmost(ve, 1u));
        }
    }
}

void exact_pr::restrict_vertices()
{
    for (auto&& v : network->vertices(config.io_ports))
    {
        z3::expr_vector ve{ctx};
        for (auto&& t : layout->ground_layer())
            ve.push_back(get_tv(t, v));

        solver.add(z3::atleast(ve, 1u));
        solver.add(z3::atmost(ve, 1u));
    }
}

void exact_pr::restrict_clocks()
{
    for (auto&& t : layout->ground_layer())
    {
        auto cl = get_tcl(t);

        z3::expr_vector ve{ctx};
        for (auto i : iter::range(layout->num_clocks()))
            ve.push_back(cl == ctx.real_val(i));

        solver.add(z3::mk_or(ve));
    }
}

void exact_pr::restrict_latches()
{
    auto zero = ctx.real_val(0);

    z3::expr_vector ve{ctx};
    for (auto&& t : layout->ground_layer())
    {
        auto l = get_tl(t);
        ve.push_back(l);

        // latches must be positive
        solver.add(l >= zero);

        // tiles without wires cannot have latches
        z3::expr_vector te{ctx};
        for (auto&& e : network->edges(config.io_ports))
            te.push_back(get_te(t, e));
        solver.add(z3::implies(z3::atmost(te, 0u), l == zero));
    }

    // minimize number of latches
    solver.minimize(z3::sum(ve));
}

void exact_pr::define_adjacent_vertex_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& v : (network->vertices(config.io_ports)))
        {
            auto co = get_tv(t, v);
            z3::expr_vector conj{ctx};
            for (auto&& av : network->adjacent_vertices(v, config.io_ports))
            {
                auto ev = network->get_edge(v, av).get();
                z3::expr_vector disj{ctx};

                if (layout->is_regularly_clocked())
                {
                    for (auto&& at : layout->outgoing_clocked_tiles(t))
                        disj.push_back((get_tv(at, av) or get_te(at, ev)) and get_tc(t, at));
                }
                else  // irregular clocking
                {
                    for (auto&& at : layout->surrounding_2d(t))
                    {
                        // clocks must differ by 1
                        auto mod = z3::mod(get_tcl(at) - get_tcl(t), layout->num_clocks()) == ctx.real_val(1);
                        disj.push_back(((get_tv(at, av) or get_te(at, ev)) and mod) and get_tc(t, at));
                    }
                }

                if (!disj.empty())
                    conj.push_back(z3::mk_or(disj));
            }
            if (!conj.empty())
                solver.add(z3::implies(co, z3::mk_and(conj)));
        }
    }
}

void exact_pr::define_inv_adjacent_vertex_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& v : network->vertices(config.io_ports))
        {
            auto co = get_tv(t, v);
            z3::expr_vector conj{ctx};
            for (auto&& iav : network->inv_adjacent_vertices(v, config.io_ports))
            {
                auto iev = network->get_edge(iav, v).get();
                z3::expr_vector disj{ctx};

                if (layout->is_regularly_clocked())
                {
                    for (auto&& iat : layout->incoming_clocked_tiles(t))
                        disj.push_back((get_tv(iat, iav) or get_te(iat, iev)) and get_tc(iat, t));
                }
                else  // irregular clocking
                {
                    for (auto&& iat : layout->surrounding_2d(t))
                    {
                        // clocks must differ by 1
                        auto mod = z3::mod(get_tcl(t) - get_tcl(iat), layout->num_clocks()) == ctx.real_val(1);
                        disj.push_back(((get_tv(iat, iav) or get_te(iat, iev)) and mod) and get_tc(iat, t));
                    }
                }

                if (!disj.empty())
                    conj.push_back(z3::mk_or(disj));
            }
            if (!conj.empty())
                solver.add(z3::implies(co, z3::mk_and(conj)));
        }
    }
}

void exact_pr::define_adjacent_edge_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& e : network->edges(config.io_ports))
        {
            auto te = network->target(e);
            z3::expr_vector disj{ctx};

            if (layout->is_regularly_clocked())
            {
                for (auto&& at : layout->outgoing_clocked_tiles(t))
                    disj.push_back((get_tv(at, te) or get_te(at, e)) and get_tc(t, at));
            }
            else  // irregular clocking
            {
                for (auto&& at : layout->surrounding_2d(t))
                {
                    // clocks must differ by 1
                    auto mod = z3::mod(get_tcl(at) - get_tcl(t), layout->num_clocks()) == ctx.real_val(1);
                    disj.push_back(((get_tv(at, te) or get_te(at, e)) and mod) and get_tc(t, at));
                }
            }

            if (!disj.empty())
                solver.add(z3::implies(get_te(t, e), z3::mk_or(disj)));
        }
    }
}

void exact_pr::define_inv_adjacent_edge_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& e : network->edges(config.io_ports))
        {
            auto se = network->source(e);
            z3::expr_vector disj{ctx};

            if (layout->is_regularly_clocked())
            {
                for (auto&& iat : layout->incoming_clocked_tiles(t))
                    disj.push_back((get_tv(iat, se) or get_te(iat, e)) and get_tc(iat, t));
            }
            else  // irregular clocking
            {
                for (auto&& iat : layout->surrounding_2d(t))
                {
                    // clocks must differ by 1
                    auto mod = z3::mod(get_tcl(t) - get_tcl(iat), layout->num_clocks()) == ctx.real_val(1);
                    disj.push_back(((get_tv(iat, se) or get_te(iat, e)) and mod) and get_tc(iat, t));
                }
            }

            if (!disj.empty())
                solver.add(z3::implies(get_te(t, e), z3::mk_or(disj)));
        }
    }
}

void exact_pr::establish_sub_paths()
{
    for (auto&& t : layout->ground_layer())
    {
        if (layout->is_regularly_clocked())
        {
            for (auto&& at : layout->outgoing_clocked_tiles(t))
                solver.add(z3::implies(get_tc(t, at), get_tp(t, at)));
        }
        else  // irregular clocking
        {
            for (auto&& at : layout->surrounding_2d(t))
                solver.add(z3::implies(get_tc(t, at), get_tp(t, at)));
        }
    }
}

void exact_pr::establish_transitive_paths()
{
    for (auto&& t1 : layout->ground_layer())
    {
        for (auto&& t2 : layout->ground_layer())
        {
            if (layout->index(t1) == layout->index(t2))
                continue;

            for (auto&& t3 : layout->ground_layer())
            {
                if (layout->index(t2) == layout->index(t3))
                    continue;

                solver.add(z3::implies(get_tp(t1, t2) and get_tp(t2, t3), get_tp(t1, t3)));
            }
        }
    }
}

void exact_pr::eliminate_cycles()
{
    for (auto&& t : layout->ground_layer())
        solver.add(not get_tp(t, t));
}

void exact_pr::assign_pi_clockings()
{
    auto assign = [&](const logic_vertex _v) -> void
    {
        auto cl = get_vcl(_v);
        for (auto&& t : layout->ground_layer())
        {
            if (layout->is_regularly_clocked())
                solver.add(z3::implies(get_tv(t, _v), cl == ctx.real_val(*layout->tile_clocking(t))));
            else  // irregular clocking
                solver.add(z3::implies(get_tv(t, _v), cl == get_tcl(t)));
        }
    };

    if (config.io_ports)
    {
        for (auto&& v : network->get_pis())
            assign(v);
    }
    else
    {
        for (auto&& pi : network->get_pis())
        {
            for (auto&& v : network->adjacent_vertices(pi))
                assign(v);
        }
    }
}

void exact_pr::fanin_length()
{
    using logic_edge_path = logic_network::edge_path;
    auto define_length = [this](const logic_vertex _v) -> void
    {
        auto paths = network->get_all_paths(_v, config.io_ports);
        if (paths.empty())
            return;

        auto max_length = std::max_element(paths.cbegin(), paths.cend(),
                                           [](const logic_edge_path& p1, const logic_edge_path& p2)
                                           {return p1.size() < p2.size();})->size();

        z3::expr_vector all_path_lengths{ctx};
        for (auto& p : paths)
        {
            z3::expr_vector path_length{ctx};

            // respect number of vertices as an offset to path length
            // this works because every vertex must be placed
            auto offset = static_cast<int>(p.size() - max_length);
            if (offset)
                path_length.push_back(ctx.real_val(offset));

            for (auto& e : p)
            {
                // respect clock zone of PI if one is involved
                auto s = network->source(e);
                if (config.io_ports && network->is_pi(s))
                    path_length.push_back(get_vcl(s));
                else if (!config.io_ports)
                {
                    if (network->pre_pi(s))
                        path_length.push_back(get_vcl(s));
                }

                tile_ite_counters(e, path_length);
            }
            all_path_lengths.push_back(z3::sum(path_length));
        }
        solver.add(mk_eq(all_path_lengths));
    };

    if (config.io_ports)
    {
        for (auto&& po : network->get_pos())
            define_length(po);
    }
    else
    {
        for (auto&& po : network->get_pos())
        {
            for (auto&& v : network->inv_adjacent_vertices(po))
                define_length(v);
        }
    }
}

void exact_pr::prevent_insufficiencies()
{
    for (auto&& t : layout->ground_layer())
    {
        if (layout->is_regularly_clocked())
        {
            for (auto&& v : network->vertices(config.io_ports))
            {
                // if vertex v has more adjacent or inversely adjacent elements than tile t
                if (layout->out_degree(t) < static_cast<unsigned>(network->out_degree(v, config.io_ports)) ||
                        layout->in_degree(t) < static_cast<unsigned>(network->in_degree(v, config.io_ports)))
                    solver.add(not get_tv(t, v));
            }

            for (auto&& e : network->edges(config.io_ports))
            {
                // if tile t has no adjacent or inversely adjacent tiles
                if (layout->out_degree(t) == 0 || layout->in_degree(t) == 0)
                    solver.add(not get_te(t, e));
            }
        }
        else  // irregular clocking
        {
            auto surrounding_tiles = layout->surrounding_2d(t);
            auto tile_degree = std::distance(surrounding_tiles.begin(), surrounding_tiles.end());

            for (auto&& v : network->vertices(config.io_ports))
            {
                // in an irregular clocking scheme, not so strict restrictions can be made
                if (tile_degree < static_cast<unsigned>(network->out_degree(v, config.io_ports) +
                        network->in_degree(v, config.io_ports)))
                    solver.add(not get_tv(t, v));
            }
        }
    }
}

void exact_pr::define_number_of_connections()
{
    for (auto&& t1 : layout->ground_layer())
    {
        z3::expr_vector ccp{ctx};

        // collect all variables representing connections t -> t'
        z3::expr_vector acc{ctx};
        if (layout->is_regularly_clocked())
        {
            for (auto&& at : layout->outgoing_clocked_tiles(t1))
            {
                auto tc = get_tc(t1, at);
                acc.push_back(tc);
                ccp.push_back(tc);
            }
        }
        else  // irregular clocking
        {
            for (auto&& at : layout->surrounding_2d(t1))
            {
                auto tc = get_tc(t1, at);
                acc.push_back(tc);
                ccp.push_back(tc);
            }
        }

        // collect all variables representing connections t' -> t
        z3::expr_vector iacc{ctx};
        if (layout->is_regularly_clocked())
        {
            for (auto&& iat : layout->incoming_clocked_tiles(t1))
            {
                auto tc = get_tc(iat, t1);
                iacc.push_back(tc);
                ccp.push_back(tc);
            }
        }
        else  // irregular clocking
        {
            for (auto&& iat : layout->surrounding_2d(t1))
            {
                auto tc = get_tc(iat, t1);
                iacc.push_back(tc);
                ccp.push_back(tc);
            }
        }

        z3::expr_vector ow{ctx};

        for (auto&& v : network->vertices(config.io_ports))
        {
            auto tv   = get_tv(t1, v);
            auto aon  = static_cast<unsigned>(network->out_degree(v, config.io_ports));
            auto iaon = static_cast<unsigned>(network->in_degree(v, config.io_ports));

            ow.push_back(tv);

            // if vertex v is assigned to a tile, the number of connections need to correspond
            if (!acc.empty())
                solver.add(z3::implies(tv, z3::atleast(acc, aon) and z3::atmost(acc, aon)));
            if (!iacc.empty())
                solver.add(z3::implies(tv, z3::atleast(iacc, iaon) and z3::atmost(iacc, iaon)));
        }

        z3::expr_vector wv{ctx};
        for (auto&& e : network->edges(config.io_ports))
        {
            auto te = get_te(t1, e);

            ow.push_back(te);
            wv.push_back(te);
        }
        // if there is any edge assigned to a tile, the number of connections need to correspond
        if (!acc.empty())
            solver.add(z3::implies(z3::atleast(wv, 1u) and z3::atmost(wv, 1u),
                                   z3::atleast(acc, 1u) and z3::atmost(acc, 1u)));
        if (!iacc.empty())
            solver.add(z3::implies(z3::atleast(wv, 1u) and z3::atmost(wv, 1u),
                                   z3::atleast(iacc, 1u) and z3::atmost(iacc, 1u)));

        // if crossings are allowed, there must be exactly two connections in each direction
        if (config.crossings)
        {
            if (!acc.empty())
                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                        z3::atleast(acc, 2u) and z3::atmost(acc, 2u)));
            if (!iacc.empty())
                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                        z3::atleast(iacc, 2u) and z3::atmost(iacc, 2u)));
        }

        // collect all variables representing paths t -> t' and t' -> t
        for (auto&& t2 : layout->ground_layer())
        {
            ccp.push_back(get_tp(t1, t2));
            // add symmetrical path only if t1 != t2
            if (layout->index(t1) != layout->index(t2))
                ccp.push_back(get_tp(t2, t1));
        }

        // if tile t is empty, there must not be any connection or path from or to tile t established
        if (!ow.empty() && !ccp.empty())
            solver.add(z3::atmost(ow, 0u) == z3::atmost(ccp, 0u));
    }
}

void exact_pr::enforce_border_io()
{
    auto assign_border = [this](const logic_vertex _v)
    {
        for (auto&& t : layout->ground_layer())
        {
            if (layout->is_border_tile(t))
                continue;

            solver.add(not get_tv(t, _v));
        }
    };

    if (config.io_ports)
    {
        for (auto&& pi : network->get_pis())
            assign_border(pi);
        for (auto&& po : network->get_pos())
            assign_border(po);
    }
    else
    {
        for (auto&& pi : network->get_pis())
        {
            for (auto&& v : network->adjacent_vertices(pi))
                assign_border(v);
        }
        for (auto&& po : network->get_pos())
        {
            for (auto&& v : network->inv_adjacent_vertices(po))
                assign_border(v);
        }
    }
}

void exact_pr::limit_wire_length()
{
    for (auto&& e : network->edges(config.io_ports))
    {
        z3::expr_vector ve{ctx};
        for (auto&& t : layout->ground_layer())
            ve.push_back(get_te(t, e));

        solver.add(z3::atmost(ve, config.wire_limit));
    }
}

void exact_pr::limit_crossings()
{
    z3::expr_vector crossings_counter{ctx};
    for (auto&& t : layout->ground_layer())
    {
        z3::expr_vector wv{ctx};
        for (auto&& e : network->edges(config.io_ports))
            wv.push_back(get_te(t, e));

        crossings_counter.push_back(z3::ite(z3::atleast(wv, 2u), ctx.real_val(1), ctx.real_val(0)));
    }

    solver.add(z3::sum(crossings_counter) <= ctx.real_val(config.crossings_limit));
}

void exact_pr::generate_smt_instance()
{
    // layout constraints
    restrict_tile_elements();
    restrict_vertices();

    // open clocking
    if (!layout->is_regularly_clocked())
        restrict_clocks();

    if (config.artificial_latch && !config.path_discrepancy)
        restrict_latches();

    // adjacency constraints
    define_adjacent_vertex_tiles();
    define_inv_adjacent_vertex_tiles();
    define_adjacent_edge_tiles();
    define_inv_adjacent_edge_tiles();

    // path/cycle constraints
    establish_sub_paths();
    establish_transitive_paths();
    eliminate_cycles();

    // fan-in constraints
    assign_pi_clockings();
    if (!config.path_discrepancy)
        fanin_length();

    // backdoor constraints
    prevent_insufficiencies();
    define_number_of_connections();

    // physical restrictions and limitations constraints
    if (config.border_io)
        enforce_border_io();

    if (config.wire_limit)
        limit_wire_length();

    if (config.crossings && config.crossings_limit)
        limit_crossings();

//    std::cout << solver.assertions() << std::endl;

}

void exact_pr::assign_layout()
{
    auto mdl = solver.get_model();

    // assign vertices to tiles
    for (auto&& t : layout->ground_layer())
    {
        auto element_placed = false;
        for (auto&& v : network->vertices(config.io_ports))
        {
            // if vertex v is set to tile t
            if (mdl.eval(get_tv(t, v)).bool_value() == Z3_L_TRUE)
            {
                // check for I/Os
                bool pi, po;
                if (config.io_ports)
                {
                    pi = network->is_pi(v);
                    po = network->is_po(v);
                }
                else
                {
                    pi = network->pre_pi(v);
                    po = network->post_po(v);
                }

                layout->assign_logic_vertex(t, v, pi, po);

                // there should not be two elements on the same tile
                element_placed = true;
                break;
            }
        }

        if (element_placed)
            continue;

        // assign edges to tiles
        for (auto&& e : network->edges(config.io_ports))
        {
            // if edge e is set to tile t
            if (mdl.eval(get_te(t, e)).bool_value() == Z3_L_TRUE)
            {
                // there is already an edge in ground level
                if (layout->is_wire_tile(t))
                    // assign to above tile
                    layout->assign_logic_edge(layout->above(t), e);
                    // no other edge yet
                else
                    // assign to ground level
                    layout->assign_logic_edge(t, e);
            }
        }
    }

    // assign clock zones to tiles of open schemes
    if (!layout->is_regularly_clocked())
    {
        for (auto&& t : layout->ground_layer())
            layout->assign_clocking(t, static_cast<fcn_clock::zone>(mdl.eval(get_tcl(t), true).get_numeral_int()));
    }  // from now on, a clocking scheme is assigned and no distinction between regular and irregular must be made


    // assign directions to tiles
    for (auto&& t1 : layout->ground_layer())
    {
        for (auto&& t2 : layout->outgoing_clocked_tiles(t1))
        {
            // if connection t1 -> t2 is established
            if (mdl.eval(get_tc(t1, t2)).bool_value() == Z3_L_TRUE)
            {
                layout_tile _t1, _t2;
                if (layout->is_data_flow(t1, t2))
                { _t1 = t1; _t2 = t2; }
                else if (layout->is_data_flow(t1, layout->above(t2)))
                { _t1 = t1; _t2 = layout->above(t2); }
                else if (layout->is_data_flow(layout->above(t1), t2))
                { _t1 = layout->above(t1); _t2 = t2; }
                else if (layout->is_data_flow(layout->above(t1), layout->above(t2)))
                { _t1 = layout->above(t1); _t2 = layout->above(t2); }

                // assign outgoing directions
                for (auto& e : layout->get_logic_edges(_t1))
                    layout->assign_wire_out_dir(_t1, e, layout->get_bearing(_t1, _t2));
                layout->assign_tile_out_dir(_t1, layout->get_bearing(_t1, _t2));

                // assign incoming directions
                for (auto& e : layout->get_logic_edges(_t2))
                    layout->assign_wire_inp_dir(_t2, e, layout->get_bearing(_t2, _t1));
                layout->assign_tile_inp_dir(_t2, layout->get_bearing(_t2, _t1));
            }
        }
    }

    // assign artificial latches if there were any in use
    if (config.artificial_latch)
    {
        for (auto&& t : layout->ground_layer())
            layout->assign_latch(t, static_cast<fcn_gate_layout::latch_delay>
                                    (mdl.eval(get_tl(t), true).get_numeral_int()) * layout->num_clocks());
    }
}
