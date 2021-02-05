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

#ifndef PFS_PARSER_ERROR_HPP
#define PFS_PARSER_ERROR_HPP

#include <exception>

namespace pfs {

class parser_error : public std::runtime_error
{
public:
    template <typename T>
    parser_error(const std::string& message, const T& extra)
        : std::runtime_error(message + " [" + extra + "]")
    {}
};

} // namespace pfs

#endif // PFS_PARSER_ERROR_HPP
