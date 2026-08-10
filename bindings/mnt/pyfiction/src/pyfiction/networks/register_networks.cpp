#include <nanobind/nanobind.h>

namespace pyfiction
{

void logic_networks(nanobind::module_& m);
void truth_tables(nanobind::module_& m);

void register_networks(nanobind::module_& m)
{
    logic_networks(m);
    truth_tables(m);
}

}  // namespace pyfiction
