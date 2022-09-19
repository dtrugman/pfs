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

#include "pfs/net.hpp"
#include "pfs/parsers.hpp"

namespace pfs {

using namespace impl;

net::net(const std::string& procfs_root)
    : _procfs_root(procfs_root), _net_root(build_net_root(procfs_root))
{}

std::string net::build_net_root(const std::string& procfs_root)
{
    static const std::string NET_DIR("net/");
    return procfs_root + NET_DIR;
}

std::vector<net_device> net::get_dev() const
{
    static const std::string DEV_FILE("dev");
    auto path = _net_root + DEV_FILE;

    static const size_t HEADER_LINES = 2;

    std::vector<net_device> output;
    parsers::parse_lines(path, std::back_inserter(output),
		         parsers::parse_net_device_line, HEADER_LINES);
    return output;
}

std::vector<net_socket> net::get_icmp() const
{
    static const std::string ICMP_FILE("icmp");
    return get_net_sockets(ICMP_FILE);
}

std::vector<net_socket> net::get_icmp6() const
{
    static const std::string ICMP6_FILE("icmp6");
    return get_net_sockets(ICMP6_FILE);
}

std::vector<net_socket> net::get_raw() const
{
    static const std::string RAW_FILE("raw");
    return get_net_sockets(RAW_FILE);
}

std::vector<net_socket> net::get_raw6() const
{
    static const std::string RAW6_FILE("raw6");
    return get_net_sockets(RAW6_FILE);
}

std::vector<net_socket> net::get_tcp() const
{
    static const std::string TCP_FILE("tcp");
    return get_net_sockets(TCP_FILE);
}

std::vector<net_socket> net::get_tcp6() const
{
    static const std::string TCP6_FILE("tcp6");
    return get_net_sockets(TCP6_FILE);
}

std::vector<net_socket> net::get_udp() const
{
    static const std::string UDP_FILE("udp");
    return get_net_sockets(UDP_FILE);
}

std::vector<net_socket> net::get_udp6() const
{
    static const std::string UDP6_FILE("udp6");
    return get_net_sockets(UDP6_FILE);
}

std::vector<net_socket> net::get_udplite() const
{
    static const std::string UDPLITE_FILE("udplite");
    return get_net_sockets(UDPLITE_FILE);
}

std::vector<net_socket> net::get_udplite6() const
{
    static const std::string UDPLITE6_FILE("udplite6");
    return get_net_sockets(UDPLITE6_FILE);
}

std::vector<netlink_socket> net::get_netlink() const
{
    static const std::string NETLINK_FILE("netlink");
    auto path = _net_root + NETLINK_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<netlink_socket> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_netlink_socket_line, HEADER_LINES);
    return output;
}

std::vector<unix_socket> net::get_unix() const
{
    static const std::string UNIX_FILE("unix");
    auto path = _net_root + UNIX_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<unix_socket> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_unix_socket_line, HEADER_LINES);
    return output;
}

std::vector<net_socket> net::get_net_sockets(const std::string& file) const
{
    auto path = _net_root + file;

    static const size_t HEADER_LINES = 1;

    std::vector<net_socket> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_net_socket_line, HEADER_LINES);
    return output;
}

} // namespace pfs
