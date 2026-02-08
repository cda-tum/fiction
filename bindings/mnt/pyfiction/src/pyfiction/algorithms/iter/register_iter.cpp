#include <pybind11/pybind11.h>

namespace pyfiction
{

void bdl_input_iterator(pybind11::module& m);

void register_iter(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    bdl_input_iterator(m);
}

}  // namespace pyfiction
