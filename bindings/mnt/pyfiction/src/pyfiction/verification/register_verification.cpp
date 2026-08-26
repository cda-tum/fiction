#include <nanobind/nanobind.h>

namespace pyfiction
{

void critical_path_length_and_throughput(nanobind::module_& m);
void design_rule_violations(nanobind::module_& m);
void equivalence_checking(nanobind::module_& m);

void register_verification(nanobind::module_& m)
{
    critical_path_length_and_throughput(m);
    design_rule_violations(m);
    equivalence_checking(m);
}

}  // namespace pyfiction
