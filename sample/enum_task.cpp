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

static void enum_task(const pfs::task& task)
{
    try
    {
        LOG("=========================================================");
        LOG("Task ID[" << task.id() << "]");
        LOG("=========================================================");

        auto status = task.get_status();
        print(status);

        auto stat = task.get_stat();
        print(stat);

        auto mem_stat = task.get_statm();
        print(mem_stat);

        auto io_stat = task.get_io();
        print(io_stat);

        auto comm = task.get_comm();
        print(comm);

        if (!task.is_kernel_thread(stat))
        {
            auto exe = task.get_exe();
            print(exe);
        }

        auto cmdline = task.get_cmdline();
        print(cmdline);

        auto cwd = task.get_cwd();
        print(cwd);

        auto environ = task.get_environ();
        print(environ);

        auto maps = task.get_maps();
        print(maps);

        if (!maps.empty())
        {
            static const size_t BYTES = 8;
            auto mem                  = task.get_mem();
            auto first_map            = *maps.begin();
            auto header_bytes = mem.read(first_map.start_address, BYTES);
            auto header       = hexlify(header_bytes);
            print(header);
        }

        auto mountinfo = task.get_mountinfo();
        print(mountinfo);

        auto cgroups = task.get_cgroups();
        print(cgroups);

        auto ns = task.get_ns();
        print(ns);

        auto fds = task.get_fds();
        print(fds);
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing task[" << task.id() << "]:");
        LOG(TAB << ex.what());
    }
}

int enum_tasks(std::vector<std::string>&& args)
{
    pfs::procfs pfs;

    if (args.empty())
    {
        for (const auto& process : pfs.get_processes())
        {
            for (const auto& thread : process.get_tasks())
            {
                enum_task(thread);
            }
        }
    }
    else
    {
        for (const auto& task : args)
        {
            auto id = std::stoi(task);
            enum_task(pfs.get_task(id));
        }
    }

    return 0;
}
