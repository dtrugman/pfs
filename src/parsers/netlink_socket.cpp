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

uint32_t parse_port_id(const std::string& port_id_str)
{
    // Older versions serialize this as %-6d, even though
    // the type has always been u32.
    // So we allow anything that might be represented by
    // an [u]int32 and cast it to uint32.
    int64_t port_id;
    utils::stot(port_id_str, port_id);
    if (port_id < std::numeric_limits<int32_t>::min() ||
        port_id > std::numeric_limits<uint32_t>::max())
    {
        throw std::out_of_range(port_id_str);
    }

    return static_cast<uint32_t>(port_id);
}

} // anonymous namespace

netlink_socket parse_netlink_socket_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // ffff9f55b3e2e000 9   1      00000001 0        0        0 2        0        13723
    // ffff9f55ba8e8800 9   4280338379 00000000 0        0        0 2        0        14098
    // ffff9f55bd951800 10  0      00000000 0        0        0 2        0        9632
    // ffff9f55bd882800 11  0      00000000 0        0        0 2        0        26
    // clang-format on

    // See 'netlink_native_seq_show'
    enum token
    {
        SKBUFF    = 0,
        PROTOCOL  = 1, // Called Eth, but is actually sk_protocol
        PORT_ID   = 2,
        GROUPS    = 3,
        RMEM      = 4,
        WMEM      = 5,
        DUMPING   = 6,
        REF_COUNT = 7, // Called Locks, but is actually ref_count
        DROPS     = 8,
        MIN_COUNT = 9, // INODE was added after 2.6.32
        INODE     = 9,
        COUNT
    };

    static const std::string NULL_STR("(null)");
    static const std::string ZERO_STR("0");

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT)
    {
        throw parser_error(
            "Corrupted netlink socket line - Unexpected token count", line);
    }

    try
    {
        netlink_socket sock;

        utils::stot(tokens[SKBUFF], sock.skbuff, utils::base::hex);

        utils::stot(tokens[PROTOCOL], sock.protocol);

        sock.port_id = parse_port_id(tokens[PORT_ID]);

        utils::stot(tokens[GROUPS], sock.groups, utils::base::hex);

        utils::stot(tokens[RMEM], sock.rmem);

        utils::stot(tokens[WMEM], sock.wmem);

        // Older versions printed a pointer to the 'cb' buffer,
        // newer versions print the boolean using '%d'.
        // Just condition on both "false" values.
        sock.dumping =
            !(tokens[DUMPING] == NULL_STR || tokens[DUMPING] == ZERO_STR);

        utils::stot(tokens[REF_COUNT], sock.ref_count);

        utils::stot(tokens[DROPS], sock.drops);

        if (tokens.size() > INODE)
        {
            utils::stot(tokens[INODE], sock.inode);
        }

        return sock;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted netlink socket - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted netlink socket - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
