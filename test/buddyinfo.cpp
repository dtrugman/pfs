#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse buddyinfo", "[procfs][buddyinfo]")
{
    pfs::zone expected;
    std::string line;

    SECTION("Sample 1")
    {
        line = "Node 0, zone  Normal   216   55  189  101   84   38   37  "
               " 27    5    3  587";

        expected.node_id = 0;
        expected.name    = "Normal";
        expected.chunks  = {216, 55, 189, 101, 84, 38, 37, 27, 5, 3, 587};
    }

    SECTION("Sample 2")
    {
        line = "Node 1, zone     DMA     1    1    1    0    2    1    1  "
               "  0    1    1    3";

        expected.node_id = 1;
        expected.name    = "DMA";
        expected.chunks  = {1, 1, 1, 0, 2, 1, 1, 0, 1, 1, 3};
    }

    auto zone = parse_buddyinfo_line(line);
    REQUIRE(zone.node_id == expected.node_id);
    REQUIRE(zone.name == expected.name);
    REQUIRE(zone.chunks == expected.chunks);
}
