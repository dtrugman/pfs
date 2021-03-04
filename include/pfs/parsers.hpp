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
#include "utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

template <typename Inserter>
using inserted_type = typename Inserter::container_type::value_type;

template <typename Inserter>
void parse_lines(
    const std::string& path, Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
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

        inserter = parser(line);
    }
}

std::pair<std::string, bool> parse_filesystems_line(const std::string& line);
std::pair<std::string, size_t> parse_meminfo_line(const std::string& line);

zone parse_buddyinfo_line(const std::string& line);
load_average parse_loadavg_line(const std::string& line);
module parse_modules_line(const std::string& line);
mem_region parse_maps_line(const std::string& line);
mem_region parse_maps_line(const std::string& line);
mount parse_mountinfo_line(const std::string& line);
net_socket parse_net_socket_line(const std::string& line);
unix_socket parse_unix_socket_line(const std::string& line);
netlink_socket parse_netlink_socket_line(const std::string& line);

dev_t parse_device(const std::string& device_str);
task_state parse_task_state(char state_char);

template <typename Output>
class file_parser
{
public:
    Output parse(const std::string& path,
                 const std::set<std::string>& keys = {})
    {
        std::ifstream in(path);
        if (!in)
        {
            throw std::runtime_error("Couldn't open file");
        }

        Output output;

        std::string line;
        while (std::getline(in, line))
        {
            static const char DELIM = ':';

            std::string key;
            std::string value;
            std::tie(key, value) = utils::split_once(line, DELIM);
            if (key.empty())
            {
                throw parser_error("Corrupted line - Missing key", line);
            }

            utils::rtrim(key);

            // Value MIGHT be an empty value, for example:
            // Process without any groups

            // If caller specified a set of keys, get only those
            if (!keys.empty() && keys.find(key) == keys.end())
            {
                continue;
            }

            auto iter = _parsers.find(key);
            if (iter != _parsers.end())
            {
                utils::ltrim(value);
                auto& parser = iter->second;
                parser(value, output);
            }
        }

        return output;
    }

protected:
    using value_parser =
        std::function<void(const std::string& value, Output& out)>;
    using value_parsers = std::unordered_map<std::string, value_parser>;

    file_parser(const value_parsers& parsers) : _parsers(parsers) {}

private:
    const value_parsers& _parsers;
};

class status_parser : public file_parser<task_status>
{
public:
    status_parser() : file_parser<task_status>(parsers) {}

private:
    static const value_parsers parsers;
};

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_HPP
