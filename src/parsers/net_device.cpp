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

net_device parse_net_device_line(const std::string& line)
{
    // Example:
    // clang-format off
    // Inter-| Receive                                                  |Transmit
    //  face |   bytes packets errs drop fifo frame compressed multicast|   bytes packets errs drop fifo colls carrier compressed
    //     lo:   93144    1366    0    0    0     0          0         0    93144    1366    0    0    0     0       0          0
    //   eth0: 2893258    9219    0    0    0     0          0       556  1029533    7276    0    0    0     0       0          0
    // clang-format on

    enum token
    {
        INTERFACE     = 0,
        RX_BYTES      = 1,
        RX_PACKETS    = 2,
        RX_ERRS       = 3,
        RX_DROP       = 4,
        RX_FIFO       = 5,
        RX_FRAME      = 6,
        RX_COMPRESSED = 7,
        RX_MULTICAST  = 8,
        TX_BYTES      = 9,
        TX_PACKETS    = 10,
        TX_ERRS       = 11,
        TX_DROP       = 12,
        TX_FIFO       = 13,
        TX_COLLS      = 14,
        TX_CARRIER    = 15,
        TX_COMPRESSED = 16,
        COUNT
    };

    auto tokens = utils::split(line);
    if (tokens.size() != COUNT)
    {
        throw parser_error("Corrupted net device line - Wrong number of tokens",
                           line);
    }

    try
    {
        net_device dev;

        dev.interface = tokens[INTERFACE];
        dev.interface.pop_back(); // Remove ':';
        
        utils::stot(tokens[RX_BYTES], dev.rx_bytes, utils::base::decimal);
        utils::stot(tokens[RX_PACKETS], dev.rx_packets, utils::base::decimal);
        utils::stot(tokens[RX_ERRS], dev.rx_errs, utils::base::decimal);
        utils::stot(tokens[RX_DROP], dev.rx_drop, utils::base::decimal);
        utils::stot(tokens[RX_FIFO], dev.rx_fifo, utils::base::decimal);
        utils::stot(tokens[RX_FRAME], dev.rx_frame, utils::base::decimal);
        utils::stot(tokens[RX_COMPRESSED], dev.rx_compressed, utils::base::decimal);
        utils::stot(tokens[RX_MULTICAST], dev.rx_multicast, utils::base::decimal);

        utils::stot(tokens[TX_BYTES], dev.tx_bytes, utils::base::decimal);
        utils::stot(tokens[TX_PACKETS], dev.tx_packets, utils::base::decimal);
        utils::stot(tokens[TX_ERRS], dev.tx_errs, utils::base::decimal);
        utils::stot(tokens[TX_DROP], dev.tx_drop, utils::base::decimal);
        utils::stot(tokens[TX_FIFO], dev.tx_fifo, utils::base::decimal);
        utils::stot(tokens[TX_COLLS], dev.tx_colls, utils::base::decimal);
        utils::stot(tokens[TX_CARRIER], dev.tx_carrier, utils::base::decimal);
        utils::stot(tokens[TX_COMPRESSED], dev.tx_compressed, utils::base::decimal);

        return dev;
    }
    catch (const std::invalid_argument& ex)
    {
        throw parser_error("Corrupted net device - Invalid argument", line);
    }
    catch (const std::out_of_range& ex)
    {
        throw parser_error("Corrupted net device - Out of range", line);
    }
}

} // namespace parsers
} // namespace impl
} // namespace pfs
