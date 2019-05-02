//
// Created by marcel on 25.07.18.
//

#include "fcn_cell_layout.h"


fcn_cell_layout::fcn_cell_layout(fcn_dimension_xyz&& lengths, fcn_clocking_scheme&& clocking,
                                 fcn::technology tech, const std::string& name) noexcept
        :
        fcn_layout(std::move(lengths), std::move(clocking)),
        technology{tech},
        name{name}
{}

fcn_cell_layout::fcn_cell_layout(fcn_dimension_xy&& lengths, fcn_clocking_scheme&& clocking,
                                 fcn::technology tech, const std::string& name) noexcept
        :
        fcn_layout(std::move(lengths), std::move(clocking)),
        technology{tech},
        name{name}
{}

fcn_cell_layout::fcn_cell_layout(fcn_gate_library_ptr&& lib)
        :
        fcn_layout(fcn_dimension_xyz{lib->get_layout()->x() * lib->gate_x_size(),
                                     lib->get_layout()->y() * lib->gate_y_size(),
                                     lib->get_layout()->z()},
                   std::move(lib->get_layout()->clocking)),
        library(std::move(lib)),
        technology{library->get_technology()},
        name{library->get_layout()->get_name()}
{
    if (!clocking.regular)
        map_irregular_clocking();

    map_layout();
}

void fcn_cell_layout::assign_cell_type(const cell& c, const fcn::cell_type t) noexcept
{
    if (t == fcn::EMPTY_CELL)
        type_map.erase(c);
    else
        type_map[c] = t;
}

fcn::cell_type fcn_cell_layout::get_cell_type(const cell& c) const noexcept
{
    try
    {
        return type_map.at(c);
    }
    catch (const std::out_of_range&)
    {
        return fcn::EMPTY_CELL;
    }
}

bool fcn_cell_layout::is_free_cell(const cell& c) const noexcept
{
    return get_cell_type(c) == fcn::EMPTY_CELL;
}

void fcn_cell_layout::assign_cell_mode(const cell& c, const fcn::cell_mode m) noexcept
{
    if (m == fcn::cell_mode::NORMAL)
        mode_map.erase(c);
    else
        mode_map[c] = m;
}

fcn::cell_mode fcn_cell_layout::get_cell_mode(const cell& c) const noexcept
{
    try
    {
        return mode_map.at(c);
    }
    catch (const std::out_of_range&)
    {
        return fcn::cell_mode::NORMAL;
    }
}

void fcn_cell_layout::assign_cell_name(const cell& c, const std::string& n) noexcept
{
    if (n.empty())
        name_map.erase(c);
    else
        name_map[c] = n;
}

std::string fcn_cell_layout::get_cell_name(const cell& c) const noexcept
{
    try
    {
        return name_map.at(c);
    }
    catch (const std::out_of_range&)
    {
        return "";
    }
}


void fcn_cell_layout::print_cell(const cell& c, std::ostream& os) const noexcept
{
    os << get_cell_type(c);
}

std::size_t fcn_cell_layout::cell_count() const noexcept
{
    return type_map.size();
}

boost::optional<fcn_clock::zone> fcn_cell_layout::cell_clocking(const cell& c) const noexcept
{
    if (clocking.regular)
    {
        std::size_t x = c[X] / library->gate_x_size(), y = c[Y] / library->gate_y_size();
        return clocking.scheme[y % clocking.num_clocks][x % clocking.num_clocks];
    }
    else  // irregular clocking accesses clocking map
    {
        try
        {
            return c_map.at(get_ground(c));
        }
        catch (const std::out_of_range&)
        {
            return boost::none;
        }
    }
}

void fcn_cell_layout::assign_gate(const cell& c, const fcn_gate& g, const latch_delay l,
                                  const std::vector<std::string>& inp_names, const std::vector<std::string>& out_names)
{
    auto start_x = c[X];
    auto start_y = c[Y];
    auto layer   = c[Z];

    auto inp_counter = 0u, out_counter = 0u;

    for (auto y = 0u; y < g.size(); ++y)
    {
        for (auto x = 0u; x < g[y].size(); ++x)
        {
            const auto pos = cell{start_x + x, start_y + y, layer};
            const auto type = g[y][x];

            assign_cell_type(pos, type);
            assign_latch(pos, l);

            if (type == fcn::INPUT_CELL)
            {
                assign_cell_name(pos, (inp_names.size() > inp_counter) ? inp_names[inp_counter] : "Input");
                ++inp_counter;
            }
            else if (type == fcn::OUTPUT_CELL)
            {
                assign_cell_name(pos, (out_names.size() > out_counter) ? out_names[out_counter] : "Output");
                ++out_counter;
            }
        }
    }
}

