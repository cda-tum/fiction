//
// Created by Jan Drewniok on 11.11.24.
//

#include <fiction/utils/version_info.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

void version_info(pybind11::module& m)
{
    m.attr("__version__")       = fiction::FICTION_VERSION;
    m.attr("__repo__")          = fiction::FICTION_REPO;
    m.attr("__compiled_date__") = fiction::COMPILED_DATE;
    m.attr("__compiled_time__") = fiction::COMPILED_TIME;
}

}  // namespace pyfiction
