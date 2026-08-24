#include <nanobind/nanobind.h>

namespace pyfiction
{

void sidb::simulation::logic::bdl_input_iterator(nanobind::module_& m);

void register_iter(nanobind::module_& m)
{
    sidb::simulation::logic::bdl_input_iterator(m);
}

}  // namespace pyfiction
