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


#include "pfs/parsers/number.hpp"
#include "pfs/parsers/task_io.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

const char task_io_parser::DELIM = ':';

task_io_parser::value_parsers task_io_parser::make_value_parsers() {
    auto io_parser = [] (unsigned long io_stats::*member) {
        return [member] (const std::string& value, io_stats& out) {
            to_number(value, out.*member);
        };
    };

    return {
        { "rchar",                 io_parser(&io_stats::rchar) },
        { "wchar",                 io_parser(&io_stats::wchar) },
        { "syscr",                 io_parser(&io_stats::syscr) },
        { "syscw",                 io_parser(&io_stats::syscw) },
        { "read_bytes",            io_parser(&io_stats::read_bytes) },
        { "write_bytes",           io_parser(&io_stats::write_bytes) },
        { "cancelled_write_bytes", io_parser(&io_stats::cancelled_write_bytes) },
    };
}

// clang-format off
const task_io_parser::value_parsers
task_io_parser::PARSERS = task_io_parser::make_value_parsers();
// clang-format on

} // namespace parsers
} // namespace impl
} // namespace pfs
