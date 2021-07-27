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

#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include <system_error>

#include "pfs/mem.hpp"

namespace pfs {

mem::mem(const std::string& path)
    : _path(path), _fd(open(path.c_str(), O_RDONLY))
{
    if (_fd < 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't open file");
    }
}

mem::~mem()
{
    close(_fd);
}

std::vector<uint8_t> mem::read(const mem_region& region)
{
    return read(region.start_address,
                region.end_address - region.start_address);
}

std::vector<uint8_t> mem::read(loff_t offset, size_t bytes)
{
    std::vector<uint8_t> buffer(bytes);

    struct iovec iov = {&buffer[0], buffer.size()};

    ssize_t bytes_read = preadv64(_fd, &iov, 1, offset);
    if (bytes_read == -1)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't read from memory");
    }
    buffer.resize(bytes_read);

    return buffer;
}

} // namespace pfs
