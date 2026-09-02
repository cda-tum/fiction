#include <nanobind/nanobind.h>

namespace pyfiction
{

void truth_tables(nanobind::module_& m);
void fanout_substitution(nanobind::module_& m);
void network_balancing(nanobind::module_& m);
void technology_mapping(nanobind::module_& m);

void register_synthesis(nanobind::module_& m)
{
    truth_tables(m);
    fanout_substitution(m);
    network_balancing(m);
    technology_mapping(m);
}

}  // namespace pyfiction
