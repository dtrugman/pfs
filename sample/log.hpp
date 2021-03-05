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

#ifndef SAMPLE_LOG_HPP
#define SAMPLE_LOG_HPP

#include <iostream>

#define TAB "\t"
#define ENDL "\n"

#define LOG(...) std::cout << __VA_ARGS__ << ENDL

template <typename K, typename V>
inline std::ostream& operator<<(std::ostream& out, const std::pair<K, V>& pair)
{
    out << pair.first << " = " << pair.second;
    return out;
}

template <typename T>
struct has_const_iterator
{
private:
    typedef int8_t yes;
    typedef int16_t no;

    template <typename C>
    static yes test(typename C::const_iterator*);

    template <typename C>
    static no test(...);

public:
    static constexpr bool value = (sizeof(test<T>(0)) == sizeof(yes));
    typedef T type;
};

template <typename T>
inline typename std::enable_if<has_const_iterator<T>::value>::type
__print(const std::string& title, const T& iterable)
{
    LOG(title);
    LOG(std::string(title.size(), '-'));
    for (const auto& iter : iterable)
    {
        LOG(iter);
    }
    LOG("");
}

template <typename T>
inline typename std::enable_if<!has_const_iterator<T>::value>::type
__print(const std::string& title, const T& value)
{
    LOG(title);
    LOG(std::string(title.size(), '-'));
    LOG(value);
    LOG("");
}

template <>
inline void __print(const std::string& title, const std::string& str)
{
    LOG(title);
    LOG(std::string(title.size(), '-'));
    LOG(str);
    LOG("");
}

#define print(x) __print(#x, x)

#endif // SAMPLE_LOG_HPP
