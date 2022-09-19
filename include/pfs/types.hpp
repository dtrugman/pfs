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

#ifndef PFS_TYPES_HPP
#define PFS_TYPES_HPP

#include <sys/types.h>

#include <array>
#include <chrono>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace pfs {

static const uid_t INVALID_UID   = (uid_t)-1;
static const pid_t INVALID_PID   = (pid_t)-1;
static const ino_t INVALID_INODE = (ino_t)0;

// Note: We only support values that exist post 2.6.32.
enum class task_state
{
    running,
    sleeping,
    disk_sleep,
    stopped,
    tracing_stop,
    zombie,
    dead,
    wakekill,
    waking,
    parked,
    idle,
};

struct task_stat
{
    pid_t pid = INVALID_PID;
    std::string comm;
    task_state state             = task_state::idle;
    pid_t ppid                   = INVALID_PID;
    pid_t pgrp                   = INVALID_PID;
    int session                  = 0;
    int tty_nr                   = 0;
    pid_t tgpid                  = INVALID_PID;
    unsigned flags               = 0;
    unsigned long minflt         = 0;
    unsigned long cminflt        = 0;
    unsigned long majflt         = 0;
    unsigned long cmajflt        = 0;
    unsigned long utime          = 0;
    unsigned long stime          = 0;
    long cutime                  = 0;
    long cstime                  = 0;
    long priority                = 0;
    long nice                    = 0;
    long num_threads             = 0;
    long itrealvalue             = 0;
    unsigned long long starttime = 0;
    unsigned long vsize          = 0; // In bytes
    long rss                     = 0; // In pages
    unsigned long rsslim         = 0;
    unsigned long startcode      = 0; // Affected by ptrace access mode
    unsigned long endcode        = 0; // Affected by ptrace access mode
    unsigned long startstack     = 0; // Affected by ptrace access mode
    unsigned long kstkesp        = 0; // Affected by ptrace access mode
    unsigned long kstkeip        = 0; // Affected by ptrace access mode
    unsigned long signal         = 0;
    unsigned long blocked        = 0;
    unsigned long sigignore      = 0;
    unsigned long sigcatch       = 0;
    unsigned long wchan          = 0; // Affected by ptrace access mode
    unsigned long nswap          = 0;
    unsigned long cnswap         = 0;
    int exit_signal              = 0;             // Since 2.1.22
    int processor                = 0;             // Since 2.2.8
    unsigned rt_priority         = 0;             // Since 2.5.19
    unsigned policy              = 0;             // Since 2.5.19
    unsigned long long delayacct_blkio_ticks = 0; // Since 2.6.18
    unsigned long guest_time                 = 0; // Since 2.6.24
    long cguest_time                         = 0; // Since 2.6.24
    unsigned long start_data = 0; // Since 3.3, Affected by ptrace access mode
    unsigned long end_data   = 0; // Since 3.3, Affected by ptrace access mode
    unsigned long start_brk  = 0; // Since 3.3, Affected by ptrace access mode
    unsigned long arg_start  = 0; // Since 3.5, Affected by ptrace access mode
    unsigned long arg_end    = 0; // Since 3.5, Affected by ptrace access mode
    unsigned long env_start  = 0; // Since 3.5, Affected by ptrace access mode
    unsigned long env_end    = 0; // Since 3.5, Affected by ptrace access mode
    unsigned long exit_code  = 0; // Since 3.5, Affected by ptrace access mode
};

struct io_stats {
    unsigned long rchar;
    unsigned long wchar;
    unsigned long syscr;
    unsigned long syscw;
    unsigned long read_bytes;
    unsigned long write_bytes;
    unsigned long cancelled_write_bytes;
};

