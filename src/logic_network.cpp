//
// Created by marcel on 18.05.18.
//

#include "logic_network.h"

logic_network::logic_network(std::string&& name) noexcept
        :
        bidirectional_graph(),
        name(name)
{
    one  = std::make_unique<vertex>(add_vertex(operation::ONE));
    zero = std::make_unique<vertex>(add_vertex(operation::ZERO));
}

logic_network::logic_network(const logic_network& ln) noexcept
        :
        bidirectional_graph(ln),
        name(ln.name)
{}

logic_network::logic_network(logic_network&& ln) noexcept
        :
        bidirectional_graph(ln),
        name(std::move(ln.name))
{}

logic_network::num_vertices_t logic_network::vertex_count(const bool ios, const bool consts) const noexcept
{
    auto count = get_vertex_count();
    if (!ios)
        count -= (pi_count() + po_count());
    if (!consts)
        count -= 2;

    return count;
}

logic_network::num_edges_t logic_network::edge_count(const bool ios, const bool consts) const noexcept
{
    auto count = get_edge_count();
    if (!ios)
        count -= (pi_count() + po_count());
    if (!consts)
        count -= const_count();

    return count;
}

logic_network::degree_t logic_network::out_degree(const vertex v, const bool ios, const bool consts) const noexcept
{
    auto out = out_edges(v, ios, consts);
    return static_cast<degree_t>(std::distance(out.begin(), out.end()));
}

logic_network::degree_t logic_network::in_degree(const vertex v, const bool ios, const bool consts) const noexcept
{
    auto in = in_edges(v, ios, consts);
    return static_cast<degree_t>(std::distance(in.begin(), in.end()));
}

logic_network::vertex logic_network::create_pi(const std::string& name) noexcept
{
    auto v = add_vertex(operation::PI);
    pi_set.emplace(v);
    io_port_map.insert(port_map::value_type(v, name));

    return v;
}

void logic_network::create_po(const vertex a, const std::string& name) noexcept
{
    auto v = add_vertex(operation::PO);
    add_edge(a, v);
    po_set.emplace(v);
    io_port_map.insert(port_map::value_type(v, name));
}

logic_network::vertex logic_network::get_constant(bool value) const noexcept
{
    return value ? *one : *zero;
}

logic_network::vertex logic_network::create_buf(const vertex a) noexcept
{
    auto v = add_vertex(operation::BUF);
    add_edge(a, v);

    return v;
}

logic_network::vertex logic_network::create_buf() noexcept
{
    return add_vertex(operation::BUF);
}

logic_network::vertex logic_network::create_not(const vertex a) noexcept
{
    auto v = add_vertex(operation::NOT);
    add_edge(a, v);

    return v;
}

