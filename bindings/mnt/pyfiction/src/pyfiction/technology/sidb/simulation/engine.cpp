#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/engine.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename EngineType>
void sidb_simulation_engine_name_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "sidb_simulation_engine_name",
        [](const EngineType& engine) -> std::string_view { return fiction::sidb::simulation::engine_name(engine); },
        py::arg("engine"), DOC(fiction_sidb_simulation_engine_name));
}

}  // namespace detail

void engine(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::sidb::simulation::engine>(m, "sidb_simulation_engine", DOC(fiction_sidb_simulation_engine))
        .value("EXGS", fiction::sidb::simulation::engine::EXGS, DOC(fiction_sidb_simulation_engine_EXGS))
        .value("QUICKSIM", fiction::sidb::simulation::engine::QUICKSIM, DOC(fiction_sidb_simulation_engine_QUICKSIM))
        .value("QUICKEXACT", fiction::sidb::simulation::engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::sidb::simulation::engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    py::enum_<fiction::sidb::simulation::exact_engine>(m, "exact_sidb_simulation_engine",
                                                       DOC(fiction_sidb_simulation_exact_engine))
        .value("EXGS", fiction::sidb::simulation::exact_engine::EXGS, DOC(fiction_sidb_simulation_exact_engine_EXGS))
        .value("QUICKEXACT", fiction::sidb::simulation::exact_engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_exact_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::sidb::simulation::exact_engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_exact_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    py::enum_<fiction::sidb::simulation::heuristic_engine>(m, "heuristic_sidb_simulation_engine",
                                                           DOC(fiction_sidb_simulation_heuristic_engine))
        .value("QUICKSIM", fiction::sidb::simulation::heuristic_engine::QUICKSIM,
               DOC(fiction_sidb_simulation_heuristic_engine_QUICKSIM))

        ;

    detail::sidb_simulation_engine_name_impl<fiction::sidb::simulation::engine>(m);
    detail::sidb_simulation_engine_name_impl<fiction::sidb::simulation::exact_engine>(m);
    detail::sidb_simulation_engine_name_impl<fiction::sidb::simulation::heuristic_engine>(m);
}

}  // namespace pyfiction
