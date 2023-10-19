#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers/block_stat.hpp"
#include "pfs/parser_error.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted block stat", "[block][stat]")
{
    SECTION("Missing token")
    {
        std::string line =
            "12735     9101   833156     2926    58398   181703  8929392    38810        0    25238";

        REQUIRE_THROWS_AS(parse_block_stat_line(line), pfs::parser_error);
    }
}

TEST_CASE("Parse block stat", "[block][stat]")
{
    pfs::block_stat expected;

    std::string line =
            "12735     9101   833156     2926    58398   181703  8929392    38810      "
            "0    25238    47489     3213        0 102155080      337    23765     5414";

    expected.read_ios = 12735;
    expected.read_merges = 9101;
    expected.read_sectors = 833156;
    expected.read_ticks = 2926;
    expected.write_ios = 58398;
    expected.write_merges = 181703;
    expected.write_sectors = 8929392;
    expected.write_ticks = 38810;
    expected.in_flight = 0;
    expected.io_ticks = 25238;
    expected.time_in_queue = 47489;
    expected.discard_ios = 3213;
    expected.discard_merges = 0;
    expected.discard_sectors = 102155080;
    expected.discard_ticks = 337;
    expected.flush_ios = 23765;
    expected.flush_ticks = 5414;

    auto stat = parse_block_stat_line(line);

    REQUIRE(stat.read_ios == expected.read_ios);
    REQUIRE(stat.read_merges == expected.read_merges);
    REQUIRE(stat.read_sectors == expected.read_sectors);
    REQUIRE(stat.read_ticks == expected.read_ticks);
    REQUIRE(stat.write_ios == expected.write_ios);
    REQUIRE(stat.write_merges == expected.write_merges);
    REQUIRE(stat.write_sectors == expected.write_sectors);
    REQUIRE(stat.write_ticks == expected.write_ticks);
    REQUIRE(stat.in_flight == expected.in_flight);
    REQUIRE(stat.io_ticks == expected.io_ticks);
    REQUIRE(stat.time_in_queue == expected.time_in_queue);
    REQUIRE(stat.discard_ios == expected.discard_ios);
    REQUIRE(stat.discard_merges == expected.discard_merges);
    REQUIRE(stat.discard_sectors == expected.discard_sectors);
    REQUIRE(stat.discard_ticks == expected.discard_ticks);
    REQUIRE(stat.flush_ios == expected.flush_ios);
    REQUIRE(stat.flush_ticks == expected.flush_ticks);
}
