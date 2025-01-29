//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
#define PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void charge_distribution_surface_layout(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;

    using py_cds = py_charge_distribution_surface_layout<Lyt>;

    /**
     * Charge distribution surface.
     */

    py::class_<py_cds, Lyt>(m, fmt::format("charge_distribution_surface{}", lattice).c_str(),
                            DOC(fiction_charge_distribution_surface), py::module_local())
        .def(py::init<const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             py::arg("params") = fiction::sidb_simulation_parameters{},
             py::arg("cs")     = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const Lyt&, const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             py::arg("lyt"), py::arg("params") = fiction::sidb_simulation_parameters{},
             py::arg("cs") = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const py_cds&>(), py::arg("lyt"))

        .def("get_all_sidb_locations_in_nm", [](py_cds& cds) { return cds.get_all_sidb_locations_in_nm(); })
        .def(
            "assign_cell_type", [](py_cds& cds, fiction::cell<py_cds> c, typename py_cds::cell_type ct)
            { return cds.assign_cell_type(c, ct); }, py::arg("c"), py::arg("ct"))
        .def(
            "assign_physical_parameters", [](py_cds& cds, fiction::sidb_simulation_parameters params)
            { return cds.assign_physical_parameters(params); }, py::arg("params"))
        .def("get_phys_params", [](py_cds& cds) { return cds.get_simulation_params(); })
        .def(
            "charge_exists", [](py_cds& cds, fiction::sidb_charge_state cs) { return cds.charge_exists(cs); },
            py::arg("cs"))
        .def(
            "cell_to_index", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.cell_to_index(c); }, py::arg("c"))
        .def(
            "assign_charge_state",
            [](py_cds& cds, fiction::cell<py_cds> c, fiction::sidb_charge_state cs,
               fiction::charge_index_mode index_mode) { return cds.assign_charge_state(c, cs, index_mode); },
            py::arg("c"), py::arg("cs"), py::arg("index_mode") = fiction::charge_index_mode::UPDATE_CHARGE_INDEX)
        .def(
            "assign_charge_by_cell_index", [](py_cds& cds, uint64_t i, fiction::sidb_charge_state cs)
            { return cds.assign_charge_by_cell_index(i, cs); }, py::arg("i"), py::arg("cs"))
        .def(
            "assign_all_charge_states",
            [](py_cds& cds, fiction::sidb_charge_state cs) { return cds.assign_all_charge_states(cs); }, py::arg("cs"))
        .def(
            "assign_dependent_cell", [](py_cds& cds, fiction::cell<py_cds> dependent_cell)
            { return cds.assign_dependent_cell(dependent_cell); }, py::arg("dependent_cell"))
        .def(
            "assign_base_number", [](py_cds& cds, uint8_t base) { return cds.assign_base_number(base); },
            py::arg("base"))
        .def(
            "add_sidb_defect_to_potential_landscape",
            [](py_cds& cds, fiction::cell<py_cds> c, fiction::sidb_defect defect)
            { return cds.add_sidb_defect_to_potential_landscape(c, defect); }, py::arg("c"), py::arg("defect"))
        .def(
            "erase_defect", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.erase_defect(c); }, py::arg("c"))

        .def(
            "assign_charge_state_by_cell_index",
            [](py_cds& cds, uint64_t index, fiction::sidb_charge_state cs, fiction::charge_index_mode index_mode)
            { return cds.assign_charge_state_by_cell_index(index, cs, index_mode); }, py::arg("index"), py::arg("cs"),
            py::arg("index_mode") = fiction::charge_index_mode::UPDATE_CHARGE_INDEX)
        .def(
            "get_charge_state", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.get_charge_state(c); },
            py::arg("c"))
        .def(
            "get_charge_state_by_index",
            [](py_cds& cds, uint64_t index) { return cds.get_charge_state_by_index(index); }, py::arg("index"))
        .def("get_all_sidb_charges", [](py_cds& cds) { return cds.get_all_sidb_charges(); })

        .def("negative_sidb_detection", [](py_cds& cds) { return cds.negative_sidb_detection(); })
        .def(
            "get_nm_distance_between_sidbs", [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_nm_distance_between_sidbs(c1, c2); }, py::arg("c1"), py::arg("c2"))
        .def(
            "get_nm_distance_by_indices", [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.get_nm_distance_by_indices(index1, index2); }, py::arg("index1"), py::arg("index2"))
        .def(
            "calculate_chargeless_potential_between_sidbs_by_index", [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.calculate_chargeless_potential_between_sidbs_by_index(index1, index2); }, py::arg("index1"),
            py::arg("index2"))
        .def(
            "calculate_chargeless_potential_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.calculate_chargeless_potential_between_sidbs(c1, c2); }, py::arg("c1"), py::arg("c2"))
        .def(
            "get_chargeless_potential_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_chargeless_potential_between_sidbs(c1, c2); }, py::arg("c1"), py::arg("c2"))
        .def(
            "get_chargless_potential_by_indices", [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.get_chargeless_potential_by_indices(index1, index2); }, py::arg("index1"), py::arg("index2"))
        .def(
            "get_potential_between_sidbs", [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_potential_between_sidbs(c1, c2); }, py::arg("c1"), py::arg("c2"))
        .def(
            "update_local_potential", [](py_cds& cds, fiction::charge_distribution_history history_mode)
            { return cds.update_local_potential(history_mode); }, py::arg("consider_history"))
        .def(
            "get_local_potential", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.get_local_potential(c); },
            py::arg("c"))
        .def(
            "get_local_potential_by_index",
            [](py_cds& cds, uint64_t index) { return cds.get_local_potential_by_index(index); }, py::arg("index"))
        .def("assign_system_energy_to_zero", [](py_cds& cds) { return cds.assign_system_energy_to_zero(); })
        .def("recompute_system_energy", [](py_cds& cds) { return cds.recompute_system_energy(); })
        .def("get_system_energy", [](py_cds& cds) { return cds.get_system_energy(); })

        .def(
            "update_after_charge_change",
            [](py_cds& cds, fiction::dependent_cell_mode dep_cell, fiction::energy_calculation energy_calculation_mode,
               fiction::charge_distribution_history history_mode)
            { return cds.update_after_charge_change(dep_cell, energy_calculation_mode, history_mode); },
            py::arg("dep_cell")                = fiction::dependent_cell_mode::FIXED,
            py::arg("energy_calculation_mode") = fiction::energy_calculation::UPDATE_ENERGY,
            py::arg("history_mode")            = fiction::charge_distribution_history::NEGLECT)

        .def("validity_check", &py_cds::validity_check)
        .def("is_physically_valid", &py_cds::is_physically_valid)
        .def("charge_distribution_to_index_general", &py_cds::charge_distribution_to_index_general)
        .def("charge_distribution_to_index", &py_cds::charge_distribution_to_index)
        .def("get_charge_index_and_base", &py_cds::get_charge_index_and_base)
        .def(
            "increase_charge_index_by_one",
            [](py_cds& cds, fiction::dependent_cell_mode dependent_cell_fixed,
               fiction::energy_calculation          recompute_system_energy,
               fiction::charge_distribution_history consider_history, fiction::exact_sidb_simulation_engine engine)
            {
                return cds.increase_charge_index_by_one(dependent_cell_fixed, recompute_system_energy, consider_history,
                                                        engine);
            },
            py::arg("dependent_cell_fixed")    = fiction::dependent_cell_mode::FIXED,
            py::arg("recompute_system_energy") = fiction::energy_calculation::UPDATE_ENERGY,
            py::arg("consider_history")        = fiction::charge_distribution_history::NEGLECT,
            py::arg("engine")                  = fiction::exact_sidb_simulation_engine::EXGS)

        .def("get_max_charge_index", &py_cds::get_max_charge_index)
        .def("assign_charge_index", &py_cds::assign_charge_index, py::arg("charge_index"), py::arg("cdc"))
        .def("adjacent_search", &py_cds::adjacent_search, py::arg("alpha"), py::arg("negative_indices"))
        .def("assign_global_external_potential", &py_cds::assign_global_external_potential, py::arg("potential_value"),
             py::arg("dependent_cell") = fiction::dependent_cell_mode::FIXED)
        .def("is_three_state_simulation_required", &py_cds::is_three_state_simulation_required)
        .def("get_positive_candidates", &py_cds::get_positive_candidates)
        .def("three_state_cell_to_index", &py_cds::three_state_cell_to_index, py::arg("c"))
        .def("two_state_cell_to_index", &py_cds::two_state_cell_to_index, py::arg("c"))
        .def("index_to_cell", &py_cds::index_to_cell, py::arg("index"))
        .def("index_to_three_state_cell", &py_cds::index_to_three_state_cell, py::arg("index"))
        .def("index_to_two_state_cell", &py_cds::index_to_two_state_cell, py::arg("index"))

        .def("chargeless_potential_at_given_distance", &py_cds::chargeless_potential_at_given_distance,
             py::arg("distance"))
        .def("chargeless_potential_generated_by_defect_at_given_distance",
             &py_cds::chargeless_potential_generated_by_defect_at_given_distance, py::arg("distance"),
             py::arg("defect") = fiction::sidb_defect{})
        .def("assign_local_external_potential", &py_cds::assign_local_external_potential, py::arg("external_potential"))
        .def("get_local_external_potentials", &py_cds::get_local_external_potentials)
        .def("get_local_defect_potentials", &py_cds::get_local_defect_potentials)
        .def("get_defects", &py_cds::get_defects)
        //
        .def("update_charge_state_of_dependent_cell", &py_cds::update_charge_state_of_dependent_cell)
        .def("get_charge_index_of_sub_layout", &py_cds::get_charge_index_of_sub_layout)
        .def("charge_index_gray_code_to_charge_distribution", &py_cds::charge_index_gray_code_to_charge_distribution,
             py::arg("new_gray_code"), py::arg("old_gray_code"))

        .def("increase_charge_index_of_sub_layout_by_one", &py_cds::increase_charge_index_of_sub_layout_by_one,
             py::arg("dependent_cell_fixed")    = fiction::dependent_cell_mode::FIXED,
             py::arg("recompute_system_energy") = fiction::energy_calculation::UPDATE_ENERGY,
             py::arg("consider_history")        = fiction::charge_distribution_history::NEGLECT,
             py::arg("engine")                  = fiction::exact_sidb_simulation_engine::EXGS)
        .def("assign_charge_index_by_gray_code", &py_cds::assign_charge_index_by_gray_code,
             py::arg("current_gray_code"), py::arg("previous_gray_code"),
             py::arg("dependent_cell")   = fiction::dependent_cell_mode::FIXED,
             py::arg("energy_calc_mode") = fiction::energy_calculation::UPDATE_ENERGY,
             py::arg("history_mode")     = fiction::charge_distribution_history::NEGLECT)
        .def("reset_charge_index_sub_layout", &py_cds::reset_charge_index_sub_layout)
        .def("get_max_charge_index_sub_layout", &py_cds::get_max_charge_index_sub_layout)
        .def("assign_charge_index_by_two_gray_codes", &py_cds::assign_charge_index_by_two_gray_codes,
             py::arg("gray_code"), py::arg("gray_code_old"))
        .def("get_sidb_order", &py_cds::get_sidb_order)
        .def("add_sidb", &py_cds::add_sidb, py::arg("cell"), py::arg("charge"))

        .def(
            "is_within_bounds", [](const py_cds& lyt, const fiction::coordinate<py_cds>& c)
            { return lyt.is_within_bounds(c); }, py::arg("c"), DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "bounding_box_2d",
            [](const py_cds& lyt)
            {
                const auto bb = fiction::bounding_box_2d<py_cds>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))

        .def("__repr__",
             [](const py_cds& lyt)
             {
                 std::stringstream ss;
                 fiction::print_layout(fiction::convert_layout_to_siqad_coordinates(lyt), ss);
                 return ss.str();
             })

        ;
}

}  // namespace detail

