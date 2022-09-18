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

int enum_net(std::vector<std::string>&& args)
{
    (void)args;

    try
    {
        LOG("=========================================================");
        LOG("Net");
        LOG("=========================================================");

        pfs::procfs pfs;
        auto net = pfs.get_net();

        auto dev = net.get_dev();
        print(dev);
        
        auto icmp = net.get_icmp();
        print(icmp);

        auto icmp6 = net.get_icmp6();
        print(icmp6);

        auto raw = net.get_raw();
        print(raw);

        auto raw6 = net.get_raw6();
        print(raw6);

        auto tcp = net.get_tcp();
        print(tcp);

        auto tcp6 = net.get_tcp6();
        print(tcp6);

        auto udp = net.get_udp();
        print(udp);

        auto udp6 = net.get_udp6();
        print(udp6);

        auto udplite = net.get_udplite();
        print(udplite);

        auto udplite6 = net.get_udplite6();
        print(udplite6);

        auto unix = net.get_unix();
        print(unix);

        auto netlink = net.get_netlink();
        print(netlink);
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing net info:");
        LOG(TAB << ex.what());
    }

    return 0;
}
