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

 #include <system_error>


#include "pfs/parsers/syscall.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

syscall parse_syscall(const std::string& path)
{

    std::string line = utils::readline(path);

    static const char DELIM = ' ';
    static const int COUNT  = 9;
    auto tokens = utils::split(line, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted syscall - Unexpected tokens count", line);
    }

    try
    {
        syscall output;

        output.number_of_syscall = std::atoi(tokens[0].c_str());
        size_t tmp;

        for (int i = 1; i < 7; i++)
        {
            pfs::impl::utils::stot(tokens[i].c_str(), tmp, utils::base::hex);    
            output.arguments.push_back(tmp);
        }
        pfs::impl::utils::stot(tokens[7].c_str(), output.stack_pointer, utils::base::hex);
        pfs::impl::utils::stot(tokens[8].c_str(), output.program_counter, utils::base::hex);

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

} // namespace parsers
} // namespace impl
} // namespace pfs
