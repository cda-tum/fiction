//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_SIQAD_LAYOUT_HPP
#define PYFICTION_SIQAD_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>

#include <cstdint>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

inline void charge_distribution_surface(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    m.def("charge_state_to_sign", &fiction::charge_state_to_sign, "cs"_a, DOC(fiction_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sign_to_charge_state, "sg"_a, DOC(fiction_sign_to_charge_state));
    m.def("charge_configuration_to_string", &fiction::charge_configuration_to_string, "charge_distribution"_a,
          DOC(fiction_charge_configuration_to_string));

    /**
     * Dependent cell mode.
     */
    py::enum_<fiction::dependent_cell_mode>(m, "dependent_cell_mode", DOC(fiction_dependent_cell_mode))
        .value("FIXED", fiction::dependent_cell_mode::FIXED, DOC(fiction_dependent_cell_mode_FIXED))
        .value("VARIABLE", fiction::dependent_cell_mode::VARIABLE, DOC(fiction_dependent_cell_mode_VARIABLE))

        ;

    /**
     * Energy calculation.
     */
    py::enum_<fiction::energy_calculation>(m, "energy_calculation", DOC(fiction_energy_calculation))
        .value("KEEP_OLD_ENERGY_VALUE", fiction::energy_calculation::KEEP_OLD_ENERGY_VALUE,
               DOC(fiction_energy_calculation_KEEP_OLD_ENERGY_VALUE))
        .value("UPDATE_ENERGY", fiction::energy_calculation::UPDATE_ENERGY,
               DOC(fiction_energy_calculation_UPDATE_ENERGY))

        ;

    /**
     * Charge distribution history.
     */
    py::enum_<fiction::charge_distribution_history>(m, "charge_distribution_history",
                                                    DOC(fiction_charge_distribution_history))
        .value("CONSIDER", fiction::charge_distribution_history::CONSIDER,
               DOC(fiction_charge_distribution_history_CONSIDER))
        .value("NEGLECT", fiction::charge_distribution_history::NEGLECT,
               DOC(fiction_charge_distribution_history_NEGLECT))

        ;

    /**
     * Charge distribution surface.
     *
     * @note The charge_distribution_type in pyfiction must be viewed as a stand-alone type. It does not inherit from
     * any other existing pyfiction types. Therefore, it does not implement functions of lower level layout types.
     */
    py::class_<py_charge_distribution_surface>(m, "charge_distribution_surface",
                                               DOC(fiction_charge_distribution_surface))
        .def(py::init<const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             "params"_a = fiction::sidb_simulation_parameters{}, "cs"_a = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const py_cds_base&, const fiction::sidb_simulation_parameters&,
                      const fiction::sidb_charge_state&>(),
             "lyt"_a, "params"_a = fiction::sidb_simulation_parameters{}, "cs"_a = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const py_charge_distribution_surface&>(), "lyt"_a)

        .def(py::init(
                 [](const py_sidb_layout&                      layout,
                    const fiction::sidb_simulation_parameters& params = fiction::sidb_simulation_parameters{},
                    const fiction::sidb_charge_state&          cs     = fiction::sidb_charge_state::NEGATIVE)
                 {
                     auto converted_layout = fiction::convert_to_siqad_coordinates(layout);
                     return fiction::charge_distribution_surface(converted_layout, params, cs);
                 }),
             "layout"_a, "params"_a = fiction::sidb_simulation_parameters{},
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

        .def(
            "get_cell_type",
            [](const py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c)
            { return lyt.get_cell_type(c); },
            "c"_a, DOC(fiction_cell_level_layout_get_cell_type))
        .def(
            "is_empty_cell",
            [](const py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c)
            { return lyt.is_empty_cell(c); },
            "c"_a, DOC(fiction_cell_level_layout_is_empty_cell))
        .def(
            "assign_cell_name",
            [](py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c,
               const std::string& n) { lyt.assign_cell_name(c, n); },
            "c"_a, "n"_a, DOC(fiction_cell_level_layout_assign_cell_name))
        .def(
            "get_cell_name",
            [](const py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c)
            { return lyt.get_cell_name(c); },
            "c"_a, DOC(fiction_cell_level_layout_get_cell_name))
        .def(
            "set_layout_name",
            [](py_charge_distribution_surface& lyt, const std::string& name) { lyt.set_layout_name(name); }, "name"_a,
            DOC(fiction_cell_level_layout_set_layout_name))
        .def(
            "get_layout_name", [](const py_charge_distribution_surface& lyt) { return lyt.get_layout_name(); },
            DOC(fiction_cell_level_layout_get_layout_name))
        .def(
            "num_cells", [](const py_charge_distribution_surface& lyt) { return lyt.num_cells(); },
            DOC(fiction_cell_level_layout_num_cells))
        .def(
            "is_empty", [](const py_charge_distribution_surface& lyt) { return lyt.is_empty(); },
            DOC(fiction_cell_level_layout_is_empty))
        .def(
            "num_pis", [](const py_charge_distribution_surface& lyt) { return lyt.num_pis(); },
            DOC(fiction_cell_level_layout_num_pis))
        .def(
            "num_pos", [](const py_charge_distribution_surface& lyt) { return lyt.num_pos(); },
            DOC(fiction_cell_level_layout_num_pos))
        .def(
            "is_pi",
            [](const py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c)
            { return lyt.is_pi(c); },
            "c"_a, DOC(fiction_cell_level_layout_is_pi))
        .def(
            "is_po",
            [](const py_charge_distribution_surface& lyt, const fiction::cell<py_charge_distribution_surface>& c)
            { return lyt.is_po(c); },
            "c"_a, DOC(fiction_cell_level_layout_is_po))

        .def("get_all_sidb_locations_in_nm", &py_charge_distribution_surface::get_all_sidb_locations_in_nm)
        .def("assign_cell_type", &py_charge_distribution_surface::assign_cell_type, "c"_a, "ct"_a)
        .def("assign_physical_parameters", &py_charge_distribution_surface::assign_physical_parameters, "params"_a)
        .def("get_phys_params", &py_charge_distribution_surface::get_phys_params)
        .def("charge_exists", &py_charge_distribution_surface::charge_exists, "cs"_a)
        .def("cell_to_index", &py_charge_distribution_surface::cell_to_index, "c"_a)
        .def("assign_charge_state", &py_charge_distribution_surface::assign_charge_state, "c"_a, "cs"_a,
             "update_charge_index"_a = true)
        .def("assign_charge_by_cell_index", &py_charge_distribution_surface::assign_charge_by_cell_index, "i"_a, "cs"_a)
        .def("assign_all_charge_states", &py_charge_distribution_surface::assign_all_charge_states, "cs"_a)
        .def("assign_dependent_cell", &py_charge_distribution_surface::assign_dependent_cell, "dependent_cell"_a)
        .def("assign_base_number", &py_charge_distribution_surface::assign_base_number, "base"_a)
        .def("add_sidb_defect_to_potential_landscape",
             &py_charge_distribution_surface::add_sidb_defect_to_potential_landscape, "c"_a, "defect"_a)
        .def("erase_defect", &py_charge_distribution_surface::erase_defect, "c"_a)
        .def("assign_charge_state_by_cell_index", &py_charge_distribution_surface::assign_charge_state_by_cell_index,
             "index"_a, "cs"_a, "update_charge_configuration"_a = true)
        .def("get_charge_state", &py_charge_distribution_surface::get_charge_state, "c"_a)
        .def("get_charge_state_by_index", &py_charge_distribution_surface::get_charge_state_by_index, "index"_a)
        .def("get_all_sidb_charges", &py_charge_distribution_surface::get_all_sidb_charges)
        .def("negative_sidb_detection", &py_charge_distribution_surface::negative_sidb_detection)
        .def("get_nm_distance_between_sidbs", &py_charge_distribution_surface::get_nm_distance_between_sidbs, "c1"_a,
             "c2"_a)
        .def("get_nm_distance_by_indices", &py_charge_distribution_surface::get_nm_distance_by_indices, "index1"_a,
             "index2"_a)
        .def("calculate_chargeless_potential_between_sidbs_by_index",
             &py_charge_distribution_surface::calculate_chargeless_potential_between_sidbs_by_index, "index1"_a,
             "index2"_a)
        .def("calculate_chargeless_potential_between_sidbs",
             &py_charge_distribution_surface::calculate_chargeless_potential_between_sidbs, "c1"_a, "c2"_a)
        .def("get_chargeless_potential_between_sidbs",
             &py_charge_distribution_surface::get_chargeless_potential_between_sidbs, "c1"_a, "c2"_a)
        .def("get_chargless_potential_by_indices", &py_charge_distribution_surface::get_chargless_potential_by_indices,
             "index1"_a, "index2"_a)
        .def("get_potential_between_sidbs", &py_charge_distribution_surface::get_potential_between_sidbs, "c1"_a,
             "c2"_a)
        .def("get_potential_by_indices", &py_charge_distribution_surface::get_potential_by_indices, "index1"_a,
             "index2"_a)
        .def("update_local_potential", &py_charge_distribution_surface::update_local_potential)
        .def("get_local_potential", &py_charge_distribution_surface::get_local_potential, "c"_a)
        .def("get_local_potential_by_index", &py_charge_distribution_surface::get_local_potential_by_index, "index"_a)
        .def("assign_system_energy_to_zero", &py_charge_distribution_surface::assign_system_energy_to_zero)
        .def("recompute_system_energy", &py_charge_distribution_surface::recompute_system_energy)
        .def("get_system_energy", &py_charge_distribution_surface::get_system_energy)
        .def("update_after_charge_change", &py_charge_distribution_surface::update_after_charge_change,
             "dependent_cell"_a          = fiction::dependent_cell_mode::FIXED,
             "energy_calculation_mode"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "history_mode"_a            = fiction::charge_distribution_history::NEGLECT)
        .def("validity_check", &py_charge_distribution_surface::validity_check)
        .def("is_physically_valid", &py_charge_distribution_surface::is_physically_valid)
        .def("charge_distribution_to_index_general",
             &py_charge_distribution_surface::charge_distribution_to_index_general)
        .def("charge_distribution_to_index", &py_charge_distribution_surface::charge_distribution_to_index)
        .def("get_charge_index_and_base", &py_charge_distribution_surface::get_charge_index_and_base)
        .def("increase_charge_index_by_one", &py_charge_distribution_surface::increase_charge_index_by_one,
             "dependent_cell_fixed"_a    = fiction::dependent_cell_mode::FIXED,
             "recompute_system_energy"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "consider_history"_a        = fiction::charge_distribution_history::NEGLECT,
             "engine"_a                  = fiction::exhaustive_sidb_simulation_engine::EXGS)

        .def("get_max_charge_index", &py_charge_distribution_surface::get_max_charge_index)
        .def("assign_charge_index", &py_charge_distribution_surface::assign_charge_index, "charge_index"_a)
        .def("adjacent_search", &py_charge_distribution_surface::adjacent_search, "alpha"_a, "negative_indices"_a)
        .def("assign_global_external_potential", &py_charge_distribution_surface::assign_global_external_potential,
             "potential_value"_a, "dependent_cell"_a = fiction::dependent_cell_mode::FIXED)
        .def("is_three_state_simulation_required", &py_charge_distribution_surface::is_three_state_simulation_required)
        .def("get_positive_candidates", &py_charge_distribution_surface::get_positive_candidates)
        .def("three_state_cell_to_index", &py_charge_distribution_surface::three_state_cell_to_index, "c"_a)
        .def("two_state_cell_to_index", &py_charge_distribution_surface::two_state_cell_to_index, "c"_a)
        .def("index_to_cell", &py_charge_distribution_surface::index_to_cell, "index"_a)
        .def("index_to_three_state_cell", &py_charge_distribution_surface::index_to_three_state_cell, "index"_a)
        .def("index_to_two_state_cell", &py_charge_distribution_surface::index_to_two_state_cell, "index"_a)

        .def("chargeless_potential_at_given_distance",
             &py_charge_distribution_surface::chargeless_potential_at_given_distance, "distance"_a)
        .def("chargeless_potential_generated_by_defect_at_given_distance",
             &py_charge_distribution_surface::chargeless_potential_generated_by_defect_at_given_distance, "distance"_a,
             "defect"_a = fiction::sidb_defect{})
        .def("assign_local_external_potential", &py_charge_distribution_surface::assign_local_external_potential,
             "external_potential"_a)
        .def("get_local_external_potentials", &py_charge_distribution_surface::get_local_external_potentials)
        .def("get_local_defect_potentials", &py_charge_distribution_surface::get_local_defect_potentials)
        .def("get_defects", &py_charge_distribution_surface::get_defects)

        .def("update_charge_state_of_dependent_cell",
             &py_charge_distribution_surface::update_charge_state_of_dependent_cell)
        .def("get_charge_index_of_sub_layout", &py_charge_distribution_surface::get_charge_index_of_sub_layout)
        .def("charge_index_gray_code_to_charge_distribution",
             &py_charge_distribution_surface::charge_index_gray_code_to_charge_distribution, "new_gray_code"_a,
             "old_gray_code"_a)

        .def("increase_charge_index_of_sub_layout_by_one",
             &py_charge_distribution_surface::increase_charge_index_of_sub_layout_by_one,
             "dependent_cell_fixed"_a    = fiction::dependent_cell_mode::FIXED,
             "recompute_system_energy"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "consider_history"_a        = fiction::charge_distribution_history::NEGLECT,
             "engine"_a                  = fiction::exhaustive_sidb_simulation_engine::EXGS)
        .def("assign_charge_index_by_gray_code", &py_charge_distribution_surface::assign_charge_index_by_gray_code,
             "current_gray_code"_a, "previous_gray_code"_a, "dependent_cell"_a = fiction::dependent_cell_mode::FIXED,
             "energy_calc_mode"_a = fiction::energy_calculation::UPDATE_ENERGY,
             "history_mode"_a     = fiction::charge_distribution_history::NEGLECT)
        .def("reset_charge_index_sub_layout", &py_charge_distribution_surface::reset_charge_index_sub_layout)
        .def("get_max_charge_index_sub_layout", &py_charge_distribution_surface::get_max_charge_index_sub_layout)
        .def("assign_charge_index_by_two_gray_codes",
             &py_charge_distribution_surface::assign_charge_index_by_two_gray_codes, "gray_code"_a, "gray_code_old"_a)
        .def("get_sidb_order", &py_charge_distribution_surface::get_sidb_order)
        .def("add_sidb", &py_charge_distribution_surface::add_sidb, "cell"_a, "charge"_a)

        .def("cells",
             [](const py_charge_distribution_surface& lyt)
             {
                 std::vector<fiction::coordinate<py_charge_distribution_surface>> cells{};
                 cells.reserve(lyt.num_cells());
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const py_charge_distribution_surface& lyt)
             {
                 std::vector<fiction::coordinate<py_charge_distribution_surface>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const py_charge_distribution_surface& lyt)
             {
                 std::vector<fiction::coordinate<py_charge_distribution_surface>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })

        .def(
            "is_within_bounds",
            [](const py_charge_distribution_surface& lyt, const py_siqad_coordinate& c)
            { return lyt.is_within_bounds(c); },
            "c"_a, DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "bounding_box_2d",
            [](const py_charge_distribution_surface& lyt)
            {
                const auto bb = fiction::bounding_box_2d<py_charge_distribution_surface>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))

        .def("__repr__",
             [](const py_charge_distribution_surface& lyt)
             {
                 std::stringstream ss;
                 print_layout(lyt, ss);
                 return ss.str();
             })

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_SIQAD_LAYOUT_HPP
