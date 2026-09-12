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
 * @brief Tests for `fiction/utils/progress.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/utils/progress.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

using namespace fiction;

namespace
{

/**
 * A `(done, total)` pair as received by a callback.
 */
using record = std::pair<std::size_t, std::size_t>;

/**
 * Records every report of a task.
 */
struct recorder
{
    std::vector<record> records{};
    std::string         task{};

    utils::progress_callback callback()
    {
        return [this](const std::string_view t, const std::size_t done, const std::size_t total)
        {
            task = t;
            records.emplace_back(done, total);
        };
    }
};

/**
 * Sleeps long enough for the reporter's time throttle to pass.
 */
void let_throttle_pass()
{
    std::this_thread::sleep_for(std::chrono::milliseconds{120});
}

bool is_monotone(const std::vector<record>& records)
{
    return std::ranges::is_sorted(records, [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
}

}  // namespace

TEST_CASE("An empty callback turns the reporter into a no-op", "[progress]")
{
    utils::progress_reporter reporter{{}, "nothing", 3};

    reporter.advance();
    reporter.set_total(5);
    reporter.reset(2);
    reporter.advance(2);

    SUCCEED();
}

TEST_CASE("The first and the final count are always reported", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items", 10};

        for (std::size_t i = 0; i < 10; ++i)
        {
            reporter.advance();
        }
    }

    CHECK(rec.task == "items");
    REQUIRE(rec.records.size() >= 2);
    CHECK(rec.records.front() == record{0, 10});
    CHECK(rec.records.back() == record{10, 10});
    CHECK(is_monotone(rec.records));
}

TEST_CASE("Intermediate reports are throttled to whole percents", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items", 1000};

        let_throttle_pass();
        reporter.advance();  // 1 of 1000 is less than a percent
        CHECK(rec.records.size() == 1);

        reporter.advance(9);  // 10 of 1000 completes the first percent
        CHECK(rec.records.size() == 2);
        CHECK(rec.records.back() == record{10, 1000});

        let_throttle_pass();
        reporter.advance(25);
        CHECK(rec.records.size() == 3);
        CHECK(rec.records.back() == record{35, 1000});
    }

    CHECK(rec.records.back() == record{35, 1000});
}

TEST_CASE("Intermediate reports are throttled in time", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items", 100};

        for (std::size_t i = 0; i < 100; ++i)
        {
            reporter.advance();
        }
    }

    // a tight loop of 100 items finishes well within the report interval
    CHECK(rec.records.size() < 100);
    CHECK(rec.records.front() == record{0, 100});
    CHECK(rec.records.back() == record{100, 100});
    CHECK(is_monotone(rec.records));
}

TEST_CASE("An unknown total is reported as zero", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items"};

        let_throttle_pass();
        reporter.advance(63);
        CHECK(rec.records.size() == 1);

        reporter.advance();
        CHECK(rec.records.size() == 2);
        CHECK(rec.records.back() == record{64, 0});
    }

    // the final count equals the last report, so it is not repeated
    CHECK(rec.records == std::vector<record>{{0, 0}, {64, 0}});
}

TEST_CASE("The total can be set once it is known", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items"};

        reporter.set_total(5);
        CHECK(rec.records == std::vector<record>{{0, 0}, {0, 5}});

        reporter.advance(5);
    }

    CHECK(rec.records.back() == record{5, 5});
}

TEST_CASE("A reporter can be reset for a new pass", "[progress]")
{
    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items", 3};

        reporter.advance(3);
        reporter.reset(4);

        CHECK(rec.records.back() == record{0, 4});

        reporter.advance(4);
    }

    CHECK(rec.records.front() == record{0, 3});
    CHECK(rec.records.back() == record{4, 4});
}

TEST_CASE("Every reset reports a new pass", "[progress]")
{
    recorder                 rec{};
    utils::progress_reporter reporter{rec.callback(), "items", 3};

    reporter.reset(3);
    reporter.reset(3);

    CHECK(rec.records == std::vector<record>{{0, 3}, {0, 3}, {0, 3}});
}

TEST_CASE("Two reporters share a caller-synchronized callback", "[progress]")
{
    recorder                       rec{};
    std::mutex                     callback_mutex{};
    const utils::progress_callback callback =
        [&rec, &callback_mutex](const std::string_view, const std::size_t done, const std::size_t total)
    {
        const std::scoped_lock lock{callback_mutex};
        rec.records.emplace_back(done, total);
    };
    const auto run = [&callback]
    {
        utils::progress_reporter reporter{callback, "items", 3};
        reporter.advance(3);
    };

    std::thread first{run};
    std::thread second{run};
    first.join();
    second.join();

    CHECK(std::ranges::count(rec.records, record{0, 3}) == 2);
    CHECK(std::ranges::count(rec.records, record{3, 3}) == 2);
}

TEST_CASE("Concurrent advances are counted exactly once", "[progress]")
{
    constexpr std::size_t num_threads      = 8;
    constexpr std::size_t items_per_thread = 10000;

    recorder rec{};

    {
        utils::progress_reporter reporter{rec.callback(), "items", num_threads * items_per_thread};

        std::vector<std::thread> workers{};
        workers.reserve(num_threads);

        for (std::size_t t = 0; t < num_threads; ++t)
        {
            workers.emplace_back(
                [&reporter]()
                {
                    for (std::size_t i = 0; i < items_per_thread; ++i)
                    {
                        reporter.advance();
                    }
                });
        }

        for (auto& worker : workers)
        {
            worker.join();
        }
    }

    CHECK(rec.records.back() == record{num_threads * items_per_thread, num_threads * items_per_thread});
    CHECK(is_monotone(rec.records));
}
