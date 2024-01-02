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

#include "enum.hpp"
#include "format.hpp"
#include "log.hpp"

#include "pfs/sysfs.hpp"

static void enum_block(const pfs::block& block)
{
    try
    {
        LOG("=========================================================");
        LOG("Block name[" << block.name() << "]");
        LOG("=========================================================");

        auto size = block.get_size();
        print(size);

        auto dev = block.get_dev();
        print(dev);

        auto stat = block.get_stat();
        print(stat);

        auto queue = block.get_queue();
        print(queue);
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing block[" << block.name() << "]:");
        LOG(TAB << ex.what());
    }
}

int enum_blocks(std::vector<std::string>&& args)
{
    pfs::sysfs sfs;

    if (args.empty())
    {
        for (const auto& block : sfs.get_blocks())
        {
            enum_block(block);
        }
    }
    else
    {
        for (const auto& block : args)
        {
            enum_block(sfs.get_block(block));
        }
    }

    return 0;
}
