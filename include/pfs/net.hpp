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

#ifndef PFS_NET_HPP
#define PFS_NET_HPP

#include <functional>
#include <string>
#include <vector>

#include "types.hpp"
#include "filter.hpp"

namespace pfs {

/*
 * An API to the (some of) /proc/net files. See proc(5) for more information.
 * Notes:
 * Since Linux 2.6.25 nad the advent of network namespaces,
 * /proc/net is a symbolic link to /proc/self/net,
 * exposing info for the network namespace of which the process is a member.
 */
class net final
{
public:
    net(const net&) = default;
    net(net&&)      = default;

    net& operator=(const net&) = delete;
    net& operator=(net&&) = delete;

public:
    using net_device_filter = std::function<filter::action(const net_device&)>;
    using net_socket_filter = std::function<filter::action(const net_socket&)>;
    using netlink_socket_filter = std::function<filter::action(const netlink_socket&)>;
    using unix_socket_filter = std::function<filter::action(const unix_socket&)>;
    using net_route_filter = std::function<filter::action(const net_route&)>;
    using net_arp_filter = std::function<filter::action(const net_arp&)>;

public:
    std::vector<net_device> get_dev(net_device_filter filter = nullptr) const;

    std::vector<net_socket> get_icmp(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_icmp6(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_raw(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_raw6(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_tcp(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_tcp6(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_udp(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_udp6(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_udplite(net_socket_filter filter = nullptr) const;
    std::vector<net_socket> get_udplite6(net_socket_filter filter = nullptr) const;

    std::vector<netlink_socket> get_netlink(netlink_socket_filter filter = nullptr) const;

    std::vector<unix_socket> get_unix(unix_socket_filter filter = nullptr) const;

    std::vector<net_route> get_route(net_route_filter filter = nullptr) const;

    std::vector<net_arp> get_arp(net_arp_filter filter = nullptr) const;

private:
    friend class task;
    net(const std::string& parent_root);

private:
    std::vector<net_socket> get_net_sockets(const std::string& file,
            net_socket_filter filter = nullptr) const;

    static std::string build_net_root(const std::string& parent_root);

private:
    // Net has a "parent root", and not a "procfs root", because we could
    // be looking at a net namespace of a specific process.
    const std::string _parent_root;
    const std::string _net_root;
};

} // namespace pfs

#endif // PFS_TASK_HPP
