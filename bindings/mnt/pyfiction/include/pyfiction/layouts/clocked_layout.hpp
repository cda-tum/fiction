//
// Created by marcel on 03.06.22.
//

#ifndef PYFICTION_CLOCKED_LAYOUT_HPP
#define PYFICTION_CLOCKED_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <set>
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename ClockedLyt>
void clocked_layout(pybind11::module& m, const std::string& topology, const std::string& coord_type)
{
    namespace py = pybind11;

    /**
     * Clocked layout.
     */
    py::class_<ClockedLyt, LytBase>(m, fmt::format("clocked_{}_layout_{}", topology, coord_type).c_str(),
                                    DOC(fiction_clocked_layout))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<fiction::coordinate<ClockedLyt>>&>(), py::arg("dimension"),
             DOC(fiction_clocked_layout_clocked_layout))
        .def(py::init(
                 [](const fiction::aspect_ratio<fiction::coordinate<ClockedLyt>>& dimension,
                    const std::string&                                            scheme_name)
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<ClockedLyt>(scheme_name); scheme.has_value())
                     {
                         return ClockedLyt{dimension, *scheme};
                     }

                     throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                 }),
             py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", DOC(fiction_clocked_layout_clocked_layout_2))

        .def("assign_clock_number", &ClockedLyt::assign_clock_number, py::arg("cz"), py::arg("cn"),
             DOC(fiction_clocked_layout_assign_clock_number))
        .def("get_clock_number", &ClockedLyt::get_clock_number, py::arg("cz"),
             DOC(fiction_clocked_layout_get_clock_number))
        .def("num_clocks", &ClockedLyt::num_clocks, DOC(fiction_clocked_layout_num_clocks))
        .def("is_regularly_clocked", &ClockedLyt::is_regularly_clocked,
             DOC(fiction_clocked_layout_is_regularly_clocked))

        .def("is_clocking_scheme", &ClockedLyt::is_clocking_scheme, py::arg("name"),
             DOC(fiction_clocked_layout_is_clocking_scheme))

        .def("is_incoming_clocked", &ClockedLyt::is_incoming_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_clocked_layout_is_incoming_clocked))
        .def("is_outgoing_clocked", &ClockedLyt::is_outgoing_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_clocked_layout_is_outgoing_clocked))

        .def("incoming_clocked_zones", &ClockedLyt::incoming_clocked_zones, py::arg("cz"),
             DOC(fiction_clocked_layout_incoming_clocked_zones))
        .def("outgoing_clocked_zones", &ClockedLyt::outgoing_clocked_zones, py::arg("cz"),
             DOC(fiction_clocked_layout_outgoing_clocked_zones))

        .def("in_degree", &ClockedLyt::in_degree, py::arg("cz"), DOC(fiction_clocked_layout_in_degree))
        .def("out_degree", &ClockedLyt::out_degree, py::arg("cz"), DOC(fiction_clocked_layout_out_degree))
        .def("degree", &ClockedLyt::degree, py::arg("cz"), DOC(fiction_clocked_layout_degree))

        .def("__repr__",
             [](const ClockedLyt& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace detail

void clocked_layouts(pybind11::module& m)
{
    detail::clocked_layout<py_cartesian_layout, py_cartesian_clocked_layout>(m, "cartesian", "offset_coordinates");
    detail::clocked_layout<py_cartesian_layout_cube_coordinates, py_cartesian_clocked_layout_cube_coordinates>(
        m, "cartesian", "cube_coordinates");
    detail::clocked_layout<py_shifted_cartesian_layout, py_shifted_cartesian_clocked_layout>(m, "shifted_cartesian",
                                                                                             "offset_coordinates");
    detail::clocked_layout<py_shifted_cartesian_layout_cube_coordinates,
                           py_shifted_cartesian_clocked_layout_cube_coordinates>(m, "shifted_cartesian",
                                                                                 "cube_coordinates");
    detail::clocked_layout<py_hexagonal_layout, py_hexagonal_clocked_layout>(m, "hexagonal", "offset_coordinates");
    detail::clocked_layout<py_hexagonal_layout_cube_coordinates, py_hexagonal_clocked_layout_cube_coordinates>(
        m, "hexagonal", "cube_coordinates");
}
/**
 * A "factory" function that Python users can call as
 *   <cartesian|shifted_cartesian|hexagonal>_clocked_layout(dimension, scheme_name="open", coordinate_type="offset")
 * to create the correct layout type (offset or cube).
 */
inline void clocked_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "clocked_cartesian_layout",
        [](const py::tuple dimension, const std::string& scheme_name, const std::string& coordinate_type)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout_cube_coordinates>(dimension);
                if (const auto scheme =
                        fiction::get_clocking_scheme<py_cartesian_clocked_layout_cube_coordinates>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_cartesian_clocked_layout_cube_coordinates{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout>(dimension);
                if (const auto scheme = fiction::get_clocking_scheme<py_cartesian_clocked_layout>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_cartesian_clocked_layout{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open",
        py::arg("coordinate_type") = "offset",  // default
        R"doc(
            Creates and returns a clocked_cartesian_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:

                - "offset" (default)
                - "cube"
        )doc");

    m.def(
        "clocked_shifted_cartesian_layout",
        [](const py::tuple dimension, const std::string& scheme_name,
           const std::string& coordinate_type /* = "offset" by default */)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_shifted_cartesian_layout_cube_coordinates>(dimension);
                if (const auto scheme =
                        fiction::get_clocking_scheme<py_shifted_cartesian_clocked_layout_cube_coordinates>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_shifted_cartesian_clocked_layout_cube_coordinates{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_shifted_cartesian_layout>(dimension);
                if (const auto scheme = fiction::get_clocking_scheme<py_shifted_cartesian_clocked_layout>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_shifted_cartesian_clocked_layout{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open",
        py::arg("coordinate_type") = "offset",  // default
        R"doc(
            Creates and returns a clocked_shifted_cartesian_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:

                - "offset" (default)
                - "cube"
        )doc");

    m.def(
        "clocked_hexagonal_layout",
        [](const py::tuple dimension, const std::string& scheme_name,
           const std::string& coordinate_type /* = "offset" by default */)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_hexagonal_layout_cube_coordinates>(dimension);
                if (const auto scheme =
                        fiction::get_clocking_scheme<py_hexagonal_clocked_layout_cube_coordinates>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_hexagonal_clocked_layout_cube_coordinates{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_hexagonal_layout>(dimension);
                if (const auto scheme = fiction::get_clocking_scheme<py_hexagonal_clocked_layout>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_hexagonal_clocked_layout{ar, *scheme});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open",
        py::arg("coordinate_type") = "offset",  // default
        R"doc(
            Creates and returns a clocked_hexagonal_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:

                - "offset" (default)
                - "cube"
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_CLOCKED_LAYOUT_HPP
