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

#include "pfs/parsers.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

module::state parse_module_state(const std::string& state_str)
{
    static const std::string LIVE("Live");
    static const std::string LOADING("Loading");
    static const std::string UNLOADING("Unloading");

    if (state_str == LIVE)
    {
        return module::state::live;
    }
    else if (state_str == LOADING)
    {
        return module::state::loading;
    }
    else if (state_str == UNLOADING)
    {
        return module::state::unloading;
    }
    else
    {
        throw parser_error("Corrupted module state - Unknown value", state_str);
    }
}

} // anonymous namespace

module parse_modules_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // vboxsf 77824 2 - Live 0xffffffffc0759000 (OE)
    // async_tx 16384 5 raid456,async_raid6_recov,async_memcpy,async_pq,async_xor, Live 0xffffffffc041b000
    // xor 24576 2 btrfs,async_xor, Live 0xffffffffc0391000
    // raid6_pq 114688 4 btrfs,raid456,async_raid6_recov,async_pq, Live 0xffffffffc03f6000
    // libcrc32c 16384 1 raid456, Live 0xffffffffc03b6000
    // raid1 40960 0 - Live 0xffffffffc03eb000
    // raid0 20480 0 - Live 0xffffffffc02ff000
    // multipath 16384 0 - Live 0xffffffffc02d7000
    // linear 16384 0 - Live 0xffffffffc0229000
    // crct10dif_pclmul 16384 0 - Live 0xffffffffc038c000
    // clang-format on

    enum token
    {
        NAME         = 0,
        SIZE         = 1,
        INSTANCES    = 2,
        DEPENDENCIES = 3,
        STATE        = 4,
        OFFSET       = 5,
        MIN_COUNT    = 6,
        FLAGS        = 6,
        COUNT
    };

    static const char FLAG_OUT_OF_TREE('O');
    static const char FLAG_UNSIGNED('E');

    static const std::string NO_DEPENDENCIES("-");

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT || tokens.size() > COUNT)
    {
        throw parser_error("Corrupted modules line - Unexpected tokens count",
                           line);
    }

    try
    {
        module mod;

        mod.name = tokens[NAME];

        utils::stot(tokens[SIZE], mod.size);

        utils::stot(tokens[INSTANCES], mod.instances);

        if (tokens[DEPENDENCIES] != NO_DEPENDENCIES)
        {
            mod.dependencies = utils::split(tokens[DEPENDENCIES], ',');
        }

        mod.module_state = parse_module_state(tokens[STATE]);

        utils::stot(tokens[OFFSET], mod.offset, utils::base::hex);

        if (tokens.size() > FLAGS)
        {
            const auto& flags = tokens[FLAGS];
            mod.is_out_of_tree =
                (flags.find(FLAG_OUT_OF_TREE) != std::string::npos);
            mod.is_unsigned = (flags.find(FLAG_UNSIGNED) != std::string::npos);
        }
        else
        {
            mod.is_out_of_tree = false;
            mod.is_unsigned    = false;
        }

        return mod;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted module - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted module - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
