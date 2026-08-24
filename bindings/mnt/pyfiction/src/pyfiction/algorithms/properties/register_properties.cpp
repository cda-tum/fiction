#include <nanobind/nanobind.h>

namespace pyfiction
{

void verification::critical_path_length_and_throughput(nanobind::module_& m);

void register_properties(nanobind::module_& m)
{
    verification::critical_path_length_and_throughput(m);
}

}  // namespace pyfiction
