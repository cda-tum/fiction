//
// Created by marcel on 25.07.18.
//

#include "fcn_layout.h"


fcn_layout::fcn_layout(fcn_dimension_xyz&& lengths, fcn_clocking_scheme&& clocking) noexcept
        :
        grid_graph(lengths),
        clocking(std::move(clocking))
{
    if (lengths[X] < 2 || lengths[Y] < 2 || lengths[Z] < 2)
        std::cerr << "NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs." << std::endl;
}

fcn_layout::fcn_layout(fcn_dimension_xy&& lengths, fcn_clocking_scheme&& clocking) noexcept
        :
        grid_graph(fcn_dimension_xyz{lengths[X], lengths[Y], 2}),
        clocking(std::move(clocking))
{
    if (lengths[X] < 2 || lengths[Y] < 2)
        std::cerr << "NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs." << std::endl;
}

fcn_layout::fcn_layout(fcn_clocking_scheme&& clocking) noexcept
        :
        grid_graph(fcn_dimension_xyz{2, 2, 2}),
        clocking(std::move(clocking))
{}

fcn_layout::fcn_layout() noexcept
        :
        grid_graph(fcn_dimension_xyz{2, 2, 2}),
        clocking(open_4_clocking)
{}

fcn_layout::~fcn_layout() = default;

void fcn_layout::resize(fcn_dimension_xyz&& lengths) noexcept
{
    if (lengths[X] < 2 || lengths[Y] < 2 || lengths[Z] < 2)
        std::cerr << "NOTE: Due to a bug in the BGL, every dimension should have a minimum size of 2 to prevent SEGFAULTs." << std::endl;

    resize_grid(std::move(lengths));
}

void fcn_layout::resize(fcn_dimension_xy&& lengths) noexcept
{
    resize(fcn_dimension_xyz{lengths[X], lengths[Y], 2});
}

fcn_layout::face fcn_layout::north(const face& f) const noexcept
{
    return previous(f, Y);
}

fcn_layout::face fcn_layout::east(const face& f) const noexcept
{
    return next(f, X);
}

fcn_layout::face fcn_layout::south(const face& f) const noexcept
{
    return next(f, Y);
}

fcn_layout::face fcn_layout::west(const face& f) const noexcept
{
    return previous(f, X);
}

fcn_layout::face fcn_layout::above(const face& f) const noexcept
{
    return next(f, Z);
}

fcn_layout::face fcn_layout::below(const face& f) const noexcept
{
    return previous(f, Z);
}

fcn_layout::ground fcn_layout::get_ground(const face& f) const noexcept
{
    return ground{f[X], f[Y]};
}

std::size_t fcn_layout::x() const noexcept
{
    return length(X);
}

std::size_t fcn_layout::y() const noexcept
{
    return length(Y);
}

std::size_t fcn_layout::z() const noexcept
{
    return length(Z);
}

fcn_layout::face fcn_layout::random_face() const noexcept
{
    std::mt19937 rgen(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, get_vertex_count() - 1); // distribution in range [0, |V|]

    return get_by_index(dist(rgen));
}

fcn_layout::face fcn_layout::random_face(const std::size_t n) const noexcept
{
    std::mt19937 rgen(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, x() * y()); // distribution in range [0, x * y]

    auto idx = dist(rgen);
    std::size_t col = idx % y();
    std::size_t row = idx / y();

    return face{col, row, n};
}

std::size_t fcn_layout::area() const noexcept
{
    return x() * y();
}

fcn_clock::number fcn_layout::num_clocks() const noexcept
{
    return clocking.num_clocks;
}

bool fcn_layout::is_regularly_clocked() const noexcept
{
    return clocking.regular;
}

void fcn_layout::assign_clocking(const face& f, const fcn_clock::number c) noexcept
{
    if (!clocking.regular && c <= clocking.num_clocks)
        c_map[get_ground(f)] = c;
}

void fcn_layout::assign_clocking(const face_index f, const fcn_clock::number c) noexcept
{
    assign_clocking(get_vertex(f), c);
}

void fcn_layout::assign_latch(const face& f, const latch_delay l) noexcept
{
    if (l == 0u)
        l_map.erase(get_ground(f));
    else
        l_map[get_ground(f)] = l;
}

fcn_layout::latch_delay fcn_layout::get_latch(const face& f) const noexcept
{
    try
    {
        return l_map.at(get_ground(f));
    }
    catch (const std::out_of_range&)
    {
        return 0u;
    }
}

std::vector<std::string> fcn_layout::latch_str_reprs() const noexcept
{
    std::vector<std::string> reprs{};
    for (const auto& l : l_map)
    {
        std::stringstream ss{};
        ss << "l@" << l.first << "=" << l.second;
        reprs.push_back(ss.str());
    }

    return reprs;
}

fcn_layout::yz_slice::yz_slice(const std::size_t x, const fcn_layout* const ptr) noexcept
        :
        x_value(x),
        fgl(std::move(ptr))
{}

fcn_layout::yz_slice::z_stack::z_stack(const std::size_t x, const std::size_t y, const fcn_layout* const ptr) noexcept
        :
        x_value(x),
        y_value(y),
        fgl(std::move(ptr))
{}

fcn_layout::face fcn_layout::yz_slice::z_stack::operator[](const std::size_t z)
{
    if (fgl->x() >= x_value && fgl->y() >= y_value && fgl->z() >= z)
        return face{x_value, y_value, z};

    throw std::out_of_range("Given indices are located outside of layout bounds.");
}

fcn_layout::yz_slice::z_stack::operator face()
{
    if (fgl->x() >= x_value && fgl->y() >= y_value) // fgl->z() >= GROUND is true by definition
        return {x_value, y_value, GROUND};

    throw std::out_of_range("Given indices are located outside of layout bounds.");
}

fcn_layout::yz_slice::z_stack fcn_layout::yz_slice::operator[](const std::size_t y) const
{
    return {x_value, y, fgl};
}

fcn_layout::yz_slice fcn_layout::operator[](const std::size_t x) const
{
    return {x, this};
}

fcn_layout::face fcn_layout::operator()(const std::size_t x, const std::size_t y, const std::size_t z) const
{
    if (this->x() >= x && this->y() >= y && this->z() >= z)
        return face{x, y, z};

    throw std::out_of_range("Given indices are located outside of layout bounds.");
}