# Progress Reporting

Long-running algorithms report their progress through a callback in their parameters. The callback receives the name
of the task the algorithm is working on, the number of work items completed so far, and the total number of work items,
where `0` stands for an unknown total. The `fiction` shell renders these reports as progress bars. Passing no callback
turns the reporting off.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/utils/progress.hpp`

```cpp
fiction::physical_design::orthogonal_physical_design_params params{};
params.on_progress = [](const std::string_view task, const std::size_t done, const std::size_t total)
{ std::cout << task << ": " << done << '/' << total << '\n'; };
```

```{doxygentypedef} fiction::utils::progress_callback

```

```{doxygenclass} fiction::utils::progress_reporter
:members:
```

:::

:::{tab-item} Python
:sync: python

```python
from mnt.pyfiction import orthogonal_params

params = orthogonal_params()
params.on_progress = lambda task, done, total: print(f"{task}: {done}/{total}")
```

The algorithms release the global interpreter lock while they run. Callbacks acquire the lock and may run on the calling
thread or a worker thread before the algorithm returns.

:::

::::

Algorithms use a `progress_reporter` to forward their progress. Each reporter serializes its callbacks and throttles
intermediate reports to whole percents and at most ten reports per second. It always forwards the first and final count
of a task, including before a reset starts another pass. Callbacks must not throw exceptions. Callbacks shared by multiple
reporters must synchronize access to shared state, including Python callbacks that release the global interpreter lock.
