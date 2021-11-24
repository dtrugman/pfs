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

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

template <typename T>
void to_number(const std::string& value, T& out,
               utils::base base = utils::base::decimal)
{
    try
    {
        utils::stot(value, out, base);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted number - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted number - Out of range", value);
    }
}

void parse_name(const std::string& value, task_status& out)
{
    out.name = value;
}

void parse_umask(const std::string& value, task_status& out)
{
    to_number(value, out.umask);
}

void parse_state(const std::string& value, task_status& out)
{
    // Format
    // clang-format off
    // State:  S (sleeping)
    //         ^
    //         |
    // Index:  0
    // clang-format on

    if (value.empty())
    {
        throw parser_error("Corrupted state - Empty value", value);
    }

    out.state = parse_task_state(value[0]);
}

void parse_tgid(const std::string& value, task_status& out)
{
    to_number(value, out.tgid);
}

void parse_ngid(const std::string& value, task_status& out)
{
    to_number(value, out.ngid);
}

void parse_pid(const std::string& value, task_status& out)
{
    to_number(value, out.pid);
}

void parse_ppid(const std::string& value, task_status& out)
{
    to_number(value, out.ppid);
}

void parse_tracer_pid(const std::string& value, task_status& out)
{
    to_number(value, out.tracer_pid);
}

void to_uid_set(const std::string& value, task_status::uid_set& out)
{
    enum token
    {
        REAL       = 0,
        EFFECTIVE  = 1,
        SAVED_SET  = 2,
        FILESYSTEM = 3,
        COUNT
    };

    static const char DELIM = '\t';

    auto tokens = utils::split(value, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted uid set - Unexpected tokens count",
                           value);
    }

    try
    {
        task_status::uid_set set;

        utils::stot(tokens[REAL], set.real);
        utils::stot(tokens[EFFECTIVE], set.effective);
        utils::stot(tokens[SAVED_SET], set.saved_set);
        utils::stot(tokens[FILESYSTEM], set.filesystem);

        out = set;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted uid set - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted uid set - Out of range", value);
    }
}

void parse_uid(const std::string& value, task_status& out)
{
    to_uid_set(value, out.uid);
}

void parse_gid(const std::string& value, task_status& out)
{
    to_uid_set(value, out.gid);
}

void parse_fdsize(const std::string& value, task_status& out)
{
    to_number(value, out.fd_size);
}

void parse_groups(const std::string& value, task_status& out)
{
    // This is a valid state, not all users are attached to groups
    if (value.empty())
    {
        return;
    }

    try
    {
        auto tokens = utils::split(value);
        for (const auto& token : tokens)
        {
            uid_t group;
            utils::stot(token, group);
            out.groups.insert(group);
        }
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted groups - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted groups - Out of range", value);
    }
}

void to_ns_ids_vector(const std::string& value, std::vector<pid_t>& out)
{
    try
    {
        static const char DELIM = '\t';

        auto tokens = utils::split(value, DELIM);
        for (const auto& token : tokens)
        {
            pid_t id;
            utils::stot(token, id);
            out.push_back(id);
        }
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted id - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted id - Out of range", value);
    }
}
void parse_ns_tgid(const std::string& value, task_status& out)
{
    to_ns_ids_vector(value, out.ns_tgid);
}

void parse_ns_pid(const std::string& value, task_status& out)
{
    to_ns_ids_vector(value, out.ns_pid);
}

void parse_ns_pgid(const std::string& value, task_status& out)
{
    to_ns_ids_vector(value, out.ns_pgid);
}

void parse_ns_sid(const std::string& value, task_status& out)
{
    to_ns_ids_vector(value, out.ns_sid);
}

void to_memory_size(const std::string& value, size_t& out)
{
    enum token
    {
        SIZE  = 0,
        UNITS = 1,
        COUNT
    };

    auto tokens = utils::split(value);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted memory size - Unexpected tokens count",
                           value);
    }

    try
    {
        utils::stot(tokens[SIZE], out);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted memory size - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted memory size - Out of range", value);
    }
}

void parse_vm_peak(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_peak);
}

void parse_vm_size(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_size);
}

void parse_vm_lck(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_lck);
}

void parse_vm_pin(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_pin);
}

void parse_vm_hwm(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_hwm);
}

void parse_vm_rss(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_rss);
}

void parse_rss_anon(const std::string& value, task_status& out)
{
    to_memory_size(value, out.rss_anon);
}

void parse_rss_file(const std::string& value, task_status& out)
{
    to_memory_size(value, out.rss_file);
}

void parse_rss_shmem(const std::string& value, task_status& out)
{
    to_memory_size(value, out.rss_shmem);
}

void parse_vm_data(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_data);
}

void parse_vm_stk(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_stk);
}

void parse_vm_exe(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_exe);
}

void parse_vm_lib(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_lib);
}

void parse_vm_pte(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_pte);
}

void parse_vm_swap(const std::string& value, task_status& out)
{
    to_memory_size(value, out.vm_swap);
}

void parse_huge_tlb_pages(const std::string& value, task_status& out)
{
    to_memory_size(value, out.huge_tlb_pages);
}

void to_boolean(const std::string& value, bool& out)
{
    if (value.empty())
    {
        throw parser_error("Corrupted bool - Empty value", value);
    }

    char c = value[0];
    if (c == '1')
    {
        out = true;
    }
    else if (c == '0')
    {
        out = false;
    }
    else
    {
        throw parser_error("Corrupted bool - Unexpected value", value);
    }
}

void parse_core_dumping(const std::string& value, task_status& out)
{
    to_boolean(value, out.core_dumping);
}

