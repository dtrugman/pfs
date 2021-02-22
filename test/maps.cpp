#include <iomanip>
#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

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
        start = 0x7f0b476b6000;
        end   = 0x7f0b476b7000;

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
        start = 0xffffffffff600000;
        end   = 0xffffffffff601000;

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
        start = 0x7f0b476c6000;
        end   = 0x7f0b476c7000;

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
        start = 0x7fcbe4bc0000;
        end   = 0x7f0b476c6000;

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
        start = 0x7f3fcf467000;
        end   = 0x7f3fcf666000;

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

    auto region = pfs::impl::parsers::parse_maps_line(line);
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
