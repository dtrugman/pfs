#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted cgroup controller", "[procfs][cgroup]")
{
    // Missing last token (enabled)
    std::string line = "devices 12      33";

    REQUIRE_THROWS_AS(parse_cgroup_controller_line(line), pfs::parser_error);
}

TEST_CASE("Parse cgroup controller", "[procfs][cgroup]")
{
    pfs::cgroup_controller expected;

    SECTION("Enabled")
    {
        expected.subsys_name = "perf_event";
        expected.hierarchy   = 11;
        expected.num_cgroups = 1;
        expected.enabled     = true;
    }

    SECTION("Disabled")
    {
        expected.subsys_name = "hugetlb";
        expected.hierarchy   = 0;
        expected.num_cgroups = 1;
        expected.enabled     = false;
    }

    std::ostringstream line;
    line << expected.subsys_name << '\t';
    line << expected.hierarchy << '\t';
    line << expected.num_cgroups << '\t';
    line << (expected.enabled ? '1' : '0');

    auto controller = parse_cgroup_controller_line(line.str());
    REQUIRE(controller.subsys_name == expected.subsys_name);
    REQUIRE(controller.hierarchy == expected.hierarchy);
    REQUIRE(controller.num_cgroups == expected.num_cgroups);
    REQUIRE(controller.enabled == expected.enabled);
}
