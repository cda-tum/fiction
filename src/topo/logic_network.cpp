//
// Created by marcel on 18.05.18.
//

#include "logic_network.h"

logic_network::logic_network(std::string&& name) noexcept
        :
        bidirectional_graph(),
        name(std::move(name)),
        operation_counter(OP_COUNT, 0ul)
{
    zero = std::make_unique<vertex>(create_logic_vertex(operation::ZERO));
    one  = std::make_unique<vertex>(create_logic_vertex(operation::ONE));
}

logic_network::logic_network(const logic_network& ln) noexcept
        :
        bidirectional_graph(ln),
        name{ln.name},
        pi_set{ln.pi_set},
        po_set{ln.po_set},
        io_port_map{ln.io_port_map},
        operation_counter{ln.operation_counter}
{
    zero = std::make_unique<vertex>(vertex(0));
    one  = std::make_unique<vertex>(vertex(1));
}

logic_network::logic_network(logic_network&& ln) noexcept
        :
        bidirectional_graph(std::move(ln)),
        name{std::move(ln.name)},
        pi_set{std::move(ln.pi_set)},
        po_set{std::move(ln.po_set)},
        io_port_map{std::move(ln.io_port_map)},
        operation_counter{std::move(ln.operation_counter)},
        zero{std::move(ln.zero)},
        one{std::move(ln.one)}
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

logic_network::vertex logic_network::create_logic_vertex(const operation o) noexcept
{
    increment_op_counter(o);
    return add_vertex(o);
}

void logic_network::remove_logic_vertex(const vertex v) noexcept
{
    decrement_op_counter(get_op(v));
    remove_vertex(v);
}

logic_network::vertex logic_network::create_pi(const std::string& name) noexcept
{
    auto v = create_logic_vertex(operation::PI);
    pi_set.emplace(v);
    io_port_map.insert(port_map::value_type(v, name));

    return v;
}

void logic_network::create_po(const vertex a, const std::string& name) noexcept
{
    auto v = create_logic_vertex(operation::PO);
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
    auto v = create_buf();
    add_edge(a, v);

    return v;
}

logic_network::vertex logic_network::create_buf() noexcept
{
    return create_logic_vertex(operation::BUF);
}

logic_network::vertex logic_network::create_not(const vertex a) noexcept
{
    auto v = create_logic_vertex(operation::NOT);
    add_edge(a, v);

    return v;
}

