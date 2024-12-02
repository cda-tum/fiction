//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_CELLLEVELLAYOUT_HPP
#define PYFICTION_CELLLEVELLAYOUT_HPP

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
#include <variant>
#include <vector>

namespace pyfiction
{

namespace detail
{

class py_cartesian_cell_layout : public fiction::clocked_layout<
                                     fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>
{
  public:
    template <typename... Args>
    py_cartesian_cell_layout(std::string technology, Args... args)
    {
        std::transform(technology.begin(), technology.end(), technology.begin(), ::tolower);

        if (technology == "qca")
        {
            underlying_layout = py_qca_layout(args...);
        }
        else if (technology == "inml")
        {
            underlying_layout = py_inml_layout(args...);
        }
        else if (technology == "sidb")
        {
            underlying_layout = py_sidb_layout(args...);
        }
        else
        {
            throw std::runtime_error(fmt::format("'{}' does not refer to a supported technology", technology));
        }
    }

    std::variant<py_qca_layout, py_inml_layout, py_sidb_layout> underlying_layout{};
};

inline void cell_level_layout(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * Cell-level clocked Cartesian layout.
     */
    py::class_<
        py_cartesian_cell_layout,
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>(
        m, "CellLevelLayout", DOC(fiction_cell_level_layout))
        .def(py::init<std::string>(), py::arg("technology"), DOC(fiction_cell_level_layout_cell_level_layout))
        .def(py::init<std::string, const fiction::aspect_ratio<py_cartesian_cell_layout>&>(), py::arg("technology"),
             py::arg("dimension"), DOC(fiction_cell_level_layout_cell_level_layout))
        // .def(py::init(
        //          [](const fiction::aspect_ratio<py_cartesian_cell_layout>& dimension,
        //             const std::string&                                                scheme_name,
        //             const std::string& layout_name) -> py_cartesian_cell_layout
        //          {
        //              if (const auto scheme =
        //                      fiction::get_clocking_scheme<py_cartesian_cell_layout>(scheme_name);
        //                  scheme.has_value())
        //              {
        //                  return py_cartesian_cell_layout{dimension, *scheme, layout_name};
        //              }
        //
        //              throw std::runtime_error("Given name does not refer to a supported clocking scheme");
        //          }),
        //      py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
        //      DOC(fiction_cell_level_layout_cell_level_layout_2))
        //
        // .def("assign_cell_type", &py_cartesian_technology_cell_layout::assign_cell_type, py::arg("c"), py::arg("ct"),
        //      DOC(fiction_cell_level_layout_assign_cell_type))
        // .def("get_cell_type", &py_cartesian_technology_cell_layout::get_cell_type, py::arg("c"),
        //      DOC(fiction_cell_level_layout_get_cell_type))
        // .def("is_empty_cell", &py_cartesian_technology_cell_layout::is_empty_cell, py::arg("c"),
        //      DOC(fiction_cell_level_layout_is_empty_cell))
        // .def("assign_cell_name", &py_cartesian_technology_cell_layout::assign_cell_name, py::arg("c"), py::arg("n"),
        //      DOC(fiction_cell_level_layout_assign_cell_name))
        // .def("get_cell_name", &py_cartesian_technology_cell_layout::get_cell_name, py::arg("c"),
        //      DOC(fiction_cell_level_layout_get_cell_name))
        // .def("set_layout_name", &py_cartesian_technology_cell_layout::set_layout_name, py::arg("name"),
        //      DOC(fiction_cell_level_layout_set_layout_name))
        // .def("get_layout_name", &py_cartesian_technology_cell_layout::get_layout_name,
        //      DOC(fiction_cell_level_layout_get_layout_name))
        // .def("num_cells", &py_cartesian_technology_cell_layout::num_cells, DOC(fiction_cell_level_layout_num_cells))
        // .def("is_empty", &py_cartesian_technology_cell_layout::is_empty, DOC(fiction_cell_level_layout_is_empty))
        // .def("num_pis", &py_cartesian_technology_cell_layout::num_pis, DOC(fiction_cell_level_layout_num_pis))
        // .def("num_pos", &py_cartesian_technology_cell_layout::num_pos, DOC(fiction_cell_level_layout_num_pos))
        // .def("is_pi", &py_cartesian_technology_cell_layout::is_pi, py::arg("c"),
        // DOC(fiction_cell_level_layout_is_pi)) .def("is_po", &py_cartesian_technology_cell_layout::is_po,
        // py::arg("c"), DOC(fiction_cell_level_layout_is_po))
        //
        .def("cells",
             [](const py_cartesian_cell_layout& lyt)
             {
                 const auto apply = [](const auto& cell_layout)
                 {
                     using Lyt = std::decay_t<decltype(cell_layout)>;

                     std::vector<fiction::coordinate<Lyt>> cells{};
                     cells.reserve(cell_layout.num_cells());
                     cell_layout.foreach_cell([&cells](const auto& c) { cells.push_back(c); });

                     return cells;
                 };

                 return std::visit(apply, lyt.underlying_layout);
             })
        // .def("pis",
        //      [](const py_cartesian_technology_cell_layout& lyt)
        //      {
        //          std::vector<fiction::coordinate<py_cartesian_technology_cell_layout>> pis{};
        //          pis.reserve(lyt.num_pis());
        //          lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
        //          return pis;
        //      })
        // .def("pos",
        //      [](const py_cartesian_technology_cell_layout& lyt)
        //      {
        //          std::vector<fiction::coordinate<py_cartesian_technology_cell_layout>> pos{};
        //          pos.reserve(lyt.num_pos());
        //          lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
        //          return pos;
        //      })
        // .def(
        //     "bounding_box_2d",
        //     [](const py_cartesian_technology_cell_layout& lyt)
        //     {
        //         const auto bb = fiction::bounding_box_2d<py_cartesian_technology_cell_layout>(lyt);
        //         return std::make_pair(bb.get_min(), bb.get_max());
        //     },
        //     DOC(fiction_bounding_box_2d_overridden))
        //
        // .def("__repr__",
        //      [](const py_cartesian_technology_cell_layout& lyt) -> std::string
        //      {
        //          std::stringstream stream{};
        //
        //          if constexpr (std::is_same_v<Technology, fiction::sidb_technology>)
        //          {
        //              print_layout(convert_layout_to_siqad_coordinates(lyt), stream);
        //          }
        //          else
        //          {
        //              print_layout(lyt, stream);
        //          }
        //
        //          return stream.str();
        //      })

        ;
}

}  // namespace detail

inline void cell_level_layout(pybind11::module& m)
{
    detail::cell_level_layout(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CELLLEVELLAYOUT_HPP
