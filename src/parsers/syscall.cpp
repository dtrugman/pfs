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

syscall parse_syscall_line(const std::string& line)
{
    // 47 0x3 0x7fffffffc780 0x0 0x357d844 0x681dfc29 0x7ffff7fc9080
    // 0x7fffffffc718 0x7ffff7d9c6a7
    const static char DELIM = ' ';
    const static int COUNT  = 9; // number of syscall; six argument registers;
                                // stack pointer; program counter

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted syscall - Unexpected tokens count", line);
    }

    try
    {
        syscall output;

        output.number_of_syscall = std::stod(tokens[0]);
        for (auto i = 1; i < 6; i++)
        {
            output.argument_registers.push_back(
                std::strtoull(tokens[i].c_str(), NULL, 0));
        }

        output.stack_pointer   = std::strtoull(tokens[7].c_str(), NULL, 0);
        output.program_counter = std::strtoull(tokens[8].c_str(), NULL, 0);

        return output;
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
} // namespace

} // namespace parsers
} // namespace impl
} // namespace pfs
