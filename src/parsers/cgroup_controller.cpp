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

cgroup_controller parse_cgroup_controller_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // #subsys_name    hierarchy       num_cgroups     enabled
    // cpuset  6       1       1
    // cpu     5       33      1
    // cpuacct 5       33      1
    // blkio   7       33      1
    // memory  8       76      1
    // devices 12      33      1
    // freezer 3       1       1
    // net_cls 2       1       1
    // perf_event      11      1       1
    // net_prio        2       1       1
    // hugetlb 10      1       1
    // pids    4       38      1
    // rdma    9       1       1
    // clang-format on

    enum token
    {
        SUBSYS_NAME = 0,
        HIERARCHY   = 1,
        NUM_CGROUPS = 2,
        ENABLED     = 3,
        COUNT
    };

    static const char DELIM = '\t';

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted cgroup controller line - Unexpected tokens count", line);
    }

    try
    {
        cgroup_controller controller;

        controller.subsys_name = tokens[SUBSYS_NAME];

        utils::stot(tokens[HIERARCHY], controller.hierarchy);

        utils::stot(tokens[NUM_CGROUPS], controller.num_cgroups);

        if (tokens[ENABLED] == "0")
        {
            controller.enabled = false;
        }
        else if (tokens[ENABLED] == "1")
        {
            controller.enabled = true;
        }
        else
        {
            throw parser_error(
                "Corrupted cgroup controller line - Unexpected enabled value",
                tokens[ENABLED]);
        }

        return controller;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted cgroup controller - Invalid argument",
                           line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted cgroup controller - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
