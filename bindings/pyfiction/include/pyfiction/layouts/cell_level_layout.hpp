//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_CELL_LEVEL_LAYOUT_HPP
#define PYFICTION_CELL_LEVEL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <type_traits>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Technology>
void fcn_technology_cells(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    // fetch technology name
    auto tech_name = std::string{fiction::tech_impl_name<Technology>};
    std::transform(tech_name.begin(), tech_name.end(), tech_name.begin(), ::tolower);

    /**
     * FCN cell technology.
     */
    const py::class_<Technology> tech(m, fmt::format("{}_technology", tech_name).c_str());

    py::enum_<typename Technology::cell_type> cell_type(tech, "cell_type");

    cell_type.value("EMPTY", Technology::cell_type::EMPTY);
    cell_type.value("NORMAL", Technology::cell_type::NORMAL);
    cell_type.value("INPUT", Technology::cell_type::INPUT);
    cell_type.value("OUTPUT", Technology::cell_type::OUTPUT);

    if constexpr (std::is_same_v<Technology, fiction::qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0, DOC(fiction_qca_technology_cell_type_CONST_0));
        cell_type.value("CONST_1", Technology::cell_type::CONST_1, DOC(fiction_qca_technology_cell_type_CONST_1));
    }
    else if constexpr (std::is_same_v<Technology, fiction::inml_technology>)
    {
        cell_type.value("SLANTED_EDGE_UP_MAGNET", Technology::cell_type::SLANTED_EDGE_UP_MAGNET,
                        DOC(fiction_inml_technology_cell_type_SLANTED_EDGE_UP_MAGNET));
        cell_type.value("SLANTED_EDGE_DOWN_MAGNET", Technology::cell_type::SLANTED_EDGE_DOWN_MAGNET,
                        DOC(fiction_inml_technology_cell_type_SLANTED_EDGE_DOWN_MAGNET));
        cell_type.value("INVERTER_MAGNET", Technology::cell_type::INVERTER_MAGNET,
                        DOC(fiction_inml_technology_cell_type_INVERTER_MAGNET));
        cell_type.value("CROSSWIRE_MAGNET", Technology::cell_type::CROSSWIRE_MAGNET,
                        DOC(fiction_inml_technology_cell_type_CROSSWIRE_MAGNET));
        cell_type.value("FANOUT_COUPLER_MAGNET", Technology::cell_type::FANOUT_COUPLER_MAGNET,
                        DOC(fiction_inml_technology_cell_type_FANOUT_COUPLER_MAGNET));
    }
    else if constexpr (std::is_same_v<Technology, fiction::sidb_technology>)
    {
        cell_type.value("LOGIC", Technology::cell_type::LOGIC);
    }
}

template <typename Technology, typename ClockedLyt, typename CellLyt>
void fcn_technology_cell_layout(pybind11::module& m, std::string coordinate_name = "")
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    // fetch technology name
    auto tech_name = std::string{fiction::tech_impl_name<Technology>};
    std::transform(tech_name.begin(), tech_name.end(), tech_name.begin(), ::tolower);

    /**
     * Cell-level clocked Cartesian layout.
     */

    py::class_<CellLyt, ClockedLyt>(m, fmt::format("{}_layout{}", tech_name, coordinate_name).c_str(),
                                    DOC(fiction_cell_level_layout))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<CellLyt>&>(), "dimension"_a,
             DOC(fiction_cell_level_layout_cell_level_layout))
        .def(py::init(
                 [](const fiction::aspect_ratio<CellLyt>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> CellLyt
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<CellLyt>(scheme_name); scheme.has_value())
                     {
                         return CellLyt{dimension, *scheme, layout_name};
                     }

                     throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave", "layout_name"_a = "",
             DOC(fiction_cell_level_layout_cell_level_layout_2))

        .def("assign_cell_type", &CellLyt::assign_cell_type, "c"_a, "ct"_a,
             DOC(fiction_cell_level_layout_assign_cell_type))
        .def("get_cell_type", &CellLyt::get_cell_type, "c"_a, DOC(fiction_cell_level_layout_get_cell_type))
        .def("is_empty_cell", &CellLyt::is_empty_cell, "c"_a, DOC(fiction_cell_level_layout_is_empty_cell))
        .def("assign_cell_name", &CellLyt::assign_cell_name, "c"_a, "n"_a,
             DOC(fiction_cell_level_layout_assign_cell_name))
        .def("get_cell_name", &CellLyt::get_cell_name, "c"_a, DOC(fiction_cell_level_layout_get_cell_name))
        .def("set_layout_name", &CellLyt::set_layout_name, "name"_a, DOC(fiction_cell_level_layout_set_layout_name))
        .def("get_layout_name", &CellLyt::get_layout_name, DOC(fiction_cell_level_layout_get_layout_name))
        .def("num_cells", &CellLyt::num_cells, DOC(fiction_cell_level_layout_num_cells))
        .def("is_empty", &CellLyt::is_empty, DOC(fiction_cell_level_layout_is_empty))
        .def("num_pis", &CellLyt::num_pis, DOC(fiction_cell_level_layout_num_pis))
        .def("num_pos", &CellLyt::num_pos, DOC(fiction_cell_level_layout_num_pos))
        .def("is_pi", &CellLyt::is_pi, "c"_a, DOC(fiction_cell_level_layout_is_pi))
        .def("is_po", &CellLyt::is_po, "c"_a, DOC(fiction_cell_level_layout_is_po))

        .def("cells",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> cells{};
                 cells.reserve(lyt.num_cells());
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })
        .def(
            "bounding_box_2d",
            [](const CellLyt& lyt)
            {
                const auto bb = fiction::bounding_box_2d<CellLyt>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))

        .def("__repr__",
             [](const CellLyt& lyt) -> std::string
             {
                 std::stringstream stream{};

                 if constexpr (std::is_same_v<Technology, fiction::sidb_technology>)
                 {
                     print_layout(convert_layout_to_siqad_coordinates(lyt), stream);
                 }
                 else
                 {
                     print_layout(lyt, stream);
                 }

                 return stream.str();
             });
}

}  // namespace detail

inline void cell_level_layouts(pybind11::module& m)
{
    detail::fcn_technology_cells<fiction::qca_technology>(m);
    detail::fcn_technology_cells<fiction::inml_technology>(m);
    detail::fcn_technology_cells<fiction::sidb_technology>(m);

    detail::fcn_technology_cell_layout<fiction::qca_technology, py_cartesian_clocked_layout, py_qca_layout>(m);
    detail::fcn_technology_cell_layout<fiction::inml_technology, py_cartesian_clocked_layout, py_inml_layout>(m);
    detail::fcn_technology_cell_layout<fiction::sidb_technology, py_cartesian_clocked_layout, py_sidb_layout>(m);
    detail::fcn_technology_cell_layout<fiction::sidb_technology, py_cartesian_clocked_layout_cube, py_sidb_layout_cube>(
        m, "cube");
}

}  // namespace pyfiction

#endif  // PYFICTION_CELL_LEVEL_LAYOUT_HPP
