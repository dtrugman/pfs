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

#ifndef PFS_BLOCK_QUEUE_HPP
#define PFS_BLOCK_QUEUE_HPP

#include "types.hpp"

namespace pfs {

// Hint: See 'https://www.kernel.org/doc/Documentation/block/queue-sysfs.txt'
class block_queue final
{
public:
    block_queue(const block_queue&) = default;
    block_queue(block_queue&&)      = default;

    block_queue& operator=(const block_queue&) = delete;
    block_queue& operator=(block_queue&&) = delete;

public: // Properties
    const std::string& dir() const;

public: // Getters
    bool get_rotational() const;

private:
    friend class block;
    block_queue(const std::string& block_root);

private:
    static std::string build_block_queue_root(const std::string& block_root);

private:
    static const std::string QUEUE_DIR;

private:
    const std::string _block_queue_root;
};

} // namespace pfs

#endif // PFS_BLOCK_HPP
