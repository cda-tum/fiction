//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
#define PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/print_layout.hpp>
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
    using namespace pybind11::literals;

    m.def(
        "charge_state_to_sign", [](fiction::sidb_charge_state& cs) { return fiction::charge_state_to_sign(cs); },
        "cs"_a, DOC(fiction_charge_state_to_sign));
    m.def(
        "sign_to_charge_state", [](int8_t sg) { return fiction::sign_to_charge_state(sg); }, "sg"_a,
        DOC(fiction_sign_to_charge_state));
    m.def(
        "charge_configuration_to_string",
        [](std::vector<fiction::sidb_charge_state> charge_distribution)
        { return fiction::charge_configuration_to_string(charge_distribution); },
        "charge_distribution"_a, DOC(fiction_charge_configuration_to_string));

    using py_cds = py_charge_distribution_surface_layout<Lyt>;

    /**
     * Charge distribution surface.
     */

    py::class_<py_cds, Lyt>(m, fmt::format("charge_distribution_surface{}", lattice).c_str(),
                            DOC(fiction_charge_distribution_surface), py::module_local())
        .def(py::init<const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             "params"_a = fiction::sidb_simulation_parameters{}, "cs"_a = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const Lyt&, const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             "lyt"_a, "params"_a = fiction::sidb_simulation_parameters{}, "cs"_a = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const py_cds&>(), "lyt"_a)

        .def("get_all_sidb_locations_in_nm", [](py_cds& cds) { return cds.get_all_sidb_locations_in_nm(); })
        .def(
            "assign_cell_type",
            [](py_cds& cds, fiction::cell<py_cds> c, typename py_cds::cell_type ct)
            { return cds.assign_cell_type(c, ct); },
            "c"_a, "ct"_a)
        .def(
            "assign_physical_parameters",
            [](py_cds& cds, fiction::sidb_simulation_parameters params)
            { return cds.assign_physical_parameters(params); },
            "params"_a)
        .def("get_phys_params", [](py_cds& cds) { return cds.get_simulation_params(); })
        .def(
            "charge_exists", [](py_cds& cds, fiction::sidb_charge_state cs) { return cds.charge_exists(cs); }, "cs"_a)
        .def(
            "cell_to_index", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.cell_to_index(c); }, "c"_a)
        .def(
            "assign_charge_state",
            [](py_cds& cds, fiction::cell<py_cds> c, fiction::sidb_charge_state cs, bool update_charge_index = true)
            { return cds.assign_charge_state(c, cs, update_charge_index); },
            "c"_a, "cs"_a, "update_charge_index"_a = true)
        .def(
            "assign_charge_by_cell_index",
            [](py_cds& cds, uint64_t i, fiction::sidb_charge_state cs)
            { return cds.assign_charge_by_cell_index(i, cs); },
            "i"_a, "cs"_a)
        .def(
            "assign_all_charge_states",
            [](py_cds& cds, fiction::sidb_charge_state cs) { return cds.assign_all_charge_states(cs); }, "cs"_a)
        .def(
            "assign_dependent_cell",
            [](py_cds& cds, fiction::cell<py_cds> dependent_cell) { return cds.assign_dependent_cell(dependent_cell); },
            "dependent_cell"_a)
        .def(
            "assign_base_number", [](py_cds& cds, uint8_t base) { return cds.assign_base_number(base); }, "base"_a)
        .def(
            "add_sidb_defect_to_potential_landscape",
            [](py_cds& cds, fiction::cell<py_cds> c, fiction::sidb_defect defect)
            { return cds.add_sidb_defect_to_potential_landscape(c, defect); },
            "c"_a, "defect"_a)
        .def(
            "erase_defect", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.erase_defect(c); }, "c"_a)

        .def(
            "assign_charge_state_by_cell_index",
            [](py_cds& cds, uint64_t index, fiction::sidb_charge_state cs, bool update_charge_configuration = true)
            { return cds.assign_charge_state_by_cell_index(index, cs, update_charge_configuration); },
            "index"_a, "cs"_a, "update_charge_configuration"_a = true)
        .def(
            "get_charge_state", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.get_charge_state(c); }, "c"_a)
        .def(
            "get_charge_state_by_index",
            [](py_cds& cds, uint64_t index) { return cds.get_charge_state_by_index(index); }, "index"_a)
        .def("get_all_sidb_charges", [](py_cds& cds) { return cds.get_all_sidb_charges(); })

        .def("negative_sidb_detection", [](py_cds& cds) { return cds.negative_sidb_detection(); })
        .def(
            "get_nm_distance_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_nm_distance_between_sidbs(c1, c2); },
            "c1"_a, "c2"_a)
        .def(
            "get_nm_distance_by_indices",
            [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.get_nm_distance_by_indices(index1, index2); },
            "index1"_a, "index2"_a)
        .def(
            "calculate_chargeless_potential_between_sidbs_by_index",
            [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.calculate_chargeless_potential_between_sidbs_by_index(index1, index2); },
            "index1"_a, "index2"_a)
        .def(
            "calculate_chargeless_potential_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.calculate_chargeless_potential_between_sidbs(c1, c2); },
            "c1"_a, "c2"_a)
        .def(
            "get_chargeless_potential_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_chargeless_potential_between_sidbs(c1, c2); },
            "c1"_a, "c2"_a)
        .def(
            "get_chargless_potential_by_indices",
            [](py_cds& cds, uint64_t index1, uint64_t index2)
            { return cds.get_chargeless_potential_by_indices(index1, index2); },
            "index1"_a, "index2"_a)
        .def(
            "get_potential_between_sidbs",
            [](py_cds& cds, fiction::cell<py_cds> c1, fiction::cell<py_cds> c2)
            { return cds.get_potential_between_sidbs(c1, c2); },
            "c1"_a, "c2"_a)
        .def(
            "update_local_potential",
            [](py_cds& cds, fiction::charge_distribution_history history_mode)
            { return cds.update_local_potential(history_mode); },
            "consider_history"_a)
        .def(
            "get_local_potential", [](py_cds& cds, fiction::cell<py_cds> c) { return cds.get_local_potential(c); },
            "c"_a)
        .def(
            "get_local_potential_by_index",
            [](py_cds& cds, uint64_t index) { return cds.get_local_potential_by_index(index); }, "index"_a)
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
               fiction::charge_distribution_history consider_history, fiction::exhaustive_sidb_simulation_engine engine)
            {
                return cds.increase_charge_index_by_one(dependent_cell_fixed, recompute_system_energy, consider_history,
                                                        engine);
            },
            "dependent_cell_fixed"_a    = fiction::dependent_cell_mode::FIXED,
            "recompute_system_energy"_a = fiction::energy_calculation::UPDATE_ENERGY,
            "consider_history"_a        = fiction::charge_distribution_history::NEGLECT,
            "engine"_a                  = fiction::exhaustive_sidb_simulation_engine::EXGS)

        .def("get_max_charge_index", &py_cds::get_max_charge_index)
        .def("assign_charge_index", &py_cds::assign_charge_index, "charge_index"_a)
        .def("adjacent_search", &py_cds::adjacent_search, "alpha"_a, "negative_indices"_a)
        .def("assign_global_external_potential", &py_cds::assign_global_external_potential, "potential_value"_a,
             "dependent_cell"_a = fiction::dependent_cell_mode::FIXED)
        .def("is_three_state_simulation_required", &py_cds::is_three_state_simulation_required)
        .def("get_positive_candidates", &py_cds::get_positive_candidates)
        .def("three_state_cell_to_index", &py_cds::three_state_cell_to_index, "c"_a)
        .def("two_state_cell_to_index", &py_cds::two_state_cell_to_index, "c"_a)
        .def("index_to_cell", &py_cds::index_to_cell, "index"_a)
        .def("index_to_three_state_cell", &py_cds::index_to_three_state_cell, "index"_a)
        .def("index_to_two_state_cell", &py_cds::index_to_two_state_cell, "index"_a)

        .def("chargeless_potential_at_given_distance", &py_cds::chargeless_potential_at_given_distance, "distance"_a)
        .def("chargeless_potential_generated_by_defect_at_given_distance",
             &py_cds::chargeless_potential_generated_by_defect_at_given_distance, "distance"_a,
             "defect"_a = fiction::sidb_defect{})
        .def("assign_local_external_potential", &py_cds::assign_local_external_potential, "external_potential"_a)
        .def("get_local_external_potentials", &py_cds::get_local_external_potentials)
        .def("get_local_defect_potentials", &py_cds::get_local_defect_potentials)
        .def("get_defects", &py_cds::get_defects)
        //
        .def("update_charge_state_of_dependent_cell", &py_cds::update_charge_state_of_dependent_cell)
        .def("get_charge_index_of_sub_layout", &py_cds::get_charge_index_of_sub_layout)
        .def("charge_index_gray_code_to_charge_distribution", &py_cds::charge_index_gray_code_to_charge_distribution,
             "new_gray_code"_a, "old_gray_code"_a)

        .def("increase_charge_index_of_sub_layout_by_one", &py_cds::increase_charge_index_of_sub_layout_by_one,
             "dependent_cell_fixed"_a    = fiction::dependent_cell_mode::FIXED,
             "recompute_system_energy"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "consider_history"_a        = fiction::charge_distribution_history::NEGLECT,
             "engine"_a                  = fiction::exhaustive_sidb_simulation_engine::EXGS)
        .def("assign_charge_index_by_gray_code", &py_cds::assign_charge_index_by_gray_code, "current_gray_code"_a,
             "previous_gray_code"_a, "dependent_cell"_a = fiction::dependent_cell_mode::FIXED,
             "energy_calc_mode"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "history_mode"_a     = fiction::charge_distribution_history::NEGLECT)
        .def("reset_charge_index_sub_layout", &py_cds::reset_charge_index_sub_layout)
        .def("get_max_charge_index_sub_layout", &py_cds::get_max_charge_index_sub_layout)
        .def("assign_charge_index_by_two_gray_codes", &py_cds::assign_charge_index_by_two_gray_codes, "gray_code"_a,
             "gray_code_old"_a)
        .def("get_sidb_order", &py_cds::get_sidb_order)
        .def("add_sidb", &py_cds::add_sidb, "cell"_a, "charge"_a)

        .def("cells",
             [](const py_cds& lyt)
             {
                 std::vector<fiction::coordinate<py_cds>> cells{};
                 cells.reserve(lyt.num_cells());
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const py_cds& lyt)
             {
                 std::vector<fiction::coordinate<py_cds>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const py_cds& lyt)
             {
                 std::vector<fiction::coordinate<py_cds>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })

        .def(
            "is_within_bounds",
            [](const py_cds& lyt, const fiction::coordinate<py_cds>& c) { return lyt.is_within_bounds(c); }, "c"_a,
            DOC(fiction_cartesian_layout_is_within_bounds))

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
                 print_layout(fiction::convert_to_siqad_coordinates(lyt), ss);
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
    //
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
