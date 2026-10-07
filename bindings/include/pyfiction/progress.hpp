/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Property annotations and garbage collection for algorithm progress callbacks.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <array>

#include <Python.h>
#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>  // NOLINT(misc-include-cleaner): exposes Python callback references

namespace pyfiction
{

/**
 * @brief Types the getter of an `on_progress` member, which reads `None` while no callback is set.
 */
inline const auto ON_PROGRESS_GETTER = nanobind::for_getter(
    nanobind::sig("def on_progress(self, /) -> collections.abc.Callable[[str, int, int], None] | None"));
/**
 * @brief Types the getter of an `on_worker_progress` member, which reads `None` while no callback is set.
 */
inline const auto ON_WORKER_PROGRESS_GETTER = nanobind::for_getter(nanobind::sig(
    "def on_worker_progress(self, /) -> collections.abc.Callable[[int, int, str, int, int, bool], None] | None"));
/**
 * @brief Lets the setter of a callback member take `None`, which clears the callback.
 */
inline const auto CALLBACK_SETTER = nanobind::for_setter(nanobind::arg("value").none());

namespace detail
{

/**
 * @brief Visits Python callbacks owned by algorithm parameters for garbage collection.
 *
 * Borrowed subobject wrappers do not own the callbacks in their parent storage.
 * @tparam Params Parameter type with stored progress callbacks.
 * @param self Python parameter instance.
 * @param visit Python garbage collector visitor.
 * @param arg Visitor state.
 * @return Zero, or the visitor error code.
 */
template <typename Params>
int progress_traverse(PyObject* self, visitproc visit, void* arg)
{
    Py_VISIT(Py_TYPE(self));
    // Borrowed reference_internal views can retain owner cycles through hidden keep_alive records.
    // Collecting those cycles requires a change to nested property ownership.
    const auto [ready, owns_value] = nanobind::inst_state(self);
    if (!ready || !owns_value)
    {
        return 0;
    }
    const auto& params = *nanobind::inst_ptr<Params>(nanobind::handle{self});
    if constexpr (requires { params.on_progress; })
    {
        const auto callback = nanobind::find(params.on_progress);
        Py_VISIT(callback.ptr());
    }
    if constexpr (requires { params.design_gate_params.on_progress; })
    {
        const auto callback = nanobind::find(params.design_gate_params.on_progress);
        Py_VISIT(callback.ptr());
    }
    if constexpr (requires { params.sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress; })
    {
        const auto callback =
            nanobind::find(params.sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress);
        Py_VISIT(callback.ptr());
    }
    if constexpr (requires { params.op_domain_params.on_progress; })
    {
        const auto callback        = nanobind::find(params.op_domain_params.on_progress);
        const auto worker_callback = nanobind::find(params.op_domain_params.on_worker_progress);
        Py_VISIT(callback.ptr());
        Py_VISIT(worker_callback.ptr());
    }
    if constexpr (requires { params.on_worker_progress; })
    {
        const auto worker_callback = nanobind::find(params.on_worker_progress);
        Py_VISIT(worker_callback.ptr());
    }
    return 0;
}

/**
 * @brief Clears parameter callbacks to release unreachable Python reference cycles.
 * @tparam Params Parameter type with stored progress callbacks.
 * @param self Python parameter instance.
 * @return Zero.
 */
template <typename Params>
int progress_clear(PyObject* self)
{
    const auto [ready, owns_value] = nanobind::inst_state(self);
    if (ready && owns_value)
    {
        auto& params = *nanobind::inst_ptr<Params>(nanobind::handle{self});
        if constexpr (requires { params.on_progress; })
        {
            params.on_progress = nullptr;
        }
        if constexpr (requires { params.design_gate_params.on_progress; })
        {
            params.design_gate_params.on_progress = nullptr;
        }
        if constexpr (requires { params.sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress; })
        {
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress = nullptr;
        }
        if constexpr (requires { params.op_domain_params.on_progress; })
        {
            params.op_domain_params.on_progress        = nullptr;
            params.op_domain_params.on_worker_progress = nullptr;
        }
        if constexpr (requires { params.on_worker_progress; })
        {
            params.on_worker_progress = nullptr;
        }
    }
    return 0;
}

}  // namespace detail

/**
 * @brief Gives parameter types garbage collection slots for their Python callbacks.
 * @tparam Params Parameter type with stored progress callbacks.
 * @return Garbage collection slots for the parameter type.
 */
template <typename Params>
nanobind::type_slots progress_type_slots()
{
    /**
     * @brief Garbage collection callbacks for this parameter type.
     */
    // CPython stores callback pointers in PyType_Slot::pfunc, whose ABI type is void*.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    static const std::array<PyType_Slot, 3> slots = {
        {{.slot = Py_tp_traverse, .pfunc = reinterpret_cast<void*>(detail::progress_traverse<Params>)},
         {.slot = Py_tp_clear, .pfunc = reinterpret_cast<void*>(detail::progress_clear<Params>)},
         {.slot = 0, .pfunc = nullptr}}};
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    return nanobind::type_slots{slots.data()};
}

}  // namespace pyfiction
