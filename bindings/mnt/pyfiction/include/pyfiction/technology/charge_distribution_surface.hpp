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
void charge_distribution_surface_layout(pybind11::module& m, const std::string& lattice, const std::string& coord_type)
{
    namespace py = pybind11;

    using py_cds = py_charge_distribution_surface_layout<Lyt>;

    /**
     * Charge distribution surface.
     */

    py::class_<py_cds, Lyt>(m, fmt::format("charge_distribution_surface{}_{}", lattice, coord_type).c_str(),
                            DOC(fiction_charge_distribution_surface), py::module_local())
        .def(py::init<const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             py::arg("params") = fiction::sidb_simulation_parameters{},
             py::arg("cs")     = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const Lyt&, const fiction::sidb_simulation_parameters&, const fiction::sidb_charge_state&>(),
             py::arg("lyt"), py::arg("params") = fiction::sidb_simulation_parameters{},
             py::arg("cs") = fiction::sidb_charge_state::NEGATIVE)

        .def(py::init<const py_cds&>(), py::arg("lyt"))

        .def("determine_effective_charge_transition_thresholds",
             &py_cds::determine_effective_charge_transition_thresholds)
        .def("get_effective_charge_transition_thresholds", &py_cds::get_effective_charge_transition_thresholds,
             py::arg("index"))
        .def("get_all_sidb_locations_in_nm", &py_cds::get_all_sidb_locations_in_nm)
        .def("assign_cell_type", &py_cds::assign_cell_type, py::arg("c"), py::arg("ct"))
        .def("assign_physical_parameters", &py_cds::assign_physical_parameters, py::arg("params"))
        .def("get_phys_params", &py_cds::get_simulation_params)
        .def("charge_exists", &py_cds::charge_exists, py::arg("cs"))
        .def("cell_to_index", &py_cds::cell_to_index, py::arg("c"))
        .def("assign_charge_state", &py_cds::assign_charge_state, py::arg("c"), py::arg("cs"),
             py::arg("index_mode") = fiction::charge_index_mode::UPDATE_CHARGE_INDEX)
        .def("assign_all_charge_states", &py_cds::assign_all_charge_states, py::arg("cs"),
             py::arg("index_mode") = fiction::charge_index_mode::UPDATE_CHARGE_INDEX)
        .def("assign_dependent_cell", &py_cds::assign_dependent_cell, py::arg("dependent_cell"))
        .def("assign_base_number", &py_cds::assign_base_number, py::arg("base"))
        .def("add_sidb_defect_to_potential_landscape", &py_cds::add_sidb_defect_to_potential_landscape, py::arg("c"),
             py::arg("defect"))
        .def("erase_defect", &py_cds::erase_defect, py::arg("c"))
        .def("assign_charge_state_by_index", &py_cds::assign_charge_state_by_index, py::arg("index"), py::arg("cs"),
             py::arg("index_mode") = fiction::charge_index_mode::UPDATE_CHARGE_INDEX)
        .def("get_charge_state", &py_cds::get_charge_state, py::arg("c"))
        .def("get_charge_state_by_index", &py_cds::get_charge_state_by_index, py::arg("index"))
        .def("get_all_sidb_charges", &py_cds::get_all_sidb_charges)
        .def("negative_sidb_detection", &py_cds::negative_sidb_detection)
        .def("get_nm_distance_between_sidbs", &py_cds::get_nm_distance_between_sidbs, py::arg("c1"), py::arg("c2"))
        .def("get_nm_distance_by_indices", &py_cds::get_nm_distance_by_indices, py::arg("index1"), py::arg("index2"))
        .def("calculate_chargeless_potential_between_sidbs_by_index",
             &py_cds::calculate_chargeless_potential_between_sidbs_by_index, py::arg("index1"), py::arg("index2"))
        .def("calculate_chargeless_potential_between_sidbs", &py_cds::calculate_chargeless_potential_between_sidbs,
             py::arg("c1"), py::arg("c2"))
        .def("get_chargeless_potential_between_sidbs", &py_cds::get_chargeless_potential_between_sidbs, py::arg("c1"),
             py::arg("c2"))
        .def("get_chargeless_potential_by_indices", &py_cds::get_chargeless_potential_by_indices, py::arg("index1"),
             py::arg("index2"))
        .def("get_potential_between_sidbs", &py_cds::get_potential_between_sidbs, py::arg("c1"), py::arg("c2"))
        .def("update_local_internal_potential", &py_cds::update_local_internal_potential, py::arg("consider_history"))
        .def("get_local_potential", &py_cds::get_local_potential, py::arg("c"))
        .def("get_local_potential_by_index", &py_cds::get_local_potential_by_index, py::arg("index"))
        .def("get_local_internal_potential", &py_cds::get_local_internal_potential, py::arg("c"))
        .def("get_local_internal_potential_by_index", &py_cds::get_local_internal_potential_by_index, py::arg("index"))
        .def("get_local_external_potential", &py_cds::get_local_external_potential, py::arg("c"))
        .def("get_local_external_potential_by_index", &py_cds::get_local_external_potential_by_index, py::arg("index"))
        .def("get_local_potential_caused_by_defects", &py_cds::get_local_potential_caused_by_defects, py::arg("c"))
        .def("get_local_potential_caused_by_defects_by_index", &py_cds::get_local_potential_caused_by_defects_by_index,
             py::arg("index"))
        .def("assign_electrostatic_potential_energy_to_zero", &py_cds::assign_electrostatic_potential_energy_to_zero)
        .def("recompute_electrostatic_potential_energy", &py_cds::recompute_electrostatic_potential_energy)
        .def("get_electrostatic_potential_energy", &py_cds::get_electrostatic_potential_energy)
        .def("update_after_charge_change", &py_cds::update_after_charge_change,
             py::arg("dep_cell")                = fiction::dependent_cell_mode::FIXED,
             py::arg("energy_calculation_mode") = fiction::energy_calculation::UPDATE_ENERGY,
             py::arg("history_mode")            = fiction::charge_distribution_history::NEGLECT)
        .def("validity_check", &py_cds::validity_check)
        .def("is_physically_valid", &py_cds::is_physically_valid)
        .def("charge_distribution_to_index_general", &py_cds::charge_distribution_to_index_general)
        .def("charge_distribution_to_index", &py_cds::charge_distribution_to_index)
        .def("get_charge_index_and_base", &py_cds::get_charge_index_and_base)
        .def("increase_charge_index_by_one", &py_cds::increase_charge_index_by_one,
             py::arg("dependent_cell_fixed")    = fiction::dependent_cell_mode::FIXED,
             py::arg("recompute_system_energy") = fiction::energy_calculation::UPDATE_ENERGY,
             py::arg("consider_history")        = fiction::charge_distribution_history::NEGLECT)
        .def("get_max_charge_index", &py_cds::get_max_charge_index)
        .def("assign_charge_index", &py_cds::assign_charge_index, py::arg("charge_index"), py::arg("cdc"))
        .def("adjacent_search", &py_cds::adjacent_search, py::arg("alpha"), py::arg("negative_indices"))
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
        .def("assign_global_external_potential", &py_cds::assign_global_external_potential, py::arg("potential_value"))
        .def("assign_local_external_potential", &py_cds::assign_local_external_potential, py::arg("external_potential"))
        .def("get_local_external_potential_map", &py_cds::get_local_external_potential_map)
        .def("reset_local_external_potential", &py_cds::reset_local_external_potentials)
        .def("get_local_defect_potentials", &py_cds::get_local_defect_potentials)
        .def("get_defects", &py_cds::get_defects)
        .def("update_charge_state_of_dependent_cell", &py_cds::update_charge_state_of_dependent_cell)
        .def("get_charge_index_of_sub_layout", &py_cds::get_charge_index_of_sub_layout)
        .def("charge_index_gray_code_to_charge_distribution", &py_cds::charge_index_gray_code_to_charge_distribution,
             py::arg("new_gray_code"), py::arg("old_gray_code"))
        .def("increase_charge_index_of_sub_layout_by_one", &py_cds::increase_charge_index_of_sub_layout_by_one,
             py::arg("dependent_cell_fixed")    = fiction::dependent_cell_mode::FIXED,
             py::arg("recompute_system_energy") = fiction::energy_calculation::UPDATE_ENERGY,
             py::arg("consider_history")        = fiction::charge_distribution_history::NEGLECT)
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
        .def("num_positive_sidbs", [](const py_cds& lyt) { return lyt.num_positive_sidbs(); })
        .def("num_negative_sidbs", [](const py_cds& lyt) { return lyt.num_negative_sidbs(); })
        .def("num_neutral_sidbs", [](const py_cds& lyt) { return lyt.num_neutral_sidbs(); })
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
                 print_layout(fiction::convert_layout_to_siqad_coordinates(lyt), ss);
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

    /**
     * Charge transition threshold bounds.
     */
    py::enum_<fiction::charge_transition_threshold_bounds>(
        m, "charge_transition_threshold_bounds", DOC(fiction_charge_transition_threshold_bounds), py::module_local())
        .value("NEGATIVE_UPPER_BOUND", fiction::charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND,
               DOC(fiction_charge_transition_threshold_bounds_NEGATIVE_UPPER_BOUND))
        .value("POSITIVE_LOWER_BOUND", fiction::charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND,
               DOC(fiction_charge_transition_threshold_bounds_POSITIVE_LOWER_BOUND))
        .value("NEUTRAL_LOWER_BOUND", fiction::charge_transition_threshold_bounds::NEUTRAL_LOWER_BOUND,
               DOC(fiction_charge_transition_threshold_bounds_NEUTRAL_LOWER_BOUND))
        .value("NEUTRAL_UPPER_BOUND", fiction::charge_transition_threshold_bounds::NEUTRAL_UPPER_BOUND,
               DOC(fiction_charge_transition_threshold_bounds_NEUTRAL_UPPER_BOUND));

    // offset coords
    detail::charge_distribution_surface_layout<py_sidb_111_lattice<py_offset_coordinate>>(m, "_111",
                                                                                          "offset_coordinates");
    detail::charge_distribution_surface_layout<py_sidb_100_lattice<py_offset_coordinate>>(m, "_100",
                                                                                          "offset_coordinates");
    detail::charge_distribution_surface_layout<py_sidb_layout<py_offset_coordinate>>(m, "", "offset_coordinates");

    // cube coords
    detail::charge_distribution_surface_layout<py_sidb_111_lattice<py_cube_coordinate>>(m, "_111", "cube_coordinate");
    detail::charge_distribution_surface_layout<py_sidb_100_lattice<py_cube_coordinate>>(m, "_100", "cube_coordinate");
    detail::charge_distribution_surface_layout<py_sidb_layout<py_cube_coordinate>>(m, "", "cube_coordinate");
}

