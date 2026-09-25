# Adds one `mnt.pyfiction` extension module. Every top-level submodule is its
# own shared object, installed flat into the package directory.
#
# add_pyfiction_python_binding(<target> <sources...> MODULE_NAME <name>)
function(add_pyfiction_python_binding target_name)
  cmake_parse_arguments(ARG "" "MODULE_NAME" "" ${ARGN})
  set(SOURCES ${ARG_UNPARSED_ARGUMENTS})

  # Split mode: the extension links no nanobind library code and resolves it at
  # import time from the `nanobind-backend` wheel, so one abi3 binary covers
  # Python 3.10 up. It implies STABLE_ABI. Free-threaded interpreters need the
  # `abi3t` of Python 3.15 (PEP 803); below that, nanobind stops the configure
  # with a message naming the version.
  #
  # The domain keeps pyfiction's type registry apart from every other nanobind
  # extension in the process, while all pyfiction modules share it and resolve
  # each other's types. `aigverse` binds `kitty::dynamic_truth_table` and
  # `mockturtle::names_view<mockturtle::aig_network>` as well, and in a shared
  # registry the module imported second does not get its class at all.
  nanobind_add_module(
    ${target_name}
    BACKEND_MODULE
    nanobind_backend
    FREE_THREADED
    LTO
    NB_SUPPRESS_WARNINGS
    NB_DOMAIN
    pyfiction
    ${SOURCES})

  # Global IPO stays off: cross-module LTO can corrupt the heap on Windows when
  # `shared_ptr`-based types pass between separate extension modules.
  set_target_properties(
    ${target_name} PROPERTIES OUTPUT_NAME ${ARG_MODULE_NAME}
                              INTERPROCEDURAL_OPTIMIZATION OFF)

  target_link_libraries(${target_name} PRIVATE libfiction)
  target_include_directories(${target_name}
                             PRIVATE ${PROJECT_SOURCE_DIR}/bindings/include)
  target_compile_features(${target_name} PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

  # `CMAKE_INSTALL_RPATH_USE_LINK_PATH` skips link directories inside the source
  # or build tree. A `z3-solver` wheel installed into a virtual environment in
  # the checkout, as `nox -s docs` does on Read the Docs, is such a directory,
  # and the module then fails to load `libz3`. Wheel builds link a Z3 outside
  # the tree.
  foreach(z3_library IN LISTS Z3_LIBRARIES)
    get_filename_component(z3_library_dir "${z3_library}" DIRECTORY)
    cmake_path(IS_PREFIX CMAKE_SOURCE_DIR "${z3_library_dir}" NORMALIZE
               z3_in_source_tree)
    cmake_path(IS_PREFIX CMAKE_BINARY_DIR "${z3_library_dir}" NORMALIZE
               z3_in_binary_tree)
    if(z3_in_source_tree OR z3_in_binary_tree)
      set_property(
        TARGET ${target_name}
        APPEND
        PROPERTY INSTALL_RPATH "${z3_library_dir}")
    endif()
  endforeach()

  # Keep the embedded static libraries' symbols local. `fiction_options` adds
  # `-fvisibility=hidden` to everything that links `libfiction`, and nanobind
  # adds `CXX_VISIBILITY_PRESET hidden` on the module's own objects; the
  # vendored archives -- `tinyxml2`, `graph-coloring`, ALGLIB -- consume neither
  # and keep default visibility, so each extension would re-export them. Ported
  # from marcelwa/aigverse#490.
  if(APPLE)
    target_link_options(${target_name} PRIVATE
                        "LINKER:-exported_symbol,_PyInit_${ARG_MODULE_NAME}")
  elseif(UNIX)
    target_link_options(${target_name} PRIVATE "LINKER:--exclude-libs,ALL")

    # `--gc-sections` collects nothing that was not compiled per-function and
    # per-data, and nanobind's own size tuning stops at `-Os`. These cover the
    # bindings; `vendors/CMakeLists.txt` covers the static archives, which is
    # where most of the collectable code turns out to be.
    target_compile_options(
      ${target_name}
      PRIVATE
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:-ffunction-sections;-fdata-sections>"
    )
    target_link_options(
      ${target_name}
      PRIVATE
      "$<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:LINKER:--gc-sections>"
    )
  endif()

  install(
    TARGETS ${target_name}
    DESTINATION .
    COMPONENT fiction_Python)

  add_dependencies(pyfiction ${target_name})
endfunction()
