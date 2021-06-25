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
#include <cstring>

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

template <typename T>
static void to_number(const std::string& value, T& out,
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

static void to_cpu(const std::string& value, proc_stat::cpu& out)
{
    // Some examples:
    // clang-format off
    // 21497341 899627 8830588 433191163 93490 0 1844976 0 0 0
    // 2684811 115236 1094082 54162041 10674 0 890071 0 0 0
    // clang-format on

    enum token
    {
        USER       = 0,
        NICE       = 1,
        SYSTEM     = 2,
        IDLE       = 3,
        MIN_COUNT  = 4,
        IOWAIT     = 4,
        IRQ        = 5,
        SOFTIRQ    = 6,
        STEAL      = 7,
        GUEST      = 8,
        GUEST_NICE = 9,
        COUNT
    };

    auto tokens = utils::split(value);
    if (tokens.size() < MIN_COUNT || tokens.size() > COUNT)
    {
        throw parser_error("Corrupted cpu - Unexpected tokens count", value);
    }

    try
    {
        utils::stot(tokens[USER], out.user);
        utils::stot(tokens[NICE], out.nice);
        utils::stot(tokens[SYSTEM], out.system);
        utils::stot(tokens[IDLE], out.idle);

        if (tokens.size() > IOWAIT)
        {
            utils::stot(tokens[IOWAIT], out.iowait);
        }

        if (tokens.size() > IRQ)
        {
            utils::stot(tokens[IRQ], out.irq);
        }

        if (tokens.size() > SOFTIRQ)
        {
            utils::stot(tokens[SOFTIRQ], out.softirq);
        }

        if (tokens.size() > STEAL)
        {
            utils::stot(tokens[STEAL], out.steal);
        }

        if (tokens.size() > GUEST)
        {
            utils::stot(tokens[GUEST], out.guest);
        }

        if (tokens.size() > GUEST_NICE)
        {
            utils::stot(tokens[GUEST_NICE], out.guest_nice);
        }
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted cpu - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted cpu - Out of range", value);
    }
}

static void parse_cpu_total(const std::string& value, proc_stat& out)
{
    proc_stat::cpu cpu;
    to_cpu(value, cpu);

    out.cpus.total = cpu;
}

static void parse_cpu_single(const std::string& value, proc_stat& out)
{
    proc_stat::cpu cpu;
    to_cpu(value, cpu);

    out.cpus.per_item.push_back(cpu);
}

static void parse_intr(const std::string& value, proc_stat& out)
{
    to_sequence(value, out.intr);
}

static void parse_ctxt(const std::string& value, proc_stat& out)
{
    to_number(value, out.ctxt);
}

static void parse_btime(const std::string& value, proc_stat& out)
{
    time_t btime;
    to_number(value, btime);
    out.btime = std::chrono::system_clock::from_time_t(btime);
}

static void parse_processes(const std::string& value, proc_stat& out)
{
    to_number(value, out.processes);
}

static void parse_procs_running(const std::string& value, proc_stat& out)
{
    to_number(value, out.procs_running);
}

static void parse_procs_blocked(const std::string& value, proc_stat& out)
{
    to_number(value, out.procs_blocked);
}

static void parse_softirq(const std::string& value, proc_stat& out)
{
    to_sequence(value, out.softirq);
}

} // anonymous namespace

const char proc_stat_parser::DELIM = ' ';

void proc_stat_parser::key_remap(std::string& key)
{
    if (key == "cpu")
    {
        key = "cpu_total";
    }
    else if (key.rfind("cpu", 0) == 0)
    {
        key = "cpu_single";
    }
}

// clang-format off
const proc_stat_parser::value_parsers proc_stat_parser::PARSERS = {
    { "cpu_total", parse_cpu_total },
    { "cpu_single", parse_cpu_single },
    { "intr", parse_intr },
    { "ctxt", parse_ctxt },
    { "btime", parse_btime },
    { "processes", parse_processes },
    { "procs_running", parse_procs_running },
    { "procs_blocked", parse_procs_blocked },
    { "softirq", parse_softirq },
};
// clang-format on

} // namespace parsers
} // namespace impl
} // namespace pfs
