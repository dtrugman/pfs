#include <sstream>

#include "catch.hpp"

#include "pfs/parsers/net_arp.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/types.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted net arp", "[net][arp]")
{
    SECTION("Missing token")
    {
        std::string line = "192.168.10.1     0x1         0x2         10:20:30:40:50:60     *";
        REQUIRE_THROWS_AS(parse_net_arp_line(line), pfs::parser_error);
    }

    SECTION("Extra token")
    {
        std::string line = "192.168.10.1     0x1         0x2         10:20:30:40:50:60     *        eth0       0x4";
        REQUIRE_THROWS_AS(parse_net_arp_line(line), pfs::parser_error);
    }
}

TEST_CASE("Parse net arp", "[net][arp]")
{
    pfs::net_arp expected;

    std::string line = "192.168.10.1     0x1         0x2         10:20:30:40:50:60     *        eth0";

    expected.ip_address = "192.168.10.1";
    expected.type       = 0x1;
    expected.flags      = 0x2;
    expected.hw_address = "10:20:30:40:50:60";
    expected.mask       = "*";
    expected.device     = "eth0";

    auto arp = parse_net_arp_line(line);

    REQUIRE(arp.ip_address == expected.ip_address);
    REQUIRE(arp.type == expected.type);
    REQUIRE(arp.flags == expected.flags);
    REQUIRE(arp.hw_address == expected.hw_address);
    REQUIRE(arp.mask == expected.mask);
    REQUIRE(arp.device == expected.device);
}
