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

#include "pfs/parsers/syscall.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

syscall parse_syscall_when_running() {
    syscall out{};
    out.state = syscall_state::running;

    return out;
}

syscall parse_syscall_when_blocked(const std::vector<std::string>& tokens) {
    syscall out{};
    out.state = syscall_state::blocked;

    enum pre_separator_token
    {
        NUMBER = 0,
        SP     = 1,
        IP     = 2
    };

    utils::stot(tokens[NUMBER], out.number);

    utils::stot(tokens[SP], out.stack_pointer, utils::base::hex);
    utils::stot(tokens[IP], out.instruction_pointer, utils::base::hex);

    return out;
}

syscall parse_syscall_when_in_syscall(const std::vector<std::string>& tokens) {
    syscall out{};
    out.state = syscall_state::syscall;

    enum pre_separator_token
    {
        NUMBER = 0,
        ARG1   = 1,
        ARG2   = 2,
        ARG3   = 3,
        ARG4   = 4,
        ARG5   = 5,
        ARG6   = 6,
        SP     = 7,
        IP     = 8
    };

    utils::stot(tokens[NUMBER], out.number);

    utils::stot(tokens[ARG1], out.argument_registers[0], utils::base::hex);
    utils::stot(tokens[ARG2], out.argument_registers[1], utils::base::hex);
    utils::stot(tokens[ARG3], out.argument_registers[2], utils::base::hex);
    utils::stot(tokens[ARG4], out.argument_registers[3], utils::base::hex);
    utils::stot(tokens[ARG5], out.argument_registers[4], utils::base::hex);
    utils::stot(tokens[ARG6], out.argument_registers[5], utils::base::hex);

    utils::stot(tokens[SP], out.stack_pointer, utils::base::hex);
    utils::stot(tokens[IP], out.instruction_pointer, utils::base::hex);

    return out;
}

} // anonymous namespace

syscall parse_syscall_line(const std::string& line)
{
    static const char DELIM = ' ';

    // If process is not blocked, then the file contains just the string "running"
    static const int COUNT_RUNNING = 1;

    // If If the process is blocked, but not in a system call, then
    // the file displays -1 in place of the system call number,
    // followed by just the values of the stack pointer and
    // program counter.
    static const int COUNT_BLOCKED = 3;

    // This file exposes the system call number and argument
    // registers for the system call currently being executed by
    // the process, followed by the values of the stack pointer
    // and program counter registers.
    static const int COUNT_IN_SYSCALL = 9;

    try
    {
        auto tokens = utils::split(line, DELIM);
        switch (tokens.size()) {
            case COUNT_RUNNING:
                return parse_syscall_when_running();
            case COUNT_BLOCKED:
                return parse_syscall_when_blocked(tokens);
            case COUNT_IN_SYSCALL:
                return parse_syscall_when_in_syscall(tokens);
            default:
                throw parser_error("Corrupted syscall - Unexpected tokens count", line);
        }
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted syscall - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted syscall - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
