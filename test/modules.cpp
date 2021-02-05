#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse modules", "[procfs][modules]")
{
    pfs::module expected;
    std::string line;

    SECTION("No dependencies")
    {
        line = "raid1 40960 0 - Live 0xffffffffc03eb000";

        expected.name           = "raid1";
        expected.size           = 40960;
        expected.instances      = 0;
        expected.dependencies   = {};
        expected.current_state  = pfs::module::state::live;
        expected.offset         = 0xffffffffc03eb000;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
    }

    SECTION("Unsigned and Out-of-tree")
    {
        line = "vboxsf 77824 2 - Live 0xffffffffc0759000 (OE)";

        expected.name           = "vboxsf";
        expected.size           = 77824;
        expected.instances      = 2;
        expected.dependencies   = {};
        expected.current_state  = pfs::module::state::live;
        expected.offset         = 0xffffffffc0759000;
        expected.is_out_of_tree = true;
        expected.is_unsigned    = true;
    }

    SECTION("Single dependency")
    {
        line = "libcrc32c 16384 1 raid456, Live 0xffffffffc03b6000";

        expected.name           = "libcrc32c";
        expected.size           = 16384;
        expected.instances      = 1;
        expected.dependencies   = {"raid456"};
        expected.current_state  = pfs::module::state::live;
        expected.offset         = 0xffffffffc03b6000;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
    }

    SECTION("Multi dependencies")
    {
        line =
            "raid6_pq 114688 4 btrfs,raid456,async_raid6_recov,async_pq, Live "
            "0xffffffffc03f6000";

        expected.name           = "raid6_pq";
        expected.size           = 114688;
        expected.instances      = 4;
        expected.dependencies   = {"btrfs", "raid456", "async_raid6_recov",
                                 "async_pq"};
        expected.current_state  = pfs::module::state::live;
        expected.offset         = 0xffffffffc03f6000;
        expected.is_out_of_tree = false;
        expected.is_unsigned    = false;
        ;
    }

    auto module = parse_modules_line(line);
    REQUIRE(module.name == expected.name);
    REQUIRE(module.size == expected.size);
    REQUIRE(module.instances == expected.instances);
    REQUIRE(module.dependencies == expected.dependencies);
    REQUIRE(module.current_state == expected.current_state);
    REQUIRE(module.offset == expected.offset);
    REQUIRE(module.is_out_of_tree == expected.is_out_of_tree);
    REQUIRE(module.is_unsigned == expected.is_unsigned);
}
