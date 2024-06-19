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
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <system_error>

#include "pfs/defer.hpp"
#include "pfs/utils.hpp"
#include "pfs/types.hpp"
#include "pfs/parser_error.hpp"

namespace pfs {
namespace impl {
namespace utils {

static const char NEWLINE('\n');

size_t iterate_files(const std::string& dir, bool include_dots,
                     std::function<void(const char*)> handle, int dirfd)
{
    static const char DOTFILE_PREFIX = '.';

    int fd = openat(dirfd, dir.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open dir");
    }
    defer close_fd([fd] { close(fd); });

    size_t count = 0;

    DIR* dp = fdopendir(fd);
    if (!dp)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open dir");
    }
    defer close_dp([dp] { closedir(dp); });

    struct dirent* entry;
    while ((entry = readdir(dp)))
    {
        // It's safe to access index 0.
        // It's either a valid char or a null-terminator.
        if (entry->d_name[0] == DOTFILE_PREFIX && !include_dots)
        {
            continue;
        }

        ++count;

        if (handle)
        {
            handle(entry->d_name);
        }
    }

    return count;
}

size_t count_files(const std::string& dir, bool include_dots, int dirfd)
{
    return iterate_files(dir, include_dots, nullptr, dirfd);
}

std::set<std::string> enumerate_files(const std::string& dir, bool include_dots, int dirfd)
{
    std::set<std::string> files;
    auto handle = [&files](const char* name) { files.emplace(name); };

    (void)iterate_files(dir, include_dots, handle, dirfd);
    return files;
}

std::set<int> enumerate_numeric_files(const std::string& dir, int dirfd)
{
    std::set<int> files;
    auto handle = [&files](const char* name) {
        try
        {
            files.insert(std::stoi(name));
        }
        catch (const std::logic_error&)
        {
            // Do nothing, not a numeric file name
        }
    };

    (void)iterate_files(dir, false /* include_dots */, handle, dirfd);
    return files;
}

ino_t get_inode(const std::string& path, int dirfd)
{
    struct stat st;
    int err = fstatat(dirfd, path.c_str(), &st, 0);
    if (err)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't stat file for inode");
    }

    return st.st_ino;
}

std::string readlink(const std::string& link, int dirfd)
{
    std::string buffer;

    // Accomodate larget possible path + null terminator
    buffer.resize(PATH_MAX + 1);

    auto bytes = ::readlinkat(dirfd, link.c_str(), &buffer[0], buffer.size());
    if (bytes == -1)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't read link");
    }
    buffer.resize(bytes); // Let our string know how much bytes it really holds
    return buffer;
}

std::string readfile(const std::string& file, size_t max_bytes,
                     bool trim_newline, int dirfd)
{
    int fd = openat(dirfd, file.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open file");
    }
    defer close_fd([fd] { close(fd); });

    std::string buffer;
    buffer.resize(max_bytes);

    ssize_t bytes_read = read(fd, &buffer[0], max_bytes);
    if (bytes_read < 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't read file");
    }
    buffer.resize(bytes_read);

    while (trim_newline && !buffer.empty() && buffer.back() == NEWLINE)
    {
        buffer.pop_back();
    }

    return buffer;
}

std::string readline(const std::string& file, int dirfd)
{
    int fd = openat(dirfd, file.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open file");
    }
    defer close_fd([fd] { close(fd); });

    FILE* fp = fdopen(fd, "r");
    if (!fp)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open file");
    }
    defer close_fp([fp] { fclose(fp); });

    char* line = NULL;
    size_t n = 0;
    ssize_t len = getline(&line, &n, fp);
    defer free_line([line] { free(line); });
    if (len == -1)
    {
        throw std::runtime_error("Couldn't read line from file");
    }

    while (len > 0 && line[len - 1] == NEWLINE)
    {
        len--;
    }

    return std::string(line, len);
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

std::pair<std::string, std::string> split_once(const std::string& buffer,
                                               char delim)
{
    size_t index = buffer.find_first_of(delim);
    if (index == std::string::npos)
    {
        return std::make_pair(buffer, "");
    }
    else
    {
        return std::make_pair(buffer.substr(0, index),
                              buffer.substr(index + 1));
    }
}

void ltrim(std::string& str)
{
    static const auto COMPARE = [](unsigned char c) {
        return !std::isspace(c);
    };

    str.erase(str.begin(), std::find_if(str.begin(), str.end(), COMPARE));
}

void rtrim(std::string& str)
{
    static const auto COMPARE = [](unsigned char c) {
        return !std::isspace(c);
    };

    str.erase(std::find_if(str.rbegin(), str.rend(), COMPARE).base(),
              str.end());
}

void trim(std::string& str)
{
    ltrim(str);
    rtrim(str);
}

void ensure_dir_terminator(std::string& dir_path)
{
    static const char DIR_SEPARATOR('/');

    if (dir_path.back() != DIR_SEPARATOR)
    {
        dir_path += DIR_SEPARATOR;
    }
}

ip parse_ipv4_address(const std::string& ip_address_str)
{
    ipv4 raw;
    utils::stot(ip_address_str, raw, utils::base::hex);
    return ip(raw);
}

ip parse_ipv6_address(const std::string& ip_address_str)
{
    static const size_t HEX_BYTE_LEN = 8;

    ipv6 raw;

    for (size_t i = 0; i < raw.size(); ++i)
    {
        auto nibble = ip_address_str.substr(i * HEX_BYTE_LEN, HEX_BYTE_LEN);
        utils::stot(nibble, raw[i], utils::base::hex);
    }

    return ip(raw);
}

std::pair<ip, uint16_t> parse_address(const std::string& address_str)
{
    enum token
    {
        IP   = 0,
        PORT = 1,
        COUNT
    };

    static const size_t HEX_BYTE_LEN = 8;

    static const char DELIM = ':';

    auto tokens = utils::split(address_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error(
            "Corrupted net socket address - Unexpected token counts",
            address_str);
    }

    ip addr;
    auto& ip_str = tokens[IP];
    if (ip_str.size() == HEX_BYTE_LEN)
    {
        addr = utils::parse_ipv4_address(ip_str);
    }
    else if (ip_str.size() == std::tuple_size<ipv6>::value * HEX_BYTE_LEN)
    {
        addr = utils::parse_ipv6_address(ip_str);
    }
    else
    {
        throw parser_error("Corrupted net socket address - Bad length",
                           address_str);
    }

    uint16_t port;
    utils::stot(tokens[PORT], port, utils::base::hex);

    return std::make_pair(addr, port);
}

} // namespace utils
} // namespace impl
} // namespace pfs
