#include <nanobind/nanobind.h>

namespace pyfiction
{

void bdl_input_iterator(nanobind::module_& m);

void register_iter(nanobind::module_& m)
{
    bdl_input_iterator(m);
}

}  // namespace pyfiction
