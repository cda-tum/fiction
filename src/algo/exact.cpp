//
// Created by marcel on 09.03.17.
//

#include "exact.h"


exact::exact(logic_network_ptr ln, exact_pd_config&& config)
        :
        physical_design(std::move(ln)),
        config{config},
        solver{ctx}
{
    layout = std::make_shared<fcn_gate_layout>(std::move(*config.scheme), network,
            config.vertical_offset ? fcn_layout::offset::VERTICAL : fcn_layout::offset::NONE);
    network->substitute_fan_outs();

    hierarchy = std::make_shared<network_hierarchy>(network, false);
    hierarchy->unify_output_ranks();
    hierarchy->unify_inv_input_ranks();

    lower_bound = network->vertex_count(config.io_ports);

    if (!config.desynchronize && !config.topolinano)
        initialize_vcl_map();

    set_timeout(config.timeout);
}

physical_design::pd_result exact::operator()()
{
    mockturtle::stopwatch<>::duration time{0};

    for (auto i = config.fixed_size ? config.fixed_size : lower_bound; i <= config.upper_bound; ++i) // <= to prevent overflow
    {

#if !(__WINDOWS__)
        mockturtle::progress_bar bar("[i] examining layout dimensions: {:>2} × {:<2}");
#endif

        for (auto& dimension : factorize(i))
        {
            // ToPoliNano optimization: skip all dimensions where X <= levels + number of fan-outs - 1 and
            // Y < maximum of number of PIs and number of POs; this should not be too restrictive
            if (config.topolinano)
            {
                if (dimension[X] <= hierarchy->height() + network->operation_count(operation::F1O2) - 1 ||
                    dimension[Y] < std::max(network->num_pis(), network->num_pos()))
                    continue;
            }

            mockturtle::stopwatch stop{time};

#if !(__WINDOWS__)
            bar(dimension[X], dimension[Y]);
#endif

            layout->resize(std::move(dimension));

            initialize_tv_map();
            initialize_te_map();
            initialize_tc_map();

            if (!config.topolinano && !config.twoddwave)  // linear schemes; no cycles by definition
                initialize_tp_map();

            if (!layout->is_regularly_clocked())
                initialize_tcl_map();

            if (config.artificial_latches && !config.desynchronize)
                initialize_tl_map();

            try
            {
                solver.push();
                generate_smt_instance();

                switch (solver.check())
                {
                    case z3::sat:
                    {
                        assign_layout();

                        stop.~stopwatch();
                        return pd_result{true, nlohmann::json{{"runtime (s)", mockturtle::to_seconds(time)}}};
                    }
                    default:
                        break;
                }
                solver.pop();

                stop.~stopwatch();

                auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
                auto time_left = (config.timeout - time_elapsed > 0 ? static_cast<unsigned>(config.timeout - time_elapsed) : 0u);

                if (!time_left)
                    throw z3::exception("timeout");

                set_timeout(time_left);
            }
            catch (const z3::exception&)
            {
                return pd_result{false, nlohmann::json{{"runtime (s)", mockturtle::to_seconds(time)}}};
            }
        }
    }

    return pd_result{false, nlohmann::json{{"runtime (s)", mockturtle::to_seconds(time)}}};
}

void exact::set_timeout(const unsigned t)
{
    z3::params p{ctx};
    p.set("timeout", t);
    solver.set(p);
}

void exact::initialize_tv_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        for (auto&& v : network->vertices(config.io_ports))
        {
            z3_expr_proxy ep{ctx.bool_const(fmt::format("tv_{}_{}", t_i, v).c_str())};
            tv_map.emplace(std::make_pair(t_i, v), ep);
        }
    }
}

void exact::initialize_te_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        for (auto&& e : network->edges(config.io_ports))
        {
            const auto src = network->source(e), tgt = network->target(e);
            z3_expr_proxy ep{ctx.bool_const(fmt::format("te_{}_({},{})", t_i, src, tgt).c_str())};
            te_map.emplace(std::make_pair(t_i, std::make_pair(src, tgt)), ep);
        }
    }
}

