#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/defer.hpp"
#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

std::string parse_test_line(const std::string& line)
{
    return line;
}

TEST_CASE("Parse lines", "[parsers]")
{
    std::vector<std::string> content;
    std::set<std::string> expected;
    size_t skipped = 0;

    std::string file;

    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    SECTION("No skipped lines")
    {
        content  = {"a", "b", "c"};
        expected = {"a", "b", "c"};
    }

    SECTION("Skip lines")
    {
        content  = {"a", "b", "c"};
        expected = {"b", "c"};
        skipped  = 1;
    }

    SECTION("Skip all lines")
    {
        content  = {"a", "b", "c"};
        expected = {};
        skipped  = 5;
    }

    file     = create_temp_file(content);
    auto out = parse_lines<decltype(expected)>(file, parse_test_line, skipped);
    REQUIRE(out == expected);
}
