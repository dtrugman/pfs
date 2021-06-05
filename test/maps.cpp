#include <iomanip>
#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted maps", "[task][maps]")
{
    // Missing last token (inode)
#if defined(ARCH_64BIT)
    std::string line = "559037183000-559037184000 rw-p 00185000 fd:00";
#elif defined(ARCH_32BIT)
    std::string line = "08153000-08158000 rw-p 0010a000 fc:00";
#endif

    REQUIRE_THROWS_AS(parse_maps_line(line), pfs::parser_error);
}

TEST_CASE("Parse maps", "[task][maps]")
{
    size_t start;
    size_t end;

    bool can_read;
    bool can_write;
    bool can_execute;
    bool is_private;

    size_t offset;

    dev_t dev_major;
    dev_t dev_minor;

    ino_t inode;

    std::string pathname;

    SECTION("Anonymous")
    {
#if defined(ARCH_64BIT)
        start = 0x7f0b476b6000;
        end   = 0x7f0b476b7000;
#elif defined(ARCH_32BIT)
        start = 0x08158000;
        end   = 0x0815d000;
#endif

        can_read    = true;
        can_write   = false;
        can_execute = false;
        is_private  = true;

        offset = 0;

        dev_major = 0xfd;
        dev_minor = 0x00;

        inode = 655791;

        // Empty pathname
    }

    SECTION("Special")
    {
#if defined(ARCH_64BIT)
        start = 0xffffffffff600000;
        end   = 0xffffffffff601000;
#elif defined(ARCH_32BIT)
        start = 0xbfbd7000;
        end   = 0xbfbf8000;
#endif

        can_read    = true;
        can_write   = false;
        can_execute = true;
        is_private  = true;

        offset = 0;

        dev_major = 0x00;
        dev_minor = 0x00;

        inode = 0;

        pathname = "[vsyscall]";
    }

    SECTION("Shared object")
    {
#if defined(ARCH_64BIT)
        start = 0x7f0b476c6000;
        end   = 0x7f0b476c7000;
#elif defined(ARCH_32BIT)
        start = 0xb75b7000;
        end   = 0xb7766000;
#endif

        can_read    = true;
        can_write   = false;
        can_execute = false;
        is_private  = true;

        offset = 0x00027000;

        dev_major = 0xfd;
        dev_minor = 0x00;

        inode = 2097554;

        pathname = "/lib/x86_64-linux-gnu/ld-2.27.so";
    }

    SECTION("Deleted")
    {
#if defined(ARCH_64BIT)
        start = 0x7fcbe4bc0000;
        end   = 0x7f0b476c6000;
#elif defined(ARCH_32BIT)
        start = 0xb7792000;
        end   = 0xb7794000;
#endif

        can_read    = true;
        can_write   = false;
        can_execute = true;
        is_private  = true;

        offset = 0x00000000;

        dev_major = 0xfd;
        dev_minor = 0x00;

        inode = 2097624;

        pathname = "/lib/x86_64-linux-gnu/libm-2.27.so (deleted)";
    }

    SECTION("Pathname with spaces")
    {
#if defined(ARCH_64BIT)
        start = 0x7f3fcf467000;
        end   = 0x7f3fcf666000;
#elif defined(ARCH_32BIT)
        start = 0xb737e000;
        end   = 0xb7384000;
#endif

        can_read    = false;
        can_write   = false;
        can_execute = false;
        is_private  = true;

        offset = 0x0019d000;

        dev_major = 0xfd;
        dev_minor = 0x00;

        inode = 3801114;

        pathname = "/tmp/lib m 2.25.so";
    }

    std::ostringstream out;
    out << std::hex << start << "-" << end << std::dec << " ";
    out << (can_read ? 'r' : '-');
    out << (can_write ? 'w' : '-');
    out << (can_execute ? 'x' : '-');
    out << (is_private ? 'p' : 's');
    out << " ";
    out << std::setfill('0') << std::setw(8) << offset << " ";
    out << build_device_string(dev_major, dev_minor) << " ";
    out << inode << " ";
    while (out.tellp() == 73)
        out << " ";
    out << pathname;

    auto line = out.str();
    INFO(line);

    auto region = parse_maps_line(line);
    REQUIRE(region.start_address == start);
    REQUIRE(region.end_address == end);
    REQUIRE(region.perm.can_read == can_read);
    REQUIRE(region.perm.can_write == can_write);
    REQUIRE(region.perm.can_write == can_write);
    REQUIRE(region.perm.is_private == is_private);
    REQUIRE(region.perm.is_shared != is_private);
    REQUIRE(region.device == MKDEV(dev_major, dev_minor));
    REQUIRE(region.inode == inode);
    REQUIRE(region.pathname == pathname);
}
