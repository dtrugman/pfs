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

#include <dirent.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <system_error>

#include "pfs/defer.hpp"
#include "pfs/parsers.hpp"
#include "pfs/task.hpp"
#include "pfs/utils.hpp"

namespace pfs {

using namespace impl;

task::task(const std::string& procfs_root, int id)
    : _id(id), _procfs_root(procfs_root),
      _task_root(build_task_root(procfs_root, id))
{}

std::string task::build_task_root(const std::string& procfs_root, int id)
{
    return procfs_root + std::to_string(id) + '/';
}

bool task::operator<(const task& rhs) const
{
    return _id < rhs._id;
}

bool task::is_kernel_thread(const task_stat& st)
{
    static const int ROOT_KERNEL_TASK_ID = 2;

    return st.pid == ROOT_KERNEL_TASK_ID || st.ppid == ROOT_KERNEL_TASK_ID;
}

int task::id() const
{
    return _id;
}

const std::string& task::dir() const
{
    return _task_root;
}

std::vector<cgroup> task::get_cgroups() const
{
    static const std::string CGROUP_FILE("cgroup");
    auto path = _task_root + CGROUP_FILE;

    std::vector<cgroup> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_cgroup_line);
    return output;
}

std::string task::get_exe() const
{
    static const std::string EXE_FILE("exe");
    auto path = _task_root + EXE_FILE;

    return utils::readlink(path);
}

std::string task::get_cwd() const
{
    static const std::string CWD_FILE("cwd");
    auto path = _task_root + CWD_FILE;

    return utils::readlink(path);
}

std::string task::get_root() const
{
    static const std::string ROOT_FILE("root");
    auto path = _task_root + ROOT_FILE;

    return utils::readlink(path);
}

std::string task::get_comm() const
{
    static const std::string COMM_FILE("comm");
    auto path = _task_root + COMM_FILE;

    return utils::readline(path);
}

std::vector<std::string> task::get_cmdline(size_t max_size) const
{
    static const std::string CMDLINE_FILE("cmdline");
    auto path = _task_root + CMDLINE_FILE;

    auto raw = utils::readfile(path, max_size);
    return utils::split(raw, '\0', true /* keep_empty */);
}

std::unordered_map<std::string, std::string>
task::get_environ(size_t max_size) const
{
    static const std::string ENVIRON_FILE("environ");
    auto path = _task_root + ENVIRON_FILE;

    auto raw    = utils::readfile(path, max_size);
    auto tokens = utils::split(raw, '\0');

    std::unordered_map<std::string, std::string> environ;
    for (const auto& token : tokens)
    {
        static const char KEY_VALUE_DELIM('=');
        size_t delim = token.find(KEY_VALUE_DELIM);
        if (delim != std::string::npos)
        {
            environ.emplace(token.substr(0, delim), token.substr(delim + 1));
        }
    }
    return environ;
}

io_stats task::get_io() const {
    static const std::string IO_FILE("io");
    auto path = _task_root + IO_FILE;

    return parsers::task_io_parser().parse(path);
}

