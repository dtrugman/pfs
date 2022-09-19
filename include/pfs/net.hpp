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

#include <string>
#include <vector>

#include "types.hpp"

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
    std::vector<net_device> get_dev() const;
    
    std::vector<net_socket> get_icmp() const;
    std::vector<net_socket> get_icmp6() const;
    std::vector<net_socket> get_raw() const;
    std::vector<net_socket> get_raw6() const;
    std::vector<net_socket> get_tcp() const;
    std::vector<net_socket> get_tcp6() const;
    std::vector<net_socket> get_udp() const;
    std::vector<net_socket> get_udp6() const;
    std::vector<net_socket> get_udplite() const;
    std::vector<net_socket> get_udplite6() const;

    std::vector<netlink_socket> get_netlink() const;

    std::vector<unix_socket> get_unix() const;

private:
    friend class task;
    net(const std::string& procfs_root);

private:
    std::vector<net_socket> get_net_sockets(const std::string& file) const;

    static std::string build_net_root(const std::string& procfs_root);

private:
    const std::string _procfs_root;
    const std::string _net_root;
};

} // namespace pfs

#endif // PFS_TASK_HPP
