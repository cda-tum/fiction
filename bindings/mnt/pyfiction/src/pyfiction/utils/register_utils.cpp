#include <nanobind/nanobind.h>

namespace pyfiction
{

void version_info(nanobind::module_& m);

void register_utils(nanobind::module_& m)
{
    version_info(m);
}

}  // namespace pyfiction
