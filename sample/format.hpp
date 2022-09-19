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

#include <chrono>
#include <iomanip>
#include <sstream>

#include "pfs/procfs.hpp"

template <typename T>
inline std::string join(const T& container)
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

inline bool is_printable(uint8_t byte)
{
    static const uint8_t PRINTABLE_MIN = 0x21;
    static const uint8_t PRINTABLE_MAX = 0x7e;

    return PRINTABLE_MIN <= byte && byte <= PRINTABLE_MAX;
}

inline std::string hexlify(const std::vector<uint8_t>& buffer)
{
    std::ostringstream out;
    out << std::hex;

    for (const auto& v : buffer)
    {
        char c = is_printable(v) ? static_cast<char>(v) : '.';
        out << c;
    }

    return out.str();
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::net_socket::timer timer)
{
    switch (timer)
    {
        case pfs::net_socket::timer::none:
            out << "None";
            break;

        case pfs::net_socket::timer::retransmit:
            out << "Retransmit";
            break;

        case pfs::net_socket::timer::another:
            out << "Another";
            break;

        case pfs::net_socket::timer::time_wait:
            out << "Time-Wait";
            break;

        case pfs::net_socket::timer::zero_window:
            out << "Zero-Window";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::net_socket::net_state state)
{
    switch (state)
    {
        case pfs::net_socket::net_state::established:
            out << "Established";
            break;

        case pfs::net_socket::net_state::syn_sent:
            out << "Syn-Sent";
            break;

        case pfs::net_socket::net_state::syn_recv:
            out << "Syn-Recv";
            break;

        case pfs::net_socket::net_state::fin_wait1:
            out << "Fin-Wait1";
            break;

        case pfs::net_socket::net_state::fin_wait2:
            out << "Fin-Wait2";
            break;

        case pfs::net_socket::net_state::time_wait:
            out << "Time-Wait";
            break;

        case pfs::net_socket::net_state::close:
            out << "Close";
            break;

        case pfs::net_socket::net_state::close_wait:
            out << "Close-Wait";
            break;

        case pfs::net_socket::net_state::last_ack:
            out << "Last-Ack";
            break;

        case pfs::net_socket::net_state::listen:
            out << "Listen";
            break;

        case pfs::net_socket::net_state::closing:
            out << "Closing";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::ostream& operator<<(std::ostream& out, 
                                const pfs::net_device& device)
{
    out << "interface[" << device.interface << "] ";

    out << "rx_bytes[" << device.rx_bytes << "] ";
    out << "rx_packets[" << device.rx_packets << "] ";
    out << "rx_errs[" << device.rx_errs << "] ";
    out << "rx_drop[" << device.rx_drop << "] ";
    out << "rx_fifo[" << device.rx_fifo << "] ";
    out << "rx_frame[" << device.rx_frame << "] ";
    out << "rx_compressed[" << device.rx_compressed << "] ";
    out << "rx_multicast[" << device.rx_multicast << "] ";

    out << "tx_bytes[" << device.tx_bytes << "] ";
    out << "tx_packets[" << device.tx_packets << "] ";
    out << "tx_errs[" << device.tx_errs << "] ";
    out << "tx_drop[" << device.tx_drop << "] ";
    out << "tx_fifo[" << device.tx_fifo << "] ";
    out << "tx_colls[" << device.tx_colls << "] ";
    out << "tx_carrier[" << device.tx_carrier << "] ";
    out << "tx_compressed[" << device.tx_compressed << "] ";

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::net_socket& socket)
{
    out << "slot[" << socket.slot << "] ";
    out << "local[" << socket.local_ip.to_string() << ":" << socket.local_port
        << "] ";
    out << "remote[" << socket.remote_ip.to_string() << ":"
        << socket.remote_port << "] ";
    out << "state[" << socket.socket_net_state << "] ";
    out << "tx_queue[" << socket.tx_queue << "] ";
    out << "rx_queue[" << socket.rx_queue << "] ";
    out << "timer[" << socket.timer_active << "] ";
    out << "timer_expire[" << socket.timer_expire_jiffies << "] ";
    out << "retransmits[" << socket.retransmits << "] ";
    out << "uid[" << socket.uid << "] ";
    out << "timeouts[" << socket.timeouts << "] ";
    out << "inode[" << socket.inode << "] ";
    out << "ref_count[" << socket.ref_count << "] ";
    out << "skbuff[0x" << std::hex << socket.skbuff << std::dec << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::unix_socket::type type)
{
    switch (type)
    {
        case pfs::unix_socket::type::stream:
            out << "Stream";
            break;

        case pfs::unix_socket::type::datagram:
            out << "Datagram";
            break;

        case pfs::unix_socket::type::seqpacket:
            out << "SeqPacket";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::unix_socket::state state)
{
    switch (state)
    {
        case pfs::unix_socket::state::free:
            out << "Free";
            break;

        case pfs::unix_socket::state::unconnected:
            out << "Unconnected";
            break;

        case pfs::unix_socket::state::connecting:
            out << "Connecting";
            break;

        case pfs::unix_socket::state::connected:
            out << "Connected";
            break;

        case pfs::unix_socket::state::disconnecting:
            out << "Disconnecting";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::unix_socket& socket)
{
    out << "skbuff[0x" << std::hex << socket.skbuff << std::dec << "] ";
    out << "ref_count[" << socket.ref_count << "] ";
    out << "protocol[" << socket.protocol << "] ";
    out << "flags[" << socket.flags << "] ";
    out << "type[" << socket.socket_type << "] ";
    out << "state[" << socket.socket_state << "] ";
    out << "inode[" << socket.inode << "] ";
    out << "path[" << socket.path << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::netlink_socket& socket)
{
    out << "skbuff[0x" << std::hex << socket.skbuff << std::dec << "] ";
    out << "protocol[" << socket.protocol << "] ";
    out << "port_id[" << socket.port_id << "] ";
    out << "groups[" << std::hex << std::setw(8) << std::setfill('0')
        << socket.groups << std::dec << "] ";
    out << "rmem[" << socket.rmem << "] ";
    out << "wmem[" << socket.wmem << "] ";
    out << "dumping[" << std::boolalpha << socket.dumping << "] ";
    out << "ref_count[" << socket.ref_count << "] ";
    out << "drops[" << socket.drops << "] ";
    out << "inode[" << socket.inode << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::task_state state)
{
    switch (state)
    {
        case pfs::task_state::running:
            out << "Running";
            break;

        case pfs::task_state::sleeping:
            out << "Sleeping";
            break;

        case pfs::task_state::disk_sleep:
            out << "Disk-Sleep";
            break;

        case pfs::task_state::stopped:
            out << "Stopped";
            break;

        case pfs::task_state::tracing_stop:
            out << "Tracing-Stop";
            break;

        case pfs::task_state::zombie:
            out << "Zombie";
            break;

        case pfs::task_state::dead:
            out << "Dead";
            break;

        case pfs::task_state::wakekill:
            out << "Wake-Kill";
            break;

        case pfs::task_state::waking:
            out << "Waking";
            break;

        case pfs::task_state::parked:
            out << "Parked";
            break;

        case pfs::task_state::idle:
            out << "Idle";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::task_status::uid_set& set)
{
    out << set.real << ',';
    out << set.effective << ',';
    out << set.saved_set << ',';
    out << set.filesystem;
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::task_status::seccomp seccomp)
{
    switch (seccomp)
    {
        case pfs::task_status::seccomp::disabled:
            out << "Disabled";
            break;

        case pfs::task_status::seccomp::strict:
            out << "Strict";
            break;

        case pfs::task_status::seccomp::filter:
            out << "Filter";
            break;

        default:
            out << "Unknown";
            break;
    }

    return out;
}

inline std::string to_octal_mask(uint64_t mask)
{
    std::ostringstream out;
    out << std::oct << std::setfill('0') << std::setw(4) << mask;
    return out.str();
}

inline std::string to_hex_mask(uint64_t mask)
{
    std::ostringstream out;
    out << std::hex << std::setfill('0') << std::setw(16) << mask;
    return out.str();
}

inline std::ostream& operator<<(std::ostream& out, const pfs::task_status& st)
{
    out << "name[" << st.name << "] ";
    out << "umask[" << to_octal_mask(st.umask) << "] ";
    out << "state[" << st.state << "] ";
    out << "tgid[" << st.tgid << "] ";
    out << "ngid[" << st.ngid << "] ";
    out << "pid[" << st.pid << "] ";
    out << "ppid[" << st.ppid << "] ";
    out << "tracer_pid[" << st.tracer_pid << "] ";
    out << "uid[" << st.uid << "] ";
    out << "gid[" << st.gid << "] ";
    out << "fdsize[" << st.fd_size << "] ";
    out << "groups[" << join(st.groups) << "] ";
    out << "ns_tgid[" << join(st.ns_tgid) << "] ";
    out << "ns_pid[" << join(st.ns_pid) << "] ";
    out << "ns_pgid[" << join(st.ns_pgid) << "] ";
    out << "ns_sid[" << join(st.ns_sid) << "] ";
    out << "vm_peak[" << st.vm_peak << "] ";
    out << "vm_size[" << st.vm_size << "] ";
    out << "vm_lck[" << st.vm_lck << "] ";
    out << "vm_pin[" << st.vm_pin << "] ";
    out << "vm_hwm[" << st.vm_hwm << "] ";
    out << "vm_rss[" << st.vm_rss << "] ";
    out << "rss_anon[" << st.rss_anon << "] ";
    out << "rss_file[" << st.rss_file << "] ";
    out << "rss_shmem[" << st.rss_shmem << "] ";
    out << "vm_data[" << st.vm_data << "] ";
    out << "vm_stk[" << st.vm_stk << "] ";
    out << "vm_exe[" << st.vm_exe << "] ";
    out << "vm_lib[" << st.vm_lib << "] ";
    out << "vm_pte[" << st.vm_pte << "] ";
    out << "vm_swap[" << st.vm_swap << "] ";
    out << "huge_tlb_pages[" << st.huge_tlb_pages << "] ";
    out << "core_dumping[" << std::boolalpha << st.core_dumping
        << std::noboolalpha << "] ";
    out << "threads[" << st.threads << "] ";
    out << "sig_q[" << st.sig_q.first << "/" << st.sig_q.second << "] ";
    out << "sig_pnd[" << to_hex_mask(st.sig_pnd.raw) << "] ";
    out << "shd_pnd[" << to_hex_mask(st.shd_pnd.raw) << "] ";
    out << "sig_blk[" << to_hex_mask(st.sig_blk.raw) << "] ";
    out << "sig_ign[" << to_hex_mask(st.sig_ign.raw) << "] ";
    out << "sig_cgt[" << to_hex_mask(st.sig_cgt.raw) << "] ";
    out << "cap_inh[" << to_hex_mask(st.cap_inh.raw) << "] ";
    out << "cap_prm[" << to_hex_mask(st.cap_prm.raw) << "] ";
    out << "cap_eff[" << to_hex_mask(st.cap_eff.raw) << "] ";
    out << "cap_bnd[" << to_hex_mask(st.cap_bnd.raw) << "] ";
    out << "cap_amb[" << to_hex_mask(st.cap_amb.raw) << "] ";
    out << "no_new_privs[" << std::boolalpha << st.no_new_privs
        << std::noboolalpha << "] ";
    out << "seccomp[" << st.seccomp_mode << "] ";
    out << "voluntary_ctxt_switches[" << st.voluntary_ctxt_switches << "] ";
    out << "nonvoluntary_ctxt_switches[" << st.nonvoluntary_ctxt_switches
        << "] ";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::task_stat& st)
{
    out << "pid[" << st.pid << "] ";
    out << "comm[" << st.comm << "] ";
    out << "state[" << st.state << "] ";
    out << "ppid[" << st.ppid << "] ";
    out << "pgrp[" << st.pgrp << "] ";
    out << "session[" << st.session << "] ";
    out << "tty_nr[" << st.tty_nr << "] ";
    out << "tgpid[" << st.tgpid << "] ";
    out << "flags[" << st.flags << "] ";
    out << "minflt[" << st.minflt << "] ";
    out << "cminflt[" << st.cminflt << "] ";
    out << "majflt[" << st.majflt << "] ";
    out << "cmajflt[" << st.cmajflt << "] ";
    out << "utime[" << st.utime << "] ";
    out << "stime[" << st.stime << "] ";
    out << "cutime[" << st.cutime << "] ";
    out << "cstime[" << st.cstime << "] ";
    out << "priority[" << st.priority << "] ";
    out << "nice[" << st.nice << "] ";
    out << "num_threads[" << st.num_threads << "] ";
    out << "itrealvalue[" << st.itrealvalue << "] ";
    out << "starttime[" << st.starttime << "] ";
    out << "vsize[" << st.vsize << "] ";
    out << "rss[" << st.rss << "] ";
    out << "rsslim[" << st.rsslim << "] ";
    out << "startcode[" << st.startcode << "] ";
    out << "endcode[" << st.endcode << "] ";
    out << "startstack[" << st.startstack << "] ";
    out << "kstkesp[" << st.kstkesp << "] ";
    out << "kstkeip[" << st.kstkeip << "] ";
    out << "signal[" << st.signal << "] ";
    out << "blocked[" << st.blocked << "] ";
    out << "sigignore[" << st.sigignore << "] ";
    out << "sigcatch[" << st.sigcatch << "] ";
    out << "wchan[" << st.wchan << "] ";
    out << "nswap[" << st.nswap << "] ";
    out << "cnswap[" << st.cnswap << "] ";
    out << "exit_signal[" << st.exit_signal << "] ";
    out << "processor[" << st.processor << "] ";
    out << "rt_priority[" << st.rt_priority << "] ";
    out << "policy[" << st.policy << "] ";
    out << "delayacct_blkio_ticks[" << st.delayacct_blkio_ticks << "] ";
    out << "guest_time[" << st.guest_time << "] ";
    out << "cguest_time[" << st.cguest_time << "] ";
    out << "start_data[" << st.start_data << "] ";
    out << "end_data[" << st.end_data << "] ";
    out << "start_brk[" << st.start_brk << "] ";
    out << "arg_start[" << st.arg_start << "] ";
    out << "arg_end[" << st.arg_end << "]";
    out << "env_start[" << st.env_start << "] ";
    out << "env_end[" << st.env_end << "]";
    out << "exit_code[" << st.exit_code << "]";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::mem_stats& mem)
{
    out << "total[" << mem.total << "] ";
    out << "resident[" << mem.resident << "] ";
    out << "shared[" << mem.shared << "] ";
    out << "text[" << mem.text << "] ";
    out << "data[" << mem.data << "]";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::io_stats& io)
{
    out << "rchar[" << io.rchar << "] ";
    out << "wchar[" << io.wchar << "] ";
    out << "syscr[" << io.syscr << "] ";
    out << "syscw[" << io.syscw << "] ";
    out << "read_bytes[" << io.read_bytes << "] ";
    out << "write_bytes[" << io.write_bytes << "] ";
    out << "cancelled_write_bytes[" << io.cancelled_write_bytes << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::mem_perm& perm)
{
    out << (perm.can_read ? 'r' : '-');
    out << (perm.can_write ? 'w' : '-');
    out << (perm.can_execute ? 'x' : '-');
    out << (perm.is_shared ? 's' : 'p');
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::mem_region& region)
{
    out << std::hex;
    out << "addr[0x" << region.start_address << "]-[0x" << region.end_address
        << "] ";
    out << "perm[" << region.perm << "]";
    out << "offset[0x" << region.offset << "] ";
    out << "device[" << region.device << "] ";
    out << std::dec;
    out << "inode[" << region.inode << "] ";
    out << "pathname[" << region.pathname << "]";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::mount& mount)
{
    out << "id[" << mount.id << "] ";
    out << "parent_id[" << mount.parent_id << "] ";
    out << "device[" << mount.device << "] ";
    out << "root[" << mount.root << "] ";
    out << "point[" << mount.point << "] ";
    out << "options[" << join(mount.options) << "] ";
    out << "optional[" << join(mount.optional) << "] ";
    out << "fs[" << mount.filesystem_type << "] ";
    out << "source[" << mount.source << "] ";
    out << "super_options[" << join(mount.super_options) << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::module::state state)
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

inline std::ostream& operator<<(std::ostream& out, const pfs::module& module)
{
    out << "name[" << module.name << "] ";
    out << "size[" << module.size << "] ";
    out << "instances[" << module.instances << "] ";
    out << "dependencies[" << join(module.dependencies) << "] ";
    out << "state[" << module.module_state << "] ";
    out << "offset[" << module.offset << "] ";
    out << std::boolalpha;
    out << "out_of_tree[" << module.is_out_of_tree << "] ";
    out << "unsigned[" << module.is_unsigned << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::load_average& load)
{
    out << "load[" << load.last_1min << ", " << load.last_5min << ", "
        << load.last_15min << "] ";
    out << "runnable_tasks[" << load.runnable_tasks << "] ";
    out << "total_tasks[" << load.total_tasks << "] ";
    out << "last_created_task[" << load.last_created_task << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::uptime& uptime)
{
    auto system_time_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(uptime.system_time)
            .count();
    out << "system_time[" << system_time_seconds << "s] ";

    auto idle_time_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(uptime.idle_time)
            .count();
    out << "idle_time[" << idle_time_seconds << "s] ";
    return out;
}
inline std::ostream& operator<<(std::ostream& out,
                                const pfs::proc_stat::cpu cpu)
{
    out << "user[" << cpu.user << "] ";
    out << "nice[" << cpu.nice << "] ";
    out << "system[" << cpu.system << "] ";
    out << "idle[" << cpu.idle << "] ";
    out << "iowait[" << cpu.iowait << "] ";
    out << "irq[" << cpu.irq << "] ";
    out << "softirq[" << cpu.softirq << "] ";
    out << "steal[" << cpu.steal << "] ";
    out << "guest[" << cpu.guest << "] ";
    out << "guest_nice[" << cpu.guest_nice << "]";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::proc_stat& stats)
{
    out << "stat_cpu_total";
    out << "[" << stats.cpus.total << "]\n";
    for (size_t i = 0; i < stats.cpus.per_item.size(); i++)
    {
        out << "stat_cpu" << i;
        out << "[" << stats.cpus.per_item[i] << "]\n";
    }

    out << "stat_intr[";
    out << "total: " << stats.intr.total << ", ";
    out << "per_interrupt: " << join(stats.intr.per_item);
    out << "]\n";

    out << "stat_ctxt[" << stats.ctxt << "] ";

    auto boot_seconds =
        std::chrono::time_point_cast<std::chrono::seconds>(stats.btime);
    out << "stat_btime[" << boot_seconds.time_since_epoch().count() << "] ";

    out << "stat_processes[" << stats.processes << "] ";
    out << "stat_procs_running[" << stats.procs_running << "] ";
    out << "stat_procs_blocked[" << stats.procs_blocked << "]\n";
    out << "stat_softirq[";
    out << "total: " << stats.softirq.total << ", ";
    out << "per_irq: " << join(stats.softirq.per_item);
    out << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::zone& zone)
{
    out << "zone[" << zone.name << "] ";
    out << "chunks[" << join(zone.chunks) << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::fd& fd)
{
    out << "target[" << fd.get_target() << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const pfs::cgroup_controller& controller)
{
    out << std::boolalpha;
    out << "subsys_name[" << controller.subsys_name << "] ";
    out << "hierarchy[" << controller.hierarchy << "] ";
    out << "num_cgroups[" << controller.num_cgroups << "] ";
    out << "enabled[" << controller.enabled << "] ";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const pfs::cgroup& cg)
{
    out << "hierarchy[" << cg.hierarchy << "] ";
    out << "controllers[" << join(cg.controllers) << "] ";
    out << "pathname[" << cg.pathname << "] ";
    return out;
}
