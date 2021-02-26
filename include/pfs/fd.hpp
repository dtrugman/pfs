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

#ifndef PFS_FD_HPP
#define PFS_FD_HPP

#include <sys/stat.h>

#include <string>

#include "types.hpp"

namespace pfs {

class fd final
{
public:
    fd(const fd&) = default;
    fd(fd&&)      = default;

    fd& operator=(const fd&) = delete;
    fd& operator=(fd&&) = delete;

    bool operator<(const fd& rhs) const;

public: // Properties
    int num() const;
    const std::string& link() const;

public: // Getters
    struct stat get_link_stat() const;

    std::string get_target() const;
    struct stat get_target_stat() const;

private:
    friend class task;
    fd(const std::string& fds_root, int num);

private:
    static std::string build_link_path(const std::string& fds_root, int num);

private:
    const int _num;
    const std::string _link;
};

} // namespace pfs

#endif // PFS_FD_HPP
