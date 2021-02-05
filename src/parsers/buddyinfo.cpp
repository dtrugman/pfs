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

zone parse_buddyinfo_line(const std::string& line)
{
    // Some examples:
    // clang-format off
    // Node 0, zone     DMA     1    1    1    0    2    1    1    0    1    1    3
    // Node 0, zone   DMA32    65   47    4   81   52   28   13   10    5    1  404
    // Node 0, zone  Normal   216   55  189  101   84   38   37   27    5    3  587
    // clang-format on

    enum token
    {
        NODE_KEYWORD = 0,
        NODE_ID      = 1,
        ZONE_KEYWORD = 2,
        ZONE_NAME    = 3,
        FIRST_CHUNK  = 4,
        LAST_CHUNK   = 14,
        COUNT
    };

    static const char NODE_DELIM = ',';

    auto tokens = utils::split(line);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted buddyinfo - Unexpected tokens count",
                           line);
    }

    std::string node_id_str = tokens[NODE_ID]; // Something like '0,'
    if (node_id_str.back() != NODE_DELIM)
    {
        throw parser_error("Corrupted buddyinfo - Missing node delim", line);
    }
    node_id_str.pop_back();

    try
    {
        zone zn;

        utils::stot(node_id_str, zn.node_id);

        zn.name = tokens[ZONE_NAME];

        for (size_t i = 0; i < zn.chunks.size(); ++i)
        {
            utils::stot(tokens[FIRST_CHUNK + i], zn.chunks[i]);
        }

        return zn;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted buddyinfo - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted buddyinfo - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