logic_network::vertex logic_network::create_and(const vertex a, const vertex b) noexcept
{
    auto v = add_vertex(operation::AND);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_or(const vertex a, const vertex b) noexcept
{
    auto v = add_vertex(operation::OR);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_xor(const vertex a, const vertex b) noexcept
{
    auto v = add_vertex(operation::XOR);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_maj(const vertex a, const vertex b, const vertex c) noexcept
{
    auto v = add_vertex(operation::MAJ);
    add_edge(a, v);
    add_edge(b, v);
    add_edge(c, v);

    return v;
}

logic_network::vertex logic_network::create_balance_vertex(const edge& e) noexcept
{
    auto v = add_vertex(operation::W);
    auto s = source(e), t = target(e);

    remove_edge(e);
    add_edge(s, v);
    add_edge(v, t);

    return v;
}

void logic_network::assign_op(const vertex v, const operation o) noexcept
{
    properties(v) = o;
}

operation logic_network::get_op(const vertex v) const noexcept
{
    return properties(v);
}

bool logic_network::is_pi(const vertex v) const noexcept
{
    return pi_set.count(v) > 0u;
}

bool logic_network::pre_pi(const vertex v) const noexcept
{
    auto pre = inv_adjacent_vertices(v, true);
    return std::any_of(pre.begin(), pre.end(), [this](const vertex _v){return is_pi(_v);});
}

bool logic_network::is_po(const vertex v) const noexcept
{
    return po_set.count(v) > 0u;
}

bool logic_network::post_po(const vertex v) const noexcept
{
    auto post = adjacent_vertices(v, true);
    return std::any_of(post.begin(), post.end(), [this](const vertex _v){return is_po(_v);});
}

bool logic_network::is_io(const vertex v) const noexcept
{
    return get_op(v) == operation::PI || get_op(v) == operation::PO;
}

bool logic_network::is_const(const vertex v) const noexcept
{
    return get_op(v) == operation::ONE || get_op(v) == operation::ZERO;
}

std::string logic_network::get_port_name(const vertex v) const noexcept
{
    try
    {
        return io_port_map.left.at(v);
    }
    catch (const std::out_of_range&)
    {
        return "";
    }
}

std::string logic_network::get_name() const noexcept
{
    return name;
}

std::vector<logic_network::edge_path> logic_network::get_all_paths(const vertex v, const bool ios, const bool consts) noexcept
{
    if (get_in_degree(v) == 0u)
        return std::vector<edge_path>{edge_path{}};

    std::vector<edge_path> paths{};
    for (auto&& e : in_edges(v, ios, consts))
    {
        auto ps = get_all_paths(source(e), ios, consts);
        for (auto& p : ps)
            p.push_back(e);

        paths.insert(paths.end(), ps.cbegin(), ps.cend());
    }

    return paths;
}

void logic_network::substitute() noexcept
{
    auto reduce_gate_inputs = [&]()
    {
        // TODO when lorina supports multi-input gates
    };

    auto decompose = [&]()
    {
        auto decompose_xor = [&](const vertex v_XOR)
        {
            auto v_FO_1  = add_vertex(operation::F1O2);
            auto v_FO_2  = add_vertex(operation::F1O2);
            auto v_AND_1 = add_vertex(operation::AND);
            auto v_AND_2 = add_vertex(operation::AND);
            auto v_NOT   = add_vertex(operation::NOT);
            auto v_OR    = add_vertex(operation::OR);

            add_edge(v_FO_1, v_AND_1);
            add_edge(v_FO_1, v_OR);
            add_edge(v_FO_2, v_AND_1);
            add_edge(v_FO_2, v_OR);
            add_edge(v_AND_1, v_NOT);
            add_edge(v_NOT, v_AND_2);
            add_edge(v_OR, v_AND_2);

            auto iaop = get_inv_adjacent_vertices(v_XOR);
            auto iao = iaop.begin();
            add_edge(*iao, v_FO_1);
            ++iao;
            add_edge(*iao, v_FO_2);

            for (auto&& ao : get_adjacent_vertices(v_XOR))
                add_edge(v_AND_2, ao);

            remove_vertex(v_XOR);
        };

        auto lvs = vertices();
        auto is_composed_vertex = [&](const vertex _v){return get_op(_v) == operation::XOR /* || ... */;};

        auto comp_vertex = std::find_if(lvs.begin(), lvs.end(), is_composed_vertex);
        while (comp_vertex != lvs.end())
        {
            if (get_op(*comp_vertex) == operation::XOR)
                decompose_xor(*comp_vertex);
            // else if (get_op(*comp_vertex) == operation:: ...)

            comp_vertex = std::find_if(lvs.begin(), lvs.end(), is_composed_vertex);
        }
    };

    auto add_fan_outs = [&]()
    {
        for (auto&& v : get_vertices())
        {
            if (get_out_degree(v) > 1u && get_op(v) != operation::F1O2)
            {
                auto predecessor = v;
                std::vector<vertex> vv;
                std::vector<edge> ve;
                for (auto&& ae : out_edges(v, true, true))
                {
                    vv.push_back(target(ae));
                    ve.push_back(ae);
                }

                for (auto i : iter::range(vv.size()))
                {
                    if (i + 1 == vv.size())
                        add_edge(predecessor, vv[i]);
                    else
                    {
                        auto fan_out = add_vertex(operation::F1O2);

                        add_edge(predecessor, fan_out);
                        add_edge(fan_out, vv[i]);

                        predecessor = fan_out;
                    }
                }
                for (auto& e : ve)
                    remove_edge(e);
            }
        }
    };

    reduce_gate_inputs();
    decompose();
    add_fan_outs();
}

void logic_network::update_index_maps() noexcept
{
    update_edge_index_map();
}
