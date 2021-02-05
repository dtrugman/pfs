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

#include <linux/kdev_t.h>

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

dev_t parse_device(const std::string& device_str)
{
    // Device format must be '<major>:<minor>'

    enum token
    {
        MAJOR = 0,
        MINOR = 1,
        COUNT
    };

    static const char DELIM = ':';

    auto tokens = utils::split(device_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted device - Unexpected tokens count",
                           device_str);
    }

    try
    {
        int major;
        utils::stot(tokens[MAJOR], major, utils::base::hex);

        int minor;
        utils::stot(tokens[MINOR], minor, utils::base::hex);

        return MKDEV(major, minor);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted device - Invalid argument", device_str);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted device - Out of range", device_str);
    }
}

task_state parse_task_state(char state_char)
{
    switch (state_char)
    {
        case 'R':
            return task_state::running;

        case 'S':
            return task_state::sleeping;

        case 'D':
            return task_state::disk_sleep;

        case 'Z':
            return task_state::zombie;

        case 'T':
            return task_state::stopped;

        case 't':
            return task_state::tracing_stop;

        case 'x':
        case 'X':
            return task_state::dead;

        case 'K':
            return task_state::wakekill;

        case 'W':
            return task_state::waking;

        case 'P':
            return task_state::parked;

        case 'I':
            return task_state::idle;

        default:
            throw parser_error("Corrupted task state - Unexpected value",
                               state_char);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
