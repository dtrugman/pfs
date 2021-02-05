/*
 *  Copyright 2020-present Daniel Trugman
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

std::pair<size_t, size_t>
parse_loadavg_task_counts(const std::string& task_counts_str)
{
    enum token
    {
        RUNNABLE = 0,
        TOTAL    = 1,
        COUNT
    };

    static const char DELIM = '/';

    auto tokens = utils::split(task_counts_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted loadavg task counts - Unexpected number of tokens",
            task_counts_str);
    }

    size_t runnable;
    utils::stot(tokens[RUNNABLE], runnable);

    size_t total;
    utils::stot(tokens[TOTAL], total);

    return std::make_pair(runnable, total);
}

} // anonymous namespace

load_average parse_loadavg_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // 0.00 0.00 0.00 1/112 5935
    // clang-format on

    enum token
    {
        LAST_1MIN         = 0,
        LAST_5MIN         = 1,
        LAST_15MIN        = 2,
        TASK_COUNTS       = 3,
        LAST_CREATED_TASK = 4,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted loadavg - Unexpected tokens count", line);
    }

    try
    {
        load_average load;

        load.last_1min  = std::stod(tokens[LAST_1MIN]);
        load.last_5min  = std::stod(tokens[LAST_5MIN]);
        load.last_15min = std::stod(tokens[LAST_15MIN]);

        std::tie(load.runnable_tasks, load.total_tasks) =
            parse_loadavg_task_counts(tokens[TASK_COUNTS]);

        utils::stot(tokens[LAST_CREATED_TASK], load.last_created_task);

        return load;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted loadavg - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted loadavg - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
