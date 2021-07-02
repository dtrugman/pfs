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

int enum_system(std::vector<std::string>&& args)
{
    (void)args;

    try
    {
        LOG("=========================================================");
        LOG("System");
        LOG("=========================================================");

        pfs::procfs pfs;

        auto buddyinfo = pfs.get_buddyinfo();
        print(buddyinfo);

        auto cmdline = pfs.get_cmdline();
        print(cmdline);

        auto modules = pfs.get_modules();
        print(modules);

        auto filesystems = pfs.get_filesystems();
        print(filesystems);

        auto loadavg = pfs.get_loadavg();
        print(loadavg);

        auto uptime = pfs.get_uptime();
        print(uptime);

        auto stats = pfs.get_stat();
        print(stats);

        auto meminfo = pfs.get_meminfo();
        print(meminfo);

        auto version = pfs.get_version();
        print(version);

        auto version_signature = pfs.get_version_signature();
        print(version_signature);

        auto controllers = pfs.get_cgroups();
        print(controllers);
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing system info:");
        LOG(TAB << ex.what());
    }

    return 0;
}
