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

TEST_CASE("Trimming", "[utils]")
{
    std::string expected;
    std::string original;

    SECTION("ltrim")
    {
        expected = "word";
        original = "   " + expected;
        ltrim(original);
    }

    SECTION("rtrim")
    {
        expected = "/sbin/agetty -o -p -- \\u --noclear tty1 linux";
        original = expected + "                          ";
        rtrim(original);
    }

    SECTION("trim")
    {
        expected = "^_^ & (-.-)";
        original = "    " + expected + "  ";
        trim(original);
    }

    SECTION("nothing to trim")
    {
        expected = "/usr/sbin/sshd -D";
        original = expected;
        trim(original);
    }

    SECTION("ltrim all spaces")
    {
        expected = "";
        original = "          ";
        ltrim(original);
    }

    SECTION("rtrim all spaces")
    {
        expected = "";
        original = " ";
        rtrim(original);
    }

    REQUIRE(original == expected);
}

TEST_CASE("Split once", "[utils]")
{
    std::string input;
    std::pair<std::string, std::string> expected;
    char delim = ' ';

    SECTION("Single word tokens")
    {
        input    = "SigPnd 0000000000000000";
        expected = {"SigPnd", "0000000000000000"};
    }

    SECTION("Multi word token")
    {
        input    = "Groups: 4 24 27 30 46 108 113 114 1000";
        expected = {"Groups", " 4 24 27 30 46 108 113 114 1000"};
        delim    = ':';
    }

    SECTION("No first token")
    {
        input    = "=value";
        expected = {"", "value"};
        delim    = '=';
    }

    SECTION("No second token")
    {
        input    = "Groups:";
        expected = {"Groups", ""};
        delim    = ':';
    }

    SECTION("No token")
    {
        input    = "Groups";
        expected = {"Groups", ""};
    }

    auto output = split_once(input, delim);
    REQUIRE(output.first == expected.first);
    REQUIRE(output.second == expected.second);
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

TEST_CASE("Readfile", "[utils]")
{
    std::vector<std::string> content;
    std::string expected;
    std::string file;
    size_t max = 1024;
    bool trim  = true;

    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    SECTION("Null-terminated line file")
    {
        std::string line =
            "BOOT_IMAGE=/boot/vmlinuz-4.15.0-58-generic\0root=/dev/mapper/"
            "vagrant--vg-root ro net.ifnames=0\0biosdevname=0\0quite";
        expected = line;
        content  = {line};
    }

    SECTION("Long line file")
    {
        expected = "BOOT_IMAGE=/boot/vmlinuz-4.15.0-58-generic ";
        max      = expected.size();
        content  = {expected + "root=/dev/mapper/vagrant--vg-root"};
    }

    SECTION("File with trimmed terminating newline")
    {
        std::string line1 = "[ 72%] Linking CXX executable out/unittest";
        std::string line2 = "[ 92%] Built target unittest";
        std::string line3 = "[100%] Built target sample";
        expected          = line1 + '\n' + line2 + '\n' + line3;
        content           = {line1, line2, line3};
    }

    SECTION("File with non-trimmed terminating newline")
    {
        std::string line1 = "[ 72%] Linking CXX executable out/unittest";
        std::string line2 = "[ 92%] Built target unittest";
        std::string line3 = "[100%] Built target sample";
        expected          = line1 + '\n' + line2 + '\n' + line3 + '\n';
        content           = {line1, line2, line3};
        trim              = false;
    }

    file = create_temp_file(content);
    REQUIRE(readfile(file, max, trim) == expected);
}
