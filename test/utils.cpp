#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/defer.hpp"
#include "pfs/utils.hpp"

using namespace pfs::impl::utils;

TEST_CASE("Dir termination", "[utils]")
{
    std::string original;
    std::string expected;

    SECTION("Requires termination")
    {
        original = "/proc";
        expected = "/proc/";
        ensure_dir_terminator(original);
    }

    SECTION("Already terminated")
    {
        original = "/proc/";
        expected = original;
        ensure_dir_terminator(original);
    }

    REQUIRE(original == expected);
}

TEST_CASE("Split", "[utils]")
{
    std::string input;
    std::vector<std::string> tokens;
    char delim      = ' ';
    bool keep_empty = false;

    SECTION("Single word tokens")
    {
        input  = "raid456,async_raid6_recov,async_memcpy,async_pq,";
        tokens = {"raid456", "async_raid6_recov", "async_memcpy", "async_pq"};
        delim  = ',';
    }

    SECTION("Multi word token")
    {
        input  = "first! second third !fourth";
        tokens = {"first", " second third ", "fourth"};
        delim  = '!';
    }

    SECTION("Keep empty")
    {
        input      = "::key:value:";
        tokens     = {"", "", "key", "value"};
        delim      = ':';
        keep_empty = true;
    }

    SECTION("No token")
    {
        input  = "Groups";
        tokens = {"Groups"};
    }

    REQUIRE(split(input, delim, keep_empty) == tokens);
}

TEST_CASE("Readline", "[utils]")
{
    std::vector<std::string> content;
    std::string file;

    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    SECTION("Single line file")
    {
        content = {"BOOT_IMAGE=/boot/vmlinuz-4.15.0-58-generic "
                   "root=/dev/mapper/vagrant--vg-root ro net.ifnames=0 "
                   "biosdevname=0 quiet"};
    }

    SECTION("Multi line file")
    {
        content = {"BOOT_IMAGE=/boot/vmlinuz-4.15.0-58-generic",
                   "root=/dev/mapper/vagrant--vg-root", "net.ifnames=0",
                   "biosdevname=0", "quiet"};
    }

    SECTION("Multi line with null-terminators file")
    {
        content = {"/bin/application\0argument0 -- ",
                   "argument1\0argument2\0--another-argument3"};
    }

    file = create_temp_file(content);
    REQUIRE(readline(file) == content[0]);
}
