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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <system_error>

#include "pfs/parsers.hpp"
#include "pfs/procfs.hpp"
#include "pfs/utils.hpp"

namespace pfs {

using namespace impl;

const std::string procfs::DEFAULT_ROOT("/proc/");

procfs::procfs(const std::string& root) : _root(build_root(root))
{
    validate_root(root);
}

std::string procfs::build_root(std::string root)
{
    utils::ensure_dir_terminator(root);
    return root;
}

void procfs::validate_root(const std::string& root)
{
    struct stat st;
    int rv = stat(root.c_str(), &st);
    if (rv != 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't stat root");
    }

    if (!S_ISDIR(st.st_mode))
    {
        throw std::runtime_error("Specified procfs root is not a directory");
    }
}

task procfs::get_task(int task_id) const
{
    return task(_root, task_id);
}

std::set<task> procfs::get_processes() const
{
    std::set<task> tasks;
    for (auto task_id : utils::enumerate_numeric_files(_root))
    {
        tasks.emplace(get_task(task_id));
    }

    return tasks;
}

net procfs::get_net(int task_id) const
{
    return get_task(task_id).get_net();
}

std::vector<zone> procfs::get_buddyinfo() const
{
    static const std::string BUDDYINFO_FILE("buddyinfo");
    auto path = _root + BUDDYINFO_FILE;

    std::vector<zone> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_buddyinfo_line);
    return output;
}

std::vector<cgroup_controller> procfs::get_cgroups() const
{
    static const size_t HEADER_LINES = 1;

    static const std::string CGROUPS_FILE("cgroups");
    auto path = _root + CGROUPS_FILE;

    std::vector<cgroup_controller> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_cgroup_controller_line, HEADER_LINES);
    return output;
}

std::string procfs::get_cmdline() const
{
    static const std::string CMDLINE_FILE("cmdline");
    auto path = _root + CMDLINE_FILE;

    return utils::readline(path);
}

std::unordered_map<std::string, bool> procfs::get_filesystems() const
{
    static const std::string FILESYSTEMS_FILE("filesystems");
    auto path = _root + FILESYSTEMS_FILE;

    std::unordered_map<std::string, bool> output;
    parsers::parse_lines(path, std::inserter(output, output.begin()),
                         parsers::parse_filesystems_line);
    return output;
}

std::unordered_map<std::string, size_t> procfs::get_meminfo() const
{
    static const std::string MEMINFO_FILE("meminfo");
    auto path = _root + MEMINFO_FILE;

    std::unordered_map<std::string, size_t> output;
    parsers::parse_lines(path, std::inserter(output, output.begin()),
                         parsers::parse_meminfo_line);
    return output;
}

load_average procfs::get_loadavg() const
{
    static const std::string LOADAVG_FILE("loadavg");
    auto path = _root + LOADAVG_FILE;

    auto line = utils::readline(path);
    return parsers::parse_loadavg_line(line);
}

uptime procfs::get_uptime() const
{
    static const std::string UPTIME_FILE("uptime");
    auto path = _root + UPTIME_FILE;

    auto line = utils::readline(path);
    return parsers::parse_uptime_line(line);
}

proc_stat procfs::get_stat() const
{
    static const std::string STATUS_FILE("stat");
    auto path = _root + STATUS_FILE;

    return parsers::proc_stat_parser().parse(path);
}

std::vector<module> procfs::get_modules() const
{
    static const std::string MODULES_FILE("modules");
    auto path = _root + MODULES_FILE;

    std::vector<module> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_modules_line);
    return output;
}

std::string procfs::get_version() const
{
    static const std::string VERSION_FILE("version");
    auto path = _root + VERSION_FILE;

    return utils::readline(path);
}

std::string procfs::get_version_signature() const
{
    static const std::string VERSION_SIGNATURE_FILE("version_signature");
    auto path = _root + VERSION_SIGNATURE_FILE;

    return utils::readline(path);
}

} // namespace pfs
