.. _onepass:

SAT-based One-pass Synthesis
----------------------------

**Header:** ``fiction/algorithms/physical_design/one_pass_synthesis.hpp``

Utilizes the SAT solver `Glucose <https://www.labri.fr/perso/lsimon/glucose/>`_ to generate minimal FCN gate-level
layouts from truth table specifications under constraints. To this end, it combines synthesis, placement, and routing
into a single step. Since this algorithm is not restricted to any logic network structure up front, it has the
opportunity to generate even smaller layouts than ``exact``. Consequently, this algorithm does also not scale.

.. doxygenstruct:: fiction::one_pass_synthesis_params
   :members:
.. doxygenstruct:: fiction::one_pass_synthesis_stats
   :members:
.. doxygenfunction:: fiction::one_pass_synthesis(const std::vector<TT>& tts, one_pass_synthesis_params ps = {}, one_pass_synthesis_stats* pst = nullptr)
.. doxygenfunction:: fiction::one_pass_synthesis(const Ntk& ntk, const one_pass_synthesis_params& ps = {}, one_pass_synthesis_stats* pst = nullptr)