void exact::initialize_tc_map()
{
    for (auto&& t1 : layout->ground_layer())
    {
        if (const auto t1_i = layout->index(t1); layout->is_regularly_clocked())
        {
            for (auto&& t2 : layout->outgoing_clocked_tiles(t1))
            {
                const auto t2_i = layout->index(t2);
                z3_expr_proxy ep{ctx.bool_const(fmt::format("tc_{}_{}", t1_i, t2_i).c_str())};
                tc_map.emplace(std::make_pair(t1_i, t2_i), ep);
            }
        }
        else  // irregular clocking
        {
            for (auto&& t2 : layout->surrounding_2d(t1))
            {
                const auto t2_i = layout->index(t2);
                z3_expr_proxy ep{ctx.bool_const(fmt::format("tc_{}_{}", t1_i, t2_i).c_str())};
                tc_map.emplace(std::make_pair(t1_i, t2_i), ep);
            }
        }
    }
}

void exact::initialize_tp_map()
{
    for (auto&& t1 : layout->ground_layer())
    {
        const auto t1_i = layout->index(t1);
        for (auto&& t2 : layout->ground_layer())
        {
            const auto t2_i = layout->index(t2);
            z3_expr_proxy v{ctx.bool_const(fmt::format("tp_{}_{}", t1_i, t2_i).c_str())};
            tp_map.emplace(std::make_pair(t1_i, t2_i), v);
        }
    }
}

void exact::initialize_vcl_map()
{
    auto initialize = [this](const logic_vertex _v) -> void
    {
        z3_expr_proxy ep{ctx.real_const(fmt::format("vcl_{}", _v).c_str())};
        vcl_map.emplace(_v, ep);
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

void exact::initialize_tx_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        z3_expr_proxy ep{ctx.bool_const(fmt::format("tx_{}", t_i).c_str())};
        tcl_map.emplace(t_i, ep);
    }
}

void exact::initialize_tcl_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        z3_expr_proxy ep{ctx.real_const(fmt::format("tcl_{}", t_i).c_str())};
        tcl_map.emplace(t_i, ep);
    }
}

void exact::initialize_tl_map()
{
    for (auto&& t : layout->ground_layer())
    {
        const auto t_i = layout->index(t);
        z3_expr_proxy ep{ctx.int_const(fmt::format("tl_{}", t_i).c_str())};
        tl_map.emplace(t_i, ep);
    }
}

z3::expr exact::get_tv(const layout_tile& t, const logic_vertex v) noexcept
{
    return tv_map[{layout->index(t), v}][0u];
}

z3::expr exact::get_te(const layout_tile& t, const logic_edge& e) noexcept
{
    return te_map[{layout->index(t), {network->source(e), network->target(e)}}][0u];
}

z3::expr exact::get_tc(const layout_tile& t1, const layout_tile& t2) noexcept
{
    return tc_map[{layout->index(t1), layout->index(t2)}][0u];
}

z3::expr exact::get_tp(const layout_tile& t1, const layout_tile& t2) noexcept
{
    return tp_map[{layout->index(t1), layout->index(t2)}][0u];
}

z3::expr exact::get_tx(const layout_tile& t) noexcept
{
    return tx_map[layout->index(t)][0u];
}

z3::expr exact::get_vcl(const logic_vertex v) noexcept
{
    return vcl_map[v][0u];
}

z3::expr exact::get_tcl(const layout_tile& t) noexcept
{
    return tcl_map[layout->index(t)][0u];
}

z3::expr exact::get_tl(const layout_tile& t) noexcept
{
    return tl_map[layout->index(t)][0u];
}

