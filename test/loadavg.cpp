#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse loadavg", "[procfs][loadavg]")
{
    pfs::load_average expected;
    std::string line;

    SECTION("Sample 1")
    {
        line = "0.12 0.34 5.04 1/112 5935";

        expected.last_1min         = 0.12;
        expected.last_5min         = 0.34;
        expected.last_15min        = 5.04;
        expected.runnable_tasks    = 1;
        expected.total_tasks       = 112;
        expected.last_created_task = 5935;
    }

    auto avg = parse_loadavg_line(line);
    REQUIRE(avg.last_1min == expected.last_1min);
    REQUIRE(avg.last_5min == expected.last_5min);
    REQUIRE(avg.last_15min == expected.last_15min);
    REQUIRE(avg.runnable_tasks == expected.runnable_tasks);
    REQUIRE(avg.total_tasks == expected.total_tasks);
    REQUIRE(avg.last_created_task == expected.last_created_task);
}
