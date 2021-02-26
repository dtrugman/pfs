#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/defer.hpp"
#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

class obj
{
public:
    explicit obj(size_t x) : _x(x) { ++default_ctor; }

    obj(const obj& rhs) : _x(rhs._x) { ++copy_ctor; }

    obj& operator=(const obj& rhs)
    {
        _x = rhs._x;
        ++copy_operator;
        return *this;
    }

    obj(obj&& rhs) = default;
    obj& operator=(obj&& rhs) = default;

    static size_t default_ctor;
    static size_t copy_ctor;
    static size_t copy_operator;

    size_t _x;
};

size_t obj::default_ctor  = 0;
size_t obj::copy_ctor     = 0;
size_t obj::copy_operator = 0;

TEST_CASE("Parse lines move()-s", "[parsers]")
{
    auto parser = [](const std::string& line) { return obj(std::stoi(line)); };

    static const size_t ITEMS = 10;

    std::vector<std::string> content;
    for (size_t i = 0; i < ITEMS; ++i)
    {
        content.push_back(std::to_string(i));
    }

    std::string file = create_temp_file(content);
    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    std::vector<obj> output;
    parse_lines(file, std::back_inserter(output), parser);

    REQUIRE(output.size() == content.size());
    for (size_t i = 0; i < output.size(); ++i)
    {
        REQUIRE(output[i]._x == i);
    }

    REQUIRE(obj::default_ctor == ITEMS);
    REQUIRE(obj::copy_ctor == 0);
    REQUIRE(obj::copy_operator == 0);
}

TEST_CASE("Parse lines functionality", "[parsers]")
{
    auto parser = [](const std::string& line) { return line; };

    std::vector<std::string> content;
    std::vector<std::string> expected;
    std::vector<std::string> output;
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

    file = create_temp_file(content);
    parse_lines(file, std::back_inserter(output), parser, skipped);
    REQUIRE(output == expected);
}