task_stat task::get_stat() const
{
    static const std::string STAT_FILE("stat");
    auto path = _task_root + STAT_FILE;

    FILE* fp = fopen(path.c_str(), "r");
    if (!fp)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open file");
    }
    defer close_fp([fp] { fclose(fp); });

    task_stat st;

    // Consume PID and space delim
    int matches = fscanf(fp, "%d ", &st.pid);
    if (matches != 1)
    {
        throw std::runtime_error("Couldn't read pid from stat");
    }

    // Consume comm while making sure we handle correctly
    // - Values that contain paranthesis '(' and/or ')' inside the value
    // - Values that contain whitespaces inside the value

    long pre_comm = ftell(fp);

    static const size_t COMM_LEN_MAX = 18; // Actual comm (16) + parenthesis (2)
    st.comm.resize(COMM_LEN_MAX);
    size_t bytes = fread(&st.comm[0], 1, st.comm.size(), fp);
    if (bytes != st.comm.size())
    {
        throw std::runtime_error("Couldn't read comm from stat");
    }

    // Reassign comm as the text between the outmost pair of parenthesis
    st.comm = st.comm.substr(1, st.comm.find_last_of(')') - 1);

    // Skip past comm: Parentheses(2) + Actual comm + Space before next token(1)
    //     +|----|++
    // ... (<comm>) <next> ...
    //    ^
    // pre-comm
    static const size_t COMM_WRAP_SIZE = 3;
    if (fseek(fp, pre_comm + st.comm.size() + COMM_WRAP_SIZE, SEEK_SET) != 0)
    {
        throw std::runtime_error("Couldn't seek past comm in stat");
    }

    char state;

    matches = fscanf(
        fp,
        "%c "   // state
        "%d "   // ppid
        "%d "   // pgrp
        "%d "   // session
        "%d "   // tty_nr
        "%d "   // tgpid
        "%u "   // flags
        "%lu "  // minflt
        "%lu "  // cminflt
        "%lu "  // majflt
        "%lu "  // cmajflt
        "%lu "  // utime
        "%lu "  // stime
        "%ld "  // cutime
        "%ld "  // cstime
        "%ld "  // priority
        "%ld "  // nice
        "%ld "  // num_threads
        "%ld "  // itrealvalue
        "%llu " // starttime
        "%lu "  // vsize
        "%ld "  // rss
        "%lu "  // rsslim
        "%lu "  // startcode
        "%lu "  // endcode
        "%lu "  // startstack
        "%lu "  // kstkesp
        "%lu "  // kstkeip
        "%lu "  // signal
        "%lu "  // blocked
        "%lu "  // sigignore
        "%lu "  // sigcatch
        "%lu "  // wchan
        "%lu "  // nswap
        "%lu "  // cnswap
        "%d "   // exit_signal
        "%d "   // processor
        "%u "   // rt_priority
        "%u "   // policy
        "%llu " // delayacct_blkio_ticks
        "%lu "  // guest_time
        "%ld "  // cguest_time
        "%lu "  // start_data
        "%lu "  // end_data
        "%lu "  // start_brk
        "%lu "  // arg_start
        "%lu "  // arg_end
        "%lu "  // env_start
        "%lu "  // env_end
        "%lu ", // exit_code
        &state, &st.ppid, &st.pgrp, &st.session, &st.tty_nr, &st.tgpid,
        &st.flags, &st.minflt, &st.cminflt, &st.majflt, &st.cmajflt, &st.utime,
        &st.stime, &st.cutime, &st.cstime, &st.priority, &st.nice,
        &st.num_threads, &st.itrealvalue, &st.starttime, &st.vsize, &st.rss,
        &st.rsslim, &st.startcode, &st.endcode, &st.startstack, &st.kstkesp,
        &st.kstkeip, &st.signal, &st.blocked, &st.sigignore, &st.sigcatch,
        &st.wchan, &st.nswap, &st.cnswap, &st.exit_signal, &st.processor,
        &st.rt_priority, &st.policy, &st.delayacct_blkio_ticks, &st.guest_time,
        &st.cguest_time, &st.start_data, &st.end_data, &st.start_brk,
        &st.arg_start, &st.arg_end, &st.env_start, &st.env_end, &st.exit_code);

    // We must have at least everything till 'cnswap'
    static const int FIELDS_MIN = 35;
    if (matches < FIELDS_MIN)
    {
        throw std::runtime_error("Corrupted stat - Not enough tokens");
    }

    st.state = parsers::parse_task_state(state);

    return st;
}

