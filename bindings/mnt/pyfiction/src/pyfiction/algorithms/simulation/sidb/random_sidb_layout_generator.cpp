//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void random_layout_generator_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("generate_random_sidb_layout", &fiction::generate_random_sidb_layout<Lyt>, py::arg("params"),
          py::arg("lyt_skeleton") = std::nullopt, DOC(fiction_generate_random_sidb_layout));

    m.def("generate_multiple_random_sidb_layouts", &fiction::generate_multiple_random_sidb_layouts<Lyt>,
          py::arg("params"), py::arg("lyt_skeleton") = std::nullopt,
          DOC(fiction_generate_multiple_random_sidb_layouts));
}

}  // namespace detail

void random_sidb_layout_generator(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<typename fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::positive_charges>(
        m, "positive_charges", DOC(fiction_generate_random_sidb_layout_params_positive_charges))
        .value("ALLOWED",
               fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::positive_charges::ALLOWED,
               DOC(fiction_generate_random_sidb_layout_params_positive_charges_ALLOWED))
        .value(
            "FORBIDDEN",
            fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::positive_charges::FORBIDDEN,
            DOC(fiction_generate_random_sidb_layout_params_positive_charges_FORBIDDEN))
        .value(
            "MAY_OCCUR",
            fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::positive_charges::MAY_OCCUR,
            DOC(fiction_generate_random_sidb_layout_params_positive_charges_MAY_OCCUR));

    /**
     * Parameters.
     */
    py::class_<fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>>(
        m, "generate_random_sidb_layout_params", DOC(fiction_generate_random_sidb_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("coordinate_pair",
                &fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::coordinate_pair,
                DOC(fiction_generate_random_sidb_layout_params_coordinate_pair))
        .def_rw("number_of_sidbs",
                &fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::number_of_sidbs,
                DOC(fiction_generate_random_sidb_layout_params_number_of_sidbs))
        .def_rw("positive_sidbs",
                &fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::positive_sidbs,
                DOC(fiction_generate_random_sidb_layout_params_positive_sidbs))
        .def_rw("simulation_parameters",
                &fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::simulation_parameters,
                DOC(fiction_generate_random_sidb_layout_params_simulation_parameters))
        .def_rw("maximal_attempts",
                &fiction::generate_random_sidb_layout_params<fiction::layouts::coords::offset>::maximal_attempts,
                DOC(fiction_generate_random_sidb_layout_params_maximal_attempts))
        .def_rw("number_of_unique_generated_layouts",
                &fiction::generate_random_sidb_layout_params<
                    fiction::layouts::coords::offset>::number_of_unique_generated_layouts,
                DOC(fiction_generate_random_sidb_layout_params_number_of_unique_generated_layouts))
        .def_rw("maximal_attempts_for_multiple_layouts",
                &fiction::generate_random_sidb_layout_params<
                    fiction::layouts::coords::offset>::maximal_attempts_for_multiple_layouts,
                DOC(fiction_generate_random_sidb_layout_params_maximal_attempts_for_multiple_layouts));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::random_layout_generator_impl<py_sidb_100_lattice>(m);
    detail::random_layout_generator_impl<py_sidb_111_lattice>(m);
    detail::random_layout_generator_impl<py_sidb_layout>(m);
}

}  // namespace pyfiction
