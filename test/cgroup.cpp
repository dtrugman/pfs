#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted cgroup", "[procfs][cgroup]")
{
    // Missing last token (enabled)
    std::string line = "1:name=systemd";

    REQUIRE_THROWS_AS(parse_cgroup_line(line), pfs::parser_error);
}

TEST_CASE("Parse cgroup", "[procfs][cgroup]")
{
    pfs::cgroup expected;

    SECTION("Root pathname")
    {
        expected.hierarchy   = 6;
        expected.controllers = {std::string("cpuset")};
        expected.pathname    = "/";
    }

    SECTION("Long pathname")
    {
        expected.hierarchy   = 4;
        expected.controllers = {std::string("pids")};
        expected.pathname    = "/user.slice/user-1000.slice/session-174.scope";
    }

    SECTION("Key value controller")
    {
        expected.hierarchy   = 1;
        expected.controllers = {std::string("name=systemd")};
        expected.pathname    = "/user.slice/user-1000.slice/session-174.scope";
    }

    SECTION("V2 controller")
    {
        expected.hierarchy   = 0;
        expected.controllers = {};
        expected.pathname    = "/user.slice/user-1000.slice/session-174.scope";
    }

    std::ostringstream line;
    line << expected.hierarchy << ':';
    line << join(expected.controllers) << ':';
    line << expected.pathname;

    auto cg = parse_cgroup_line(line.str());
    REQUIRE(cg.hierarchy == expected.hierarchy);
    REQUIRE(cg.controllers == expected.controllers);
    REQUIRE(cg.pathname == expected.pathname);
}
