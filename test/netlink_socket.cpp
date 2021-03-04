#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted netlink socket", "[net][netlink_socket]")
{
    // Missing last token (drops)
    std::string line =
        "ffff9f55ba8e8800 9   4280338379 00000000 0        0        0 2";

    REQUIRE_THROWS_AS(parse_netlink_socket_line(line), pfs::parser_error);
}

TEST_CASE("Parse netlink socket", "[net][netlink_socket]")
{
    pfs::netlink_socket expected;
    std::string line;

    SECTION("High port id")
    {
        line = "ffff9f55ba8e8800 9   4280338379 00000000 0        0        0 2 "
               "       0        14098";

        expected.skbuff    = 0xffff9f55ba8e8800;
        expected.protocol  = 9;
        expected.port_id   = 4280338379;
        expected.groups    = 0;
        expected.rmem      = 0;
        expected.wmem      = 0;
        expected.dumping   = false;
        expected.ref_count = 2;
        expected.drops     = 0;
        expected.inode     = 14098;
    }

    SECTION("Hex groups")
    {
        line = "0000000000000000 0   464    000405d1 0        0        0 2 "
               "       0        15644";

        expected.skbuff    = 0x0;
        expected.protocol  = 0;
        expected.port_id   = 464;
        expected.groups    = 0x405d1;
        expected.rmem      = 0;
        expected.wmem      = 0;
        expected.dumping   = false;
        expected.ref_count = 2;
        expected.drops     = 0;
        expected.inode     = 15644;
    }

    SECTION("Without inode")
    {
        line = "ffff880037c4a800 15  -4167  00000002 0        0        1 2     "
               "   0";

        expected.skbuff    = 0xffff880037c4a800;
        expected.protocol  = 15;
        expected.port_id   = -4167;
        expected.groups    = 2;
        expected.rmem      = 0;
        expected.wmem      = 0;
        expected.dumping   = true;
        expected.ref_count = 2;
        expected.drops     = 0;
        expected.inode     = pfs::INVALID_INODE;
    }

    SECTION("Without inode and null dumping")
    {
        line = "ffff880037c10800 0   4195519 00000000 0        0        (null) "
               "2        0";

        expected.skbuff    = 0xffff880037c10800;
        expected.protocol  = 0;
        expected.port_id   = 4195519;
        expected.groups    = 0;
        expected.rmem      = 0;
        expected.wmem      = 0;
        expected.dumping   = false;
        expected.ref_count = 2;
        expected.drops     = 0;
        expected.inode     = pfs::INVALID_INODE;
    }

    auto socket = parse_netlink_socket_line(line);
    REQUIRE(socket.skbuff == expected.skbuff);
    REQUIRE(socket.protocol == expected.protocol);
    REQUIRE(socket.port_id == expected.port_id);
    REQUIRE(socket.groups == expected.groups);
    REQUIRE(socket.rmem == expected.rmem);
    REQUIRE(socket.wmem == expected.wmem);
    REQUIRE(socket.dumping == expected.dumping);
    REQUIRE(socket.ref_count == expected.ref_count);
    REQUIRE(socket.drops == expected.drops);
    REQUIRE(socket.inode == expected.inode);
}
