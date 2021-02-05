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

#include <iostream>

namespace pfs {
namespace impl {
namespace parsers {

std::pair<std::string, bool> parse_filesystems_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // nodev   sysfs
    // nodev   rootfs
    // nodev   ramfs
    // nodev   bdev
    // nodev   proc
    // nodev   cpuset
    // nodev   cgroup
    //         ext3
    //         ext4
    // clang-format on

    static const size_t TOKENS_DEV   = 1;
    static const size_t TOKENS_NODEV = 2;

    static const char DELIM = '\t';

    auto tokens = utils::split(line, DELIM);
    if (tokens.size() == TOKENS_DEV)
    {
        return std::make_pair(tokens.back(), true);
    }
    else if (tokens.size() == TOKENS_NODEV)
    {
        return std::make_pair(tokens.back(), false);
    }
    else
    {
        throw parser_error("Corrupted filesystem - Unexpected tokens count",
                           line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
