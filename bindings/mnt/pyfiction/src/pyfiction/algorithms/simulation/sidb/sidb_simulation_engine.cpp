#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename EngineType>
void sidb_simulation_engine_name_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "sidb_simulation_engine_name",
        [](const EngineType& engine) -> std::string_view { return fiction::sidb_simulation_engine_name(engine); },
        py::arg("engine"), DOC(fiction_sidb_simulation_engine_name));
}

}  // namespace detail

void sidb_simulation_engine(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::sidb_simulation_engine>(m, "sidb_simulation_engine", DOC(fiction_sidb_simulation_engine))
        .value("EXGS", fiction::sidb_simulation_engine::EXGS, DOC(fiction_sidb_simulation_engine_EXGS))
        .value("QUICKSIM", fiction::sidb_simulation_engine::QUICKSIM, DOC(fiction_sidb_simulation_engine_QUICKSIM))
        .value("QUICKEXACT", fiction::sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::sidb_simulation_engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    py::enum_<fiction::exact_sidb_simulation_engine>(m, "exact_sidb_simulation_engine",
                                                     DOC(fiction_exact_sidb_simulation_engine))
        .value("EXGS", fiction::exact_sidb_simulation_engine::EXGS, DOC(fiction_exact_sidb_simulation_engine_EXGS))
        .value("QUICKEXACT", fiction::exact_sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_exact_sidb_simulation_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::exact_sidb_simulation_engine::CLUSTERCOMPLETE,
               DOC(fiction_exact_sidb_simulation_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    py::enum_<fiction::heuristic_sidb_simulation_engine>(m, "heuristic_sidb_simulation_engine",
                                                         DOC(fiction_heuristic_sidb_simulation_engine))
        .value("QUICKSIM", fiction::heuristic_sidb_simulation_engine::QUICKSIM,
               DOC(fiction_heuristic_sidb_simulation_engine_QUICKSIM))

        ;

    detail::sidb_simulation_engine_name_impl<fiction::sidb_simulation_engine>(m);
    detail::sidb_simulation_engine_name_impl<fiction::exact_sidb_simulation_engine>(m);
    detail::sidb_simulation_engine_name_impl<fiction::heuristic_sidb_simulation_engine>(m);
}

}  // namespace pyfiction
