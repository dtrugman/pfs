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

std::pair<std::string, size_t> parse_meminfo_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // VmallocTotal:   34359738367 kB
    // VmallocUsed:           0 kB
    // VmallocChunk:          0 kB
    // HardwareCorrupted:     0 kB
    // AnonHugePages:         0 kB
    // ShmemHugePages:        0 kB
    // ShmemPmdMapped:        0 kB
    // CmaTotal:              0 kB
    // CmaFree:               0 kB
    // HugePages_Total:       0
    // HugePages_Free:        0
    // HugePages_Rsvd:        0
    // HugePages_Surp:        0
    // Hugepagesize:       2048 kB
    // DirectMap4k:       67520 kB
    // DirectMap2M:      980992 kB
    // clang-format on

    enum token
    {
        DESCRIPTION = 0,
        AMOUNT      = 1,
        MIN_COUNT   = 2, // KB is not always present
        KB          = 2,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT || tokens.size() > COUNT)
    {
        throw parser_error("Corrupted meminfo - Unexpected tokens count", line);
    }

    try
    {
        auto& description = tokens[DESCRIPTION];
        description.pop_back(); // Remove ':'

        size_t amount;
        utils::stot(tokens[AMOUNT], amount);

        return std::make_pair(description, amount);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted address - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted address - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