enum class capability
{
    chown            = 0,
    dac_override     = 1,
    dac_read_search  = 2,
    fowner           = 3,
    fsetid           = 4,
    kill             = 5,
    setgid           = 6,
    setuid           = 7,
    setpcap          = 8,
    linux_immutable  = 9,
    net_bind_service = 10,
    net_broadcast    = 11,
    net_admin        = 12,
    net_raw          = 13,
    ipc_lock         = 14,
    ipc_owner        = 15,
    sys_module       = 16,
    sys_rawio        = 17,
    sys_chroot       = 18,
    sys_ptrace       = 19,
    sys_pacct        = 20,
    sys_admin        = 21,
    sys_boot         = 22,
    sys_nice         = 23,
    sys_resource     = 24,
    sys_time         = 25,
    sys_tty_config   = 26,
    mknod            = 27,
    lease            = 28,
    audit_write      = 29,
    audit_control    = 30,
    setfcap          = 31,
    mac_override     = 32,
    mac_admin        = 33,
    syslog           = 34,
    wake_alarm       = 35,
    block_suspend    = 36,
};

struct capabilities_mask
{
    using raw_type = uint64_t;

    explicit capabilities_mask(raw_type raw = 0);

    bool is_set(capability b);

    bool operator==(const capabilities_mask& rhs) const;

    raw_type raw;
};

// Signal numbers on x86/ARM and most other architectures
enum class signal
{
    sighup    = 1,
    sigint    = 2,
    sigquit   = 3,
    sigill    = 4,
    sigtrap   = 5,
    sigabrt   = 6,
    sigiot    = sigabrt,
    sigbus    = 7,
    sigfpe    = 8,
    sigkill   = 9,
    sigusr1   = 10,
    sigsegv   = 11,
    sigusr2   = 12,
    sigpipe   = 13,
    sigalrm   = 14,
    sigterm   = 15,
    sigstkflt = 16,
    sigchld   = 17,
    sigcont   = 18,
    sigstop   = 19,
    sigtstp   = 20,
    sigttin   = 21,
    sigttou   = 22,
    sigurg    = 23,
    sigxcpu   = 24,
    sigxfsz   = 25,
    sigvtalrm = 26,
    sigprof   = 27,
    sigwinch  = 28,
    sigio     = 29,
    sigpoll   = sigio,
    sigpwr    = 30,
    sigsys    = 31,
    sigunused = sigsys,
};

struct signal_mask
{
    using raw_type = uint64_t;

    explicit signal_mask(raw_type raw = 0);

    bool is_set(signal b);

    bool operator==(const signal_mask& rhs) const;

    raw_type raw;
};

struct task_status
{
    enum class seccomp
    {
        disabled = 0,
        strict   = 1,
        filter   = 2,
    };

    struct uid_set
    {
        uid_t real       = INVALID_UID;
        uid_t effective  = INVALID_UID;
        uid_t saved_set  = INVALID_UID;
        uid_t filesystem = INVALID_UID;

        bool operator==(const uid_set& rhs) const
        {
            return real == rhs.real && effective == rhs.effective &&
                   saved_set == rhs.saved_set && filesystem == rhs.filesystem;
        }
    };

    std::string name;
    mode_t umask     = 0;
    task_state state = task_state::running;
    pid_t tgid       = INVALID_PID;
    pid_t ngid       = INVALID_PID;
    pid_t pid        = INVALID_PID;
    pid_t ppid       = INVALID_PID;
    pid_t tracer_pid = INVALID_PID;
    uid_set uid;
    uid_set gid;
    size_t fd_size = 0;
    std::set<uid_t> groups;
    std::vector<pid_t> ns_tgid;
    std::vector<pid_t> ns_pid;
    std::vector<pid_t> ns_pgid;
    std::vector<pid_t> ns_sid;
    size_t vm_peak                  = 0; // In kB
    size_t vm_size                  = 0; // In kB
    size_t vm_lck                   = 0; // In kB
    size_t vm_pin                   = 0; // In kB
    size_t vm_hwm                   = 0; // In kB
    size_t vm_rss                   = 0; // In kB
    size_t rss_anon                 = 0; // In kB
    size_t rss_file                 = 0; // In kB
    size_t rss_shmem                = 0; // In kB
    size_t vm_data                  = 0; // In kB
    size_t vm_stk                   = 0; // In kB
    size_t vm_exe                   = 0; // In kB
    size_t vm_lib                   = 0; // In kB
    size_t vm_pte                   = 0; // In kB
    size_t vm_swap                  = 0; // In kB
    size_t huge_tlb_pages           = 0; // In kB
    bool core_dumping               = false;
    size_t threads                  = 1;
    std::pair<size_t, size_t> sig_q = {0, 0};
    signal_mask sig_pnd;
    signal_mask shd_pnd;
    signal_mask sig_blk;
    signal_mask sig_ign;
    signal_mask sig_cgt;
    capabilities_mask cap_inh;
    capabilities_mask cap_prm;
    capabilities_mask cap_eff;
    capabilities_mask cap_bnd;
    capabilities_mask cap_amb;
    bool no_new_privs    = false;
    seccomp seccomp_mode = seccomp::disabled;
    // Speculation_Store_Bypass will be added upon request.
    // Cpus_allowed[_list] will be added upon request.
    // Mems_allowed[_list] will be added upon request.
    size_t voluntary_ctxt_switches    = 0;
    size_t nonvoluntary_ctxt_switches = 0;
};

