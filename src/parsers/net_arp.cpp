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

#include "pfs/parsers/net_arp.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"
#include "pfs/types.hpp"

namespace pfs {
namespace impl {
namespace parsers {

net_arp parse_net_arp_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // IP address       HW type     Flags       HW address            Mask     Device
    // 192.168.10.1     0x1         0x2         10:20:30:40:50:60     *        eth0
    // clang-format on

    enum token
    {
        IP_ADDRESS     = 0,
        TYPE           = 1,
        FLAGS          = 2,
        HW_ADDRESS     = 3,
        MASK           = 4,
        DEVICE         = 5,
        COUNT
    };

    net_arp arp;

    static const char DELIM = ' ';

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted net arp - Unexpected tokens count", line);
    }

    try
    {
        // Parse tokens into arp struct
        arp.ip_address = tokens[IP_ADDRESS];
        utils::stot(tokens[TYPE], arp.type, utils::base::hex);
        utils::stot(tokens[FLAGS], arp.flags, utils::base::hex);
        arp.hw_address = tokens[HW_ADDRESS];
        arp.mask = tokens[MASK];
        arp.device = tokens[DEVICE];
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted net arp - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted net arp - Out of range", line);
    }

    return arp;
}

} // namespace parsers
} // namespace impl
} // namespace pfs
