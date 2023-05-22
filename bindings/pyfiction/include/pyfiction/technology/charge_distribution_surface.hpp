//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_SIQAD_LAYOUT_HPP
#define PYFICTION_SIQAD_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_charge_state.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

namespace pyfiction
{

inline void charge_distribution_surface(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * SiDB charge state.
     */
    py::enum_<fiction::sidb_charge_state>(m, "sidb_charge_state", DOC(fiction_sidb_charge_state))
        .value("NEGATIVE", fiction::sidb_charge_state::NEGATIVE, DOC(fiction_sidb_charge_state_NEGATIVE))
        .value("NEUTRAL", fiction::sidb_charge_state::NEUTRAL, DOC(fiction_sidb_charge_state_NEUTRAL))
        .value("POSITIVE", fiction::sidb_charge_state::POSITIVE, DOC(fiction_sidb_charge_state_POSITIVE))
        .value("NONE", fiction::sidb_charge_state::NONE, DOC(fiction_sidb_charge_state_NONE))

        ;

    m.def("charge_state_to_sign", &fiction::charge_state_to_sign, "cs"_a, DOC(fiction_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sign_to_charge_state, "sg"_a, DOC(fiction_sign_to_charge_state));
    m.def("charge_configuration_to_string", &fiction::charge_configuration_to_string, "charge_distribution"_a,
          DOC(fiction_charge_configuration_to_string));

    /**
     * Charge distribution surface.
     *
     * @note All functions had to be redefined, because in the regular C++ version, this layout extends a specific
     * hexagonal layout, which we do not expose in pyfiction.
     */
    py::class_<py_charge_distribution_surface>(m, "charge_distribution_surface",
                                               DOC(fiction_charge_distribution_surface))
        .def(py::init<const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             "params"_a = fiction::sidb_simulation_parameters{}, "cs"_a = fiction::sidb_charge_state::NEGATIVE)
        .def(py::init<const fiction::aspect_ratio<py_charge_distribution_surface>&,
                      const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             "dimension"_a, "params"_a = fiction::sidb_simulation_parameters{},
             "cs"_a = fiction::sidb_charge_state::NEGATIVE)
        .def(
            "x", [](const py_charge_distribution_surface& lyt) { return lyt.x(); }, DOC(fiction_cartesian_layout_x))
        .def(
            "y", [](const py_charge_distribution_surface& lyt) { return lyt.y(); }, DOC(fiction_cartesian_layout_y))
        .def(
            "z", [](const py_charge_distribution_surface& lyt) { return lyt.z(); }, DOC(fiction_cartesian_layout_z))
        .def(
            "area", [](const py_charge_distribution_surface& lyt) { return lyt.area(); },
            DOC(fiction_cartesian_layout_area))
        .def(
            "resize",
            [](py_charge_distribution_surface& lyt, const py_siqad_coordinate& dimension) { lyt.resize(dimension); },
            "dimension"_a, DOC(fiction_cartesian_layout_resize))

        .def("set_physical_parameters", &py_charge_distribution_surface::set_physical_parameters, "params"_a)
        .def("get_phys_params", &py_charge_distribution_surface::get_phys_params)
        .def("get_all_sidb_locations_in_nm", &py_charge_distribution_surface::get_all_sidb_locations_in_nm)

        .def("charge_exists", &py_charge_distribution_surface::charge_exists, "cs"_a)

        .def("cell_to_index", &py_charge_distribution_surface::cell_to_index, "c"_a)

        .def("assign_charge_state", &py_charge_distribution_surface::assign_charge_state, "c"_a, "cs"_a)
        .def("assign_charge_by_cell_index", &py_charge_distribution_surface::assign_charge_by_cell_index, "index"_a,
             "cs"_a)
        .def("assign_charge_state_by_cell_index", &py_charge_distribution_surface::assign_charge_state_by_cell_index,
             "index"_a, "cs"_a, "update_chargeconf"_a = true)
        .def("set_all_charge_states", &py_charge_distribution_surface::set_all_charge_states, "cs"_a)

        .def("get_charge_state", &py_charge_distribution_surface::get_charge_state, "c"_a)
        .def("get_charge_state_by_index", &py_charge_distribution_surface::get_charge_state_by_index, "index"_a)
        .def("get_all_sidb_charges", &py_charge_distribution_surface::get_all_sidb_charges)

        .def("negative_sidb_detection", &py_charge_distribution_surface::negative_sidb_detection)

        .def("get_chargeless_potential_between_sidbs",
             &py_charge_distribution_surface::get_chargeless_potential_between_sidbs, "c1"_a, "c2"_a)
        .def("get_potential_between_sidbs", &py_charge_distribution_surface::get_potential_between_sidbs, "c1"_a,
             "c2"_a)
        .def("potential_between_sidbs", &py_charge_distribution_surface::potential_between_sidbs, "c1"_a, "c2"_a)

        .def("get_electrostatic_potential_by_indices",
             &py_charge_distribution_surface::get_electrostatic_potential_by_indices, "index1"_a, "index2"_a)
        .def("potential_between_sidbs_by_index", &py_charge_distribution_surface::potential_between_sidbs_by_index,
             "index1"_a, "index2"_a)

        .def("update_local_potential", &py_charge_distribution_surface::update_local_potential)
        .def("get_local_potential", &py_charge_distribution_surface::get_local_potential, "c"_a)
        .def("get_local_potential_by_index", &py_charge_distribution_surface::get_local_potential_by_index, "index"_a)

        .def("set_system_energy_to_zero", &py_charge_distribution_surface::set_system_energy_to_zero)
        .def("recompute_system_energy", &py_charge_distribution_surface::recompute_system_energy)
        .def("get_system_energy", &py_charge_distribution_surface::get_system_energy)

        .def("update_after_charge_change", &py_charge_distribution_surface::update_after_charge_change)

        .def("validity_check", &py_charge_distribution_surface::validity_check)
        .def("is_physically_valid", &py_charge_distribution_surface::is_physically_valid)

        .def("charge_distribution_to_index", &py_charge_distribution_surface::charge_distribution_to_index)
        .def("index_to_charge_distribution", &py_charge_distribution_surface::index_to_charge_distribution)
        .def("get_charge_index", &py_charge_distribution_surface::get_charge_index)

        .def("increase_charge_index_by_one", &py_charge_distribution_surface::increase_charge_index_by_one)
        .def("get_max_charge_index", &py_charge_distribution_surface::get_max_charge_index)
        .def("assign_charge_index", &py_charge_distribution_surface::assign_charge_index, "index"_a)

        .def("adjacent_search", &py_charge_distribution_surface::adjacent_search, "alpha"_a, "negative_indices"_a)

        .def(
            "is_within_bounds",
            [](const py_charge_distribution_surface& lyt, const py_siqad_coordinate& c)
            { return lyt.is_within_bounds(c); },
            "c"_a, DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const py_charge_distribution_surface& lyt)
            {
                std::vector<fiction::coordinate<py_charge_distribution_surface>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_coordinates))

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_SIQAD_LAYOUT_HPP
