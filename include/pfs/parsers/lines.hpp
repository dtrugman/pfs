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

#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <stdexcept>

#include "pfs/filter.hpp"
#include "pfs/defer.hpp"

namespace pfs {
namespace impl {
namespace parsers {

template <typename Inserter>
using inserted_type = typename Inserter::container_type::value_type;

template <typename Inserter>
void parse_file_lines(
    int dirfd,
    const std::string& path,
    Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
    std::function<filter::action(const inserted_type<Inserter>&)> filter = nullptr,
    size_t lines_to_skip = 0)
{
    static const char NEWLINE('\n');

    int fd = openat(dirfd, path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::runtime_error("Couldn't open file");
    }
    defer close_fd([fd] { close(fd); });

    FILE* fp = fdopen(fd, "r");
    if (!fp)
    {
        throw std::runtime_error("Couldn't open file");
    }
    defer close_fp([fp] { fclose(fp); });

    ssize_t len = 0;
    size_t n = 0;
    char* linebuf = NULL;

    // free linebuf even if getline() fails
    defer free_line([linebuf] { free(linebuf); });
    for (size_t i = 0; (len = getline(&linebuf, &n, fp)) != -1; ++i)
    {
        if (i < lines_to_skip)
        {
            continue;
        }

        while (len > 0 && linebuf[len - 1] == NEWLINE)
        {
            len--;
        }

        if (len == 0)
        {
            continue;
        }

        auto inserted = parser(std::string(linebuf, len));

        if (filter && filter(inserted) != filter::action::keep)
        {
            continue;
        }

        inserter = std::move(inserted);
    }
}

template <typename Inserter>
void parse_file_lines(
    const std::string& path,
    Inserter inserter,
    std::function<inserted_type<Inserter>(const std::string&)> parser,
    std::function<filter::action(const inserted_type<Inserter>&)> filter = nullptr,
    size_t lines_to_skip = 0)
{
    parse_file_lines(AT_FDCWD, path, inserter, parser, filter, lines_to_skip);
}

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_GENERIC_HPP
