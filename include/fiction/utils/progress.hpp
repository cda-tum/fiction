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
 * @brief A throttled, thread-safe progress hook for long-running algorithms.
 * @author Marcel Walter (marcelwa)
 * @author Anthropic (Claude)
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

namespace fiction::utils
{

/**
 * A callback that receives progress reports from a long-running algorithm.
 *
 * Algorithms that accept a `progress_callback` in their parameters call it with the name of the task they are working
 * on, the number of work items completed so far, and the total number of work items. A total of `0` means that the
 * total is unknown. Every task is reported with `done == 0` first, and the final report of a task carries its final
 * count. In between, reports are throttled, so consumers must not rely on seeing every intermediate value.
 *
 * The callback may be invoked from any thread the algorithm spawns, but never concurrently with itself. It must not
 * throw.
 */
using progress_callback = std::function<void(std::string_view task, std::size_t done, std::size_t total)>;

/**
 * Forwards the progress of a single task to a `progress_callback`, throttling the reports.
 *
 * A reporter is created at the start of a task with the callback of the algorithm's parameters, a task name, and,
 * where known, the total number of work items. Each completed item is announced with `advance()`, which is safe to call
 * concurrently from any number of threads. The reporter forwards the first report, the final report in its destructor,
 * and intermediate reports whenever another percent of the total (or another 64 items when the total is unknown) was
 * completed and at least 100 ms have passed since the last report. An empty callback turns every member into a no-op.
 */
class progress_reporter
{
  public:
    /**
     * Starts a task and reports `done == 0` to the callback.
     *
     * @param callback The callback to forward reports to. May be empty.
     * @param task The name of the task, e.g., `"placing gates"`.
     * @param total The number of work items the task consists of, or `0` if it is unknown.
     */
    progress_reporter(progress_callback callback, std::string task, const std::size_t total = 0) :
            on_progress{std::move(callback)},
            task_name{std::move(task)},
            total_items{total},
            step{step_for(total)}
    {
        if (on_progress)
        {
            const std::lock_guard lock{mutex};
            report_locked(0);
        }
    }
    /**
     * Reports the final count of the task to the callback unless that count was reported already.
     */
    ~progress_reporter()
    {
        if (on_progress)
        {
            const std::lock_guard lock{mutex};

            try
            {
                report_locked(count.load(std::memory_order_relaxed));
            }
            catch (...)  // NOLINT(bugprone-empty-catch): a destructor cannot propagate the callback's exception
            {}
        }
    }

    progress_reporter(const progress_reporter&)            = delete;
    progress_reporter(progress_reporter&&)                 = delete;
    progress_reporter& operator=(const progress_reporter&) = delete;
    progress_reporter& operator=(progress_reporter&&)      = delete;

    /**
     * Records `n` completed work items and forwards a report if one is due.
     *
     * This function may be called concurrently from any thread.
     *
     * @param n The number of work items completed since the last call.
     */
    void advance(const std::size_t n = 1)
    {
        if (!on_progress)
        {
            return;
        }

        const auto done = count.fetch_add(n, std::memory_order_relaxed) + n;

        if (done < next_report.load(std::memory_order_relaxed))
        {
            return;
        }

        const std::lock_guard lock{mutex};

        // another thread may have reported in the meantime
        if (done < next_report.load(std::memory_order_relaxed))
        {
            return;
        }

        if (std::chrono::steady_clock::now() - last_report_time < REPORT_INTERVAL)
        {
            // too early; try again after the next step
            next_report.store(done + step, std::memory_order_relaxed);
            return;
        }

        report_locked(done);
    }
    /**
     * Sets the total number of work items once it becomes known and forwards a report.
     *
     * @param total The number of work items the task consists of, or `0` if it is unknown.
     */
    void set_total(const std::size_t total)
    {
        if (!on_progress)
        {
            return;
        }

        const std::lock_guard lock{mutex};

        total_items = total;
        step        = step_for(total);

        report_locked(count.load(std::memory_order_relaxed));
    }
    /**
     * Restarts the task with a new total and reports `done == 0`.
     *
     * Use this for algorithms that iterate over the same set of items repeatedly, e.g., until a fixpoint is reached.
     *
     * @param total The number of work items the task consists of, or `0` if it is unknown.
     */
    void reset(const std::size_t total)
    {
        if (!on_progress)
        {
            return;
        }

        const std::lock_guard lock{mutex};

        count.store(0, std::memory_order_relaxed);
        total_items = total;
        step        = step_for(total);

        report_locked(0);
    }

  private:
    /**
     * The minimum time between two intermediate reports.
     */
    static constexpr auto REPORT_INTERVAL = std::chrono::milliseconds{100};
    /**
     * The number of items between two intermediate reports while the total is unknown.
     */
    static constexpr std::size_t UNKNOWN_TOTAL_STEP = 64;
    /**
     * The callback to forward reports to.
     */
    progress_callback on_progress;
    /**
     * The name of the task.
     */
    std::string task_name;
    /**
     * The number of completed work items.
     */
    std::atomic<std::size_t> count{0};
    /**
     * The count at which the next intermediate report is due.
     */
    std::atomic<std::size_t> next_report{0};
    /**
     * The total number of work items, or `0` if unknown. Guarded by `mutex`.
     */
    std::size_t total_items;
    /**
     * The number of items between two intermediate reports. Guarded by `mutex`.
     */
    std::size_t step;
    /**
     * The count and total of the last report. Guarded by `mutex`.
     */
    std::pair<std::size_t, std::size_t> last_report{0, 0};
    /**
     * Whether a report was forwarded yet. Guarded by `mutex`.
     */
    bool reported{false};
    /**
     * The time of the last report. Guarded by `mutex`.
     */
    std::chrono::steady_clock::time_point last_report_time{};
    /**
     * Serializes the reports.
     */
    std::mutex mutex{};

    /**
     * Computes the number of items between two intermediate reports for a given total.
     *
     * @param total The total number of work items, or `0` if unknown.
     * @return One percent of `total`, at least `1`, or `UNKNOWN_TOTAL_STEP` if the total is unknown.
     */
    [[nodiscard]] static constexpr std::size_t step_for(const std::size_t total) noexcept
    {
        return total == 0 ? UNKNOWN_TOTAL_STEP : std::max<std::size_t>(1, total / 100);
    }
    /**
     * Forwards a report for `done` completed items unless the same report was forwarded already.
     *
     * Must be called with `mutex` held.
     *
     * @param done The number of completed work items to report.
     */
    void report_locked(const std::size_t done)
    {
        const std::pair current{done, total_items};

        next_report.store(done + step, std::memory_order_relaxed);

        if (reported && current == last_report)
        {
            return;
        }

        last_report      = current;
        reported         = true;
        last_report_time = std::chrono::steady_clock::now();

        on_progress(task_name, done, total_items);
    }
};

}  // namespace fiction::utils
