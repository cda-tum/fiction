STL Utilities
=============

Array Utils
-----------

**Header:** ``fiction/utils/stl/array_utils.hpp``

.. doxygenfunction:: fiction::utils::stl::create_array
.. doxygenfunction:: fiction::utils::stl::convert_array
.. doxygenfunction:: fiction::utils::stl::convert_array_of_arrays

STL Extensions
--------------

**Header:** ``fiction/utils/stl/stl_utils.hpp``

.. doxygenfunction:: fiction::utils::stl::find_first_two_of
.. doxygenclass:: fiction::utils::stl::searchable_priority_queue
    :members:
.. doxygenfunction:: fiction::utils::stl::safe_localtime

Execution Policy Macros
-----------------------

**Header:** ``fiction/utils/stl/execution_utils.hpp``

Handling parallel STL algorithms is a bit cumbersome due to their platform dependence. The following macros are provided
to simplify the usage of parallel STL algorithms while CMake and some pre-processor magic take care of all the
boilerplate.

One can use the following macros to specify the execution policy for parallel STL algorithms in a (mostly)
platform-independent way::

    std::for_each(FICTION_EXECUTION_POLICY_PAR v.begin(), v.end(), lambda);
    //                                        ^ note the missing comma

If parallelism or execution policies are not available, this will expand to::

    std::for_each(v.begin(), v.end(), lambda);

.. note::
    Only include this header and do not include ``<execution>`` directly. This header will include ``<execution>`` if
    available and will define the macros accordingly.

.. doxygendefine:: FICTION_EXECUTION_POLICY_SEQ
.. doxygendefine:: FICTION_EXECUTION_POLICY_PAR
.. doxygendefine:: FICTION_EXECUTION_POLICY_PAR_UNSEQ

Hashing
-------

**Header:** ``fiction/utils/stl/hash.hpp``

This header defines implementations for ``std::hash`` for several data types.

.. doxygenfunction:: fiction::utils::stl::hash_combine

``phmap``
---------

**Header:** ``fiction/utils/stl/phmap_utils.hpp``

.. doxygentypedef:: fiction::utils::stl::locked_parallel_flat_hash_map
