#include <nanobind/nanobind.h>

namespace pyfiction
{

void logic_networks(nanobind::module_& m);
void dynamic_truth_table(nanobind::module_& m);

void register_networks(nanobind::module_& m)
{
    logic_networks(m);
    dynamic_truth_table(m);
}

}  // namespace pyfiction
