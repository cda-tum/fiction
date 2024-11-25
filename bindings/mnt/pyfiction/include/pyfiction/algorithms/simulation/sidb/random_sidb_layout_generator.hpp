//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP
#define PYFICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void random_layout_generator(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("generate_random_sidb_layout", &fiction::generate_random_sidb_layout<Lyt>, py::arg("lyt_skeleton"),
          py::arg("params") = fiction::generate_random_sidb_layout_params<fiction::cell<Lyt>>{},
          DOC(fiction_generate_random_sidb_layout));

    m.def("generate_multiple_random_sidb_layouts", &fiction::generate_multiple_random_sidb_layouts<Lyt>,
          py::arg("lyt_skeleton"),
          py::arg("params") = fiction::generate_random_sidb_layout_params<fiction::cell<Lyt>>{},
          DOC(fiction_generate_multiple_random_sidb_layouts));
}

}  // namespace detail

inline void random_sidb_layout_generator(pybind11::module& m)
{
    namespace py = pybind11;
    namespace py = pybind11;

    py::enum_<typename fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::positive_charges>(
        m, "positive_charges", DOC(fiction_generate_random_sidb_layout_params_positive_charges))
        .value("ALLOWED",
               fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::positive_charges::ALLOWED,
               DOC(fiction_generate_random_sidb_layout_params_positive_charges_ALLOWED))
        .value("FORBIDDEN",
               fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::positive_charges::FORBIDDEN,
               DOC(fiction_generate_random_sidb_layout_params_positive_charges_FORBIDDEN))
        .value("MAY_OCCUR",
               fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::positive_charges::MAY_OCCUR,
               DOC(fiction_generate_random_sidb_layout_params_positive_charges_MAY_OCCUR));

    /**
     * Parameters.
     */
    py::class_<fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>>(
        m, "generate_random_sidb_layout_params", DOC(fiction_generate_random_sidb_layout_params))
        .def(py::init<>())
        .def_readwrite("coordinate_pair",
                       &fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::coordinate_pair,
                       DOC(fiction_generate_random_sidb_layout_params_coordinate_pair))
        .def_readwrite("number_of_sidbs",
                       &fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::number_of_sidbs,
                       DOC(fiction_generate_random_sidb_layout_params_number_of_sidbs))
        .def_readwrite("positive_sidbs",
                       &fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::positive_sidbs,
                       DOC(fiction_generate_random_sidb_layout_params_positive_sidbs))
        .def_readwrite("maximal_attempts",
                       &fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::maximal_attempts,
                       DOC(fiction_generate_random_sidb_layout_params_maximal_attempts))
        .def_readwrite(
            "number_of_unique_generated_layouts",
            &fiction::generate_random_sidb_layout_params<fiction::offset::ucoord_t>::number_of_unique_generated_layouts,
            DOC(fiction_generate_random_sidb_layout_params_number_of_unique_generated_layouts))
        .def_readwrite("maximal_attempts_for_multiple_layouts",
                       &fiction::generate_random_sidb_layout_params<
                           fiction::offset::ucoord_t>::maximal_attempts_for_multiple_layouts,
                       DOC(fiction_generate_random_sidb_layout_params_maximal_attempts_for_multiple_layouts));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::random_layout_generator<py_sidb_100_lattice>(m);
    detail::random_layout_generator<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_RANDOM_SIDB_LAYOUT_GENERATOR_HPP
