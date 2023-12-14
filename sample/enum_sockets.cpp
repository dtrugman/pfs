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

int enum_sockets(std::vector<std::string>&& args)
{
    pfs::procfs pfs;

    if (args.empty())
    {
        LOG("Need process ID to list sockets for");
        return 1;
    }

    auto id = std::stoi(args[0]);
    pfs::task task = pfs.get_task(id);

    LOG("Process " << task.get_exe() << "(" << id << ") has the following network sockets:");
    LOG("Local                    Remote                State");
    for(pfs::net_socket& socket : task.get_net_sockets()){
        LOG(socket.local_ip.to_string() << ":" << socket.local_port << "  ->  " << socket.remote_ip.to_string() << ":" << socket.remote_port << "    " << socket.socket_net_state );
    }

    return 0;
}
