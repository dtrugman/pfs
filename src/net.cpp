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
#include "pfs/parsers/net_route.hpp"
#include "pfs/parsers/net_arp.hpp"
#include "pfs/parsers/net_device.hpp"
#include "pfs/parsers/net_socket.hpp"
#include "pfs/parsers/unix_socket.hpp"
#include "pfs/parsers/netlink_socket.hpp"
#include "pfs/parsers/lines.hpp"

namespace pfs {

using namespace impl;

net::net(const std::string& parent_root)
    : _parent_root(parent_root), _net_root(build_net_root(parent_root))
{}

std::string net::build_net_root(const std::string& parent_root)
{
    static const std::string NET_DIR("net/");
    return parent_root + NET_DIR;
}

std::vector<net_device> net::get_dev(net_device_filter filter) const
{
    static const std::string DEV_FILE("dev");
    auto path = _net_root + DEV_FILE;

    static const size_t HEADER_LINES = 2;

    std::vector<net_device> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
		                      parsers::parse_net_device_line,
                              filter, HEADER_LINES);
    return output;
}

std::vector<net_socket> net::get_icmp(net_socket_filter filter) const
{
    static const std::string ICMP_FILE("icmp");
    return get_net_sockets(ICMP_FILE, filter);
}

std::vector<net_socket> net::get_icmp6(net_socket_filter filter) const
{
    static const std::string ICMP6_FILE("icmp6");
    return get_net_sockets(ICMP6_FILE, filter);
}

std::vector<net_socket> net::get_raw(net_socket_filter filter) const
{
    static const std::string RAW_FILE("raw");
    return get_net_sockets(RAW_FILE, filter);
}

std::vector<net_socket> net::get_raw6(net_socket_filter filter) const
{
    static const std::string RAW6_FILE("raw6");
    return get_net_sockets(RAW6_FILE, filter);
}

std::vector<net_socket> net::get_tcp(net_socket_filter filter) const
{
    static const std::string TCP_FILE("tcp");
    return get_net_sockets(TCP_FILE, filter);
}

std::vector<net_socket> net::get_tcp6(net_socket_filter filter) const
{
    static const std::string TCP6_FILE("tcp6");
    return get_net_sockets(TCP6_FILE, filter);
}

std::vector<net_socket> net::get_udp(net_socket_filter filter) const
{
    static const std::string UDP_FILE("udp");
    return get_net_sockets(UDP_FILE, filter);
}

std::vector<net_socket> net::get_udp6(net_socket_filter filter) const
{
    static const std::string UDP6_FILE("udp6");
    return get_net_sockets(UDP6_FILE, filter);
}

std::vector<net_socket> net::get_udplite(net_socket_filter filter) const
{
    static const std::string UDPLITE_FILE("udplite");
    return get_net_sockets(UDPLITE_FILE, filter);
}

std::vector<net_socket> net::get_udplite6(net_socket_filter filter) const
{
    static const std::string UDPLITE6_FILE("udplite6");
    return get_net_sockets(UDPLITE6_FILE, filter);
}

std::vector<netlink_socket> net::get_netlink(netlink_socket_filter filter) const
{
    static const std::string NETLINK_FILE("netlink");
    auto path = _net_root + NETLINK_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<netlink_socket> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
                              parsers::parse_netlink_socket_line,
                              filter, HEADER_LINES);
    return output;
}

std::vector<unix_socket> net::get_unix(unix_socket_filter filter) const
{
    static const std::string UNIX_FILE("unix");
    auto path = _net_root + UNIX_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<unix_socket> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
                              parsers::parse_unix_socket_line,
                              filter, HEADER_LINES);
    return output;
}

std::vector<net_socket> net::get_net_sockets(const std::string& file,
        net_socket_filter filter) const
{
    auto path = _net_root + file;

    static const size_t HEADER_LINES = 1;

    std::vector<net_socket> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
                              parsers::parse_net_socket_line,
                              filter, HEADER_LINES);
    return output;
}

std::vector<net_route> net::get_route(net_route_filter filter) const
{
    static const std::string ROUTES_FILE("route");
    auto path = _net_root + ROUTES_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<net_route> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
                              parsers::parse_net_route_line,
                              filter, HEADER_LINES);
    return output;
}

std::vector<net_arp> net::get_arp(net_arp_filter filter) const
{
    static const std::string ARP_FILE("arp");
    auto path = _net_root + ARP_FILE;

    static const size_t HEADER_LINES = 1;

    std::vector<net_arp> output;
    parsers::parse_file_lines(path, std::back_inserter(output),
                              parsers::parse_net_arp_line,
                              filter, HEADER_LINES);
    return output;
}


} // namespace pfs
