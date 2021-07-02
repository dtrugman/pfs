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

#ifndef PFS_PROCFS_HPP
#define PFS_PROCFS_HPP

#include <unistd.h>

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "task.hpp"
#include "types.hpp"

namespace pfs {

class procfs final
{
public:
    static const std::string DEFAULT_ROOT;

public:
    procfs(const std::string& root = DEFAULT_ROOT);

    procfs(const procfs&) = default;
    procfs(procfs&&)      = default;

    procfs& operator=(const procfs&) = delete;
    procfs& operator=(procfs&&) = delete;

public: // Task API
    task get_task(int task_id = getpid()) const;
    std::set<task> get_processes() const;

public: // Network API
    net get_net(int task_id = getpid()) const;

public: // System API
    std::vector<zone> get_buddyinfo() const;

    std::vector<cgroup_controller> get_cgroups() const;

    std::string get_cmdline() const;

    std::unordered_map<std::string, bool> get_filesystems() const;

    load_average get_loadavg() const;

    uptime get_uptime() const;

    proc_stat get_stat() const;

    std::unordered_map<std::string, size_t> get_meminfo() const;

    std::vector<module> get_modules() const;

    std::string get_version() const;

    std::string get_version_signature() const;

private: // Private utilities
    static std::string build_root(std::string root);
    static void validate_root(const std::string& root);

private:
    const std::string _root;
};

} // namespace pfs

#endif // PFS_PROCFS_HPP
