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
 * @brief A test helper that records the reports of a `fiction::utils::progress_callback`.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/utils/progress.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief A single report received by a `progress_recorder`.
 */
struct progress_record
{
    /**
     * @brief Task reported by the algorithm.
     */
    std::string task;
    /**
     * @brief Number of completed work items.
     */
    std::size_t done;
    /**
     * @brief Total work items, or zero when unknown.
     */
    std::size_t total;
};

/**
 * @brief Records every report an algorithm forwards to its `on_progress` callback.
 *
 * The recorder must outlive the algorithm call it observes. Its callback may be invoked from any thread.
 * Read reports after the algorithm call returns.
 */
class progress_recorder
{
  public:
    /**
     * @brief Creates a callback that appends each report to this recorder.
     *
     * @return A callback to assign to an algorithm's `on_progress` parameter.
     */
    [[nodiscard]] fiction::utils::progress_callback callback()
    {
        return [this](const std::string_view task, const std::size_t done, const std::size_t total)
        {
            const std::scoped_lock lock{mutex};
            reports.push_back({.task = std::string{task}, .done = done, .total = total});
        };
    }
    /**
     * @brief Returns the reports of a single task in the order they were received.
     *
     * @param task The task name to filter by.
     * @return All reports of `task`.
     */
    [[nodiscard]] std::vector<progress_record> reports_of(const std::string_view task) const
    {
        std::vector<progress_record> filtered{};
        std::ranges::copy_if(reports, std::back_inserter(filtered), [&task](const auto& r) { return r.task == task; });
        return filtered;
    }
    /**
     * @brief Checks that a task was reported consistently: it starts at `0`, never counts backwards except when it is
     * restarted at `0`, and it ends at its total if that was known.
     *
     * @param task The task name to check.
     * @return `true` iff the task was reported at least once and all its reports are consistent.
     */
    [[nodiscard]] bool is_consistent(const std::string_view task) const
    {
        /** @brief Most recent report of the selected task. */
        const progress_record* previous{nullptr};

        for (const auto& report : reports)
        {
            if (report.task != task)
            {
                continue;
            }
            if ((previous == nullptr && report.done != 0) ||
                (previous != nullptr && report.done < previous->done && report.done != 0))
            {
                return false;
            }
            previous = &report;
        }

        return previous != nullptr && (previous->total == 0 || previous->done == previous->total);
    }
    /**
     * @brief Returns the final count reported for a task.
     *
     * @param task The task name to look up.
     * @return The `done` value of the last report of `task`, or `0` if it was never reported.
     */
    [[nodiscard]] std::size_t final_count(const std::string_view task) const
    {
        /** @brief Last matching report in callback order. */
        const auto last = std::find_if(reports.crbegin(), reports.crend(),
                                       [task](const auto& report) { return report.task == task; });

        return last == reports.crend() ? 0 : last->done;
    }

  private:
    /**
     * @brief Reports in callback order.
     */
    std::vector<progress_record> reports{};
    /**
     * @brief Serializes callback writes to the report sequence.
     */
    std::mutex mutex{};
};
