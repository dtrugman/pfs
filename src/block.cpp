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
#include "pfs/parsers/block_stat.hpp"
#include "pfs/parsers/common.hpp"
#include "pfs/parsers/number.hpp"
#include "pfs/block.hpp"
#include "pfs/utils.hpp"

namespace pfs {

using namespace impl;

const std::string block::BLOCK_DIR("block/");

block::block(const std::string& sysfs_root, const std::string& name)
    : _name(name), _sysfs_root(sysfs_root),
      _block_root(build_block_root(sysfs_root, name))
{}

std::string block::build_block_root(const std::string& procfs_root, const std::string& name)
{
    return procfs_root + BLOCK_DIR + name + '/';
}

bool block::operator<(const block& rhs) const
{
    return _name.compare(rhs._name);
}

const std::string& block::name() const
{
    return _name;
}

const std::string& block::dir() const
{
    return _block_root;
}

size_t block::get_size() const
{
    static const std::string SIZE_FILE("size");
    auto path = _block_root + SIZE_FILE;

    size_t size;
    auto line = utils::readline(path);
    parsers::to_number(line, size);
    return size;
}

dev_t block::get_dev() const
{
    static const std::string DEV_FILE("dev");
    auto path = _block_root + DEV_FILE;

    auto line = utils::readline(path);
    return parsers::parse_device(line, utils::base::decimal);
}

block_stat block::get_stat() const
{
    static const std::string STAT_FILE("stat");
    auto path = _block_root + STAT_FILE;

    auto line = utils::readline(path);
    return parsers::parse_block_stat_line(line);
}

block_queue block::get_queue() const
{
    return block_queue(_block_root);
}

} // namespace pfs
