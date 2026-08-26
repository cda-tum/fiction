#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/technology_mapping.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void technology_mapping_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("technology_mapping", &fiction::synthesis::technology_mapping<Ntk>, py::arg("network"),
          py::arg("params") = fiction::synthesis::technology_mapping_params{}, py::arg("stats") = nullptr,
          DOC(fiction_technology_mapping));
}

}  // namespace detail

void technology_mapping(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::synthesis::missing_required_gates_exception>(
        m, "missing_required_gates_exception",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    py::class_<fiction::synthesis::technology_mapping_params>(m, "technology_mapping_params",
                                                              DOC(fiction_technology_mapping_params))
        .def(py::init<>(), "Default constructor.")

        .def_rw("decay", &fiction::synthesis::technology_mapping_params::decay,
                DOC(fiction_technology_mapping_params_decay))

        .def_rw("inv", &fiction::synthesis::technology_mapping_params::inv, DOC(fiction_technology_mapping_params_inv))

        .def_rw("and2", &fiction::synthesis::technology_mapping_params::and2,
                DOC(fiction_technology_mapping_params_and2))
        .def_rw("nand2", &fiction::synthesis::technology_mapping_params::nand2,
                DOC(fiction_technology_mapping_params_nand2))
        .def_rw("or2", &fiction::synthesis::technology_mapping_params::or2, DOC(fiction_technology_mapping_params_or2))
        .def_rw("nor2", &fiction::synthesis::technology_mapping_params::nor2,
                DOC(fiction_technology_mapping_params_nor2))
        .def_rw("xor2", &fiction::synthesis::technology_mapping_params::xor2,
                DOC(fiction_technology_mapping_params_xor2))
        .def_rw("xnor2", &fiction::synthesis::technology_mapping_params::xnor2,
                DOC(fiction_technology_mapping_params_xnor2))

        .def_rw("and3", &fiction::synthesis::technology_mapping_params::and3,
                DOC(fiction_technology_mapping_params_and3))
        .def_rw("xor_and", &fiction::synthesis::technology_mapping_params::xor_and,
                DOC(fiction_technology_mapping_params_xor_and))
        .def_rw("or_and", &fiction::synthesis::technology_mapping_params::or_and,
                DOC(fiction_technology_mapping_params_or_and))
        .def_rw("onehot", &fiction::synthesis::technology_mapping_params::onehot,
                DOC(fiction_technology_mapping_params_onehot))
        .def_rw("maj3", &fiction::synthesis::technology_mapping_params::maj3,
                DOC(fiction_technology_mapping_params_maj3))
        .def_rw("gamble", &fiction::synthesis::technology_mapping_params::gamble,
                DOC(fiction_technology_mapping_params_gamble))
        .def_rw("dot", &fiction::synthesis::technology_mapping_params::dot, DOC(fiction_technology_mapping_params_dot))
        .def_rw("mux", &fiction::synthesis::technology_mapping_params::mux, DOC(fiction_technology_mapping_params_mux))
        .def_rw("and_xor", &fiction::synthesis::technology_mapping_params::and_xor,
                DOC(fiction_technology_mapping_params_and_xor))

        ;

    py::class_<fiction::synthesis::technology_mapping_stats>(m, "technology_mapping_stats",
                                                             DOC(fiction_technology_mapping_stats))
        .def(py::init<>(), "Default constructor.")
        .def("report", &fiction::synthesis::technology_mapping_stats::report,
             DOC(fiction_technology_mapping_stats_report))
        .def_ro("mapper_stats", &fiction::synthesis::technology_mapping_stats::mapper_stats,
                DOC(fiction_technology_mapping_stats_mapper_stats));

    m.def("and_or_not", &fiction::synthesis::and_or_not, DOC(fiction_and_or_not));

    m.def("and_or_not_maj", &fiction::synthesis::and_or_not_maj, DOC(fiction_and_or_not_maj));

    m.def("all_standard_2_input_functions", &fiction::synthesis::all_standard_2_input_functions,
          DOC(fiction_all_standard_2_input_functions));

    m.def("all_standard_3_input_functions", &fiction::synthesis::all_standard_3_input_functions,
          DOC(fiction_all_standard_3_input_functions));

    m.def("all_supported_standard_functions", &fiction::synthesis::all_supported_standard_functions,
          DOC(fiction_all_supported_standard_functions));

    detail::technology_mapping_impl<py_logic_network>(m);
}

}  // namespace pyfiction