void parse_threads(const std::string& value, task_status& out)
{
    to_number(value, out.threads);
}

void parse_sig_q(const std::string& value, task_status& out)
{
    enum token
    {
        SIGNALS_QUEUED = 0,
        SIGNALS_LIMIT  = 1,
        COUNT
    };

    static const char DELIM = '/';

    auto tokens = utils::split(value, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted sig queue - Unexpected tokens count",
                           value);
    }

    try
    {
        size_t queued;
        utils::stot(tokens[SIGNALS_QUEUED], queued);

        size_t limit;
        utils::stot(tokens[SIGNALS_LIMIT], limit);

        out.sig_q = std::make_pair(queued, limit);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted sig queue - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted sig queue - Out of range", value);
    }
}

void to_signal_mask(const std::string& value, signal_mask& mask)
{
    to_number(value, mask.raw, utils::base::hex);
}

void parse_sig_pnd(const std::string& value, task_status& out)
{
    to_signal_mask(value, out.sig_pnd);
}

void parse_shd_pnd(const std::string& value, task_status& out)
{
    to_signal_mask(value, out.shd_pnd);
}

void parse_sig_blk(const std::string& value, task_status& out)
{
    to_signal_mask(value, out.sig_blk);
}

void parse_sig_ign(const std::string& value, task_status& out)
{
    to_signal_mask(value, out.sig_ign);
}

void parse_sig_cgt(const std::string& value, task_status& out)
{
    to_signal_mask(value, out.sig_cgt);
}

void to_capabilities_mask(const std::string& value, capabilities_mask& mask)
{
    to_number(value, mask.raw, utils::base::hex);
}

void parse_cap_inh(const std::string& value, task_status& out)
{
    to_capabilities_mask(value, out.cap_inh);
}

void parse_cap_prm(const std::string& value, task_status& out)
{
    to_capabilities_mask(value, out.cap_prm);
}

void parse_cap_eff(const std::string& value, task_status& out)
{
    to_capabilities_mask(value, out.cap_eff);
}

void parse_cap_bnd(const std::string& value, task_status& out)
{
    to_capabilities_mask(value, out.cap_bnd);
}

void parse_cap_amb(const std::string& value, task_status& out)
{
    to_capabilities_mask(value, out.cap_amb);
}

void parse_no_new_privs(const std::string& value, task_status& out)
{
    to_boolean(value, out.no_new_privs);
}

void parse_seccomp(const std::string& value, task_status& out)
{
    unsigned numeric;
    to_number(value, numeric);

    task_status::seccomp mode = static_cast<task_status::seccomp>(numeric);
    if (mode < task_status::seccomp::disabled ||
        mode > task_status::seccomp::filter)
    {
        throw parser_error("Corrupted seccomp - Unexpected value", value);
    }

    out.seccomp_mode = mode;
}

void parse_voluntary_ctx_switches(const std::string& value, task_status& out)
{
    to_number(value, out.voluntary_ctxt_switches);
}

void parse_nonvoluntary_ctx_switches(const std::string& value, task_status& out)
{
    to_number(value, out.nonvoluntary_ctxt_switches);
}

} // anonymous namespace

const char task_status_parser::DELIM = ':';

// clang-format off
const task_status_parser::value_parsers task_status_parser::PARSERS = {
    { "Name", parse_name },
    { "Umask", parse_umask },
    { "State", parse_state },
    { "Tgid", parse_tgid },
    { "Ngid", parse_ngid },
    { "Pid", parse_pid },
    { "PPid", parse_ppid },
    { "TracerPid", parse_tracer_pid },
    { "Uid", parse_uid },
    { "Gid", parse_gid },
    { "FDSize", parse_fdsize },
    { "Groups", parse_groups },
    { "NStgid", parse_ns_tgid },
    { "NSpid", parse_ns_pid },
    { "NSpgid", parse_ns_pgid },
    { "NSsid", parse_ns_sid },
    { "VmPeak", parse_vm_peak },
    { "VmSize", parse_vm_size },
    { "VmLck", parse_vm_lck },
    { "VmPin", parse_vm_pin },
    { "VmHWM", parse_vm_hwm },
    { "VmRSS", parse_vm_rss },
    { "RssAnon", parse_rss_anon },
    { "RssFile", parse_rss_file },
    { "RssShmem", parse_rss_shmem },
    { "VmData", parse_vm_data },
    { "VmStk", parse_vm_stk },
    { "VmExe", parse_vm_exe },
    { "VmLib", parse_vm_lib },
    { "VmPTE", parse_vm_pte },
    { "VmSwap", parse_vm_swap },
    { "HugetlbPages", parse_huge_tlb_pages },
    { "CoreDumping", parse_core_dumping },
    { "Threads", parse_threads },
    { "SigQ", parse_sig_q },
    { "SigPnd", parse_sig_pnd },
    { "ShdPnd", parse_shd_pnd },
    { "SigBlk", parse_sig_blk },
    { "SigIgn", parse_sig_ign },
    { "SigCgt", parse_sig_cgt },
    { "CapInh", parse_cap_inh },
    { "CapPrm", parse_cap_prm },
    { "CapEff", parse_cap_eff },
    { "CapBnd", parse_cap_bnd },
    { "CapAmb", parse_cap_amb },
    { "NoNewPrivs", parse_no_new_privs },
    { "Seccomp", parse_seccomp },
    { "voluntary_ctxt_switches", parse_voluntary_ctx_switches },
    { "nonvoluntary_ctxt_switches", parse_nonvoluntary_ctx_switches }
};
// clang-format on

} // namespace parsers
} // namespace impl
} // namespace pfs
