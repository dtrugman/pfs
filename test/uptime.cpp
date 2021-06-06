#include "catch.hpp"

#include <chrono>

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse uptime", "[procfs][uptime]")
{
    pfs::uptime expected;
    std::string line;

    SECTION("Sample 1")
    {
        line = "523340.48 2063904.01";
        expected.system_time = std::chrono::duration_cast<
            std::chrono::steady_clock::duration
        >(std::chrono::duration<double>(523340.48));
        expected.idle_time = std::chrono::duration_cast<
            std::chrono::steady_clock::duration
        >(std::chrono::duration<double>(2063904.01));
    }

    auto uptime = parse_uptime_line(line);
    REQUIRE(uptime.system_time == expected.system_time);
    REQUIRE(uptime.idle_time == expected.idle_time);
}
