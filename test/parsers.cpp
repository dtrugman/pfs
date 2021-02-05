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

    std::string file;

    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    SECTION("Sample 1")
    {
        content  = {"a", "b", "c"};
        expected = {"a", "b", "c"};
    }

    file     = create_temp_file(content);
    auto out = parse_lines<decltype(expected)>(file, parse_test_line);
    REQUIRE(out == expected);
}
