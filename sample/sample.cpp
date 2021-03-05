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

#include <iostream>

#include "enum.hpp"
#include "log.hpp"
#include "menu.hpp"
#include "tool.hpp"

#include "pfs/version.hpp"

int main(int argc, char** argv)
{
    int rv = 1;

    try
    {
        auto app = "Sample application using pfs";

        auto version = std::to_string(PFS_VER_MAJOR) + "." +
                       std::to_string(PFS_VER_MINOR) + "." +
                       std::to_string(PFS_VER_PATCH);

        // clang-format off
        auto commands = std::vector<command>{
            {command("system", "", "Enumerate system-wide information", enum_system)},
            {command("net", "", "Enumerate network information", enum_net)},
            {command("tasks", "[task-id]...", "Enumerate running tasks", enum_tasks)},
            {command("fds", "[task-id]...", "Enumerate fds for a specific task", enum_fds)},
            {command("lsmod", "[filter]", "Enumerate all loaded modules that match the filter", tool_lsmod)},
        };
        // clang-format on

        auto m = menu(app, version, commands, argc, argv);
        if (m.run() == -EINVAL)
        {
            LOG(m.usage());
        }
    }
    catch (const std::exception& ex)
    {
        LOG(ex.what());
    }

    return rv;
}