fcn_gate_library_ptr fcn_cell_layout::get_library() const noexcept
{
    return library;
}

fcn::technology fcn_cell_layout::get_technology() const noexcept
{
    return technology;
}

std::string fcn_cell_layout::get_name() const noexcept
{
    return name;
}

void fcn_cell_layout::write_layout(std::ostream& os, bool io_color) const noexcept
{
    // Escape color sequence for input colors (green).
    const char* INP_COLOR = "\033[38;5;28m";
    // Escape color sequence for output colors (red).
    const char* OUT_COLOR = "\033[38;5;166m";
    // Escape color sequence for latch colors (yellow on black).
    const char* LATCH_COLOR = "\033[48;5;232;38;5;226m";
    // Escape color sequence for resetting colors.
    const char* COLOR_RESET = "\033[0m";

    for (auto y_pos : iter::range(y()))
    {
        for (auto x_pos : iter::range(x()))
        {
            cell c{x_pos, y_pos, GROUND};

            auto type_0 = get_cell_type(c);
            auto type_1 = get_cell_type(above(c));

            // is crossing
            if (type_1 != fcn::EMPTY_CELL)
                os << std::string(1u, type_1);
            else
            {
                if (io_color && get_latch(c) > 0u)
                    os << LATCH_COLOR;
                if (io_color && type_0 == fcn::INPUT_CELL)
                    os << INP_COLOR;
                else if (io_color && type_0 == fcn::OUTPUT_CELL)
                    os << OUT_COLOR;

                os << (type_0 == fcn::NORMAL_CELL ? "▢" : std::string(1u, type_0)) << COLOR_RESET;
            }
        }
        os << std::endl;
    }

    // print legend
    if (io_color)
        std::cout << std::endl << "Legend: " << LATCH_COLOR << "L" << COLOR_RESET << ", "
                  << INP_COLOR << "I" << COLOR_RESET << ", " << OUT_COLOR << "O" << COLOR_RESET << std::endl;
}

void fcn_cell_layout::map_irregular_clocking()
{
    auto layout = library->get_layout();

    for (auto&& c : this->ground_layer())
    {
        std::size_t x = c[X] / library->gate_x_size(), y = c[Y] / library->gate_y_size();
        auto t = (*layout)(x, y);

        if (auto clk = layout->tile_clocking(t))
            assign_clocking(c, *clk);
    }
}

void fcn_cell_layout::assign_vias() noexcept
{
    for (auto&& c : crossing_layers() | iter::filterfalse([this](const cell& _c){return is_free_cell(_c);}))
    {
        auto surrounding = surrounding_2d(c) | iter::filterfalse([this](const cell& _c){return is_free_cell(_c);});
        // if number of surrounding cells is 1 or less, it is a via cell
        if (std::distance(surrounding.begin(), surrounding.end()) <= 1u)
        {
            // change mode to via
            assign_cell_mode(c, fcn::cell_mode::VERTICAL);
            // create a via ground cell
            auto ground_via = cell{c[X], c[Y], GROUND};
            assign_cell_type(ground_via, fcn::NORMAL_CELL);
            assign_cell_mode(ground_via, fcn::cell_mode::VERTICAL);
        }
    }
}

void fcn_cell_layout::map_layout()
{
    auto layout = library->get_layout();

    for (auto&& t : layout->tiles())
    {
        if (layout->is_free_tile(t))
            continue;

        assign_gate(cell{t[X] * library->gate_x_size(), t[Y] * library->gate_y_size(), t[Z]}, library->set_up_gate(t),
                    layout->get_latch(t), layout->get_inp_names(t), layout->get_out_names(t));
    }

    assign_vias();
}
