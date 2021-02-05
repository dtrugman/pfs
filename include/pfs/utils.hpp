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

// Ensure directory path is terminated using a directory separator '/'
void ensure_dir_terminator(std::string& dir_path);

} // namespace utils
} // namespace impl
} // namespace pfs

#endif // PFS_UTILS_HPP
