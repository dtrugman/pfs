#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted unix socket", "[net][unix_socket]")
{
    // Missing last token (inode)
    std::string line = "ffff8db2f3e09400: 00000002 00000000 00000000 0002 01";

    REQUIRE_THROWS_AS(parse_unix_socket_line(line), pfs::parser_error);
}

TEST_CASE("Parse unix socket", "[net][unix_socket]")
{
    pfs::unix_socket expected;
    std::string line;

    SECTION("Anonymous")
    {
        line = "ffff8db2f3e09400: 00000002 00000000 00000000 0002 01 21401";

        expected.skbuff       = 0xffff8db2f3e09400;
        expected.ref_count    = 0x00000002;
        expected.protocol     = 0x00000000;
        expected.flags        = 0x00000000;
        expected.socket_type  = pfs::unix_socket::type::datagram;
        expected.socket_state = pfs::unix_socket::state::unconnected;
        expected.inode        = 21401;
    }

    SECTION("File backed")
    {
        line = "ffff8db2fd23a000: 00000003 00000000 00000000 0001 03 17031 "
               "/run/systemd/journal/stdout";

        expected.skbuff       = 0xffff8db2fd23a000;
        expected.ref_count    = 0x00000003;
        expected.protocol     = 0x00000000;
        expected.flags        = 0x00000000;
        expected.socket_type  = pfs::unix_socket::type::stream;
        expected.socket_state = pfs::unix_socket::state::connected;
        expected.inode        = 17031;
        expected.path         = "/run/systemd/journal/stdout";
    }

    SECTION("Abstract namespace socket")
    {
        line = "ffff880037a393c0: 00000002 00000000 00000000 0002 01  "
               "9050 @/org/kernel/udev/udevd";

        expected.skbuff       = 0xffff880037a393c0;
        expected.ref_count    = 0x00000002;
        expected.protocol     = 0x00000000;
        expected.flags        = 0x00000000;
        expected.socket_type  = pfs::unix_socket::type::datagram;
        expected.socket_state = pfs::unix_socket::state::unconnected;
        expected.inode        = 9050;
        expected.path         = "@/org/kernel/udev/udevd";
    }

    auto socket = parse_unix_socket_line(line);
    REQUIRE(socket.skbuff == expected.skbuff);
    REQUIRE(socket.ref_count == expected.ref_count);
    REQUIRE(socket.flags == expected.flags);
    REQUIRE(socket.socket_type == expected.socket_type);
    REQUIRE(socket.socket_state == expected.socket_state);
    REQUIRE(socket.inode == expected.inode);
    REQUIRE(socket.path == expected.path);
}
