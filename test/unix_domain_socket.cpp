#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse unix domain socket", "[net][unix_domain_socket]")
{
    pfs::unix_domain_socket expected;
    std::string line;

    SECTION("Anonymous")
    {
        line = "ffff8db2f3e09400: 00000002 00000000 00000000 0002 01 21401";

        expected.skbuff       = 0xffff8db2f3e09400;
        expected.ref_count    = 0x00000002;
        expected.protocol     = 0x00000000;
        expected.flags        = 0x00000000;
        expected.socket_type  = pfs::unix_domain_socket::type::datagram;
        expected.socket_state = pfs::unix_domain_socket::state::unconnected;
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
        expected.socket_type  = pfs::unix_domain_socket::type::stream;
        expected.socket_state = pfs::unix_domain_socket::state::connected;
        expected.inode        = 17031;
        expected.path         = "/run/systemd/journal/stdout";
    }

    auto uds = parse_unix_domain_socket_line(line);
    REQUIRE(uds.skbuff == expected.skbuff);
    REQUIRE(uds.ref_count == expected.ref_count);
    REQUIRE(uds.flags == expected.flags);
    REQUIRE(uds.socket_type == expected.socket_type);
    REQUIRE(uds.socket_state == expected.socket_state);
    REQUIRE(uds.inode == expected.inode);
    REQUIRE(uds.path == expected.path);
}