mem_stats task::get_statm() const
{
    enum token
    {
        TOTAL    = 0,
        RESIDENT = 1,
        SHARED   = 2,
        TEXT     = 3,
        LIB      = 4,
        DATA     = 5,
        DIRTY    = 6,
        COUNT
    };

    static const std::string STATM_FILE("statm");
    auto path = _task_root + STATM_FILE;

    auto line   = utils::readline(path);
    auto tokens = utils::split(line);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted statm - Unexpected tokens count", line);
    }

    try
    {
        mem_stats ms;

        utils::stot(tokens[TOTAL], ms.total);
        utils::stot(tokens[RESIDENT], ms.resident);
        utils::stot(tokens[SHARED], ms.shared);
        utils::stot(tokens[TEXT], ms.text);
        // lib - unused since 2.6, should always be 0
        utils::stot(tokens[DATA], ms.data);
        // dirty pages - unused since 2.6, should always be 0

        return ms;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted statm - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted statm - Out of range", line);
    }
}

task_status task::get_status(const std::set<std::string>& keys) const
{
    static const std::string STATUS_FILE("status");
    auto path = _task_root + STATUS_FILE;

    return parsers::task_status_parser().parse(path, keys);
}

std::vector<mem_region> task::get_maps() const
{
    static const std::string MAPS_FILE("maps");
    auto path = _task_root + MAPS_FILE;

    std::vector<mem_region> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_maps_line);
    return output;
}

mem task::get_mem() const
{
    static const std::string MEM_FILE("mem");
    auto path = _task_root + MEM_FILE;

    return mem(path);
}

std::vector<mount> task::get_mountinfo() const
{
    static const std::string MOUNTINFO_FILE("mountinfo");
    auto path = _task_root + MOUNTINFO_FILE;

    std::vector<mount> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_mountinfo_line);
    return output;
}

size_t task::count_fds() const
{
    static const std::string FDS_DIR("fd/");
    auto path = _task_root + FDS_DIR;

    return utils::count_files(path);
}

std::unordered_map<int, fd> task::get_fds() const
{
    static const std::string FDS_DIR("fd/");
    auto path = _task_root + FDS_DIR;

    std::unordered_map<int, fd> fds;
    for (const auto& num : utils::enumerate_numeric_files(path))
    {
        fds.emplace(num, fd(path, num));
    }
    return fds;
}

net task::get_net() const
{
    return net(_procfs_root);
}

ino_t task::get_ns(const std::string& ns) const
{
    static const std::string NS_DIR("ns/");
    auto path = _task_root + NS_DIR + ns;

    return utils::get_inode(path);
}

std::unordered_map<std::string, ino_t> task::get_ns() const
{
    static const std::string NS_DIR("ns/");
    auto path = _task_root + NS_DIR;

    int dirfd = open(path.c_str(), O_DIRECTORY);
    if (dirfd == -1)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open ns directory");
    }
    defer close_dirfd([dirfd] { close(dirfd); });

    std::unordered_map<std::string, ino_t> ns;

    for (const auto& file :
         utils::enumerate_files(path, /* include_dots */ false))
    {
        ns.emplace(file, utils::get_inode(file, dirfd));
    }

    return ns;
}

task task::get_task(int id) const
{
    static const std::string TASKS_DIR("task/");
    auto path = _task_root + TASKS_DIR;

    // Important, see README note about collecting information
    // about threads to understand why we pass 'path' as the root dir.
    return task(path, id);
}

std::set<task> task::get_tasks() const
{
    static const std::string TASKS_DIR("task/");
    auto path = _task_root + TASKS_DIR;

    std::set<task> threads;

    for (auto thread_id : utils::enumerate_numeric_files(path))
    {
        // Important, see README note about collecting information
        // about threads to understand why we pass 'path' as the root dir.
        threads.emplace(task(path, thread_id));
    }

    return threads;
}

std::vector<id_map> task::get_uid_map() const
{
    static const std::string UID_MAP_FILE("uid_map");
    auto path = _task_root + UID_MAP_FILE;

    std::vector<id_map> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_id_map_line);
    return output;
}

std::vector<id_map> task::get_gid_map() const
{
    static const std::string GID_MAP_FILE("gid_map");
    auto path = _task_root + GID_MAP_FILE;

    std::vector<id_map> output;
    parsers::parse_lines(path, std::back_inserter(output),
                         parsers::parse_id_map_line);
    return output;
}

} // namespace pfs
