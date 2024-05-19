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

static void safe(const std::string& what, const std::function<void(void)>& func)
{
    try
    {
        func();
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error getting [" << what << "]:");
        LOG(TAB << ex.what());
    }
}

static void enum_task(const pfs::task& task)
{
    LOG("=========================================================");
    LOG("Task ID[" << task.id() << "]");
    LOG("=========================================================");

    safe("status", [&]{
        auto status = task.get_status();
        print(status);
    });

    bool is_kernel_thread = false;

    safe("stat", [&]{
        auto stat = task.get_stat();
        print(stat);

        is_kernel_thread = task.is_kernel_thread(stat);
    });

    safe("statm", [&]{
        auto mem_stat = task.get_statm();
        print(mem_stat);
    });

    safe("io", [&]{
        auto io_stat = task.get_io();
        print(io_stat);
    });

    safe("comm", [&]{
        auto comm = task.get_comm();
        print(comm);
    });

    if (!is_kernel_thread)
    {
        safe("exe", [&]{
            auto exe = task.get_exe();
            print(exe);
        });
    }

    safe("cmdline", [&]{
        auto cmdline = task.get_cmdline();
        print(cmdline);
    });

    safe("cwd", [&]{
        auto cwd = task.get_cwd();
        print(cwd);
    });

    safe("environ", [&]{
        auto environ = task.get_environ();
        print(environ);
    });

    std::vector<pfs::mem_region> maps;

    safe("maps", [&]{
        maps = task.get_maps();
        print(maps);
    });

    if (!maps.empty())
    {
        safe("mem", [&]{
            static const size_t BYTES = 8;
            auto mem          = task.get_mem();
            auto first_map    = *maps.begin();
            auto header_bytes = mem.read(first_map.start_address, BYTES);
            auto header       = hexlify(header_bytes);
            print(header);
        });
    }

    safe("mountinfo", [&]{
        auto mountinfo = task.get_mountinfo();
        print(mountinfo);
    });

    safe("cgroups", [&]{
        auto cgroups = task.get_cgroups();
        print(cgroups);
    });

    safe("ns", [&]{
        auto ns = task.get_ns();
        print(ns);
    });

    safe("fds", [&]{
        auto fds = task.get_fds();
        print(fds);
    });
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
