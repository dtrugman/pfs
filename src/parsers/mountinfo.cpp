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

mount parse_mountinfo_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // 27 0 253:0 / / rw,relatime shared:1 - ext4 /dev/mapper/vagrant--vg-root rw,errors=remount-ro,data=ordered
    // 28 21 0:7 / /sys/kernel/security rw,nosuid,nodev,noexec,relatime shared:8 - securityfs securityfs rw
    // 29 23 0:24 / /dev/shm rw,nosuid,nodev shared:4 - tmpfs tmpfs rw
    // 30 25 0:25 / /run/lock rw,nosuid,nodev,noexec,relatime shared:6 - tmpfs tmpfs rw,size=5120k
    // 31 21 0:26 / /sys/fs/cgroup ro,nosuid,nodev,noexec shared:9 - tmpfs tmpfs ro,mode=755
    // 32 31 0:27 / /sys/fs/cgroup/unified rw,nosuid,nodev,noexec,relatime shared:10 - cgroup2 cgroup rw
    // 33 31 0:28 / /sys/fs/cgroup/systemd rw,nosuid,nodev,noexec,relatime shared:11 - cgroup cgroup rw,xattr,name=systemd
    // 34 21 0:29 / /sys/fs/pstore rw,nosuid,nodev,noexec,relatime shared:12 - pstore pstore rw
    // clang-format on

    enum pre_separator_token
    {
        MOUNT_ID      = 0,
        PARENT_ID     = 1,
        DEVICE        = 2,
        ROOT          = 3,
        MOUNT_POINT   = 4,
        MOUNT_OPTIONS = 5,
        OPTIONAL      = 6, // Zero or more fields + Separator
        PRE_COUNT
    };

    enum post_separator_token
    {
        FILESYSTEM_TYPE = 0,
        MOUNT_SOURCE    = 1,
        SUPER_OPTIONS   = 2,
        POST_COUNT
    };

    static const std::string SEPARATOR("-");

    static const char OPTIONS_DELIM = ',';

    auto tokens = utils::split(line);
    if (tokens.size() < PRE_COUNT + POST_COUNT)
    {
        throw parser_error("Corrupted mountinfo - Unexpected tokens count",
                           line);
    }

    try
    {
        mount mnt;

        utils::stot(tokens[MOUNT_ID], mnt.id);
        utils::stot(tokens[PARENT_ID], mnt.parent_id);

        mnt.device = parse_device(tokens[DEVICE]);

        mnt.root  = tokens[ROOT];
        mnt.point = tokens[MOUNT_POINT];

        mnt.options = utils::split(tokens[MOUNT_OPTIONS], OPTIONS_DELIM);

        size_t i;
        for (i = OPTIONAL; tokens[i] != SEPARATOR; ++i)
        {
            mnt.optional.push_back(tokens[i]);
        }
        ++i; // Skip separator

        mnt.filesystem_type = tokens[i + FILESYSTEM_TYPE];

        mnt.source = tokens[i + MOUNT_SOURCE];

        mnt.super_options =
            utils::split(tokens[i + SUPER_OPTIONS], OPTIONS_DELIM);

        return mnt;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted mountinfo - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted mountinfo - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
