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

#include <arpa/inet.h>
#include <sys/stat.h>

#include <sstream>

#include "log.hpp"

#include "pfs/procfs.hpp"
#include "pfs/version.hpp"

// =========================================================================
// General printing utilities
// =========================================================================

template <typename T>
std::string join(const T& container)
{
    if (container.empty())
    {
        return "";
    }

    std::ostringstream out;
    for (const auto& v : container)
    {
        out << v << ',';
    }
    auto out_str = out.str();
    out_str.pop_back();
    return out_str;
}

// =========================================================================
// Custom procfs type printing utilities
// =========================================================================

std::ostream& operator<<(std::ostream& out, const pfs::module::state state)
{
    switch (state)
    {
        case pfs::module::state::live:
            out << "Live";
            break;

        case pfs::module::state::loading:
            out << "Loading";
            break;

        case pfs::module::state::unloading:
            out << "Unloading";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const pfs::module& module)
{
    out << "name[" << module.name << "] ";
    out << "size[" << module.size << "] ";
    out << "instances[" << module.instances << "] ";
    out << "dependencies[" << join(module.dependencies) << "] ";
    out << "state[" << module.current_state << "] ";
    out << "offset[" << module.offset << "] ";
    out << std::boolalpha;
    out << "out_of_tree[" << module.is_out_of_tree << "] ";
    out << "unsigned[" << module.is_unsigned << "] ";
    return out;
}

std::ostream& operator<<(std::ostream& out, const pfs::load_average& load)
{
    out << "load[" << load.last_1min << ", " << load.last_5min << ", "
        << load.last_15min << "] ";
    out << "runnable_tasks[" << load.runnable_tasks << "] ";
    out << "total_tasks[" << load.total_tasks << "] ";
    out << "last_created_task[" << load.last_created_task << "] ";
    return out;
}

std::ostream& operator<<(std::ostream& out, const pfs::zone& zone)
{
    out << "zone[" << zone.name << "] ";
    out << "chunks[" << join(zone.chunks) << "] ";
    return out;
}

void print_system(const pfs::procfs& pfs)
{
    try
    {
        LOG("=========================================================");
        LOG("System");
        LOG("=========================================================");

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

        auto meminfo = pfs.get_meminfo();
        print(meminfo);

        auto version = pfs.get_version();
        print(version);

        auto version_signature = pfs.get_version_signature();
        print(version_signature);
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing system info:");
        LOG(TAB << ex.what());
    }
}

// =========================================================================
// Main
// =========================================================================

void usage(char* argv0)
{
    LOG("Sample application using pfs v"
        << PFS_VER_MAJOR << "." << PFS_VER_MINOR << "." << PFS_VER_PATCH);
    LOG("Usage: " << argv0 << " [args]...");
    LOG("");
    LOG("   system         Enumerate system-wide information");
    LOG("");
}

void safe_main(int argc, char** argv)
{
    static const std::string CMD_ENUM_SYSTEM("system");

    if (argc < 2)
    {
        usage(argv[0]);
        return;
    }

    pfs::procfs pfs;

    if (CMD_ENUM_SYSTEM.compare(argv[1]) == 0)
    {
        print_system(pfs);
        return;
    }
}

int main(int argc, char** argv)
{
    try
    {
        safe_main(argc, argv);
    }
    catch (const std::invalid_argument& ex)
    {
        usage(argv[0]);
    }
    catch (const std::exception& ex)
    {
        LOG(ex.what());
    }

    return 0;
}