struct mem_stats
{
    // Note: All values are in pages!

    size_t total;
    size_t resident;
    size_t shared;
    size_t text;
    size_t data;
};

struct mem_perm
{
    bool can_read    = false;
    bool can_write   = false;
    bool can_execute = false;
    bool is_shared   = false;
    bool is_private  = false; // Is copy on write
};

struct mem_region
{
    size_t start_address = 0;
    size_t end_address   = 0;
    mem_perm perm;
    size_t offset = 0;
    dev_t device  = 0;
    ino_t inode   = INVALID_INODE;
    std::string pathname;

    bool operator<(const mem_region& rhs) const
    {
        return start_address < rhs.start_address;
    }
};

struct module
{
    enum class state
    {
        live      = 0,
        loading   = 1,
        unloading = 2,
    };

    std::string name;
    size_t size;
    size_t instances;
    std::vector<std::string> dependencies;
    state module_state;
    size_t offset;
    bool is_out_of_tree;
    bool is_unsigned;

    bool operator<(const module& rhs) const { return name < rhs.name; }
};

struct uptime
{
    std::chrono::steady_clock::duration system_time;
    std::chrono::steady_clock::duration idle_time;
};

struct load_average
{
    double last_1min;
    double last_5min;
    double last_15min;
    int runnable_tasks;
    int total_tasks;
    int last_created_task;
};

struct proc_stat
{
    template <typename T>
    struct sequence
    {
        T total;
        std::vector<T> per_item;
    };

    struct cpu
    {
        unsigned long long user{0};
        unsigned long long nice{0};
        unsigned long long system{0};
        unsigned long long idle{0};
        unsigned long long iowait{0};
        unsigned long long irq{0};
        unsigned long long softirq{0};
        unsigned long long steal{0};
        unsigned long long guest{0};
        unsigned long long guest_nice{0};
    };

    sequence<cpu> cpus;
    sequence<unsigned long long> intr;
    unsigned long long ctxt;
    std::chrono::system_clock::time_point btime;
    unsigned long long processes;
    size_t procs_running;
    size_t procs_blocked;
    sequence<unsigned long long> softirq;
};

struct mount
{
    unsigned id;
    unsigned parent_id;
    dev_t device;
    std::string root;
    std::string point;
    std::vector<std::string> options;
    std::vector<std::string> optional;
    std::string filesystem_type;
    std::string source;
    std::vector<std::string> super_options;

    bool operator<(const mount& rhs) const { return id < rhs.id; }
};

struct zone
{
    size_t node_id;
    std::string name;

    static constexpr size_t CHUNKS_COUNT = 11;
    std::array<size_t, CHUNKS_COUNT> chunks;

    bool operator<(const zone& rhs) const
    {
        return (node_id < rhs.node_id) || (name < rhs.name);
    }
};

using ipv4 = uint32_t;
using ipv6 = std::array<uint32_t, 4>;
struct ip
{
    explicit ip();
    explicit ip(ipv4 addr);
    explicit ip(ipv6 addr);

