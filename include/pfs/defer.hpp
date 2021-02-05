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

#ifndef PFS_DEFER_HPP
#define PFS_DEFER_HPP

#include <functional>

namespace pfs {
namespace impl {

class defer
{
public:
    using action = std::function<void(void)>;

public:
    defer(const action& act) : _action(act) {}
    defer(action&& act) : _action(std::move(act)) {}

    defer(const defer& act) = delete;
    defer& operator=(const defer& act) = delete;

    defer(defer&& act) = delete;
    defer& operator=(defer&& act) = delete;

    ~defer() { _action(); }

private:
    action _action;
};

} // namespace impl
} // namespace pfs

#endif // PFS_DEFER_HPP
