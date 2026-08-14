//
// Created by marcel on 03.06.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <sstream>
#include <stdexcept>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename ClockedLyt>
void clocked_layout(nanobind::module_& m, const std::string& topology)
{
    namespace py = nanobind;

    /**
     * Clocked Cartesian layout.
     */
    py::class_<ClockedLyt, LytBase>(m, fmt::format("clocked_{}_layout", topology).c_str(), DOC(fiction_clocked_layout))
        .def(py::init<>(), DOC(fiction_clocked_layout_clocked_layout))
        .def(py::init<const fiction::aspect_ratio<ClockedLyt>&>(), py::arg("dimension"),
             DOC(fiction_clocked_layout_clocked_layout))
        .def(
            "__init__",
            [](py::pointer_and_handle<ClockedLyt> self, const fiction::aspect_ratio<ClockedLyt>& dimension,
               const std::string& scheme_name)
            {
                if (const auto scheme = fiction::get_clocking_scheme<ClockedLyt>(scheme_name); scheme.has_value())
                {
                    new (self.p) ClockedLyt{dimension, *scheme};
                    return;
                }

                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            },
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

        .def(
            "__repr__",
            [](const ClockedLyt& lyt) -> std::string
            {
                std::stringstream stream{};
                print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

void clocked_layouts(nanobind::module_& m)
{
    detail::clocked_layout<py_cartesian_layout, py_cartesian_clocked_layout>(m, "cartesian");
    detail::clocked_layout<py_shifted_cartesian_layout, py_shifted_cartesian_clocked_layout>(m, "shifted_cartesian");
    detail::clocked_layout<py_hexagonal_layout, py_hexagonal_clocked_layout>(m, "hexagonal");
}

}  // namespace pyfiction
