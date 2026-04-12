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
    auto make_value_parser = [](const std::string& field,
                                unsigned long io_stats::*member) {

        auto parser = [field, member](const std::string& value, io_stats& out) {
            to_number(field, value, utils::base::decimal, out.*member);
        };

        return std::make_pair(field, parser);
    };

    return {
        make_value_parser("rchar",                 &io_stats::rchar),
        make_value_parser("wchar",                 &io_stats::wchar),
        make_value_parser("syscr",                 &io_stats::syscr),
        make_value_parser("syscw",                 &io_stats::syscw),
        make_value_parser("read_bytes",            &io_stats::read_bytes),
        make_value_parser("write_bytes",           &io_stats::write_bytes),
        make_value_parser("cancelled_write_bytes", &io_stats::cancelled_write_bytes),
    };
}

// clang-format off
const task_io_parser::value_parsers
task_io_parser::PARSERS = task_io_parser::make_value_parsers();
// clang-format on

} // namespace parsers
} // namespace impl
} // namespace pfs