/**
 *  A factory function for automatically wrapping an existing layout in a charge_distribution_surface.
 *
 *  Example usage (Python):
 *     layout_one = sidb_layout((10, 10))  # offset by default
 *     cds = charge_distribution_surface(layout_one)
 *
 *  This tries to dynamically cast the given `layout` to each recognized type, builds
 *  a `charge_distribution_surface_<orientation>_<coordinate_type>`, and returns it.
 */
inline void charge_distribution_surface_factory(pybind11::module& m)
{
    namespace py = pybind11;

    // -------------------------------------------------------------------------
    // 1) Overload for NO arguments: creates a default-constructed
    //    `charge_distribution_surface_layout` using some chosen underlying layout type
    //    (`py_sidb_layout<py_offset_coordinate>`).
    // -------------------------------------------------------------------------
    m.def(
        "charge_distribution_surface",
        [](const fiction::sidb_simulation_parameters& params, fiction::sidb_charge_state cs)
        {
            using default_layout_type = py_sidb_layout<py_offset_coordinate>;
            using default_cds_type    = py_charge_distribution_surface_layout<default_layout_type>;
            return default_cds_type{params, cs};
        },
        py::arg("params") = fiction::sidb_simulation_parameters{}, py::arg("cs") = fiction::sidb_charge_state::NEGATIVE,
        R"doc(
            Default constructor-like overload of `charge_distribution_surface`.

            Creates a charge_distribution_surface with a default underlying SiDB layout
            (generic py_sidb_layout in offset coordinates), default simulation parameters,
            and negative initial SiDB charge state.
        )doc");

    // -------------------------------------------------------------------------
    // 2) Overload that takes a `layout` argument and deduces the specialized type.
    // -------------------------------------------------------------------------
    m.def(
        "charge_distribution_surface",
        [](py::object layout, const fiction::sidb_simulation_parameters& params, fiction::sidb_charge_state cs)
        {
            // 1. Attempt to cast to each known layout type (100/111 offset/cube, or generic sidb_layout)
            //    If it matches, wrap that layout in a corresponding py_charge_distribution_surface_layout and return.

            // --- 100 offset ---
            if (py::isinstance<py_sidb_100_lattice<py_offset_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_100_lattice<py_offset_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_100_lattice<py_offset_coordinate>>(l, params, cs));
            }
            // --- 111 offset ---
            if (py::isinstance<py_sidb_111_lattice<py_offset_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_111_lattice<py_offset_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_111_lattice<py_offset_coordinate>>(l, params, cs));
            }
            // --- generic sidb_layout offset ---
            if (py::isinstance<py_sidb_layout<py_offset_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_layout<py_offset_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_layout<py_offset_coordinate>>(l, params, cs));
            }

            // --- 100 cube ---
            if (py::isinstance<py_sidb_100_lattice<py_cube_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_100_lattice<py_cube_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_100_lattice<py_cube_coordinate>>(l, params, cs));
            }
            // --- 111 cube ---
            if (py::isinstance<py_sidb_111_lattice<py_cube_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_111_lattice<py_cube_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_111_lattice<py_cube_coordinate>>(l, params, cs));
            }
            // --- generic sidb_layout cube ---
            if (py::isinstance<py_sidb_layout<py_cube_coordinate>>(layout))
            {
                auto& l = layout.cast<py_sidb_layout<py_cube_coordinate>&>();
                return py::cast(
                    py_charge_distribution_surface_layout<py_sidb_layout<py_cube_coordinate>>(l, params, cs));
            }

            // If none matched, raise an exception
            throw std::invalid_argument(
                "[pyfiction] Provided layout is not recognized as a supported SiDB layout type. "
                "Perhaps an unsupported orientation or coordinate system?");
        },
        py::arg("layout"), py::arg("params") = fiction::sidb_simulation_parameters{},
        py::arg("cs") = fiction::sidb_charge_state::NEGATIVE,
        R"doc(
            Wraps an existing SiDB layout in a charge_distribution_surface, deducing its
            orientation and coordinate type automatically.

            :param layout:
                A sidb_layout, sidb_100_lattice, or sidb_111_lattice object in either
                offset or cube coordinates.
            :param params:
                Optional sidb_simulation_parameters to assign (default constructed if not given).
            :param cs:
                Optional initial SiDB charge state for the newly created surface
                (defaults to sidb_charge_state.NEGATIVE).

            Example usage::

                from pyfiction import sidb_layout, charge_distribution_surface
                lay = sidb_layout((10, 10))
                cds = charge_distribution_surface(lay)
                print(cds.get_system_energy())

        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
