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

#ifndef PFS_PARSERS_NUMBER_HPP
#define PFS_PARSERS_NUMBER_HPP

#include <string>

#include "pfs/parser_error.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

template <typename T>
static void to_number(const std::string& value, T& out,
                      utils::base base = utils::base::decimal)
{
    try
    {
        utils::stot(value, out, base);
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted number - Invalid argument", value);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted number - Out of range", value);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_NUMBER_HPP
