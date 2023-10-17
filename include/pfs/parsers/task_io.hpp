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

#ifndef PFS_PARSERS_TASK_IO_HPP
#define PFS_PARSERS_TASK_IO_HPP

#include "file_parser.hpp"
#include "pfs/types.hpp"

namespace pfs {
namespace impl {
namespace parsers {

class task_io_parser : public file_parser<io_stats>
{
public:
    task_io_parser() : file_parser<io_stats>(DELIM, PARSERS) {}

private:
    static value_parsers make_value_parsers();

private:
    static const char DELIM;
    static const value_parsers PARSERS;
};

} // namespace parsers
} // namespace impl
} // namespace pfs

#endif // PFS_PARSERS_TASK_IO_HPP
