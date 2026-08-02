#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void design_sidb_gates_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("design_sidb_gates", &fiction::design_sidb_gates<Lyt, py_tt>, py::arg("skeleton"), py::arg("spec"),
          py::arg("params") = fiction::design_sidb_gates_params<fiction::cell<Lyt>>{}, py::arg("stats") = nullptr,
          DOC(fiction_design_sidb_gates));
}

}  // namespace detail

void design_sidb_gates(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::design_sidb_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_design_sidb_gates_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::design_sidb_gates_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.");

    /**
     * Design approach selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode>(
        m, "design_sidb_gates_mode", DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode))
        .value("QUICKCELL",
               fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::QUICKCELL,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL))
        .value("AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER))
        .value("RANDOM", fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::RANDOM,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM))
        .value("PRUNING_ONLY",
               fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::PRUNING_ONLY,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_PRUNING_ONLY));
    /**
     * Termination condition selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition>(
        m, "termination_condition")
        .value(
            "AFTER_FIRST_SOLUTION",
            fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition::AFTER_FIRST_SOLUTION)
        .value("ALL_COMBINATIONS_ENUMERATED",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::termination_condition::ALL_COMBINATIONS_ENUMERATED);

    /**
     * Parameters.
     */
    py::class_<fiction::design_sidb_gates_params<fiction::offset::ucoord_t>>(m, "design_sidb_gates_params",
                                                                             DOC(fiction_design_sidb_gates_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_params", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::operational_params,
                DOC(fiction_design_sidb_gates_params_operational_params))
        .def_rw("design_mode", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_mode,
                DOC(fiction_design_sidb_gates_params_design_mode))
        .def_rw("canvas", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::canvas,
                DOC(fiction_design_sidb_gates_params_canvas))
        .def_rw("number_of_canvas_sidbs",
                &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::number_of_canvas_sidbs,
                DOC(fiction_design_sidb_gates_params_number_of_canvas_sidbs))
        .def_rw("termination_cond", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_cond,
                DOC(fiction_design_sidb_gates_params_termination_condition));

    detail::design_sidb_gates_impl<py_sidb_100_lattice>(m);
    detail::design_sidb_gates_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
