#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted meminfo", "[procfs][meminfo]")
{
    // Missing last token (value)
    std::string line = "VmallocTotal";

    REQUIRE_THROWS_AS(parse_meminfo_line(line), pfs::parser_error);
}

TEST_CASE("Parse meminfo", "[procfs][meminfo]")
{
    std::stringstream line;

    std::string description;
    size_t amount;

    SECTION("With kB")
    {
        description = "VmallocTotal";
#if defined(ARCH_64BIT)
        amount      = 34359738367;
#elif defined(ARCH_32BIT)
        amount      = 512056;
#endif

        line << description << ":   " << amount << " kB";
    }

    SECTION("Zero with kB")
    {
        description = "VmallocUsed";
        amount      = 0;

        line << description << ":           " << amount << " kB";
    }

    SECTION("Zero without kB")
    {
        description = "HugePages_Total";
        amount      = 0;

        line << description << ":       " << amount;
    }

    auto output = parse_meminfo_line(line.str());
    REQUIRE(output.first == description);
    REQUIRE(output.second == amount);
}
