#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse io", "[task][io]")
{
    pfs::io_stats expected;
    std::vector<std::string> content;

    SECTION("Sample 1")
    {
        // clang-format off
        content = {
            "rchar: 4292",
            "wchar: 0",
            "syscr: 13",
            "syscw: 0",
            "read_bytes: 0",
            "write_bytes: 0",
            "cancelled_write_bytes: 0"
        };
        // clang-format on

        expected.rchar = 4292;
        expected.wchar = 0;
        expected.syscr = 13;
        expected.syscw = 0;
        expected.read_bytes = 0;
        expected.write_bytes = 0;
        expected.cancelled_write_bytes = 0;
    }

    SECTION("Sample 2")
    {
        // clang-format off
        content = {
            "rchar: 9999",
            "wchar: 8888",
            "syscr: 7777",
            "syscw: 6666",
            "read_bytes: 5555",
            "write_bytes: 4444",
            "cancelled_write_bytes: 3333"
        };
        // clang-format on

        expected.rchar = 9999;
        expected.wchar = 8888;
        expected.syscr = 7777;
        expected.syscw = 6666;
        expected.read_bytes = 5555;
        expected.write_bytes = 4444;
        expected.cancelled_write_bytes = 3333;
    }

    std::string file = create_temp_file(content);
    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });
    auto io = task_io_parser().parse(file);

    REQUIRE(io.rchar == expected.rchar);
    REQUIRE(io.wchar == expected.wchar);
    REQUIRE(io.syscr == expected.syscr);
    REQUIRE(io.syscw == expected.syscw);
    REQUIRE(io.read_bytes == expected.read_bytes);
    REQUIRE(io.write_bytes == expected.write_bytes);
    REQUIRE(io.cancelled_write_bytes == expected.cancelled_write_bytes);
}
