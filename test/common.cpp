#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse task state", "[common][state]")
{
    REQUIRE(parse_task_state('R') == pfs::task_state::running);
    REQUIRE(parse_task_state('S') == pfs::task_state::sleeping);
    REQUIRE(parse_task_state('D') == pfs::task_state::disk_sleep);
    REQUIRE(parse_task_state('Z') == pfs::task_state::zombie);
    REQUIRE(parse_task_state('T') == pfs::task_state::stopped);
    REQUIRE(parse_task_state('t') == pfs::task_state::tracing_stop);
    REQUIRE(parse_task_state('x') == pfs::task_state::dead);
    REQUIRE(parse_task_state('X') == pfs::task_state::dead);
    REQUIRE(parse_task_state('K') == pfs::task_state::wakekill);
    REQUIRE(parse_task_state('W') == pfs::task_state::waking);
    REQUIRE(parse_task_state('P') == pfs::task_state::parked);
    REQUIRE(parse_task_state('I') == pfs::task_state::idle);
    REQUIRE_THROWS_AS(parse_task_state('a'), pfs::parser_error);
}

TEST_CASE("Parse device", "[common][device]")
{
    dev_t dev;

    SECTION("Zero") { dev = 0x00; }

    SECTION("Random") { dev = generate_random<uint16_t>(); }

    auto device_str = build_device_string(dev);
    INFO(device_str);

    REQUIRE(parse_device(device_str) == dev);
}

TEST_CASE("Parse uid_map", "[common][uid_map]")
{
    pfs::id_map expected;
    std::string line;

    SECTION("Valid 1")
    {
        line = "         0          0 4294967295";

        expected.id_inside_ns  = 0;
        expected.id_outside_ns = 0;
        expected.length        = UINT32_MAX;
    }

    SECTION("Valid 2")
    {
        line = "0 1000 1";

        expected.id_inside_ns  = 0;
        expected.id_outside_ns = 1000;
        expected.length        = 1;
    }

    auto idmap = parse_id_map_line(line);
    REQUIRE(idmap.id_inside_ns == expected.id_inside_ns);
    REQUIRE(idmap.id_outside_ns == expected.id_outside_ns);
    REQUIRE(idmap.length == expected.length);
}
