//
// Created by marcel on 04.06.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/io/print_layout.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/layouts/utils/layout_utils.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Technology>
void fcn_technology_cell_level_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // fetch technology name
    auto tech_name = std::string{fiction::tech_impl_name<Technology>};
    std::ranges::transform(tech_name, tech_name.begin(), ::tolower);
    if constexpr (std::is_same_v<Technology, fiction::mol_qca_technology>)
    {
        // Keep the Python technology name readable by preserving the word boundary in molQCA.
        tech_name = "mol_qca";
    }

    /**
     * FCN cell technology.
     */
    const py::class_<Technology> tech(m, fmt::format("{}_technology", tech_name).c_str());

    py::enum_<typename Technology::cell_type> cell_type(tech, "cell_type");

    cell_type.value("EMPTY", Technology::cell_type::EMPTY);
    if constexpr (std::is_same_v<Technology, fiction::mol_qca_technology>)
    {
        cell_type.value("NORMAL1", Technology::cell_type::NORMAL1, DOC(fiction_mol_qca_technology_cell_type_NORMAL1));
        cell_type.value("NORMAL2", Technology::cell_type::NORMAL2, DOC(fiction_mol_qca_technology_cell_type_NORMAL2));
        cell_type.value("NORMAL3", Technology::cell_type::NORMAL3, DOC(fiction_mol_qca_technology_cell_type_NORMAL3));
        cell_type.value("NORMAL4", Technology::cell_type::NORMAL4, DOC(fiction_mol_qca_technology_cell_type_NORMAL4));
    }
    else
    {
        cell_type.value("NORMAL", Technology::cell_type::NORMAL);
    }
    cell_type.value("INPUT", Technology::cell_type::INPUT);
    cell_type.value("OUTPUT", Technology::cell_type::OUTPUT);

    if constexpr (std::is_same_v<Technology, fiction::qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0, DOC(fiction_qca_technology_cell_type_CONST_0));
        cell_type.value("CONST_1", Technology::cell_type::CONST_1, DOC(fiction_qca_technology_cell_type_CONST_1));
    }
    else if constexpr (std::is_same_v<Technology, fiction::mol_qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0, DOC(fiction_mol_qca_technology_cell_type_CONST_0));
        cell_type.value("CONST_1", Technology::cell_type::CONST_1, DOC(fiction_mol_qca_technology_cell_type_CONST_1));
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
    // NOTE: more technologies go here

    using py_cartesian_technology_cell_layout = py_cartesian_cell_layout<Technology>;

    /**
     * Cell-level clocked Cartesian layout.
     */
    py::class_<py_cartesian_technology_cell_layout,
               fiction::layouts::clocked_layout<fiction::layouts::tile_based_layout<
                   fiction::layouts::cartesian_layout<fiction::layouts::coords::offset>>>>(
        m, fmt::format("{}_layout", tech_name).c_str(), DOC(fiction_cell_level_layout))
        .def(py::init<>(), DOC(fiction_cell_level_layout_cell_level_layout))
        .def(py::init<const fiction::aspect_ratio<py_cartesian_technology_cell_layout>&>(), py::arg("dimension"),
             DOC(fiction_cell_level_layout_cell_level_layout))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_cartesian_technology_cell_layout>       self,
               const fiction::aspect_ratio<py_cartesian_technology_cell_layout>& dimension,
               const std::string& scheme_name, const std::string& layout_name)
            {
                if (const auto scheme =
                        fiction::layouts::get_clocking_scheme<py_cartesian_technology_cell_layout>(scheme_name);
                    scheme.has_value())
                {
                    new (self.p) py_cartesian_technology_cell_layout{dimension, *scheme, layout_name};
                    return;
                }

                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            },
            py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
            DOC(fiction_cell_level_layout_cell_level_layout_2))

        .def("assign_cell_type", &py_cartesian_technology_cell_layout::assign_cell_type, py::arg("c"), py::arg("ct"),
             DOC(fiction_cell_level_layout_assign_cell_type))
        .def("get_cell_type", &py_cartesian_technology_cell_layout::get_cell_type, py::arg("c"),
             DOC(fiction_cell_level_layout_get_cell_type))
        .def("is_empty_cell", &py_cartesian_technology_cell_layout::is_empty_cell, py::arg("c"),
             DOC(fiction_cell_level_layout_is_empty_cell))
        .def("assign_cell_name", &py_cartesian_technology_cell_layout::assign_cell_name, py::arg("c"), py::arg("n"),
             DOC(fiction_cell_level_layout_assign_cell_name))
        .def("get_cell_name", &py_cartesian_technology_cell_layout::get_cell_name, py::arg("c"),
             DOC(fiction_cell_level_layout_get_cell_name))
        .def("set_layout_name", &py_cartesian_technology_cell_layout::set_layout_name, py::arg("name"),
             DOC(fiction_cell_level_layout_set_layout_name))
        .def("get_layout_name", &py_cartesian_technology_cell_layout::get_layout_name,
             DOC(fiction_cell_level_layout_get_layout_name))
        .def("num_cells", &py_cartesian_technology_cell_layout::num_cells, DOC(fiction_cell_level_layout_num_cells))
        .def("is_empty", &py_cartesian_technology_cell_layout::is_empty, DOC(fiction_cell_level_layout_is_empty))
        .def("num_pis", &py_cartesian_technology_cell_layout::num_pis, DOC(fiction_cell_level_layout_num_pis))
        .def("num_pos", &py_cartesian_technology_cell_layout::num_pos, DOC(fiction_cell_level_layout_num_pos))
        .def("is_pi", &py_cartesian_technology_cell_layout::is_pi, py::arg("c"), DOC(fiction_cell_level_layout_is_pi))
        .def("is_po", &py_cartesian_technology_cell_layout::is_po, py::arg("c"), DOC(fiction_cell_level_layout_is_po))

        .def("get_cell_type", &py_cartesian_technology_cell_layout::get_cell_type, py::arg("c"),
             DOC(fiction_cell_level_layout_get_cell_type))
        .def("get_cells_by_type", &py_cartesian_technology_cell_layout::get_cells_by_type, py::arg("type"),
             DOC(fiction_cell_level_layout_get_cells_by_type))
        .def("num_cells_of_given_type", &py_cartesian_technology_cell_layout::num_cells_of_given_type, py::arg("type"),
             DOC(fiction_cell_level_layout_num_cells_of_given_type))

        .def("cells",
             [](const py_cartesian_technology_cell_layout& lyt)
             {
                 std::vector<fiction::coordinate<py_cartesian_technology_cell_layout>> cells{};
                 cells.reserve(lyt.num_cells());
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const py_cartesian_technology_cell_layout& lyt)
             {
                 std::vector<fiction::coordinate<py_cartesian_technology_cell_layout>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const py_cartesian_technology_cell_layout& lyt)
             {
                 std::vector<fiction::coordinate<py_cartesian_technology_cell_layout>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })
        .def(
            "bounding_box_2d",
            [](const py_cartesian_technology_cell_layout& lyt)
            {
                const auto bb = fiction::layouts::bounding_box_2d<py_cartesian_technology_cell_layout>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))

        .def(
            "__repr__",
            [](const py_cartesian_technology_cell_layout& lyt) -> std::string
            {
                std::stringstream stream{};

                if constexpr (std::is_same_v<Technology, fiction::sidb_technology>)
                {
                    layouts::io::print_layout(layouts::utils::convert_layout_to_siqad_coordinates(lyt), stream);
                }
                else
                {
                    layouts::io::print_layout(lyt, stream);
                }

                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

void cell_level_layouts(nanobind::module_& m)
{
    detail::fcn_technology_cell_level_layout<fiction::qca_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::mol_qca_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::inml_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::sidb_technology>(m);
}

}  // namespace pyfiction
