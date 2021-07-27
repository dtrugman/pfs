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

#ifndef PFS_MEM_HPP
#define PFS_MEM_HPP

#include <fcntl.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include "types.hpp"

namespace pfs {

class mem final
{
public:
    mem(const mem&) = default;
    mem(mem&&)      = default;

    mem& operator=(const mem&) = delete;
    mem& operator=(mem&&) = delete;

    ~mem();

public: // API
    std::vector<uint8_t> read(const mem_region& region);
    std::vector<uint8_t> read(loff_t offset, size_t len);

private:
    friend class task;
    mem(const std::string& path);

private:
    const std::string _path;
    int _fd;
};

} // namespace pfs

#endif // PFS_MEM_HPP
