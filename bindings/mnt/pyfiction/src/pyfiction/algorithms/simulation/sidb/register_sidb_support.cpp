#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void sidb_simulation_engine(nanobind::module_& m);
void sidb_simulation_parameters(nanobind::module_& m);
void sidb_simulation_result(nanobind::module_& m);

void register_sidb_support(nanobind::module_& m)
{
    sidb_simulation_engine(m);
    sidb_simulation_parameters(m);
    sidb_simulation_result(m);
}

}  // namespace pyfiction
