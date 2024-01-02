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

#include <regex>

#include "format.hpp"
#include "log.hpp"
#include "tool.hpp"

static const std::string TCP("tcp");
static const std::string UDP("udp");

void task_netstat(const pfs::task& task, const std::string& type)
{
    LOG("=========================================================");
    LOG("Netstat for task ID[" << task.id() << "]");
    LOG("=========================================================");

    auto inodes = task.get_fds_inodes();
    pfs::net::net_socket_filter filter = [&inodes](const pfs::net_socket& sock){
        return inodes.find(sock.inode) != inodes.end()
            ? pfs::filter::action::keep
            : pfs::filter::action::drop;
    };

    auto net = task.get_net();
    std::vector<pfs::net_socket> sockets;
    if (type == TCP)
    {
        sockets = net.get_tcp(filter);
    }
    else if (type == UDP)
    {
        sockets = net.get_udp(filter);
    }

    print(sockets);
}

int tool_netstat(std::vector<std::string>&& args)
{
    if (args.size() < 1)
    {
        return -EINVAL;
    }

    std::string type(args[0]);
    if (type != TCP && type != UDP)
    {
        return -EINVAL;
    }

    try
    {
        pfs::procfs pfs;

        if (args.size() > 1)
        {
            for (unsigned arg = 1; arg < args.size(); ++arg)
            {
                auto id = std::stoi(args[arg]);
                auto task = pfs.get_task(id);
                task_netstat(task, type);
            }
        }
        else
        {
            for (auto& task : pfs.get_processes())
            {
                task_netstat(task, type);
            }
        }
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing netstat:");
        LOG(TAB << ex.what());
    }

    return 0;
}
