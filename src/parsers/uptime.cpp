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

uptime parse_uptime_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // 522823.22 2059843.84
    // clang-format on

    enum token
    {
        SYSTEM_TIME         = 0,
        IDLE_TIME            = 1,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted uptime - Unexpected tokens count", line);
    }

    try
    {
        uptime system_uptime;

        double system_time;
        double idle_time;

        system_time = std::stod(tokens[SYSTEM_TIME]);
        idle_time = std::stod(tokens[IDLE_TIME]);

        system_uptime.system_time = std::chrono::duration_cast<
            std::chrono::steady_clock::duration
        >(std::chrono::duration<double>(system_time));
        system_uptime.idle_time = std::chrono::duration_cast<
            std::chrono::steady_clock::duration
        >(std::chrono::duration<double>(idle_time));

        return system_uptime;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted uptime - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted uptime - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
