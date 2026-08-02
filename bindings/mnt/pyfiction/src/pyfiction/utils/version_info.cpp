//
// Created by Jan Drewniok on 11.11.24.
//

#include <fiction/utils/version_info.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void version_info(nanobind::module_& m)
{
    m.attr("__version__")       = fiction::FICTION_VERSION;
    m.attr("__repo__")          = fiction::FICTION_REPO;
    m.attr("__compiled_date__") = fiction::COMPILED_DATE;
    m.attr("__compiled_time__") = fiction::COMPILED_TIME;
}

}  // namespace pyfiction
