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

#include <arpa/inet.h>
#include <sys/socket.h>

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

bool capabilities_mask::operator==(const capabilities_mask& rhs) const
{
    return raw == rhs.raw;
}

// =============================================================
// Signal mask
// =============================================================

signal_mask::signal_mask(raw_type raw) : raw(raw) {}

bool signal_mask::is_set(signal sig)
{
    return raw & static_cast<decltype(raw)>(sig);
}

bool signal_mask::operator==(const signal_mask& rhs) const
{
    return raw == rhs.raw;
}

// =============================================================
// IP
// =============================================================

ip::ip() : domain(AF_UNSPEC), storage({0, 0, 0, 0}) {}

ip::ip(ipv4 addr) : domain(AF_INET), storage({addr, 0, 0, 0}) {}

ip::ip(ipv6 addr) : domain(AF_INET6), storage(addr) {}

bool ip::is_v4() const
{
    return domain == AF_INET;
}

bool ip::is_v6() const
{
    return domain == AF_INET6;
}

std::string ip::to_string() const
{
    char buff[INET_ADDRSTRLEN];
    inet_ntop(domain, &storage, buff, INET_ADDRSTRLEN);
    return std::string(buff);
}

} // namespace pfs
