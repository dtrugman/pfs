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
#include <sys/types.h>
#include <unistd.h>

#include <system_error>

#include "pfs/sysfs.hpp"
#include "pfs/utils.hpp"

namespace pfs {

using namespace impl;

const std::string sysfs::DEFAULT_ROOT("/sys/");

sysfs::sysfs(const std::string& root) : _root(build_root(root))
{
    validate_root(root);
}

std::string sysfs::build_root(std::string root)
{
    utils::ensure_dir_terminator(root);
    return root;
}

void sysfs::validate_root(const std::string& root)
{
    struct stat st;
    int rv = stat(root.c_str(), &st);
    if (rv != 0)
    {
        throw std::system_error(errno, std::system_category(),
                                "Couldn't stat root");
    }

    if (!S_ISDIR(st.st_mode))
    {
        throw std::runtime_error("Specified sysfs root is not a directory");
    }
}

block sysfs::get_block(const std::string& block_name) const
{
    return block(_root, block_name);
}

std::set<block> sysfs::get_blocks() const
{
    std::set<block> blocks;
    for (auto block_name : utils::enumerate_files(_root + block::BLOCK_DIR))
    {
        blocks.emplace(get_block(block_name));
    }

    return blocks;
}

} // namespace pfs
