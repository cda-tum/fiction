//
// Created by marcel on 02.08.19.
//

#include "network_hierarchy.h"

network_hierarchy::network_hierarchy(logic_network_ptr ln, const bool store_original_edges) noexcept
        :
        network{std::move(ln)},
        store_edges{store_original_edges}
{
    levelize();
}

network_hierarchy::level network_hierarchy::height() const noexcept
{
    return max_level;
}

network_hierarchy::level network_hierarchy::inv_height() const noexcept
{
    return max_inv_level;
}

void network_hierarchy::set_level(const logic_network::vertex v, const level l) noexcept
{
    l_map[v] = l;

    if (l > max_level)
        max_level = l;
}

network_hierarchy::level network_hierarchy::get_level(const logic_network::vertex v) const noexcept
{
    if (auto it = l_map.find(v); it != l_map.cend())
    {
        return it->second;
    }
    else
    {
        return 0u;
    }
}

void network_hierarchy::set_inv_level(const logic_network::vertex v, const level l) noexcept
{
    il_map[v] = l;

    if (l > max_inv_level)
        max_inv_level = l;
}

network_hierarchy::level network_hierarchy::get_inv_level(const logic_network::vertex v) const noexcept
{
    if (auto it = il_map.find(v); it != il_map.cend())
    {
        return it->second;
    }
    else
    {
        return 0u;
    }
}

logic_network::vertex network_hierarchy::subdivide_edge(const logic_network::edge& e) noexcept
{
    auto src = network->source(e);
    auto b = network->create_balance_vertex(e);

    if (store_edges)
        store_balance_edge(b, e);

    auto l = get_level(src) + 1u;
    set_level(b, l);

    return b;
}

std::optional<logic_network::edge> network_hierarchy::get_balance_edge(const logic_network::vertex v) const noexcept
{
    if (auto it = b_map.find(v); it != b_map.cend())
    {
        return it->second;
    }
    else
    {
        return std::nullopt;
    }
}

void network_hierarchy::balance_paths() noexcept
{
    std::vector<logic_network::edge> unbalanced{};
    auto edges = network->edges(true);

    // all edges which need to be subdivided
    std::copy_if(edges.begin(), edges.end(), std::back_inserter(unbalanced),
            [this](const auto& e)
            {
                auto src = network->source(e), tgt = network->target(e);
                return get_level(src) + 1u < get_level(tgt);
            });

    // subdivide them
    std::for_each(unbalanced.cbegin(), unbalanced.cend(),
            [this](auto& e)
            {
                auto level_diff = static_cast<long int>(get_level(network->target(e))) -
                                  static_cast<long int>(get_level(network->source(e))) - 1;

                auto div = e;
                for ([[maybe_unused]] auto&& i : iter::range(level_diff))
                {
                    auto b = subdivide_edge(div);
                    div = *network->out_edges(b).begin();
                }
            });
}

void network_hierarchy::unify_output_ranks() noexcept
{
    for (auto&& po : network->get_pos())
        set_level(po, height());
}

void network_hierarchy::unify_inv_input_ranks() noexcept
{
    for (auto&& pi : network->get_pis())
        set_inv_level(pi, inv_height());
}

void network_hierarchy::store_balance_edge(const logic_network::vertex v, const logic_network::edge& e) noexcept
{
    if (auto it_s = b_map.find(network->source(e)); it_s != b_map.cend())
    {
        b_map[v] = it_s->second;
    }
    else if (auto it_t = b_map.find(network->target(e)); it_t != b_map.cend())
    {
        b_map[v] = it_t->second;
    }
    else
    {
        b_map[v] = e;
    }
}

void network_hierarchy::levelize() noexcept
{
    // store discovery of vertices
    std::vector<bool> discovered(network->vertex_count(true), false);
    // helper function to check if a given vertex has already been discovered
    const auto is_discovered = [&discovered](const logic_network::vertex _v) { return discovered[_v]; };

    // joint depth first search
    const std::function<void(const logic_network::vertex _v)>
            jdfs = [&](const logic_network::vertex _v)
    {
        auto iav = network->inv_adjacent_vertices(_v, true);
        const std::vector<logic_network::vertex> iavv(iav.begin(), iav.end());
        // if all predecessors are yet discovered
        if (std::all_of(iavv.begin(), iavv.end(), is_discovered))
        {
            discovered[_v] = true;

            // determine predecessor's maximum level
            auto pre_l = std::max_element(iavv.cbegin(), iavv.cend(),
                                          [this](const logic_network::vertex _v1, const logic_network::vertex _v2)
                                          {
                                              return get_level(_v1) < get_level(_v2);
                                          });

            // if there are no predecessors, level of current vertex is 0, else it is one higher than theirs
            set_level(_v, pre_l != iavv.cend() ? std::max(get_level(_v), get_level(*pre_l) + 1u) : 0u);

            for (auto&& av : network->adjacent_vertices(_v, true) | iter::filterfalse(is_discovered))
                jdfs(av);
        }
    };

    // call joint dfs for every PI node
    for (auto&& pi : network->get_pis())
        jdfs(pi);

    // reset discovered
    std::vector<bool> inv_discovered(network->vertex_count(true), false);
    // helper function to check if a given vertex has already been discovered
    const auto is_inv_discovered = [&inv_discovered](const logic_network::vertex _v) { return inv_discovered[_v]; };

    // inverse joint depth first search
    const std::function<void(const logic_network::vertex _v)>
            inv_jdfs = [&](const logic_network::vertex _v)
    {
        auto av = network->adjacent_vertices(_v, true);
        const std::vector<logic_network::vertex> avv(av.begin(), av.end());
        // if all predecessors are yet discovered
        if (std::all_of(avv.cbegin(), avv.cend(), is_inv_discovered))
        {
            inv_discovered[_v] = true;

            // determine successor's maximum level
            auto post_l = std::max_element(avv.cbegin(), avv.cend(),
                                          [this](const logic_network::vertex _v1, const logic_network::vertex _v2)
                                          {
                                              return get_inv_level(_v1) < get_inv_level(_v2);
                                          });

            // if there are no successors, level of current vertex is 0, else it is one higher than theirs
            set_inv_level(_v, post_l != avv.cend() ? std::max(get_inv_level(_v), get_inv_level(*post_l) + 1u) : 0u);

            for (auto&& iav : network->inv_adjacent_vertices(_v, true) | iter::filterfalse(is_inv_discovered))
                inv_jdfs(iav);
        }
    };

    // call inverse joint dfs for every PO node
    for (auto&& po : network->get_pos())
        inv_jdfs(po);
}
