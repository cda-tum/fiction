#include <nanobind/nanobind.h>

namespace pyfiction
{

void area(nanobind::module_& m);

void register_fcn(nanobind::module_& m)
{
    area(m);
}

}  // namespace pyfiction
