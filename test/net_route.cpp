#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers/net_route.hpp"
#include "pfs/parser_error.hpp"
#include "pfs/types.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted net route", "[net][route]")
{
    SECTION("Missing token")
    {
        std::string line = "eth0\t00000000\t012017AC\t0003\t0\t0\t0\t00000000\t0\t0";
        REQUIRE_THROWS_AS(parse_net_route_line(line), pfs::parser_error);
    }

    SECTION("Extra token")
    {
        std::string line = "eth0\t00000000\t07030301\t0003\t0\t0\t0\t00000000\t0\t0\t0\t0";
        REQUIRE_THROWS_AS(parse_net_route_line(line), pfs::parser_error);
    }
}

TEST_CASE("Parse net route", "[net][route]")
{
    pfs::net_route expected;

    std::string line = "eth0\t00000000\t07030301\t0003\t0\t0\t0\t00F0FFFF\t0\t0\t0";

    expected.iface       = "eth0";
    expected.destination = pfs::ip(0x00000000);
    expected.gateway     = pfs::ip(0x07030301);
    expected.flags       = 0x0003;
    expected.refcnt      = 0;
    expected.use         = 0;
    expected.metric      = 0;
    expected.mask        = pfs::ip(0x00F0FFFF);
    expected.mtu         = 0;
    expected.window      = 0;
    expected.irtt        = 0;

    auto route = parse_net_route_line(line);

    REQUIRE(route.iface == expected.iface);
    REQUIRE(route.destination == expected.destination);
    REQUIRE(route.gateway == expected.gateway);
    REQUIRE(route.flags == expected.flags);
    REQUIRE(route.refcnt == expected.refcnt);
    REQUIRE(route.use == expected.use);
    REQUIRE(route.metric == expected.metric);
    REQUIRE(route.mask == expected.mask);
    REQUIRE(route.mtu == expected.mtu);
    REQUIRE(route.window == expected.window);
    REQUIRE(route.irtt == expected.irtt);
}
