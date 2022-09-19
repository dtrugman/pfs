#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted net device", "[net][net_device]")
{
    SECTION("Missing token")
    {
        std::string line =
            "lo: 27267010   48045    0    0    0     0          0         0 "
            "27267010   48045    0    0    0     0       0";

        REQUIRE_THROWS_AS(parse_net_device_line(line), pfs::parser_error);
    }

    SECTION("Extra token")
    {
        std::string line =
            "lo: 27267010   48045    0    0    0     0          0         0 "
            "27267010   48045    0    0    0     0       0          0     0";

        REQUIRE_THROWS_AS(parse_net_device_line(line), pfs::parser_error);
    }
}

TEST_CASE("Parse net device", "[net][net_device]")
{
    pfs::net_device expected;

    std::string line =
            "eth0: 335754274   58179    1    2    3     4          5         "
            "6  9805218   48519   11   12   13    14      15         16";

    expected.interface     = "eth0";

    expected.rx_bytes      = 335754274;
    expected.rx_packets    = 58179;
    expected.rx_errs       = 1;
    expected.rx_drop       = 2;
    expected.rx_fifo       = 3;
    expected.rx_frame      = 4;
    expected.rx_compressed = 5;
    expected.rx_multicast  = 6;

    expected.tx_bytes      = 9805218;
    expected.tx_packets    = 48519;
    expected.tx_errs       = 11;
    expected.tx_drop       = 12;
    expected.tx_fifo       = 13;
    expected.tx_colls      = 14;
    expected.tx_carrier    = 15;
    expected.tx_compressed = 16;
 
    auto device = parse_net_device_line(line);

    REQUIRE(device.interface == expected.interface);

    REQUIRE(device.rx_bytes == expected.rx_bytes);
    REQUIRE(device.rx_packets == expected.rx_packets);
    REQUIRE(device.rx_errs == expected.rx_errs);
    REQUIRE(device.rx_drop == expected.rx_drop);
    REQUIRE(device.rx_fifo == expected.rx_fifo);
    REQUIRE(device.rx_compressed == expected.rx_compressed);
    REQUIRE(device.rx_multicast == expected.rx_multicast);

    REQUIRE(device.tx_bytes == expected.tx_bytes);
    REQUIRE(device.tx_packets == expected.tx_packets);
    REQUIRE(device.tx_errs == expected.tx_errs);
    REQUIRE(device.tx_drop == expected.tx_drop);
    REQUIRE(device.tx_fifo == expected.tx_fifo);
    REQUIRE(device.tx_colls == expected.tx_colls);
    REQUIRE(device.tx_carrier == expected.tx_carrier);
    REQUIRE(device.tx_compressed == expected.tx_compressed);
}
