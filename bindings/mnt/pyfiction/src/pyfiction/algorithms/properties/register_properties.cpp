#include <pybind11/pybind11.h>

namespace pyfiction
{

void critical_path_length_and_throughput(pybind11::module& m);

void register_properties(pybind11::module& m)
{
    critical_path_length_and_throughput(m);
}

}  // namespace pyfiction
