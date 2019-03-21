//
// Created by marcel on 26.04.18.
//

#include "fcn_gate_layout.h"


fcn_gate_layout::fcn_gate_layout(fcn_dimension_xyz&& lengths, fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept
        :
        fcn_layout(std::move(lengths), std::move(clocking)),
        network(std::move(ln))
{}

fcn_gate_layout::fcn_gate_layout(fcn_dimension_xy&& lengths, fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept
        :
        fcn_layout(fcn_dimension_xyz{lengths[X], lengths[Y], 2}, std::move(clocking)),
        network(std::move(ln))
{}

fcn_gate_layout::fcn_gate_layout(fcn_dimension_xy&& lengths, logic_network_ptr ln) noexcept
        :
        fcn_layout(fcn_dimension_xyz{lengths[X], lengths[Y], 2}, std::move(open_4_clocking)),
        network(std::move(ln))
{}

fcn_gate_layout::fcn_gate_layout(fcn_clocking_scheme&& clocking, logic_network_ptr ln) noexcept
        :
        fcn_layout(fcn_dimension_xyz{2, 2, 2}, std::move(clocking)),
        network(std::move(ln))
{}

fcn_gate_layout::fcn_gate_layout(logic_network_ptr ln) noexcept
        :
        fcn_layout(fcn_dimension_xyz{2, 2, 2}, std::move(open_4_clocking)),
        network(std::move(ln))
{}

bool fcn_gate_layout::is_border_tile(const tile& t) const noexcept
{
    auto surrounding = surrounding_2d(t);
    return std::distance(surrounding.begin(), surrounding.end()) < 4;
}

layout::directions fcn_gate_layout::closest_border(const tile& t) const noexcept
{
    // tile is on the left side of the layout
    if (t[X] <= x() / 2)
    {
        // tile is on the upper side of the layout
        if (t[Y] <= y() / 2)
        {
            return t[X] <= t[Y] ? layout::DIR_W : layout::DIR_N;
        }
        // tile is on the lower side of the layout
        else
        {
            return t[X] <= y() - t[Y] ? layout::DIR_W : layout::DIR_S;
        }
    }
    // tile is on the right side of the layout
    else
    {
        // tile is on the upper side of the layout
        if (t[Y] <= y() / 2)
        {
            return x() - t[X] <= t[Y] ? layout::DIR_E : layout::DIR_N;
        }
            // tile is on the lower side of the layout
        else
        {
            return x() - t[X] <= y() - t[Y] ? layout::DIR_E : layout::DIR_S;
        }
    }
}

boost::optional<fcn_clock::zone> fcn_gate_layout::tile_clocking(const tile& t) const noexcept
{
    if (clocking.regular)
    {
        auto x = t[X], y = t[Y];
        return clocking.scheme[y % clocking.num_clocks][x % clocking.num_clocks];
    }
    else  // irregular clocking accesses clocking map
    {
        try
        {
            return c_map.at(get_ground(t));
        }
        catch (const std::out_of_range&)
        {
            return boost::none;
        }
    }
}

boost::optional<fcn_clock::zone> fcn_gate_layout::tile_clocking(const tile_index t) const noexcept
{
    return tile_clocking(get_vertex(t));
}

void fcn_gate_layout::assign_logic_vertex(const tile& t, const logic_network::vertex v, const bool pi, const bool po) noexcept
{
    dissociate_logic_edges(t);
    dissociate_logic_vertex(t);
    v_map.insert(vertex_map::value_type(t, v));

    // keep track of I/O sets
    if (pi)
        pi_set.emplace(t);
    if (po)
        po_set.emplace(t);
}

void fcn_gate_layout::dissociate_logic_vertex(const tile& t) noexcept
{
    v_map.left.erase(t);

    // keep track of I/O sets
    pi_set.erase(t);
    po_set.erase(t);

    // remove directions associated with t
    inp_dir_map.erase(t);
    out_dir_map.erase(t);
}

boost::optional<logic_network::vertex> fcn_gate_layout::get_logic_vertex(const tile& t) const noexcept
{
    try
    {
        return v_map.left.at(t);
    }
    catch (const std::out_of_range&)
    {
        return boost::none;
    }
}

bool fcn_gate_layout::is_gate_tile(const tile& t) const noexcept
{
    try
    {
        v_map.left.at(t);
        return true;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

bool fcn_gate_layout::has_logic_vertex(const tile& t, const logic_network::vertex v) const noexcept
{
    try
    {
        return v_map.left.at(t) == v;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

boost::optional<fcn_gate_layout::tile> fcn_gate_layout::get_logic_tile(const logic_network::vertex v) const noexcept
{
    try
    {
        return v_map.right.at(v);
    }
    catch (const std::out_of_range&)
    {
        return boost::none;
    }
}

void fcn_gate_layout::assign_logic_edge(const tile& t, const logic_network::edge& e) noexcept
{
    dissociate_logic_vertex(t);
    e_map[t].emplace(e);
}

void fcn_gate_layout::dissociate_logic_edge(const tile& t, const logic_network::edge& e) noexcept
{
    try
    {
        e_map.at(t).erase(e);
        // if this was the last edge assigned, save memory
        if (e_map.at(t).empty())
            e_map.erase(t);

        // remove directions
        inp_dir_map.at(t) &= ~get_wire_inp_dirs(t, e);
        out_dir_map.at(t) &= ~get_wire_out_dirs(t, e);

        edge_inp_dir_map.erase(std::make_pair(t, e));
        edge_out_dir_map.erase(std::make_pair(t, e));
    }
    catch (const std::out_of_range&)
    {
        // if tile t does not have any edges assigned, do nothing
    }
}

void fcn_gate_layout::dissociate_logic_edges(const tile& t) noexcept
{
    inp_dir_map.erase(t);
    out_dir_map.erase(t);

    for (auto& e : get_logic_edges(t))
    {
        edge_inp_dir_map.erase(std::make_pair(t, e));
        edge_out_dir_map.erase(std::make_pair(t, e));
    }

    e_map.erase(t);
}

fcn_gate_layout::edge_set fcn_gate_layout::get_logic_edges(const tile& t) const noexcept
{
    try
    {
        return e_map.at(t);
    }
    catch (const std::out_of_range&)
    {
        return edge_set{};
    }
}

bool fcn_gate_layout::is_wire_tile(const tile& t) const noexcept
{
    try
    {
        return !e_map.at(t).empty();
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

bool fcn_gate_layout::has_logic_edge(const tile& t, const logic_network::edge& e) const noexcept
{
    try
    {
        return e_map.at(t).count(e) > 0u;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

void fcn_gate_layout::clear_tile(const tile& t) noexcept
{
    inp_dir_map.erase(t);
    out_dir_map.erase(t);

    for (auto& e : get_logic_edges(t))
    {
        edge_inp_dir_map.erase(std::make_pair(t, e));
        edge_out_dir_map.erase(std::make_pair(t, e));
    }

    dissociate_logic_vertex(t);
    dissociate_logic_edges(t);
    assign_latch(t, 0);
}

bool fcn_gate_layout::is_free_tile(const tile& t) const noexcept
{
    return !get_logic_vertex(t) && !is_wire_tile(t);
}

std::vector<fcn_gate_layout::tile> fcn_gate_layout::outgoing_information_flow_tiles(const tile& t) const noexcept
{
    std::vector<tile> oift{}; // outgoing information flow tiles

    auto v = get_logic_vertex(t);
    auto edges = get_logic_edges(t);

    // for all outgoing clocked tiles
    for (auto&& _t : outgoing_clocked_tiles(t))
    {
        // if t got assigned a vertex
        if (v)
        {
            // for all adjacent logic vertices
            for (auto&& av : network->adjacent_vertices(*v, true))
            {
                // check the tile plus above and below for matches
                if (has_logic_vertex(_t, av))
                    oift.push_back(_t);
                if (has_logic_vertex(above(_t), av))
                    oift.push_back(above(_t));
                if (has_logic_vertex(below(_t), av))
                    oift.push_back(below(_t));
            }
            // for all adjacent logic edges
            for (auto&& ae : network->out_edges(*v, true))
            {
                // check the tile plus above and below for matches
                if (has_logic_edge(_t, ae))
                    oift.push_back(_t);
                if (has_logic_edge(above(_t), ae))
                    oift.push_back(above(_t));
                if (has_logic_edge(below(_t), ae))
                    oift.push_back(below(_t));
            }
        }
        // if t got assigned edges instead
        else
        {
            for (auto& e : edges)
            {
                // check if there is edge's target
                const auto tgt = network->target(e);
                if (has_logic_vertex(_t, tgt) || has_logic_edge(_t, e))
                    oift.push_back(_t);
                if (has_logic_vertex(above(_t), tgt) || has_logic_edge(above(_t), e))
                    oift.push_back(above(_t));
                if (has_logic_vertex(below(_t), tgt) || has_logic_edge(below(_t), e))
                    oift.push_back(below(_t));
            }
        }
    }

    // remove duplicates. vector is sorted due to order of pushing
    oift.erase(std::unique(oift.begin(), oift.end()), oift.end());

    return oift;
}

std::vector<fcn_gate_layout::tile> fcn_gate_layout::incoming_information_flow_tiles(const tile& t) const noexcept
{
    std::vector<tile> iift{}; // incoming information flow tiles

    auto v = get_logic_vertex(t);
    auto edges = get_logic_edges(t);

    // for all incoming clocked tiles
    for (auto&& _t : incoming_clocked_tiles(t))
    {
        // if t got assigned a vertex
        if (v)
        {
            // for all inversely adjacent logic vertices
            for (auto&& iav : network->inv_adjacent_vertices(*v, true))
            {
                // check the tile plus above and below for matches
                if (has_logic_vertex(_t, iav))
                    iift.push_back(_t);
                if (has_logic_vertex(above(_t), iav))
                    iift.push_back(above(_t));
                if (has_logic_vertex(below(_t), iav))
                    iift.push_back(below(_t));
            }
            // for all inversely adjacent logic edges
            for (auto&& iae : network->in_edges(*v, true))
            {
                // check the tile plus above and below for matches
                if (has_logic_edge(_t, iae))
                    iift.push_back(_t);
                if (has_logic_edge(above(_t), iae))
                    iift.push_back(above(_t));
                if (has_logic_edge(below(_t), iae))
                    iift.push_back(below(_t));
            }
        }
        // if t got assigned edges instead
        else
        {
            for (auto& e : edges)
            {
                // check if there is edge's source
                const auto src = network->source(e);
                if (has_logic_vertex(_t, src) || has_logic_edge(_t, e))
                    iift.push_back(_t);
                if (has_logic_vertex(above(_t), src) || has_logic_edge(above(_t), e))
                    iift.push_back(above(_t));
                if (has_logic_vertex(below(_t), src) || has_logic_edge(below(_t), e))
                    iift.push_back(below(_t));
            }
        }
    }

    // remove duplicates. vector is sorted due to order of pushing
    iift.erase(std::unique(iift.begin(), iift.end()), iift.end());

    return iift;
}

operation fcn_gate_layout::get_op(const tile& t) const noexcept
{
    try
    {
        return network->get_op(v_map.left.at(t));
    }
    catch (const std::out_of_range&)
    {
        try
        {
            return !e_map.at(t).empty() ? operation::W : operation::NONE;
        }
        catch (const std::out_of_range&)
        {
            return operation::NONE;
        }
    }
}

fcn_gate_layout::path_info fcn_gate_layout::signal_delay(const tile& t, delay_cache& dc) const noexcept
{
    if (is_free_tile(t))
        return {};

    auto iift = incoming_information_flow_tiles(t);
    if (iift.empty())
        return {1, *tile_clocking(t), 0};
    else if (dc.count(t)) // cache hit
        return dc.at(t);
    else // cache miss
    {
        // fetch information about all incoming paths
        std::vector<path_info> infos{};
        for (const auto& cur_t : iift)
            infos.push_back(signal_delay(cur_t, dc));

        path_info dominant_path{};
        if (infos.size() == 1) // size cannot be 0
            dominant_path = infos.front();
        else  // fetch highest delay and difference
        {
            // sort by path length
            std::sort(infos.begin(), infos.end(), [](const auto& i1, const auto& i2){return i1.length < i2.length;});

            dominant_path.length = infos.back().length;
            dominant_path.delay = infos.back().delay;
            dominant_path.diff = infos.back().delay - infos.front().delay;
        }

        // incorporate self
        ++dominant_path.length;
        ++dominant_path.delay;

        // cache value for gates only
        if (is_gate_tile(t))
            dc[t] = dominant_path;

        return dominant_path;
    }
}

std::pair<std::size_t, std::size_t> fcn_gate_layout::critical_path_length_and_throughput() const noexcept
{
    delay_cache dc{};
    std::size_t critical_path = 0;
    for (auto&& po : get_pos())
        critical_path = std::max(signal_delay(po, dc).length, critical_path);

    std::size_t throughput = 0;
    if (!dc.empty())
        throughput = std::max_element(dc.begin(), dc.end(),
                [](const auto& i1, const auto& i2){return i1.second.diff < i2.second.diff;})->second.diff;


    // give throughput in cycles, not in phases
    throughput /= num_clocks();

    // if all paths are synchronized
    if (throughput == 0)
        throughput = 1;

    return std::make_pair(critical_path, throughput);
}

void fcn_gate_layout::assign_tile_inp_dir(const tile& t, layout::directions d) noexcept
{
    // do not to anything if t is a free tile
    if (is_free_tile(t))
        return;

    if (d == layout::DIR_NONE)
        inp_dir_map.erase(t);
    else
        inp_dir_map[t] |= d;
}

void fcn_gate_layout::assign_wire_inp_dir(const tile& t, const logic_network::edge& e, layout::directions d) noexcept
{
    // do not do anything if e is not a wire on t
    if (!has_logic_edge(t, e))
        return;

    if (d == layout::DIR_NONE)
    {
        inp_dir_map.erase(t);
        edge_inp_dir_map.erase(std::make_pair(t, e));
    }
    else
    {
        inp_dir_map[t] |= d;
        edge_inp_dir_map[std::make_pair(t, e)] |= d;
    }
}

bool fcn_gate_layout::is_tile_inp_dir(const tile& t, const layout::directions& d) const noexcept
{
    try
    {
        return (inp_dir_map.at(t) & d) == d;
    }
    catch (const std::out_of_range&)
    {
        return d == layout::DIR_NONE;
    }
}

bool fcn_gate_layout::is_wire_inp_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept
{
    try
    {
        return (edge_inp_dir_map.at(std::make_pair(t, e)) & d) == d;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

layout::directions fcn_gate_layout::get_tile_inp_dirs(const tile& t) const noexcept
{
    try
    {
        return (inp_dir_map.at(t));
    }
    catch (const std::out_of_range&)
    {
        return layout::DIR_NONE;
    }
}

layout::directions fcn_gate_layout::get_wire_inp_dirs(const tile& t, const logic_network::edge& e) const noexcept
{
    try
    {
        return (edge_inp_dir_map.at(std::make_pair(t, e)));
    }
    catch (const std::out_of_range&)
    {
        return layout::DIR_NONE;
    }
}

void fcn_gate_layout::assign_tile_out_dir(const tile& t, layout::directions d) noexcept
{
    // do not to anything if t is a free tile
    if (is_free_tile(t))
        return;

    if (d == layout::DIR_NONE)
        out_dir_map.erase(t);
    else
        out_dir_map[t] |= d;
}

void fcn_gate_layout::assign_wire_out_dir(const tile& t, const logic_network::edge& e, layout::directions d) noexcept
{
    // do not do anything if e is not a wire on t
    if (!has_logic_edge(t, e))
        return;

    if (d == layout::DIR_NONE)
    {
        out_dir_map.erase(t);
        edge_out_dir_map.erase(std::make_pair(t, e));
    }
    else
    {
        out_dir_map[t] |= d;
        edge_out_dir_map[std::make_pair(t, e)] |= d;
    }
}

bool fcn_gate_layout::is_tile_out_dir(const tile& t, const layout::directions& d) const noexcept
{
    try
    {
        return (out_dir_map.at(t) & d) == d;
    }
    catch (const std::out_of_range&)
    {
        return d == layout::DIR_NONE;
    }
}

bool fcn_gate_layout::is_wire_out_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept
{
    try
    {
        return (edge_out_dir_map.at(std::make_pair(t, e)) & d) == d;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }

}

layout::directions fcn_gate_layout::get_tile_out_dirs(const tile& t) const noexcept
{
    try
    {
        return (out_dir_map.at(t));
    }
    catch (const std::out_of_range&)
    {
        return layout::DIR_NONE;
    }
}

layout::directions fcn_gate_layout::get_wire_out_dirs(const tile& t, const logic_network::edge& e) const noexcept
{
    try
    {
        return (edge_out_dir_map.at(std::make_pair(t, e)));
    }
    catch (const std::out_of_range&)
    {
        return layout::DIR_NONE;
    }
}

layout::directions fcn_gate_layout::get_unused_tile_dirs(const tile& t) const noexcept
{
    return ~(get_tile_inp_dirs(t) | get_tile_out_dirs(t));
}

bool fcn_gate_layout::is_tile_dir(const tile& t, const layout::directions& d) const noexcept
{
    return ((get_tile_inp_dirs(t) | get_tile_out_dirs(t)) & d) == d;
}

bool fcn_gate_layout::is_wire_dir(const tile& t, const logic_network::edge& e, const layout::directions& d) const noexcept
{
    return ((get_wire_inp_dirs(t, e) | get_wire_out_dirs(t, e)) & d) == d;
}

layout::directions fcn_gate_layout::get_bearing(const tile& t1, const tile& t2) const noexcept
{
    // if x-dimensions are the same
    if (t1[X] == t2[X])
    {
        if (t1[Y] - t2[Y] == 1)
            return layout::DIR_N;
        if (static_cast<int>(t1[Y]) - static_cast<int>(t2[Y]) == -1)
            return layout::DIR_S;
    }
    // if y-dimensions are the same
    if (t1[Y] == t2[Y])
    {
        if (t1[X] - t2[X] == 1)
            return layout::DIR_W;
        if (static_cast<int>(t1[X]) - static_cast<int>(t2[X]) == -1)
            return layout::DIR_E;
    }
    // tiles are not straightly reachable
    return layout::DIR_NONE;
}

bool fcn_gate_layout::is_pi(const tile& t) const noexcept
{
    return pi_set.count(t) > 0u;
}

bool fcn_gate_layout::is_po(const tile& t) const noexcept
{
    return po_set.count(t) > 0u;
}

std::vector<std::string> fcn_gate_layout::get_inp_names(const tile& t) const noexcept
{
    std::vector<std::string> inp_names{};

    if (get_op(t) == operation::PI)
    {
        inp_names.push_back(network->get_port_name(*get_logic_vertex(t)));
        return inp_names;
    }
    else
    {
        if (auto v = get_logic_vertex(t))
        {
            for (auto&& iav : network->inv_adjacent_vertices(*v, true))
            {
                if (network->get_op(iav) == operation::PI)
                    inp_names.push_back(network->get_port_name(iav));
            }
        }

        return inp_names;
    }
}

std::vector<std::string> fcn_gate_layout::get_out_names(const tile& t) const noexcept
{
    std::vector<std::string> out_names{};

    if (get_op(t) == operation::PO)
    {
        out_names.push_back(network->get_port_name(*get_logic_vertex(t)));
        return out_names;
    }
    else
    {
        if (auto v = get_logic_vertex(t))
        {
            for (auto&& av : network->adjacent_vertices(*v, true))
            {
                if (network->get_op(av) == operation::PO)
                    out_names.push_back(network->get_port_name(av));
            }
        }

        return out_names;
    }
}

std::string fcn_gate_layout::get_name() const noexcept
{
    return network->get_name();
}

fcn_gate_layout::bounding_box fcn_gate_layout::determine_bounding_box() const noexcept
{
    // calculate min_x
    std::size_t min_x = 0u;
    for (std::size_t x = 0u; x < this->x(); ++x)
    {
        bool elem_found = false;
        for (std::size_t y = 0u; y < this->y(); ++y)
        {
            if (!this->is_free_tile(tile{x, y, GROUND}) || !this->is_free_tile(tile{x, y, 1}))
            {
                elem_found = true;
                break;
            }
        }

        min_x = x;
        if (elem_found)
            break;
    }

    // calculate min_y
    std::size_t min_y = 0u;
    for (std::size_t y = 0u; y < this->y(); ++y)
    {
        bool elem_found = false;
        for (std::size_t x = 0u; x < this->x(); ++x)
        {
            if (!this->is_free_tile(tile{x, y, GROUND}) || !this->is_free_tile(tile{x, y, 1}))
            {
                elem_found = true;
                break;
            }
        }

        min_y = y;
        if (elem_found)
            break;
    }

    // calculate max_x
    std::size_t max_x = this->x() - 1;
    for (long x = this->x() - 1; x >= 0; --x)
    {
        bool elem_found = false;
        for (std::size_t y = 0u; y < this->y(); ++y)
        {
            if (!this->is_free_tile(tile{static_cast<std::size_t>(x), y, GROUND}) ||
                    !this->is_free_tile(tile{static_cast<std::size_t>(x), y, 1}))
            {
                elem_found = true;
                break;
            }
        }

        max_x = static_cast<std::size_t>(x);
        if (elem_found)
            break;
    }

    // calculate max_y
    std::size_t max_y = this->y() - 1;
    for (long y = this->y() - 1; y >= 0; --y)
    {
        bool elem_found = false;
        for (std::size_t x = 0u; x < this->x(); ++x)
        {
            if (!this->is_free_tile(tile{x, static_cast<std::size_t>(y), GROUND}) ||
                    !this->is_free_tile(tile{x, static_cast<std::size_t>(y), 1}))
            {
                elem_found = true;
                break;
            }
        }

        max_y = static_cast<std::size_t>(y);
        if (elem_found)
            break;
    }

    return bounding_box{min_x, min_y, max_x, max_y};
}

void fcn_gate_layout::shrink_to_fit() noexcept
{
    auto bb = determine_bounding_box();
    resize(fcn_dimension_xyz{bb.max_x + 1, bb.max_y + 1, z()});
}

fcn_gate_layout::energy_info fcn_gate_layout::calculate_energy() const noexcept
{
    float slow_energy = 0.0f, fast_energy = 0.0f;

    auto num_wires = wire_count();
    auto num_crossings = crossing_count();

    // adding wire energy (subtract 2 wires for each crossing)
    slow_energy += (num_wires - num_crossings * 2) * energy::WIRE_SLOW;
    fast_energy += (num_wires - num_crossings * 2) * energy::WIRE_FAST;

    // adding crossing energy
    slow_energy += num_crossings * energy::CROSSING_SLOW;
    fast_energy += num_crossings * energy::CROSSING_FAST;

    // counting gates
    auto num_inv_s = 0u, num_inv_b = 0u, num_and = 0u, num_or = 0u, num_maj = 0u, num_fan_out = 0u;

    for (auto&& tv : v_map.left)
    {
        auto t = tv.first;
        switch (get_op(t))
        {
            case operation::NOT:
            {
                // inputs are opposite to outputs --> straight inverter
                if (get_tile_inp_dirs(t) == layout::opposite(get_tile_out_dirs(t)))
                    ++num_inv_s;
                else  // else bent inverter
                    ++num_inv_b;
                break;
            }
            case operation::AND:
                ++num_and;
                break;
            case operation::OR:
                ++num_or;
                break;
            case operation::MAJ:
                ++num_maj;
                break;
            case operation::F1O2:
            case operation::F1O3:  // TODO do 1-3-Fan-outs have the same energy dissipation?
                ++num_fan_out;
                break;
            default:
                break;
        }
    }

    // adding inverter energy
    slow_energy += num_inv_s * energy::INVERTER_STRAIGHT_SLOW + num_inv_b * energy::INVERTER_BENT_SLOW;
    fast_energy += num_inv_s * energy::INVERTER_STRAIGHT_FAST + num_inv_b * energy::INVERTER_BENT_FAST;

    // adding conjunction energy
    slow_energy += num_and * energy::AND_SLOW;
    fast_energy += num_and * energy::AND_FAST;

    // adding disjunction energy
    slow_energy += num_or * energy::OR_SLOW;
    fast_energy += num_or * energy::OR_FAST;

    // adding majority energy
    slow_energy += num_maj * energy::MAJORITY_SLOW;
    fast_energy += num_maj * energy::MAJORITY_FAST;

    // adding fan-out energy
    slow_energy += num_fan_out * energy::FANOUT_SLOW;
    fast_energy += num_fan_out * energy::FANOUT_FAST;

    return std::make_pair(slow_energy, fast_energy);
}

void fcn_gate_layout::write_layout(std::ostream& os, bool io_color, bool clk_color) const noexcept
{
    // empty layout
    if (!area())
    {
        os << "∅" << std::endl;
        return;
    }

    const auto num_cols = x();
    const auto num_rows = y();

    // cache operations and directions in a 2d-matrix like object
    using s_matrix = std::vector<std::vector<std::string>>;
    s_matrix ops(num_rows, std::vector<std::string>(num_cols));
    s_matrix x_dirs(num_rows, std::vector<std::string>(num_cols + 1u, " "));
    s_matrix y_dirs(num_rows + 1u, std::vector<std::string>(num_cols, " "));

    for (auto i : iter::range(num_rows))
    {
        for (auto j : iter::range(num_cols))
        {
            auto t1 = tile{j, i, GROUND};
            auto t2 = above(t1);
            auto o  = get_op(t1);

            // a wire above indicates a crossing
            if (o == operation::W && get_op(t2) == operation::W)
                ops[i][j] = "+";
            else
                ops[i][j] = str(o);

            // determine outgoing directions
            if ((is_tile_out_dir(t1, layout::DIR_E) && is_tile_out_dir(tile{j + 1, i, GROUND}, layout::DIR_W)) ||
                (is_tile_out_dir(t2, layout::DIR_E) && is_tile_out_dir(tile{j + 1, i, GROUND + 1}, layout::DIR_W)))
                x_dirs[i][j] = "↔";
            else if (is_tile_out_dir(t1, layout::DIR_E) || is_tile_out_dir(t2, layout::DIR_E))
                x_dirs[i][j] = "→";
            if ((is_tile_out_dir(t1, layout::DIR_W) || is_tile_out_dir(t2, layout::DIR_W)) && j > 0u) // safety check to prevent SEGFAULT
                x_dirs[i][j - 1u] = "←";

            if ((is_tile_out_dir(t1, layout::DIR_N) && is_tile_out_dir(tile{j, i - 1, GROUND}, layout::DIR_S)) ||
                (is_tile_out_dir(t2, layout::DIR_N) && is_tile_out_dir(tile{j, i - 1, GROUND + 1}, layout::DIR_S)))
                y_dirs[i][j] = "↕";
            else if (is_tile_out_dir(t1, layout::DIR_N) || is_tile_out_dir(t2, layout::DIR_N))
                y_dirs[i][j] = "↑";
            if (is_tile_out_dir(t1, layout::DIR_S) || is_tile_out_dir(t2, layout::DIR_S))
                y_dirs[i + 1u][j] = "↓";
        }
    }

    // Escape color sequence for input colors (green).
    const char* INP_COLOR = "\033[38;5;28m";
    // Escape color sequence for output colors (red).
    const char* OUT_COLOR = "\033[38;5;166m";
    // Escape color sequence for latch colors (yellow on black).
    const char* LATCH_COLOR = "\033[48;5;232;38;5;226m";
    // Escape color sequence for resetting colors.
    const char* COLOR_RESET = "\033[0m";
    // Escape color sequences for clock background colors (white to dark grey).
    std::vector<const char*> CLOCK_COLORS{{"\033[48;5;255;38;5;232m",  // white tile, black text
                                           "\033[48;5;248;38;5;232m",  // light grey tile, black text
                                           "\033[48;5;240;38;5;255m",  // grey tile, white text
                                           "\033[48;5;236;38;5;255m"   // dark grey tile, white text
                                          }};

    // actual printing
    auto r_ctr = 0u, c_ctr = 0u;
    for (auto& row : ops)
    {
        for (auto& d : y_dirs[r_ctr])
            os << d << " ";
        os << std::endl;

        for (auto& o : row)
        {
            auto t = tile{c_ctr, r_ctr, GROUND};

            if (clk_color && tile_clocking(t))
                os << CLOCK_COLORS[*tile_clocking(t)];
            if (io_color && (get_latch(t) > 0u))
                os << LATCH_COLOR;
            if (io_color && is_pi(t))
                os << INP_COLOR;
            else if (io_color && is_po(t))
                os << OUT_COLOR;

            os << o << COLOR_RESET;

            os << x_dirs[r_ctr][c_ctr];
            ++c_ctr;
        }
        c_ctr = 0u;
        os << std::endl;

        ++r_ctr;
    }

    // print legend
    if (io_color || clk_color)
        std::cout << std::endl << "Legend: ";
    if (clk_color)
        std::cout << CLOCK_COLORS[0] << "0" << COLOR_RESET << ", " << CLOCK_COLORS[1] << "1" << COLOR_RESET << ", "
                  << CLOCK_COLORS[2] << "2" << COLOR_RESET << ", " << CLOCK_COLORS[3] << "3" << COLOR_RESET << ", ";
    if (io_color)
        std::cout << LATCH_COLOR << "L" << COLOR_RESET << ", " << INP_COLOR << "I" << COLOR_RESET << ", "
                  << OUT_COLOR << "O" << COLOR_RESET;
    if (io_color || clk_color)
        std::cout << std::endl;
}
