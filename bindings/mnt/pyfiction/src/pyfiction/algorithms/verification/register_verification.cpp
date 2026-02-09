#include <pybind11/pybind11.h>

namespace pyfiction
{

void design_rule_violations(pybind11::module& m);
void equivalence_checking(pybind11::module& m);

void register_verification(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    design_rule_violations(m);
    equivalence_checking(m);
}

}  // namespace pyfiction
