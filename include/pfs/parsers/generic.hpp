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

namespace pfs {
namespace impl {
namespace parsers {

template <typename Inserter>
using inserted_type = typename Inserter::container_type::value_type;

template <typename Inserter>
void parse_and_filter_lines(
    const std::string& path,
    Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
    std::function<bool(const inserted_type<Inserter>&)> filter = nullptr,
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
        if (filter && filter(inserted))
        {
            continue;
        }

        inserter = std::move(inserted);
    }
}

template <typename Inserter>
void parse_lines(
    const std::string& path,
    Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
    size_t lines_to_skip = 0)
{
    parse_and_filter_lines(path, inserter, parser, nullptr, lines_to_skip);
}

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

template <typename T>
static void to_sequence(const std::string& value, proc_stat::sequence<T>& out)
{
    // Some examples:
    // clang-format off
    // 975101428 40707218 345522235 433770 2054357 19668 0 1807723 381659448 33954 202863055
    // clang-format on

    enum token
    {
        TOTAL     = 0,
        MIN_COUNT = 1,
    };

    auto tokens = utils::split(value);
    if (tokens.size() < MIN_COUNT)
    {
        throw parser_error("Corrupted sequence - Unexpected tokens count",
                           value);
    }

    try
    {
        proc_stat::sequence<T> sequence;

        utils::stot(tokens[TOTAL], out.total);

        for (size_t i = MIN_COUNT; i < tokens.size(); i++)
        {
            unsigned long long value;
            utils::stot(tokens[i], value);
            out.per_item.push_back(value);
        }
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted sequence - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted sequence - Out of range", value);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_GENERIC_HPP
