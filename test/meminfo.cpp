#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse meminfo", "[procfs][meminfo]")
{
    std::string line;

    std::string description;
    size_t amount;

    SECTION("With kB")
    {
        line = "VmallocTotal:   34359738367 kB";

        description = "VmallocTotal";
        amount      = 34359738367;
    }

    SECTION("Zero with kB")
    {
        line = "VmallocUsed:           0 kB";

        description = "VmallocUsed";
        amount      = 0;
    }

    SECTION("Zero without kB")
    {
        line = "HugePages_Total:       0";

        description = "HugePages_Total";
        amount      = 0;
    }

    auto output = parse_meminfo_line(line);
    REQUIRE(output.first == description);
    REQUIRE(output.second == amount);
}
