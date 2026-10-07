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
#include <vector>

namespace fiction::utils
{

/**
 * @brief A callback that receives progress reports from a long-running algorithm.
 *
 * Algorithms that accept a `progress_callback` in their parameters call it with the name of the task they are working
 * on, the number of work items completed so far, and the total number of work items. A total of `0` means that the
 * total is unknown. Every task is reported with `done == 0` first, and the final report of a task carries its final
 * count. In between, reports are throttled, so consumers must not rely on seeing every intermediate value.
 *
 * The callback may be invoked from any thread the algorithm spawns. Each reporter serializes its own invocations;
 * callbacks shared by multiple reporters must synchronize access to shared state. The callback must not throw.
 */
using progress_callback = std::function<void(std::string_view task, std::size_t done, std::size_t total)>;

/**
 * @brief Reports a logical worker's activity without changing its identity when its description changes.
 *
 * Arguments are the zero-based worker ID, worker count, description, completed work, total work (0 if unknown),
 * and whether the worker is active. Worker counts stay fixed for one invocation. Callbacks must not throw.
 */
using worker_progress_callback =
    std::function<void(std::size_t, std::size_t, std::string_view, std::size_t, std::size_t, bool)>;

/**
 * @brief Serializes worker reports and throttles updates to ten per second per worker.
 *
 * Activity transitions and final counts are reported immediately. Destruction clears remaining active workers.
 */
class worker_progress_reporter
{
  public:
    /**
     * @brief Creates reporting state only when a callback is present.
     * @param callback Receives worker snapshots.
     * @param count Number of logical workers, including workers that may remain idle.
     */
    worker_progress_reporter(worker_progress_callback callback, const std::size_t count) :
            on_worker{std::move(callback)},
            workers(on_worker ? count : 0)
    {}
    /** @brief Clears every active worker before releasing callback state. */
    ~worker_progress_reporter()
    {
        for (std::size_t i = 0; i < workers.size(); ++i)
        {
            try
            {
                finish(i);
            }
            catch (...)  // NOLINT(bugprone-empty-catch): cleanup cannot propagate callback exceptions
            {}
        }
    }
    /** @brief Reporters own synchronization state and cannot be copied. */
    worker_progress_reporter(const worker_progress_reporter&) = delete;
    /** @brief Workers may reference reporters, so reporters cannot be moved. */
    worker_progress_reporter(worker_progress_reporter&&) = delete;
    /** @brief Reporters do not support copy assignment. */
    worker_progress_reporter& operator=(const worker_progress_reporter&) = delete;
    /** @brief Reporters do not support move assignment. */
    worker_progress_reporter& operator=(worker_progress_reporter&&) = delete;
    /**
     * @brief Updates a worker's snapshot and publishes activity changes immediately.
     * @param id Stable logical worker index.
     * @param description Human-readable work and candidate description.
     * @param done Completed work items.
     * @param total Work budget, or 0 if unknown.
     * @param force Publish a significant status change immediately.
     */
    void update(const std::size_t id, const std::string_view description, const std::size_t done = 0,
                const std::size_t total = 0, const bool force = false)
    {
        if (!on_worker)
        {
            return;
        }
        const std::scoped_lock lock{mutex};
        auto&                  state   = workers.at(id);
        const auto             now     = std::chrono::steady_clock::now();
        const bool             publish = force || !state.active || (total != 0 && done == total) ||
                                         now - state.last_report >= std::chrono::milliseconds{100};
        state.description              = description;
        state.done                     = done;
        state.total                    = total;
        state.active                   = true;
        if (publish)
        {
            state.last_report = now;
            on_worker(id, workers.size(), state.description, done, total, true);
        }
    }
    /**
     * @brief Publishes the final snapshot and marks the worker inactive.
     * @param id Stable logical worker index.
     */
    void finish(const std::size_t id)
    {
        if (!on_worker)
        {
            return;
        }
        const std::scoped_lock lock{mutex};
        auto&                  state = workers.at(id);
        if (state.active)
        {
            state.active = false;
            on_worker(id, workers.size(), state.description, state.done, state.total, false);
        }
    }

  private:
    /** @brief The most recent snapshot of a logical worker, guarded by mutex. */
    struct worker_state
    {
        /** @brief Current work description. */
        std::string description{};
        /** @brief Completed work items. */
        std::size_t done{};
        /** @brief Work budget, or zero if unknown. */
        std::size_t total{};
        /** @brief Whether the worker has unfinished work. */
        bool active{};
        /** @brief Time of the last published snapshot. */
        std::chrono::steady_clock::time_point last_report{};
    };
    /** @brief Receives serialized snapshots. */
    worker_progress_callback on_worker;
    /** @brief State indexed by logical worker ID. */
    std::vector<worker_state> workers;
    /** @brief Serializes callbacks and state changes. */
    std::mutex mutex{};
};

/** @brief Clears a worker's activity on every exit from a computation. */
class worker_progress_scope
{
  public:
    /**
     * @brief Tracks the lifetime of an active worker.
     * @param reporter Shared reporter, which outlives this scope.
     * @param worker Logical worker ID.
     */
    worker_progress_scope(worker_progress_reporter& reporter, const std::size_t worker) : progress{reporter}, id{worker}
    {}
    /** @brief Marks the worker inactive, including during exception unwinding. */
    ~worker_progress_scope()
    {
        try
        {
            progress.finish(id);
        }
        catch (...)  // NOLINT(bugprone-empty-catch): cleanup cannot propagate callback exceptions
        {}
    }

