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
from mnt.pyfiction.physical_design import orthogonal_params

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

Parallel algorithms accept an optional `on_worker_progress` callback. Its arguments are the stable logical
worker ID, fixed worker count, description, completed count, total (`0` for unknown), and active state.
Descriptions can change without changing the worker ID. `gold` uses search-space graph IDs across rounds;
`exact` describes dimensions in tiles. A candidate's placed-node count describes that candidate, not the
fraction of the search completed.

```python
from mnt.pyfiction.physical_design import exact_params

params = exact_params()
params.on_worker_progress = lambda worker, count, description, done, total, active: print(
    worker, description, done, total, active
)
```

```{doxygentypedef} fiction::utils::worker_progress_callback
```

```{doxygenclass} fiction::utils::worker_progress_reporter
:members:
```

Worker reporters serialize callbacks and throttle ordinary updates to ten per second per worker. Activity
transitions publish immediately. Inactive reports retain the final completed count, including on early exits.
Algorithms join their workers before destroying callback state. Callbacks run outside solver, queue, and
result locks. A callback must not throw or re-enter its reporter.

Gate-library application and layout writers also accept aggregate callbacks, either as `on_progress`
arguments or in their parameter structs. Counted phases use their actual units: gates, rows, tiles, cell
positions, dots, defects, or rendering passes. The shell hides empty phases and keeps the last completed
count on failure. Readers do not scan input solely to determine progress totals.
