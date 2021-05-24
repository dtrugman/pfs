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

#include "enum.hpp"
#include "format.hpp"
#include "log.hpp"

#include "pfs/procfs.hpp"

template <typename T>
void add_sockets(const std::vector<T>& sockets,
                 std::unordered_map<ino_t, std::string>& output)
{
    for (auto& socket : sockets)
    {
        std::ostringstream out;
        out << socket;
        output.emplace(socket.inode, out.str());
    }
}

std::unordered_map<ino_t, std::string> enum_sockets(const pfs::net& net)
{
    std::unordered_map<ino_t, std::string> sockets;
    add_sockets(net.get_icmp(), sockets);
    add_sockets(net.get_icmp6(), sockets);
    add_sockets(net.get_raw(), sockets);
    add_sockets(net.get_raw6(), sockets);
    add_sockets(net.get_tcp(), sockets);
    add_sockets(net.get_tcp6(), sockets);
    add_sockets(net.get_udp(), sockets);
    add_sockets(net.get_udp6(), sockets);
    add_sockets(net.get_udplite(), sockets);
    add_sockets(net.get_udplite6(), sockets);
    add_sockets(net.get_unix(), sockets);
    add_sockets(net.get_netlink(), sockets);
    return sockets;
}

void enum_task_fds(const pfs::task& task)
{
    try
    {
        LOG("fds (total: " << task.count_fds() << ")");
        LOG("---");

        auto sockets = enum_sockets(task.get_net());

        for (auto& iter : task.get_fds())
        {
            auto num = iter.first;
            auto& fd = iter.second;

            auto st    = fd.get_target_stat();
            auto inode = st.st_ino;

            std::ostringstream out;
            out << "target[" << fd.get_target() << "] ";

            auto socket = sockets.find(inode);
            if (socket != sockets.end())
            {
                out << socket->second;
            }

            LOG(num << ": " << out.str());
        }

        LOG("");
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing task[" << task.id() << "] fds:");
        LOG(TAB << ex.what());
    }
}

int enum_fds(std::vector<std::string>&& args)
{
    pfs::procfs pfs;

    if (args.empty())
    {
        for (const auto& process : pfs.get_processes())
        {
            for (const auto& thread : process.get_tasks())
            {
                enum_task_fds(thread);
            }
        }
    }
    else
    {
        for (const auto& task : args)
        {
            auto id = std::stoi(task);
            enum_task_fds(pfs.get_task(id));
        }
    }

    return 0;
}
