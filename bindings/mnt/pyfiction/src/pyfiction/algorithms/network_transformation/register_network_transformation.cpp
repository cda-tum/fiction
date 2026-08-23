#include <nanobind/nanobind.h>

namespace pyfiction
{

void synthesis::fanout_substitution(nanobind::module_& m);
void synthesis::network_balancing(nanobind::module_& m);
void synthesis::technology_mapping(nanobind::module_& m);

void register_network_transformation(nanobind::module_& m)
{
    synthesis::fanout_substitution(m);
    synthesis::network_balancing(m);
    synthesis::technology_mapping(m);
}

}  // namespace pyfiction