inline void charge_distribution_surfaces(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * Dependent cell mode.
     */
    py::enum_<fiction::dependent_cell_mode>(m, "dependent_cell_mode", DOC(fiction_dependent_cell_mode),
                                            py::module_local())
        .value("FIXED", fiction::dependent_cell_mode::FIXED, DOC(fiction_dependent_cell_mode_FIXED))
        .value("VARIABLE", fiction::dependent_cell_mode::VARIABLE, DOC(fiction_dependent_cell_mode_VARIABLE))

        ;

    /**
     * Energy calculation.
     */
    py::enum_<fiction::energy_calculation>(m, "energy_calculation", DOC(fiction_energy_calculation), py::module_local())
        .value("KEEP_OLD_ENERGY_VALUE", fiction::energy_calculation::KEEP_OLD_ENERGY_VALUE,
               DOC(fiction_energy_calculation_KEEP_OLD_ENERGY_VALUE))
        .value("UPDATE_ENERGY", fiction::energy_calculation::UPDATE_ENERGY,
               DOC(fiction_energy_calculation_UPDATE_ENERGY))

        ;

    /**
     * Charge distribution mode.
     */
    py::enum_<fiction::charge_distribution_mode>(m, "charge_distribution_mode", py::module_local(),
                                                 DOC(fiction_charge_distribution_mode))
        .value("UPDATE_CHARGE_DISTRIBUTION", fiction::charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION,
               DOC(fiction_charge_distribution_mode_UPDATE_CHARGE_DISTRIBUTION))
        .value("KEEP_CHARGE_DISTRIBUTION", fiction::charge_distribution_mode::KEEP_CHARGE_DISTRIBUTION,
               DOC(fiction_charge_distribution_mode_KEEP_CHARGE_DISTRIBUTION))

        ;

    /**
     * Charge index mode.
     */
    py::enum_<fiction::charge_index_mode>(m, "charge_index_mode", DOC(fiction_energy_calculation), py::module_local())
        .value("UPDATE_CHARGE_INDEX", fiction::charge_index_mode::UPDATE_CHARGE_INDEX,
               DOC(fiction_charge_index_mode_UPDATE_CHARGE_INDEX))
        .value("KEEP_CHARGE_INDEX", fiction::charge_index_mode::KEEP_CHARGE_INDEX,
               DOC(fiction_charge_index_mode_KEEP_CHARGE_INDEX))

        ;

    /**
     * Charge distribution history.
     */
    py::enum_<fiction::charge_distribution_history>(m, "charge_distribution_history",
                                                    DOC(fiction_charge_distribution_history), py::module_local())
        .value("CONSIDER", fiction::charge_distribution_history::CONSIDER,
               DOC(fiction_charge_distribution_history_CONSIDER))
        .value("NEGLECT", fiction::charge_distribution_history::NEGLECT,
               DOC(fiction_charge_distribution_history_NEGLECT));

    detail::charge_distribution_surface_layout<py_sidb_111_lattice>(m, "_111");
    detail::charge_distribution_surface_layout<py_sidb_100_lattice>(m, "_100");
    detail::charge_distribution_surface_layout<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
