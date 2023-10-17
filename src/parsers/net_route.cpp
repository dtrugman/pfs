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

#include "pfs/parsers/net_route.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"
#include "pfs/types.hpp"

#include <set>
#include <sstream>

namespace pfs {
namespace impl {
namespace parsers {

net_route parse_net_route_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT
    // eth0    00000000        07030301        0003    0       0       0       00000000        0       0       0
    // eth0    07030301        00000000        0001    0       0       0       00F0FFFF        0       0       0
    // clang-format on

    enum token
    {
        INTERFACE      = 0,
        DESTINATION    = 1,
        GATEWAY        = 2,
        FLAGS          = 3,
        REFCNT         = 4,
        USE            = 5,
        METRIC         = 6,
        MASK           = 7,
        MTU            = 8,
        WINDOW         = 9,
        IRTT           = 10,
        COUNT
    };

    net_route route;

    static const char DELIM = '\t';

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted net route - Unexpected tokens count", line);
    }

    try
    {
        // Parse tokens into route struct
        route.iface = tokens[INTERFACE];
        route.destination = utils::parse_ipv4_address(tokens[DESTINATION]);
        route.gateway = utils::parse_ipv4_address(tokens[GATEWAY]);
        route.mask = utils::parse_ipv4_address(tokens[MASK]);

        utils::stot(tokens[FLAGS], route.flags, utils::base::hex);
        utils::stot(tokens[REFCNT], route.refcnt, utils::base::decimal);
        utils::stot(tokens[USE], route.use, utils::base::decimal);
        utils::stot(tokens[METRIC], route.metric, utils::base::decimal);
        utils::stot(tokens[MTU], route.mtu, utils::base::decimal);
        utils::stot(tokens[WINDOW], route.window, utils::base::decimal);
        utils::stot(tokens[IRTT], route.irtt, utils::base::decimal);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted net route - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted net route - Out of range", line);
    }

    return route;
}

} // namespace parsers
} // namespace impl
} // namespace pfs