logic_network::vertex logic_network::create_and(const vertex a, const vertex b) noexcept
{
    auto v = create_logic_vertex(operation::AND);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_or(const vertex a, const vertex b) noexcept
{
    auto v = create_logic_vertex(operation::OR);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_xor(const vertex a, const vertex b) noexcept
{
    auto v = create_logic_vertex(operation::XOR);
    add_edge(a, v);
    add_edge(b, v);

    return v;
}

logic_network::vertex logic_network::create_maj(const vertex a, const vertex b, const vertex c) noexcept
{
    auto v = create_logic_vertex(operation::MAJ);
    add_edge(a, v);
    add_edge(b, v);
    add_edge(c, v);

    return v;
}

logic_network::vertex logic_network::create_balance_vertex(const edge& e) noexcept
{
    auto v = create_logic_vertex(operation::W);
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

std::size_t logic_network::operation_count(const operation o) const noexcept
{
    return operation_counter[o];
}

bool logic_network::is_MIG() const noexcept
{
    for (auto&& oc : iter::range(operation_counter.size()))
    {
        switch (oc)
        {
            case operation::MAJ:
            case operation::NOT:
            case operation::F1O2:
            case operation::F1O3:
            case operation::W:
            case operation::PI:
            case operation::PO:
            case operation::ONE:
            case operation::ZERO:
            case operation::BUF:
                continue;
            default:
            {
                if (operation_counter[oc] != 0lu)
                    return false;
            }
        }
    }

    return true;
}

bool logic_network::is_AIG() const noexcept
{
    for (auto&& oc : iter::range(operation_counter.size()))
    {
        switch (oc)
        {
            case operation::AND:
            case operation::NOT:
            case operation::F1O2:
            case operation::F1O3:
            case operation::W:
            case operation::PI:
            case operation::PO:
            case operation::ONE:
            case operation::ZERO:
            case operation::BUF:
                continue;
            default:
            {
                if (operation_counter[oc] != 0lu)
                    return false;
            }
        }
    }

    return true;
}

bool logic_network::is_OIG() const noexcept
{
    for (auto&& oc : iter::range(operation_counter.size()))
    {
        switch (oc)
        {
            case operation::OR:
            case operation::NOT:
            case operation::F1O2:
            case operation::F1O3:
            case operation::W:
            case operation::PI:
            case operation::PO:
            case operation::ONE:
            case operation::ZERO:
            case operation::BUF:
                continue;
            default:
            {
                if (operation_counter[oc] != 0lu)
                    return false;
            }
        }
    }

    return true;
}

bool logic_network::is_AOIG() const noexcept
{
    for (auto&& oc : iter::range(operation_counter.size()))
    {
        switch (oc)
        {
            case operation::AND:
            case operation::OR:
            case operation::NOT:
            case operation::F1O2:
            case operation::F1O3:
            case operation::W:
            case operation::PI:
            case operation::PO:
            case operation::ONE:
            case operation::ZERO:
            case operation::BUF:
                continue;
            default:
            {
                if (operation_counter[oc] != 0lu)
                    return false;
            }
        }
    }

    return true;
}

bool logic_network::is_MAOIG() const noexcept
{
    for (auto&& oc : iter::range(operation_counter.size()))
    {
        switch (oc)
        {
            case operation::MAJ:
            case operation::AND:
            case operation::OR:
            case operation::NOT:
            case operation::F1O2:
            case operation::F1O3:
            case operation::W:
            case operation::PI:
            case operation::PO:
            case operation::ONE:
            case operation::ZERO:
            case operation::BUF:
                continue;
            default:
            {
                if (operation_counter[oc] != 0lu)
                    return false;
            }
        }
    }

    return true;
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
    auto reduce_gate_inputs = []()
    {
        // TODO when lorina supports multi-input gates
    };

    auto decompose = [this]()
    {
        auto decompose_xor = [this](const vertex v_XOR)
        {
            auto v_FO_1  = create_logic_vertex(operation::F1O2);
            auto v_FO_2  = create_logic_vertex(operation::F1O2);
            auto v_AND_1 = create_logic_vertex(operation::AND);
            auto v_AND_2 = create_logic_vertex(operation::AND);
            auto v_NOT   = create_logic_vertex(operation::NOT);
            auto v_OR    = create_logic_vertex(operation::OR);

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

            remove_logic_vertex(v_XOR);
        };

        auto lvs = vertices();
        auto is_composed_vertex = [this](const vertex _v){return get_op(_v) == operation::XOR /* || ... */;};

        auto comp_vertex = std::find_if(lvs.begin(), lvs.end(), is_composed_vertex);
        while (comp_vertex != lvs.end())
        {
            if (get_op(*comp_vertex) == operation::XOR)
                decompose_xor(*comp_vertex);
            // else if (get_op(*comp_vertex) == operation:: ...)

            comp_vertex = std::find_if(lvs.begin(), lvs.end(), is_composed_vertex);
        }
    };

    auto add_fan_outs = [this]()
    {
        for (auto&& v : get_vertices())
        {
            if (get_out_degree(v) > 1u && get_op(v) != operation::F1O2)
            {
                auto predecessor = v;
                std::vector<vertex> vv{};
                std::vector<edge> ve{};
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
                        auto fan_out = create_logic_vertex(operation::F1O2);

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

void logic_network::write_network(std::ostream& os) noexcept
{
    std::stringstream graph{};

    graph << "digraph G {\n";

    for (auto&& v : vertices(true, true))
        graph << boost::str(boost::format("%d [label=<<B>%d</B><br/>%s>];\n") % v % v % name_str(get_op(v)));

    for (auto&& e : edges(true, true))
        graph << boost::str(boost::format("%d->%d;\n") % source(e) % target(e));

    graph << "}\n";

    os << graph.str() << std::endl;
}

void logic_network::increment_op_counter(const operation o) noexcept
{
    ++operation_counter[o];
}

void logic_network::decrement_op_counter(const operation o) noexcept
{
    --operation_counter[o];
}
