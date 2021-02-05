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

#include <dirent.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <system_error>

#include "pfs/defer.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace utils {

std::string readline(const std::string& file)
{
    std::ifstream in(file);
    if (!in)
    {
        throw std::runtime_error("Couldn't open file");
    }

    std::string line;
    if (!std::getline(in, line))
    {
        throw std::runtime_error("Couldn't read line from file");
    }

    return line;
}

std::vector<std::string> split(const std::string& buffer, char delim,
                               bool keep_empty)
{
    std::vector<std::string> out;

    size_t last, curr;
    for (last = curr = 0; curr < buffer.size(); ++curr)
    {
        if (buffer[curr] != delim)
        {
            continue;
        }

        auto arg = buffer.substr(last, curr - last);
        if (!arg.empty() || keep_empty)
        {
            out.emplace_back(std::move(arg));
        }

        last = curr + 1;
    }

    if (last < curr)
    {
        out.emplace_back(buffer.substr(last, curr - last));
    }

    return out;
}

void ensure_dir_terminator(std::string& dir_path)
{
    static const char DIR_SEPARATOR('/');

    if (dir_path.back() != DIR_SEPARATOR)
    {
        dir_path += DIR_SEPARATOR;
    }
}

} // namespace utils
} // namespace impl
} // namespace pfs
