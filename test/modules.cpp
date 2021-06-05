#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted modules", "[procfs][modules]")
{
    // Missing last token (offset)
    std::string line = "raid1 40960 0 - Live";

    REQUIRE_THROWS_AS(parse_modules_line(line), pfs::parser_error);
}

TEST_CASE("Parse modules", "[procfs][modules]")
{
    pfs::module expected;
    std::ostringstream line;
    line << std::hex;

    SECTION("No dependencies")
    {
#if defined(ARCH_64BIT)
        size_t offset = 0xffffffffc03eb000;
#elif defined(ARCH_32BIT)
        size_t offset = 0xe089a000;
#endif

        line << "raid1 40960 0 - Live 0x" << offset;

        expected.name           = "raid1";
        expected.size           = 40960;
        expected.instances      = 0;
        expected.dependencies   = {};
        expected.module_state   = pfs::module::state::live;
        expected.offset         = offset;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
    }

    SECTION("Unsigned and Out-of-tree")
    {
#if defined(ARCH_64BIT)
        size_t offset = 0xffffffffc0759000;
#elif defined(ARCH_32BIT)
        size_t offset = 0xe0ac1000;
#endif

        line << "vboxsf 77824 2 - Live 0x" << offset << " (OE)";

        expected.name           = "vboxsf";
        expected.size           = 77824;
        expected.instances      = 2;
        expected.dependencies   = {};
        expected.module_state   = pfs::module::state::live;
        expected.offset         = offset;
        expected.is_out_of_tree = true;
        expected.is_unsigned    = true;
    }

    SECTION("Single dependency")
    {
#if defined(ARCH_64BIT)
        size_t offset = 0xffffffffc03b6000;
#elif defined(ARCH_32BIT)
        size_t offset = 0xe089f000;
#endif

        line << "libcrc32c 16384 1 raid456, Live 0x" << offset;

        expected.name           = "libcrc32c";
        expected.size           = 16384;
        expected.instances      = 1;
        expected.dependencies   = {"raid456"};
        expected.module_state   = pfs::module::state::live;
        expected.offset         = offset;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
    }

    SECTION("Multi dependencies")
    {
#if defined(ARCH_64BIT)
        size_t offset = 0xffffffffc03f6000;
#elif defined(ARCH_32BIT)
        size_t offset = 0xe081b000;
#endif

        line << "raid6_pq 114688 4 btrfs,raid456,async_raid6_recov,async_pq, Live 0x"
             << offset;

        expected.name           = "raid6_pq";
        expected.size           = 114688;
        expected.instances      = 4;
        expected.dependencies   = {"btrfs", "raid456", "async_raid6_recov",
                                 "async_pq"};
        expected.module_state   = pfs::module::state::live;
        expected.offset         = offset;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
        ;
    }

    auto module = parse_modules_line(line.str());
    REQUIRE(module.name == expected.name);
    REQUIRE(module.size == expected.size);
    REQUIRE(module.instances == expected.instances);
    REQUIRE(module.dependencies == expected.dependencies);
    REQUIRE(module.module_state == expected.module_state);
    REQUIRE(module.offset == expected.offset);
    REQUIRE(module.is_out_of_tree == expected.is_out_of_tree);
    REQUIRE(module.is_unsigned == expected.is_unsigned);
}
