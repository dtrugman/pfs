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

#include <string>

#include "pfs/defer.hpp"
#include "pfs/parsers/common.hpp"
#include "pfs/parsers/number.hpp"
#include "pfs/block_queue.hpp"

namespace pfs {

using namespace impl;

const std::string block_queue::QUEUE_DIR("queue/");

block_queue::block_queue(const std::string& block_root)
    : _block_queue_root(build_block_queue_root(block_root))
{}

std::string block_queue::build_block_queue_root(const std::string& block_root)
{
    return block_root + QUEUE_DIR;
}

const std::string& block_queue::dir() const
{
    return _block_queue_root;
}

bool block_queue::get_rotational() const
{
    static const std::string SIZE_FILE("rotational");
    auto path = _block_queue_root + SIZE_FILE;

    auto value = utils::readline(path);

    int number;
    parsers::to_number(value, number, utils::base::decimal);
    return number != 0;
}

} // namespace pfs
