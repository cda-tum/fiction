Aspect Ratio Iterator
---------------------

**Header:** ``fiction/physical_design/utils/aspect_ratio_iterator.hpp``

.. doxygenclass:: fiction::physical_design::utils::aspect_ratio_iterator
   :members:


Gray Code Iterator
------------------

**Header:** ``fiction/utils/stl/gray_code_iterator.hpp``

.. doxygenclass:: fiction::utils::stl::gray_code_iterator
   :members:


BDL Input Iterator
------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp``

        .. doxygenstruct:: fiction::sidb::simulation::logic::bdl_input_iterator_params
           :members:
        .. doxygenclass:: fiction::sidb::simulation::logic::bdl_input_iterator
           :members:
        .. doxygenfunction:: fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const Lyt& lyt, const bdl_input_iterator_params& ps = {})
        .. doxygenfunction:: fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const Lyt& lyt, const bdl_input_iterator_params& ps, const std::vector<bdl_wire<Lyt>>& input_wires)

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.input_bdl_configuration
            :members:
        .. autoclass:: mnt.pyfiction.bdl_input_iterator_params
            :members:
        .. autoclass:: mnt.pyfiction.bdl_input_iterator_100
            :members:
        .. autoclass:: mnt.pyfiction.bdl_input_iterator_111
            :members:
        .. autofunction:: mnt.pyfiction.generate_bdl_input_pattern_layouts
