#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse filesystems", "[procfs][filesystems]")
{
    std::string fs;
    bool device_required;

    SECTION("No device")
    {
        fs              = "sysfs";
        device_required = false;
    }

    SECTION("Device")
    {
        fs              = "ext3";
        device_required = true;
    }

    std::string line = (device_required ? "\t" : "nodev");
    line += '\t';
    line += fs;

    INFO(line);
    auto out = parse_filesystems_line(line);
    REQUIRE(out.first == fs);
    REQUIRE(out.second == device_required);
}
