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

std::pair<size_t, size_t>
parse_mem_region_address(const std::string& address_str)
{
    // Address must be a range '<start>-<end>'
    enum address_token
    {
        START = 0,
        END   = 1,
        COUNT
    };

    static const char DELIM = '-';

    auto tokens = utils::split(address_str, DELIM);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted address - Unexpected tokens count",
                           address_str);
    }

    try
    {
        size_t start;
        utils::stot(tokens[START], start, utils::base::hex);

        size_t end;
        utils::stot(tokens[END], end, utils::base::hex);

        return std::make_pair(start, end);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted address - Invalid argument", address_str);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted address - Out of range", address_str);
    }
}

mem_perm parse_mem_region_permissions(const std::string& perm_str)
{
    enum bit
    {
        BIT_READ  = 0,
        BIT_WRITE = 1,
        BIT_EXEC  = 2,
        BIT_MODE  = 3,
        BIT_COUNT
    };

    static const char CHAR_READ    = 'r';
    static const char CHAR_WRITE   = 'w';
    static const char CHAR_EXEC    = 'x';
    static const char CHAR_PRIVATE = 'p';
    static const char CHAR_SHARED  = 's';

    if (perm_str.size() != BIT_COUNT)
    {
        throw parser_error("Corrupted permissions", perm_str);
    }

    mem_perm perm;
    perm.can_read    = (perm_str[BIT_READ] == CHAR_READ);
    perm.can_write   = (perm_str[BIT_WRITE] == CHAR_WRITE);
    perm.can_execute = (perm_str[BIT_EXEC] == CHAR_EXEC);
    perm.is_private  = (perm_str[BIT_MODE] == CHAR_PRIVATE);
    perm.is_shared   = (perm_str[BIT_MODE] == CHAR_SHARED);
    return perm;
}

size_t parse_mem_region_offset(const std::string& offset_str)
{
    try
    {
        size_t offset;
        utils::stot(offset_str, offset, utils::base::hex);
        return offset;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted offset - Invalid argument", offset_str);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted offset - Out of range", offset_str);
    }
}

ino_t parse_mem_region_inode(const std::string& inode_str)
{
    try
    {
        ino_t inode;
        utils::stot(inode_str, inode);
        return inode;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted inode - Invalid argument", inode_str);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted inode - Out of range", inode_str);
    }
}

} // anonymous namespace

mem_region parse_maps_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // 7f0b476b6000-7f0b476b7000 r--p 00000000 fd:00 655791                     /usr/lib/locale/C.UTF-8/LC_ADDRESS
    // 7f0b476b7000-7f0b476b8000 r--p 00000000 fd:00 655802                     /usr/lib/locale/C.UTF-8/LC_TELEPHONE
    // 7f0b476b8000-7f0b476b9000 r--p 00000000 fd:00 655795                     /usr/lib/locale/C.UTF-8/LC_MEASUREMENT
    // 7f0b476b9000-7f0b476be000 rw-p 00000000 00:00 0
    // 7f0b476be000-7f0b476c5000 r--s 00000000 fd:00 657410                     /usr/lib/x86_64-linux-gnu/gconv/gconv-modules.cache
    // 7f0b476c5000-7f0b476c6000 r--p 00000000 fd:00 655794                     /usr/lib/locale/C.UTF-8/LC_IDENTIFICATION
    // 7f0b476c6000-7f0b476c7000 r--p 00027000 fd:00 2097554                    /lib/x86_64-linux-gnu/ld-2.27.so
    // 7f0b476c7000-7f0b476c8000 rw-p 00028000 fd:00 2097554                    /lib/x86_64-linux-gnu/ld-2.27.so
    // 7f0b476c8000-7f0b476c9000 rw-p 00000000 00:00 0
    // 7fff964a7000-7fff964c8000 rw-p 00000000 00:00 0                          [stack]
    // 7fff96532000-7fff96535000 r--p 00000000 00:00 0                          [vvar]
    // 7fff96535000-7fff96537000 r-xp 00000000 00:00 0                          [vdso]
    // ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
    // clang-format on

    enum token
    {
        ADDRESS     = 0,
        PERMISSIONS = 1,
        OFFSET      = 2,
        DEVICE      = 3,
        INODE       = 4,
        MIN_COUNT   = 5, // PATHNAME is not always present
        PATHNAME    = 5,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() < MIN_COUNT)
    {
        throw parser_error("Corrupted maps line - Unexpected tokens count",
                           line);
    }

    mem_region region;

    std::tie(region.start_address, region.end_address) =
        parse_mem_region_address(tokens[ADDRESS]);

    region.perm = parse_mem_region_permissions(tokens[PERMISSIONS]);

    region.offset = parse_mem_region_offset(tokens[OFFSET]);

    region.device = parse_device(tokens[DEVICE]);

    region.inode = parse_mem_region_inode(tokens[INODE]);

    for (size_t i = PATHNAME; i < tokens.size(); ++i)
    {
        if (!region.pathname.empty())
        {
            region.pathname += " ";
        }

        region.pathname += tokens[i];
    }

    return region;
}

} // namespace parsers
} // namespace impl
} // namespace pfs
