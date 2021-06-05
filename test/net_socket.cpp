#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted net socket", "[net][net_socket]")
{
    // Missing last token (skbuff)
    std::string line =
        "1: 3500007F:0035 00000000:0000 0A 00000000:00000000 00:00000000 "
        "00000000   101        0 15979 1";

    REQUIRE_THROWS_AS(parse_net_socket_line(line), pfs::parser_error);
}

TEST_CASE("Parse net socket", "[net][net_socket]")
{
    pfs::net_socket expected;

    std::stringstream line;
    line << std::hex;

    SECTION("IPv4")
    {
#if defined(ARCH_64BIT)
        size_t skbuff = 0xffff9f55b1420800;
#elif defined(ARCH_32BIT)
        size_t skbuff = 0xd8658c00;
#endif

        line <<
            "1: 3500007F:0035 00000000:0000 0A 00000000:00000000 00:00000000 "
            "00000000   101        0 15979 1 " << skbuff << " 100 0 0 10 0";

        expected.slot                 = 1;
        expected.local_ip             = pfs::ip(0x3500007F);
        expected.local_port           = 0x0035;
        expected.remote_ip            = pfs::ip(0x00000000);
        expected.remote_port          = 0x0000;
        expected.socket_net_state     = pfs::net_socket::net_state::listen;
        expected.tx_queue             = 0x00000000;
        expected.rx_queue             = 0x00000000;
        expected.timer_active         = pfs::net_socket::timer::none;
        expected.timer_expire_jiffies = 0x00000000;
        expected.retransmits          = 0x00000000;
        expected.uid                  = 101;
        expected.timeouts             = 0;
        expected.inode                = 15979;
        expected.ref_count            = 1;
        expected.skbuff               = skbuff;
    }

    SECTION("IPv6")
    {
#if defined(ARCH_64BIT)
        size_t skbuff = 0xffff9f55bdb91980;
#elif defined(ARCH_32BIT)
        size_t skbuff = 0xdb2b0000;
#endif

        line << "5: 00000000000000000000000000000000:006F "
                "00000000000000000000000000000000:0000 0A 00000000:00000000 "
                "00:00000000 00000000     0        0 15737 1 " << skbuff << " "
                "100 0 0 10 0";

        expected.slot     = 5;
        expected.local_ip = pfs::ip(
            pfs::ipv6({0x00000000, 0x00000000, 0x00000000, 0x00000000}));
        expected.local_port = 0x006F;
        expected.remote_ip  = pfs::ip(
            pfs::ipv6({0x00000000, 0x00000000, 0x00000000, 0x00000000}));
        expected.remote_port          = 0x0000;
        expected.socket_net_state     = pfs::net_socket::net_state::listen;
        expected.tx_queue             = 0x00000000;
        expected.rx_queue             = 0x00000000;
        expected.timer_active         = pfs::net_socket::timer::none;
        expected.timer_expire_jiffies = 0x00000000;
        expected.retransmits          = 0x00000000;
        expected.uid                  = 0;
        expected.timeouts             = 0;
        expected.inode                = 15737;
        expected.ref_count            = 1;
        expected.skbuff               = skbuff;
    }

    auto socket = parse_net_socket_line(line.str());
    REQUIRE(socket.slot == expected.slot);
    REQUIRE(socket.local_ip == expected.local_ip);
    REQUIRE(socket.local_port == expected.local_port);
    REQUIRE(socket.remote_ip == expected.remote_ip);
    REQUIRE(socket.remote_port == expected.remote_port);
    REQUIRE(socket.socket_net_state == expected.socket_net_state);
    REQUIRE(socket.tx_queue == expected.tx_queue);
    REQUIRE(socket.rx_queue == expected.rx_queue);
    REQUIRE(socket.timer_active == expected.timer_active);
    REQUIRE(socket.timer_expire_jiffies == expected.timer_expire_jiffies);
    REQUIRE(socket.retransmits == expected.retransmits);
    REQUIRE(socket.uid == expected.uid);
    REQUIRE(socket.timeouts == expected.timeouts);
    REQUIRE(socket.inode == expected.inode);
    REQUIRE(socket.ref_count == expected.ref_count);
    REQUIRE(socket.skbuff == expected.skbuff);
}