z3::expr exact::mk_eq(const z3::expr_vector& v) const noexcept
{
    z3::expr_vector eq{v.ctx()};
    for (int i : iter::range(1u, v.size()))
        eq.push_back(v[i - 1] == v[i]);

    return z3::mk_and(eq);
}

void exact::tile_ite_counters(const logic_edge& e, z3::expr_vector& ve) noexcept
{
    z3::expr one  = ctx.real_val(1u);
    z3::expr zero = ctx.real_val(0u);

    z3::expr num_phases = ctx.real_val(static_cast<unsigned>(layout->num_clocks()));

    for (auto&& t : layout->ground_layer())
    {
        // an artificial latch variable counts as an extra 1 clock cycle (n clock phases)
        if (config.artificial_latches)
            ve.push_back(z3::ite(get_te(t, e), get_tl(t) * num_phases + one, zero));
        else
            ve.push_back(z3::ite(get_te(t, e), one, zero));
    }
}

void exact::restrict_tile_elements()
{
    for (auto&& t : layout->ground_layer())
    {
        if (config.crossings /* || config.multi_wires */)
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

void exact::restrict_vertices()
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

void exact::restrict_clocks()
{
    for (auto&& t : layout->ground_layer())
    {
        auto cl = get_tcl(t);

        z3::expr_vector ve{ctx};
        for (auto i : iter::range(layout->num_clocks()))
            ve.push_back(cl == ctx.real_val(static_cast<unsigned>(i)));

        solver.add(z3::mk_or(ve));
    }
}

void exact::restrict_latches()
{
    auto zero = ctx.real_val(0u);

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

void exact::define_adjacent_vertex_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& v : (network->vertices(config.io_ports)))
        {
            auto tv = get_tv(t, v);
            z3::expr_vector conj{ctx};
            for (auto&& ae : network->out_edges(v, config.io_ports))
            {
                z3::expr_vector disj{ctx};

                if (auto tgt = network->target(ae); layout->is_regularly_clocked())
                {
                    for (auto&& at : layout->outgoing_clocked_tiles(t))
                        disj.push_back((get_tv(at, tgt) or get_te(at, ae)) and get_tc(t, at));
                }
                else  // irregular clocking
                {
                    for (auto&& at : layout->surrounding_2d(t))
                    {
                        // clocks must differ by 1
                        auto mod = z3::mod(get_tcl(at) - get_tcl(t), layout->num_clocks()) == ctx.real_val(1u);
                        disj.push_back(((get_tv(at, tgt) or get_te(at, ae)) and mod) and get_tc(t, at));
                    }
                }

                if (!disj.empty())
                    conj.push_back(z3::mk_or(disj));
            }
            if (!conj.empty())
                solver.add(z3::implies(tv, z3::mk_and(conj)));
        }
    }
}

void exact::define_inv_adjacent_vertex_tiles()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& v : network->vertices(config.io_ports))
        {
            auto tv = get_tv(t, v);
            z3::expr_vector conj{ctx};
            for (auto&& iae : network->in_edges(v, config.io_ports))
            {
                z3::expr_vector disj{ctx};

                if (auto src = network->source(iae); layout->is_regularly_clocked())
                {
                    for (auto&& iat : layout->incoming_clocked_tiles(t))
                        disj.push_back((get_tv(iat, src) or get_te(iat, iae)) and get_tc(iat, t));
                }
                else  // irregular clocking
                {
                    for (auto&& iat : layout->surrounding_2d(t))
                    {
                        // clocks must differ by 1
                        auto mod = z3::mod(get_tcl(t) - get_tcl(iat), layout->num_clocks()) == ctx.real_val(1u);
                        disj.push_back(((get_tv(iat, src) or get_te(iat, iae)) and mod) and get_tc(iat, t));
                    }
                }

                if (!disj.empty())
                    conj.push_back(z3::mk_or(disj));
            }
            if (!conj.empty())
                solver.add(z3::implies(tv, z3::mk_and(conj)));
        }
    }
}

