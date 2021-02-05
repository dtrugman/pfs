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

#include "pfs/types.hpp"

namespace pfs {

// =============================================================
// Capabilities mask
// =============================================================

capabilities_mask::capabilities_mask(raw_type raw) : raw(raw) {}

bool capabilities_mask::is_set(capability cap)
{
    return raw & static_cast<decltype(raw)>(cap);
}

// =============================================================
// Signal mask
// =============================================================

signal_mask::signal_mask(raw_type raw) : raw(raw) {}

bool signal_mask::is_set(signal sig)
{
    return raw & static_cast<decltype(raw)>(sig);
}

} // namespace pfs
