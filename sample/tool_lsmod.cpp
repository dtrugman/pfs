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

#include <regex>

#include "format.hpp"
#include "log.hpp"
#include "tool.hpp"

int tool_lsmod(std::vector<std::string>&& args)
{
    try
    {
        LOG("=========================================================");
        LOG("lsmod");
        LOG("=========================================================");

        std::string filter;
        if (!args.empty())
        {
            filter = args[0];
        }
        const std::regex rx(filter);

        pfs::procfs pfs;
        auto modules = pfs.get_modules();

        for (const auto& module : modules)
        {
            if (std::regex_search(module.name, rx))
            {
                LOG(module);
            }
        }
    }
    catch (const std::runtime_error& ex)
    {
        LOG("Error when printing lsmod:");
        LOG(TAB << ex.what());
    }

    return 0;
}
