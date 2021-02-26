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

#include <arpa/inet.h>

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

static const size_t HEX_BYTE_LEN = 8;

ip parse_ipv4_address(const std::string& ip_address_str)
{
    ipv4 raw;
    utils::stot(ip_address_str, raw, utils::base::hex);
    return ip(raw);
}

ip parse_ipv6_address(const std::string& ip_address_str)
{
    ipv6 raw;

    for (size_t i = 0; i < raw.size(); ++i)
    {
        auto nibble = ip_address_str.substr(i * HEX_BYTE_LEN, HEX_BYTE_LEN);
        utils::stot(nibble, raw[i], utils::base::hex);
    }

    return ip(raw);
}

std::pair<ip, uint16_t> parse_address(const std::string& address_str)
{
    enum token
    {
        IP   = 0,
        PORT = 1,
        COUNT
    };

    static const char DELIM = ':';

    auto tokens = utils::split(address_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted net socket address - Unexpected token counts",
            address_str);
    }

    ip addr;
    auto& ip_str = tokens[IP];
    if (ip_str.size() == HEX_BYTE_LEN)
    {
        addr = parse_ipv4_address(ip_str);
    }
    else if (ip_str.size() == std::tuple_size<ipv6>::value * HEX_BYTE_LEN)
    {
        addr = parse_ipv6_address(ip_str);
    }
    else
    {
        throw parser_error("Corrupted net socket address - Bad length",
                           address_str);
    }

    uint16_t port;
    utils::stot(tokens[PORT], port, utils::base::hex);

    return std::make_pair(addr, port);
}

net_socket::net_state parse_state(const std::string& state_str)
{
    int state_int;
    utils::stot(state_str, state_int, utils::base::hex);

    auto state = static_cast<net_socket::net_state>(state_int);
    if (state < net_socket::net_state::established ||
        state > net_socket::net_state::closing)
    {
        throw parser_error("Corrupted net socket state - Illegal value",
                           state_str);
    }

    return state;
}

std::pair<size_t, size_t> parse_queues(const std::string& queues_str)
{
    enum token
    {
        TX = 0,
        RX = 1,
        COUNT
    };

    static const char DELIM = ':';

    auto tokens = utils::split(queues_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted net socket queues - Unexpected token counts",
            queues_str);
    }

    size_t tx_queue;
    utils::stot(tokens[TX], tx_queue, utils::base::hex);

    size_t rx_queue;
    utils::stot(tokens[RX], rx_queue, utils::base::hex);

    return std::make_pair(tx_queue, rx_queue);
}

std::pair<net_socket::timer, size_t> parse_timer(const std::string& timer_str)
{
    enum token
    {
        ACTIVE = 0,
        EXPIRE = 1,
        COUNT
    };

    static const char DELIM = ':';

    auto tokens = utils::split(timer_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted net socket timer - Unexpected token counts", timer_str);
    }

    int timer_int;
    utils::stot(tokens[ACTIVE], timer_int);

    auto timer = static_cast<net_socket::timer>(timer_int);
    if (timer < net_socket::timer::none ||
        timer > net_socket::timer::zero_window)
    {
        throw parser_error("Corrupted net socket timer - Illegal value",
                           timer_str);
    }

    size_t expire_jiffies;
    utils::stot(tokens[EXPIRE], expire_jiffies, utils::base::hex);

    return std::make_pair(timer, expire_jiffies);
}

} // anonymous namespace

net_socket parse_net_socket_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // IPv4:
    // sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
    // 0: 00000000:006F 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 15734 1 ffff9f55b1421800 100 0 0 10 0
    // 1: 3500007F:0035 00000000:0000 0A 00000000:00000000 00:00000000 00000000   101        0 15989 1 ffff9f55b1420800 100 0 0 10 0
    // 2: 00000000:0016 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 18657 1 ffff9f55b1422000 100 0 0 10 0
    // 3: 0F02000A:0016 0202000A:DA94 01 0000002C:00000000 01:00000014 00000000     0        0 71261 4 ffff9f55b1420000 20 4 25 10 -1
    // IPv6:
    // sl  local_address                         remote_address                        st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
    // 0: 00000000000000000000000000000000:006F 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 15737 1 ffff9f55bdb91980 100 0 0 10 0
    // 1: 00000000000000000000000000000000:0016 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 18668 1 ffff9f55bdb94c80 100 0 0 10 0
    // clang-format on

    enum token
    {
        SLOT           = 0,
        LOCAL_ADDRESS  = 1,
        REMOTE_ADDRESS = 2,
        STATE          = 3,
        QUEUES         = 4,
        TIMER          = 5,
        RETRANSMITS    = 6,
        UID            = 7,
        TIMEOUTS       = 8,
        INODE          = 9,
        REF_COUNT      = 10,
        SKBUFF         = 11,
        MIN_COUNT // More tokens are expected, but ignored
    };

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT)
    {
        throw parser_error("Corrupted net socket line - Not enough tokens",
                           line);
    }

    try
    {
        net_socket sock;

        utils::stot(tokens[SLOT], sock.slot, utils::base::hex);

        std::tie(sock.local_ip, sock.local_port) =
            parse_address(tokens[LOCAL_ADDRESS]);
        std::tie(sock.remote_ip, sock.remote_port) =
            parse_address(tokens[REMOTE_ADDRESS]);

        sock.socket_net_state = parse_state(tokens[STATE]);

        std::tie(sock.tx_queue, sock.rx_queue) = parse_queues(tokens[QUEUES]);

        std::tie(sock.timer_active, sock.timer_expire_jiffies) =
            parse_timer(tokens[TIMER]);

        utils::stot(tokens[RETRANSMITS], sock.retransmits);

        utils::stot(tokens[UID], sock.uid);

        utils::stot(tokens[TIMEOUTS], sock.timeouts);

        utils::stot(tokens[INODE], sock.inode);

        utils::stot(tokens[REF_COUNT], sock.ref_count);

        utils::stot(tokens[SKBUFF], sock.skbuff, utils::base::hex);

        return sock;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted net socket - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted net socket - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
