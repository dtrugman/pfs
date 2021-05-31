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

#ifndef PFS_UTILS_HPP
#define PFS_UTILS_HPP

#include <fcntl.h>
#include <stddef.h>

#include <functional>
#include <limits>
#include <set>
#include <string>
#include <vector>

namespace pfs {
namespace impl {
namespace utils {

enum class base
{
    octal   = 8,
    decimal = 10,
    hex     = 16
};

// Generic template using std's sto[u]ll implementations.
// Notes:
// - Output is a variable so that the compiler can deduce the type
// automatically.
// - Removed the optional 'pos' variable from the signature (unused atm).
// Throws:
// Same exceptions as std::sto* implementation:
// - std::invalid_argument
// - std::out_of_range
template <typename T>
typename std::enable_if<std::is_signed<T>::value>::type
stot(const std::string& str, T& out, base b = base::decimal)
{
    std::size_t* POS = nullptr;

    long long temp;
    static_assert(sizeof(T) <= sizeof(temp), "signed stot is ill-defined");

    temp = std::stoll(str, POS, static_cast<int>(b));
    if (temp < std::numeric_limits<T>::min() ||
        temp > std::numeric_limits<T>::max())
    {
        throw std::out_of_range(str);
    }

    out = static_cast<T>(temp);
}

template <typename T>
typename std::enable_if<std::is_unsigned<T>::value>::type
stot(const std::string& str, T& out, base b = base::decimal)
{
    std::size_t* POS = nullptr;

    unsigned long long temp;
    static_assert(sizeof(T) <= sizeof(temp), "unsigned stot is ill-defined");

    temp = std::stoull(str, POS, static_cast<int>(b));
    if (temp < std::numeric_limits<T>::min() ||
        temp > std::numeric_limits<T>::max())
    {
        throw std::out_of_range(str);
    }

    out = static_cast<T>(temp);
}

// Iterate over all the files in a given directory.
// Calls 'handle' for every file found.
// Note: 'handle' can be nullptr. Use this to count the number of files in a
// directory. Returns the number of files found.
size_t iterate_files(const std::string& dir, bool include_dots,
                     std::function<void(const char*)> handle);

// Count all the files under the specified directory.
// File can be any unix file type, i.e. regular file, directory, link, etc.
size_t count_files(const std::string& dir, bool include_dots = false);

// Get a set of all the files under the specified directory.
// File can be any unix file type, i.e. regular file, directory, link, etc.
std::set<std::string> enumerate_files(const std::string& dir,
                                      bool include_dots = false);

// Get a set of all the files under the specified directory whose name is a
// number. File can be any unix file type, i.e. regular file, directory, link,
// etc.
std::set<int> enumerate_numeric_files(const std::string& dir);

// Get the inode number of the file.
// If the linkname is relative, then it is interpreted relative to the directory
// referred to by the file descriptor dirfd.
ino_t get_inode(const std::string& path, int dirfd = AT_FDCWD);

// Return the path to which the specified link points.
// If the linkname is relative, then it is interpreted relative to the directory
// referred to by the file descriptor dirfd.
std::string readlink(const std::string& link, int dirfd = AT_FDCWD);

// Return a buffer containing the content of the specified file.
// If the file is longer than 'max_size', only the first 'max_size' bytes are
// read.
// If requested to trim newline terminators, removes all of the from the
// end of the string.
std::string readfile(const std::string& file, size_t max_size,
                     bool trim_newline = true);

// Return a string containing the first line of the specified file.
// The returned string doesn't contain the line terminator.
std::string readline(const std::string& file);

// Split a buffer into multiple parts.
// The delimiters themselves are dropped.
// If a token (the text between two consequent delimiters) is an empty string,
// the decision whether to add it to the output or not is governed by the
// 'keep_empty' boolean flag.
std::vector<std::string> split(const std::string& buffer, char delim = ' ',
                               bool keep_empty = false);

// Split a buffer into two: Before and after the first occurence of the
// delimiter. The delimiter itself is dropped.
std::pair<std::string, std::string> split_once(const std::string& buffer,
                                               char delim = ' ');

// Remove all whitespace chars from the beginning of the string
void ltrim(std::string& str);

// Remove all whitespace chars from the end of the string
void rtrim(std::string& str);

// Remove all whitespace chars from both beginning and end of the string
void trim(std::string& str);

// Ensure directory path is terminated using a directory separator '/'
void ensure_dir_terminator(std::string& dir_path);

} // namespace utils
} // namespace impl
} // namespace pfs

#endif // PFS_UTILS_HPP
