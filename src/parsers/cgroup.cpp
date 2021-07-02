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

cgroup parse_cgroup_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // 12:devices:/user.slice
    // 11:perf_event:/
    // 10:hugetlb:/
    // 9:rdma:/
    // 8:memory:/user.slice
    // 7:blkio:/user.slice
    // 6:cpuset:/
    // 5:cpu,cpuacct:/user.slice
    // 4:pids:/user.slice/user-1000.slice/session-174.scope
    // 3:freezer:/
    // 2:net_cls,net_prio:/
    // 1:name=systemd:/user.slice/user-1000.slice/session-174.scope
    // 0::/user.slice/user-1000.slice/session-174.scope
    // clang-format on

    enum token
    {
        HIERARCHY   = 0,
        CONTROLLERS = 1,
        PATHNAME    = 2,
        COUNT
    };

    static const char DELIM             = ':';
    static const char CONTROLLERS_DELIM = ',';

    auto tokens = utils::split(line, DELIM, /* keep_empty = */ true);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted cgroup line - Unexpected tokens count",
                           line);
    }

    try
    {
        cgroup cg;

        utils::stot(tokens[HIERARCHY], cg.hierarchy);

        cg.controllers = utils::split(tokens[CONTROLLERS], CONTROLLERS_DELIM);

        cg.pathname = tokens[PATHNAME];

        return cg;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted cgroup - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted cgroup - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
