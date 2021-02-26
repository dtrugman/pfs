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

#include <sys/types.h>
#include <unistd.h>

#include <system_error>

#include "pfs/fd.hpp"
#include "pfs/utils.hpp"

namespace pfs {

using namespace impl;

fd::fd(const std::string& fds_root, int num)
    : _num(num), _link(build_link_path(fds_root, num))
{}

std::string fd::build_link_path(const std::string& fds_root, int num)
{
    return fds_root + std::to_string(num);
}

bool fd::operator<(const fd& rhs) const
{
    return _num < rhs._num;
}

int fd::num() const
{
    return _num;
}

const std::string& fd::link() const
{
    return _link;
}

struct stat fd::get_link_stat() const
{
    struct stat st;
    if (lstat(_link.c_str(), &st) != 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't stat link");
    }

    return st;
}

std::string fd::get_target() const
{
    return utils::readlink(_link);
}

struct stat fd::get_target_stat() const
{
    struct stat st;
    if (stat(_link.c_str(), &st) != 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't stat target");
    }

    return st;
}

} // namespace pfs
