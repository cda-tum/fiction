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

void logic_simulation(nanobind::module_& m);
void register_sidb_simulation(nanobind::module_& m);

void register_simulation(nanobind::module_& m)
{
    logic_simulation(m);
    register_sidb_simulation(m);
}

}  // namespace pyfiction