    bool is_v4() const;
    bool is_v6() const;
    std::string to_string() const;

    bool operator==(const ip& rhs) const
    {
        return (domain == rhs.domain) && (storage == rhs.storage);
    }

    int domain;
    ipv6 storage;
};

struct net_device
{
    std::string interface;
    uint64_t rx_bytes;
    uint64_t rx_packets;
    uint64_t rx_errs;
    uint64_t rx_drop;
    uint64_t rx_fifo;
    uint64_t rx_frame;
    uint64_t rx_compressed;
    uint64_t rx_multicast;
    uint64_t tx_bytes;
    uint64_t tx_packets;
    uint64_t tx_errs;
    uint64_t tx_drop;
    uint64_t tx_fifo;
    uint64_t tx_colls;
    uint64_t tx_carrier;
    uint64_t tx_compressed;
};

// Hint: See 'get_tcp4_sock @ tcp_ipv4.c'
struct net_socket
{
    enum class timer
    {
        none       = 0, // No timer is pending
        retransmit = 1, // Retransmit-timer is pending
        another = 2, // Another timer (e.g. delayed ack or keepalive) is pending
        time_wait = 3,   // This is a socket in TIME_WAIT state. Not all fields
                         // will contain data (or even exist)
        zero_window = 4, // zero window probe timer is pending
    };

    enum class net_state
    {
        established = 1,
        syn_sent    = 2,
        syn_recv    = 3,
        fin_wait1   = 4,
        fin_wait2   = 5,
        time_wait   = 6,
        close       = 7,
        close_wait  = 8,
        last_ack    = 9,
        listen      = 10,
        closing     = 11,
    };

    size_t slot;
    ip local_ip;
    uint16_t local_port;
    ip remote_ip;
    uint16_t remote_port;
    net_state socket_net_state;
    size_t tx_queue;
    size_t rx_queue;
    timer timer_active;
    size_t timer_expire_jiffies;
    size_t retransmits;
    uid_t uid;
    size_t timeouts;
    ino_t inode;
    int ref_count;
    size_t skbuff;

    // NOTE: Additional fields will be added upon request

    bool operator<(const net_socket& rhs) const
    {
        return (skbuff < rhs.skbuff) || (inode < rhs.inode);
    }
};

// Hint: See 'unix_seq_show @ af_unix.c'
struct unix_socket
{
    enum class type
    {
        stream    = 1,
        datagram  = 2,
        seqpacket = 5,
    };

    // See the Kerne's 'socket_state' enum for more information
    enum class state
    {
        free          = 0, // Not allocated
        unconnected   = 1, // Unconnected to any socket
        connecting    = 2, // In process of connecting
        connected     = 3, // Connected to socket
        disconnecting = 4, // In process of disconnecting
    };

    size_t skbuff;
    int ref_count;
    int protocol;
    int flags;
    type socket_type;
    state socket_state;
    ino_t inode;
    std::string path;

    bool operator<(const unix_socket& rhs) const
    {
        return (skbuff < rhs.skbuff) || (inode < rhs.inode);
    }
};

struct netlink_socket
{
    size_t skbuff    = 0;
    int protocol     = 0;
    unsigned port_id = 0;
    unsigned groups  = 0;
    int rmem         = 0;
    int wmem         = 0;
    bool dumping     = false;
    int ref_count    = 0;
    unsigned drops   = 0;
    ino_t inode      = INVALID_INODE;

    bool operator<(const unix_socket& rhs) const
    {
        return (skbuff < rhs.skbuff) || (inode < rhs.inode);
    }
};

struct cgroup_controller
{
    std::string subsys_name;
    unsigned hierarchy;
    unsigned num_cgroups;
    bool enabled;
};

struct cgroup
{
    unsigned hierarchy;
    std::vector<std::string> controllers;
    std::string pathname;
};

struct id_map
{
    uid_t id_inside_ns = 0;
    uid_t id_outside_ns = 0;
    size_t length = UINT32_MAX;
};

} // namespace pfs

#endif // PFS_TYPES_HPP
