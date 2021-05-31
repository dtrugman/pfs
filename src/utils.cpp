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

namespace pfs {
namespace impl {
namespace utils {

size_t iterate_files(const std::string& dir, bool include_dots,
                     std::function<void(const char*)> handle)
{
    static const char DOTFILE_PREFIX = '.';

    size_t count = 0;

    DIR* dp = opendir(dir.c_str());
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

size_t count_files(const std::string& dir, bool include_dots)
{
    return iterate_files(dir, include_dots, nullptr);
}

std::set<std::string> enumerate_files(const std::string& dir, bool include_dots)
{
    std::set<std::string> files;
    auto handle = [&files](const char* name) { files.emplace(name); };

    (void)iterate_files(dir, include_dots, handle);
    return files;
}

std::set<int> enumerate_numeric_files(const std::string& dir)
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

    (void)iterate_files(dir, false /* include_dots */, handle);
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
                     bool trim_newline)
{
    int fd = open(file.c_str(), O_RDONLY);
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

    static const char NEWLINE('\n');
    while (trim_newline && !buffer.empty() && buffer.back() == NEWLINE)
    {
        buffer.pop_back();
    }

    return buffer;
}

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

} // namespace utils
} // namespace impl
} // namespace pfs
