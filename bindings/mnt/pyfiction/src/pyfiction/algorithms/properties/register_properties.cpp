#include <nanobind/nanobind.h>

namespace pyfiction
{

void critical_path_length_and_throughput(nanobind::module_& m);

void register_properties(nanobind::module_& m)
{
    critical_path_length_and_throughput(m);
}

}  // namespace pyfiction
