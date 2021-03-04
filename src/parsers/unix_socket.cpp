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

unix_socket::type parse_type(const std::string& type_str)
{
    int type_int;
    utils::stot(type_str, type_int, utils::base::hex);

    auto type = static_cast<unix_socket::type>(type_int);
    if (type < unix_socket::type::stream || type > unix_socket::type::seqpacket)
    {
        throw parser_error("Corrupted unix socket type - Illegal value",
                           type_str);
    }

    return type;
}

unix_socket::state parse_state(const std::string& state_str)
{
    int state_int;
    utils::stot(state_str, state_int);

    auto state = static_cast<unix_socket::state>(state_int);
    if (state < unix_socket::state::free ||
        state >= unix_socket::state::disconnecting)
    {
        throw parser_error("Corrupted unix socket state - Illegal value",
                           state_str);
    }

    return state;
}

} // anonymous namespace

unix_socket parse_unix_socket_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // ffff8db2fd23a400: 00000003 00000000 00000000 0001 03 16757
    // ffff8db2f3d35c00: 00000003 00000000 00000000 0001 03 17525 /var/run/dbus/system_bus_socket
    // ffff8db2fd23a000: 00000003 00000000 00000000 0001 03 17031 /run/systemd/journal/stdout
    // ffff8db2f696e000: 00000003 00000000 00000000 0001 03 15699 /run/systemd/journal/stdout
    // ffff8db2f3e09400: 00000002 00000000 00000000 0002 01 21401
    // clang-format on

    enum token
    {
        SKBUFF    = 0,
        REF_COUNT = 1,
        PROTOCOL  = 2,
        FLAGS     = 3,
        TYPE      = 4,
        STATE     = 5,
        INODE     = 6,
        MIN_COUNT = 7, // Only path might be missing
        PATH      = 7,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT || tokens.size() > COUNT)
    {
        throw parser_error(
            "Corrupted unix socket line - Unexpected token count", line);
    }

    try
    {
        unix_socket sock;

        utils::stot(tokens[SKBUFF], sock.skbuff, utils::base::hex);

        utils::stot(tokens[REF_COUNT], sock.ref_count, utils::base::hex);

        utils::stot(tokens[PROTOCOL], sock.protocol, utils::base::hex);

        utils::stot(tokens[FLAGS], sock.flags, utils::base::hex);

        sock.socket_type = parse_type(tokens[TYPE]);

        sock.socket_state = parse_state(tokens[STATE]);

        utils::stot(tokens[INODE], sock.inode);

        if (tokens.size() > PATH)
        {
            sock.path = tokens[PATH];
        }

        return sock;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted unix socket - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted unix socket - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
