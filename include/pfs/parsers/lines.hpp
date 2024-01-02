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

#ifndef PFS_PARSERS_GENERIC_HPP
#define PFS_PARSERS_GENERIC_HPP

#include <fstream>
#include <string>

#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"
#include "pfs/filter.hpp"

namespace pfs {
namespace impl {
namespace parsers {

template <typename Inserter>
using inserted_type = typename Inserter::container_type::value_type;

template <typename Inserter>
void parse_file_lines(
    const std::string& path,
    Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
    std::function<filter::action(const inserted_type<Inserter>&)> filter = nullptr,
    size_t lines_to_skip = 0)
{
    std::ifstream in(path);
    if (!in)
    {
        throw std::runtime_error("Couldn't open file");
    }

    std::string line;
    for (size_t i = 0; std::getline(in, line); ++i)
    {
        if (i < lines_to_skip)
        {
            continue;
        }

        if (line.empty())
        {
            continue;
        }

        auto inserted = parser(line);

        if (filter && filter(inserted) != filter::action::keep)
        {
            continue;
        }

        inserter = std::move(inserted);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_GENERIC_HPP
