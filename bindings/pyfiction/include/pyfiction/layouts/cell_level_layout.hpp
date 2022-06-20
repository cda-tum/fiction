//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_CELL_LEVEL_LAYOUT_HPP
#define PYFICTION_CELL_LEVEL_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <string>
#include <type_traits>

namespace pyfiction
{

namespace detail
{

template <typename Technology>
void fcn_technology_cell_level_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    // fetch technology name
    auto tech_name = std::string{fiction::tech_impl_name<Technology>};
    std::transform(tech_name.begin(), tech_name.end(), tech_name.begin(), ::tolower);

    /**
     * FCN cell technology.
     */
    py::class_<Technology> tech(m, fmt::format("{}_technology", tech_name).c_str());

    py::enum_<typename Technology::cell_type> cell_type(tech, "cell_type");

    cell_type.value("EMPTY", Technology::cell_type::EMPTY);
    cell_type.value("NORMAL", Technology::cell_type::NORMAL);
    cell_type.value("INPUT", Technology::cell_type::INPUT);
    cell_type.value("OUTPUT", Technology::cell_type::OUTPUT);

    if constexpr (std::is_same_v<Technology, fiction::qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0);
        cell_type.value("CONST_1", Technology::cell_type::CONST_1);
    }
    else if constexpr (std::is_same_v<Technology, fiction::inml_technology>)
    {
        cell_type.value("SLANTED_EDGE_UP_MAGNET", Technology::cell_type::SLANTED_EDGE_UP_MAGNET);
        cell_type.value("SLANTED_EDGE_DOWN_MAGNET", Technology::cell_type::SLANTED_EDGE_DOWN_MAGNET);
        cell_type.value("INVERTER_MAGNET", Technology::cell_type::INVERTER_MAGNET);
        cell_type.value("CROSSWIRE_MAGNET", Technology::cell_type::CROSSWIRE_MAGNET);
        cell_type.value("FANOUT_COUPLER_MAGNET", Technology::cell_type::FANOUT_COUPLER_MAGNET);
    }
    else if constexpr (std::is_same_v<Technology, fiction::sidb_technology>)
    {
        // no additional cell types required
    }
    // NOTE: more technologies go here

    using cell_clk_cart_lyt = fiction::cell_level_layout<
        Technology, fiction::synchronization_element_layout<fiction::clocked_layout<
                        fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>;

    /**
     * Cell-level clocked Cartesian layout.
     */
    py::class_<cell_clk_cart_lyt,
               fiction::synchronization_element_layout<fiction::clocked_layout<
                   fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>(
        m, fmt::format("{}_layout", tech_name).c_str())
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<cell_clk_cart_lyt>&>(), "dimension"_a)
        .def(py::init(
                 [](const fiction::aspect_ratio<cell_clk_cart_lyt>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> cell_clk_cart_lyt
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<cell_clk_cart_lyt>(scheme_name);
                         scheme.has_value())
                     {
                         return cell_clk_cart_lyt{dimension, *scheme, layout_name};
                     }
                     else
                     {
                         throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                     }
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave", "layout_name"_a = "")

        .def("assign_cell_type", &cell_clk_cart_lyt::assign_cell_type)
        .def("get_cell_type", &cell_clk_cart_lyt::get_cell_type)
        .def("is_empty_cell", &cell_clk_cart_lyt::is_empty_cell)
        .def("get_layout_name", &cell_clk_cart_lyt::get_layout_name)
        .def("assign_cell_name", &cell_clk_cart_lyt::assign_cell_name)
        .def("get_cell_name", &cell_clk_cart_lyt::get_cell_name)
        .def("num_cells", &cell_clk_cart_lyt::num_cells)
        .def("is_empty", &cell_clk_cart_lyt::is_empty)
        .def("num_pis", &cell_clk_cart_lyt::num_pis)
        .def("num_pos", &cell_clk_cart_lyt::num_pos)
        .def("is_pi", &cell_clk_cart_lyt::is_pi)
        .def("is_po", &cell_clk_cart_lyt::is_po)

        .def("cells",
             [](const cell_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::coordinate<cell_clk_cart_lyt>> cells{};
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const cell_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::coordinate<cell_clk_cart_lyt>> pis{};
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const cell_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::coordinate<cell_clk_cart_lyt>> pos{};
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })

        ;
}

}  // namespace detail

inline void cell_level_layouts(pybind11::module& m)
{
    detail::fcn_technology_cell_level_layout<fiction::qca_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::inml_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::sidb_technology>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CELL_LEVEL_LAYOUT_HPP