void exact::define_adjacent_edge_tiles()
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
                    auto mod = z3::mod(get_tcl(at) - get_tcl(t), layout->num_clocks()) == ctx.real_val(1u);
                    disj.push_back(((get_tv(at, te) or get_te(at, e)) and mod) and get_tc(t, at));
                }
            }

            if (!disj.empty())
                solver.add(z3::implies(get_te(t, e), z3::mk_or(disj)));
        }
    }
}

void exact::define_inv_adjacent_edge_tiles()
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
                    auto mod = z3::mod(get_tcl(t) - get_tcl(iat), layout->num_clocks()) == ctx.real_val(1u);
                    disj.push_back(((get_tv(iat, se) or get_te(iat, e)) and mod) and get_tc(iat, t));
                }
            }

            if (!disj.empty())
                solver.add(z3::implies(get_te(t, e), z3::mk_or(disj)));
        }
    }
}

void exact::establish_sub_paths()
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

void exact::establish_transitive_paths()
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

void exact::eliminate_cycles()
{
    for (auto&& t : layout->ground_layer())
        solver.add(not get_tp(t, t));
}

void exact::assign_pi_clockings()
{
    auto assign = [&](const logic_vertex _v) -> void
    {
        auto cl = get_vcl(_v);
        for (auto&& t : layout->ground_layer())
        {
            if (layout->is_regularly_clocked())
                solver.add(z3::implies(get_tv(t, _v), cl == ctx.real_val(static_cast<unsigned>(*layout->tile_clocking(t)))));
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

void exact::fanin_length()
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
            if (int offset = static_cast<int>(p.size() - max_length); offset)
                path_length.push_back(ctx.real_val(offset));

            for (auto& e : p)
            {
                // respect clock zone of PI if one is involved
                if (auto s = network->source(e); config.io_ports && network->is_pi(s))
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

void exact::prevent_insufficiencies()
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

void exact::define_number_of_connections()
{
    for (auto&& t1 : layout->ground_layer())
    {
        z3::expr_vector tcc{ctx};

        // collect all variables representing connections t -> t'
        z3::expr_vector acc{ctx};
        if (layout->is_regularly_clocked())
        {
            for (auto&& at : layout->outgoing_clocked_tiles(t1))
            {
                auto tc = get_tc(t1, at);
                acc.push_back(tc);
                tcc.push_back(tc);
            }
        }
        else  // irregular clocking
        {
            for (auto&& at : layout->surrounding_2d(t1))
            {
                auto tc = get_tc(t1, at);
                acc.push_back(tc);
                tcc.push_back(tc);
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
                tcc.push_back(tc);
            }
        }
        else  // irregular clocking
        {
            for (auto&& iat : layout->surrounding_2d(t1))
            {
                auto tc = get_tc(iat, t1);
                iacc.push_back(tc);
                tcc.push_back(tc);
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

        // if crossings are allowed, there must be exactly two connections in each direction for two assigned edges
        if (config.crossings)
        {
            if (!acc.empty())
                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                           z3::atleast(acc, 2u) and z3::atmost(acc, 2u)));
            if (!iacc.empty())
                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
                           z3::atleast(iacc, 2u) and z3::atmost(iacc, 2u)));
        }
//        // crossing are not allowed, but multi-wires are: there can be one or two connections for two assigned edges
//        else if (config.multi_wires)
//        {
//            if (!acc.empty())
//                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
//                                       z3::atleast(acc, 1u) and z3::atmost(acc, 2u)));
//            if (!iacc.empty())
//                solver.add(z3::implies(z3::atleast(wv, 2u) and z3::atmost(wv, 2u),
//                                       z3::atleast(iacc, 1u) and z3::atmost(iacc, 2u)));
//        }

        // if tile t is empty, there must not be any connection from or to tile t established
        if (!ow.empty() && !tcc.empty())
            solver.add(z3::atmost(ow, 0u) == z3::atmost(tcc, 0u));

        if (!config.topolinano && !config.twoddwave)  // linear schemes; no path constraints needed
        {
            z3::expr_vector ccp{ctx};

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
}

void exact::utilize_hierarchical_information()
{
    if (config.topolinano)
    {
        // restrict vertex placement according to their hierarchy level
        if (config.io_ports && config.border_io)
        {
            for (auto&& v : network->vertices(config.io_ports))
            {
                auto l  = hierarchy->get_level(v);
                auto il = hierarchy->get_inv_level(v);
                auto oe = network->out_edges(v, config.io_ports);
                auto ie = network->in_edges(v, config.io_ports);

                // cannot be placed with too little distance to left border
                for (auto column : iter::range(l))
                {
                    for (auto row : iter::range(layout->y()))
                    {
                        auto t = layout_tile{column, row, GROUND};
                        solver.add(not get_tv(t, v));

                        // same for the outgoing edges
                        for (auto&& e : oe)
                            solver.add(not get_te(t, e));
                    }
                }

                // cannot be placed with too little distance to right border
                for (auto column : iter::range(layout->x() - il, layout->x()))
                {
                    for (auto row : iter::range(layout->y()))
                    {
                        auto t = layout_tile{column, row, GROUND};
                        solver.add(not get_tv(t, v));

                        // same for the incoming edges
                        for (auto&& e : ie)
                            solver.add(not get_te(t, e));
                    }
                }
            }
        }
    }
}

void exact::enforce_border_io()
{
    auto assign_border = [this](const logic_vertex _v)
    {
        for (auto&& t : layout->ground_layer())
        {
            if (layout->is_border(t))
                continue;

            solver.add(not get_tv(t, _v));
        }
    };

    auto assign_left = [this](const logic_vertex _v)
    {
        for (auto&& t : layout->ground_layer())
        {
            if (t[X] == 0)
                continue;

            solver.add(not get_tv(t, _v));
        }
    };

    auto assign_right = [this](const logic_vertex _v)
    {
        for (auto&& t : layout->ground_layer())
        {
            if (t[X] == layout->x() - 1)
                continue;

            solver.add(not get_tv(t, _v));
        }
    };

    if (config.io_ports)
    {
        for (auto&& pi : network->get_pis())
            config.topolinano ? assign_left(pi) : assign_border(pi);
        for (auto&& po : network->get_pos())
            config.topolinano ? assign_right(po) : assign_border(po);
    }
    else
    {
        for (auto&& pi : network->get_pis())
        {
            for (auto&& v : network->adjacent_vertices(pi))
                config.topolinano ? assign_left(v) : assign_border(v);
        }
        for (auto&& po : network->get_pos())
        {
            for (auto&& v : network->inv_adjacent_vertices(po))
                config.topolinano ? assign_right(v) : assign_border(v);
        }
    }
}

void exact::limit_wire_length()
{
    for (auto&& e : network->edges(config.io_ports))
    {
        z3::expr_vector ve{ctx};
        for (auto&& t : layout->ground_layer())
            ve.push_back(get_te(t, e));

        solver.add(z3::atmost(ve, config.wire_limit));
    }
}

void exact::limit_crossings()
{
    z3::expr_vector crossings_counter{ctx};
    for (auto&& t : layout->ground_layer())
    {
        z3::expr_vector wv{ctx};
        for (auto&& e : network->edges(config.io_ports))
            wv.push_back(get_te(t, e));

        crossings_counter.push_back(z3::ite(z3::atleast(wv, 2u), ctx.real_val(1u), ctx.real_val(0u)));
    }

    solver.add(z3::sum(crossings_counter) <= ctx.real_val(config.crossings_limit));
}

void exact::minimize_crossings()
{
    z3::expr_vector crossings_counter{ctx};
    for (auto&& t : layout->ground_layer())
    {
        z3::expr_vector wv{ctx};
        for (auto&& e : network->edges(config.io_ports))
            wv.push_back(get_te(t, e));

        crossings_counter.push_back(z3::ite(z3::atleast(wv, 2u), ctx.real_val(1u), ctx.real_val(0u)));
    }

    solver.minimize(z3::sum(crossings_counter));
}

void exact::enforce_straight_inverters()
{
    for (auto&& t : layout->ground_layer())
    {
        for (auto&& inv : network->vertices())
        {
            // skip all operations except for NOT
            if (network->get_op(inv) != operation::NOT)
                continue;

            // I/Os are always straight, so they can be skipped as well
            if (network->pre_pi(inv) || network->post_po(inv))
                continue;

            auto n = layout->north(t), e = layout->east(t),
                 s = layout->south(t), w = layout->west(t);

            auto ne = layout->north_east(t), se = layout->south_east(t),
                 sw = layout->south_west(t), nw = layout->north_west(t);

            // vector to store possible direction combinations
            z3::expr_vector ve{ctx};

            if (layout->is_regularly_clocked())
            {
                if (layout->is_incoming_clocked(t, n) && layout->is_outgoing_clocked(t, s))
                {
                    // enforce north to south connection if inverter gets placed here
                    ve.push_back(get_tc(n, t) and get_tc(t, s));
                }
                else if (layout->is_incoming_clocked(t, s) && layout->is_outgoing_clocked(t, n))
                {
                    // enforce south to north connection if inverter gets placed here
                    ve.push_back(get_tc(s, t) and get_tc(t, n));
                }

                // if layout is vertically shifted
                if (config.vertical_offset)
                {
                    // if t is in an even column
                    if (layout->is_even_column(t))
                    {
                        if (ne && nw)
                        {
                            if (layout->is_incoming_clocked(t, *nw) && layout->is_outgoing_clocked(t, *ne))
                            {
                                // enforce north-west to north-east connection if inverter gets placed here
                                ve.push_back(get_tc(*nw, t) and get_tc(t, *ne));
                            }
                            else if (layout->is_incoming_clocked(t, *ne) && layout->is_outgoing_clocked(t, *nw))
                            {
                                // enforce north-east to north-west connection if inverter gets placed here
                                ve.push_back(get_tc(*ne, t) and get_tc(t, *nw));
                            }
                        }
                    }
                    else  // odd column
                    {
                        if (se && sw)
                        {
                            if (layout->is_incoming_clocked(t, *sw) && layout->is_outgoing_clocked(t, *se))
                            {
                                // enforce south-west to south-east connection if inverter gets placed here
                                ve.push_back(get_tc(*sw, t) and get_tc(t, *se));
                            }
                            else if (layout->is_incoming_clocked(t, *se) && layout->is_outgoing_clocked(t, *sw))
                            {
                                // enforce south-east to south-west connection if inverter gets placed here
                                ve.push_back(get_tc(*se, t) and get_tc(t, *sw));
                            }
                        }
                    }
                }
                // west-east and east-west is possible regardless of shift
                if (layout->is_incoming_clocked(t, e) && layout->is_outgoing_clocked(t, w))
                {
                    // enforce east to west connection if inverter gets placed here
                    ve.push_back(get_tc(e, t) and get_tc(t, w));
                }
                else if (layout->is_incoming_clocked(t, w) && layout->is_outgoing_clocked(t, e))
                {
                    // enforce west to east connection if inverter gets placed here
                    ve.push_back(get_tc(w, t) and get_tc(t, e));
                }
            }
            else  // irregular clocking
            {
                // t is neither at northern nor at southern border
                if (t != n && t != s)
                {
                    ve.push_back(get_tc(n, t) and get_tc(t, s));
                    ve.push_back(get_tc(s, t) and get_tc(t, n));
                }
                // if layout is vertically shifted
                if (config.vertical_offset)
                {
                    // if t is in an even column
                    if (layout->is_even_column(t))
                    {
                        if (ne && nw)
                        {
                            ve.push_back(get_tc(*ne, t) and get_tc(t, *nw));
                            ve.push_back(get_tc(*nw, t) and get_tc(t, *ne));
                        }
                    }
                    else  // odd column
                    {
                        if (se && sw)
                        {
                            ve.push_back(get_tc(*se, t) and get_tc(t, *sw));
                            ve.push_back(get_tc(*sw, t) and get_tc(t, *se));
                        }
                    }
                }
                // t is neither at eastern nor at western border (possible regardless of shift)
                if (t != e && t != w)
                {
                    ve.push_back(get_tc(e, t) and get_tc(t, w));
                    ve.push_back(get_tc(w, t) and get_tc(t, e));
                }
            }

            if (!ve.empty())
            {
                // inverter can be placed here; enforce any of the direction combinations found possible above
                solver.add(z3::implies(get_tv(t, inv), z3::mk_or(ve)));
            }
            else
            {
                // inverter cannot be placed here, add constraint to avoid this case and speed up solving
                solver.add(not get_tv(t, inv));
            }
        }
    }
}

void exact::topology_specific_constraints()
{
    if (config.topolinano && config.vertical_offset)
    {
        // fan-outs (couplers) cannot be followed by fan-outs and can only have inputs from their north-western tiles
        for (auto&& fo : network->vertices())
        {
            // only argue about 1-to-2-fan-outs
            if (network->get_op(fo) != operation::F1O2)
                continue;

            // prohibit succeeding fan-outs
            for (auto&& afo : network->adjacent_vertices(fo))
            {
                if (network->get_op(afo) != operation::F1O2)
                    continue;

                for (auto&& t : layout->ground_layer())
                {
                    for (auto&& at : layout->outgoing_clocked_tiles(t))
                    {
                        solver.add(z3::implies(get_tv(t, fo), not get_tv(at, afo)));
                    }
                }
            }

            // do not argue about fan-outs without predecessors in the following
            if (network->in_degree(fo, config.io_ports) == 0)
                continue;

            // enforce north-western input
            for (auto&& t : layout->ground_layer())
            {
                // if fo gets placed here, its predecessor must be on the north-western tile
                if (auto nw = layout->north_west(t); nw)
                {
                    solver.add(z3::implies(get_tv(t, fo), get_tc(*nw, t)));

                    // additionally, no crossing can preceed a fan-out
                    z3::expr_vector wv{ctx};
                    for (auto&& e : network->edges(config.io_ports))
                        wv.push_back(get_te(*nw, e));

                    solver.add(z3::implies(get_tv(t, fo), z3::atmost(wv, 1u)));
                }
                // if tile t doesn't have a north-western adjacent tile, fo cannot be placed here
                else
                {
                    solver.add(not get_tv(t, fo));
                }
            }
        }

        for (auto&& v1 : network->vertices())
        {
            // AND/OR/MAJ gates cannot be followed directly by an AND/OR/MAJ gate or a fan-out (coupler)
            // additionally, if straight inverters are enforced, AND/OR/MAJ cannot be followed by NOT either
            if (auto op1 = network->get_op(v1); op1 != operation::AND && op1 != operation::OR && op1 != operation::MAJ)
                continue;

            for (auto&& v2 : network->adjacent_vertices(v1))
            {
                // only argue about AND/OR/MAJ/fan-outs and additionally about NOT if straight inverters are enforced
                if (auto op2 = network->get_op(v2); op2 != operation::AND && op2 != operation::OR &&
                                                    op2 != operation::MAJ && op2 != operation::F1O2 &&
                                                    !(config.straight_inverters && op2 == operation::NOT))
                {
                    continue;
                }

                for (auto&& t : layout->ground_layer())
                {
                    for (auto&& at : layout->outgoing_clocked_tiles(t))
                    {
                        solver.add(z3::implies(get_tv(t, v1), not get_tv(at, v2)));
                    }
                }
            }

            // AND/OR/MAJ gates cannot be followed by a south-eastern connection
            // and cannot be followed by a crossing directly
            for (auto&& t : layout->ground_layer())
            {
                if (auto ne = layout->north_east(t); !ne)
                {
                    // no north-eastern tile, do not place v1 here
                    solver.add(not get_tv(t, v1));
                }
                else if (ne)
                {
                    // north-eastern tile exists, do not create a crossing here
                    z3::expr_vector wv{ctx};
                    for (auto&& e : network->edges(config.io_ports))
                        wv.push_back(get_te(*ne, e));

                    solver.add(z3::implies(get_tv(t, v1), z3::atmost(wv, 1u)));
                }
                if (auto se = layout->south_east(t); se)
                {
                    // south-eastern tile exists, do not route a connection here
                    solver.add(z3::implies(get_tv(t, v1), not get_tc(t, *se)));
                }
            }
        }
    }
}

void exact::generate_smt_instance()
{
    // layout constraints
    restrict_tile_elements();
    restrict_vertices();

    // open clocking
    if (!layout->is_regularly_clocked())
        restrict_clocks();

    if (config.artificial_latches && !config.desynchronize)
        restrict_latches();

    // adjacency constraints
    define_adjacent_vertex_tiles();
    define_inv_adjacent_vertex_tiles();
    define_adjacent_edge_tiles();
    define_inv_adjacent_edge_tiles();

    // path/cycle constraints
    if (!config.topolinano && !config.twoddwave)  // linear schemes; no cycles by definition
    {
        establish_sub_paths();
        establish_transitive_paths();
        eliminate_cycles();
    }

    // crossings constraints
    if (config.crossings)
    {
        if (config.crossings_limit)
            limit_crossings();

        if (config.minimize_crossings)
            minimize_crossings();
    }

    // fan-in constraints
    if (!config.desynchronize && !config.topolinano)
    {
        assign_pi_clockings();
        fanin_length();
    }

    // straight inverter constraints
    if (config.straight_inverters)
        enforce_straight_inverters();

    // backdoor constraints
    prevent_insufficiencies();
    define_number_of_connections();
    utilize_hierarchical_information();

    // physical restrictions and limitations constraints
    if (config.border_io)
        enforce_border_io();

    if (config.wire_limit)
        limit_wire_length();

    topology_specific_constraints();

//    std::cout << solver.assertions() << std::endl;

}

void exact::assign_layout()
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
                // if this is a balance vertex
                if (auto b = hierarchy->get_balance_edge(v); b)
                {
                    layout->assign_logic_edge(t, *b);
                }
                else  // regular gate
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
                }

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

    // adjust wires for ToPoliNano clocking as multi wires are supported
    if (config.topolinano)
    {
        for (auto&& t : layout->crossing_layers())
        {
            for (auto& e : layout->get_logic_edges(t))
            {
                if (auto inp = layout->get_wire_inp_dirs(t, e), out = layout->get_wire_out_dirs(t, e);
                        inp != layout::opposite(out))
                {
                    // move edge to ground layer
                    auto g = layout_tile{t[X], t[Y], GROUND};
                    layout->dissociate_logic_edge(t, e);
                    layout->assign_logic_edge(g, e);
                    layout->assign_wire_inp_dir(g, e, inp);
                    layout->assign_wire_out_dir(g, e, out);
                }
            }
        }
    }

    // assign artificial latches if there were any in use
    if (config.artificial_latches)
    {
        for (auto&& t : layout->ground_layer())
            layout->assign_latch(t, static_cast<fcn_gate_layout::latch_delay>
                                    (mdl.eval(get_tl(t), true).get_numeral_int()) * layout->num_clocks());
    }
}