    /** @brief One scope owns the worker's activity lifetime. */
    worker_progress_scope(const worker_progress_scope&) = delete;
    /** @brief A worker scope stays at its construction site. */
    worker_progress_scope(worker_progress_scope&&) = delete;
    /** @brief Worker scopes do not support copy assignment. */
    worker_progress_scope& operator=(const worker_progress_scope&) = delete;
    /** @brief Worker scopes do not support move assignment. */
    worker_progress_scope& operator=(worker_progress_scope&&) = delete;

  private:
    /** @brief Shared serialized reporter. */
    worker_progress_reporter& progress;
    /** @brief Logical worker ID. */
    std::size_t id;
};

/**
 * @brief Forwards the progress of a single task to a `progress_callback`, throttling the reports.
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
     * @brief Starts a task and reports `done == 0` to the callback.
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
            const std::scoped_lock lock{mutex};
            report_locked(0);
        }
    }
    /**
     * @brief Reports the final count of the task to the callback unless that count was reported already.
     */
    ~progress_reporter()
    {
        if (on_progress)
        {
            const std::scoped_lock lock{mutex};

            try
            {
                report_locked(count.load(std::memory_order_relaxed));
            }
            catch (...)  // NOLINT(bugprone-empty-catch): a destructor cannot propagate the callback's exception
            {}
        }
    }

    /**
     * @brief Reporters cannot be copied because each reporter owns its synchronization state.
     */
    progress_reporter(const progress_reporter&) = delete;
    /**
     * @brief Reporters cannot be moved while worker threads may access them.
     */
    progress_reporter(progress_reporter&&) = delete;
    /**
     * @brief Reporters do not support copy assignment.
     */
    progress_reporter& operator=(const progress_reporter&) = delete;
    /**
     * @brief Reporters do not support move assignment.
     */
    progress_reporter& operator=(progress_reporter&&) = delete;

    /**
     * @brief Records `n` completed work items and forwards a report if one is due.
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

        const std::scoped_lock lock{mutex};

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
     * @brief Sets the total number of work items once it becomes known and forwards a report.
     *
     * @param total The number of work items the task consists of, or `0` if it is unknown.
     */
    void set_total(const std::size_t total)
    {
        if (!on_progress)
        {
            return;
        }

        const std::scoped_lock lock{mutex};

        total_items = total;
        step        = step_for(total);

        report_locked(count.load(std::memory_order_relaxed));
    }
    /**
     * @brief Restarts the task with a new total and reports `done == 0`.
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

        const std::scoped_lock lock{mutex};

        report_locked(count.load(std::memory_order_relaxed));

        count.store(0, std::memory_order_relaxed);
        total_items = total;
        step        = step_for(total);
        reported    = false;

        report_locked(0);
    }

  private:
    /**
     * @brief The minimum time between two intermediate reports.
     */
    static constexpr auto REPORT_INTERVAL = std::chrono::milliseconds{100};
    /**
     * @brief The number of items between two intermediate reports while the total is unknown.
     */
    static constexpr std::size_t UNKNOWN_TOTAL_STEP = 64;
    /**
     * @brief The callback to forward reports to.
     */
    progress_callback on_progress;
    /**
     * @brief The name of the task.
     */
    std::string task_name;
    /**
     * @brief The number of completed work items.
     */
    std::atomic<std::size_t> count{0};
    /**
     * @brief The count at which the next intermediate report is due.
     */
    std::atomic<std::size_t> next_report{0};
    /**
     * @brief The total number of work items, or `0` if unknown. Guarded by `mutex`.
     */
    std::size_t total_items;
    /**
     * @brief The number of items between two intermediate reports. Guarded by `mutex`.
     */
    std::size_t step;
    /**
     * @brief The count and total of the last report. Guarded by `mutex`.
     */
    std::pair<std::size_t, std::size_t> last_report{0, 0};
    /**
     * @brief Whether a report was forwarded yet. Guarded by `mutex`.
     */
    bool reported{false};
    /**
     * @brief The time of the last report. Guarded by `mutex`.
     */
    std::chrono::steady_clock::time_point last_report_time{};
    /**
     * @brief Serializes the reports.
     */
    std::mutex mutex{};

    /**
     * @brief Computes the number of items between two intermediate reports for a given total.
     *
     * @param total The total number of work items, or `0` if unknown.
     * @return One percent of `total`, at least `1`, or `UNKNOWN_TOTAL_STEP` if the total is unknown.
     */
    [[nodiscard]] static constexpr std::size_t step_for(const std::size_t total) noexcept
    {
        return total == 0 ? UNKNOWN_TOTAL_STEP : std::max<std::size_t>(1, total / 100);
    }
    /**
     * @brief Forwards a report for `done` completed items unless the same report was forwarded already.
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
