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

#ifndef PFS_PARSERS_HPP
#define PFS_PARSERS_HPP

#include <fstream>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#include "parser_error.hpp"
#include "types.hpp"

namespace pfs {
namespace impl {
namespace parsers {

template <typename Container>
Container parse_lines(
    const std::string& path,
    std::function<typename Container::value_type(const std::string&)> parser)
{
    std::ifstream in(path);
    if (!in)
    {
        throw std::runtime_error("Couldn't open file");
    }

    Container output;

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        output.emplace(parser(line));
    }

    return output;
}

std::pair<std::string, bool> parse_filesystems_line(const std::string& line);
std::pair<std::string, size_t> parse_meminfo_line(const std::string& line);

zone parse_buddyinfo_line(const std::string& line);
load_average parse_loadavg_line(const std::string& line);
module parse_modules_line(const std::string& line);

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_HPP
