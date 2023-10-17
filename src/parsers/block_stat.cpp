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

#include "pfs/parsers/block_stat.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"
#include "pfs/types.hpp"

#include <iostream>
#include <set>
#include <sstream>

namespace pfs {
namespace impl {
namespace parsers {

block_stat parse_block_stat_line(const std::string &line)
{
    // Some examples:
    // clang-format off
    //    12735     9101   833156     2926    57856   181036  8661664    38034        0    24572    46621     3213        0 102155080      337    23605     5323
    // clang-format on

    enum token
    {
        READ_IOS        = 0,
        READ_MERGES     = 1,
        READ_SECTORS    = 2,
        READ_TICKS      = 3,
        WRITE_IOS       = 4,
        WRITE_MERGES    = 5,
        WRITE_SECTORS   = 6,
        WRITE_TICKS     = 7,
        IN_FLIGHT       = 8,
        IO_TICKS        = 9,
        TIME_IN_QUEUE   = 10,
        DISCARD_IOS     = 11,
        DISCARD_MERGES  = 12,
        DISCARD_SECTORS = 13,
        DISCARD_TICKS   = 14,
        MIN_COUNT       = 14,
        FLUSH_IOS       = 15,
        FLUSH_TICKS     = 16,
        COUNT
    };

    block_stat stat;

    static const char DELIM = ' ';

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() < MIN_COUNT)
    {
        throw parser_error("Corrupted block stat - Unexpected tokens count", line);
    }

    try
    {
        utils::stot(tokens[READ_IOS], stat.read_ios, utils::base::decimal);
        utils::stot(tokens[READ_MERGES], stat.read_merges, utils::base::decimal);
        utils::stot(tokens[READ_SECTORS], stat.read_sectors, utils::base::decimal);
        utils::stot(tokens[READ_TICKS], stat.read_ticks, utils::base::decimal);
        utils::stot(tokens[WRITE_IOS], stat.write_ios, utils::base::decimal);
        utils::stot(tokens[WRITE_MERGES], stat.write_merges, utils::base::decimal);
        utils::stot(tokens[WRITE_SECTORS], stat.write_sectors, utils::base::decimal);
        utils::stot(tokens[WRITE_TICKS], stat.write_ticks, utils::base::decimal);
        utils::stot(tokens[IN_FLIGHT], stat.in_flight, utils::base::decimal);
        utils::stot(tokens[IO_TICKS], stat.io_ticks, utils::base::decimal);
        utils::stot(tokens[TIME_IN_QUEUE], stat.time_in_queue, utils::base::decimal);
        utils::stot(tokens[DISCARD_IOS], stat.discard_ios, utils::base::decimal);
        utils::stot(tokens[DISCARD_MERGES], stat.discard_merges, utils::base::decimal);
        utils::stot(tokens[DISCARD_SECTORS], stat.discard_sectors, utils::base::decimal);
        utils::stot(tokens[DISCARD_TICKS], stat.discard_ticks, utils::base::decimal);
        utils::stot(tokens[FLUSH_IOS], stat.flush_ios, utils::base::decimal);
        utils::stot(tokens[FLUSH_TICKS], stat.flush_ticks, utils::base::decimal);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted block stat - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted block stat - Out of range", line);
    }

    return stat;
}

} // namespace parsers
} // namespace impl
} // namespace pfs
