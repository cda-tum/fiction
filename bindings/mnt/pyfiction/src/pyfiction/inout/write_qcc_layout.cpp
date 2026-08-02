//
// Created by marcel on 16.12.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_qcc_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void write_qcc_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "write_qcc_layout",
        [](const py_inml_layout& lyt, const std::string_view& filename) { fiction::write_qcc_layout(lyt, filename); },
        py::arg("layout"), py::arg("filename"), DOC(fiction_write_qcc_layout));
}

}  // namespace pyfiction
