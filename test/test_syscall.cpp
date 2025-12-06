#include <sstream>

#include "catch.hpp"

#include "test_utils.hpp"

#include "pfs/procfs.hpp"
#include "pfs/parsers/syscall.hpp"
#include "pfs/parser_error.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted syscall", "[procfs][syscall]")
{
    std::string line;

    SECTION("In syscall, missing argument")
    {
        line = "232 0x4 0x55a79ea3b180 0x94 0xffffffff 0x0 0x2830 0x7ffc8543e1b0";
    }

    REQUIRE_THROWS_AS(parse_syscall_line(line), pfs::parser_error);
}

TEST_CASE("Parse syscall controller", "[procfs][syscall]")
{
    std::string line;
    pfs::syscall expected;

    SECTION("Running")
    {
        line = "running";

        expected.state                 = pfs::syscall_state::running;
        expected.number                = 0;
        expected.argument_registers[0] = 0;
        expected.argument_registers[1] = 0;
        expected.argument_registers[2] = 0;
        expected.argument_registers[3] = 0;
        expected.argument_registers[4] = 0;
        expected.argument_registers[5] = 0;
        expected.stack_pointer         = 0;
        expected.instruction_pointer   = 0;
    }

    SECTION("Blocked")
    {
        line = "232 0x7ffc8543e1b0 0x7f6e19d9768e";

        expected.state                 = pfs::syscall_state::blocked;
        expected.number                = 232;
        expected.argument_registers[0] = 0;
        expected.argument_registers[1] = 0;
        expected.argument_registers[2] = 0;
        expected.argument_registers[3] = 0;
        expected.argument_registers[4] = 0;
        expected.argument_registers[5] = 0;
        expected.stack_pointer         = 0x7ffc8543e1b0;
        expected.instruction_pointer   = 0x7f6e19d9768e;
    }

    SECTION("In syscall")
    {
        line = "260 0xffffffffffffffff 0xffffdb44c620 0xa 0x0 0x8 0xaaaaf2a24d90 0xffffdb44c560 0xffff8e4ff86c";

        expected.state                 = pfs::syscall_state::syscall;
        expected.number                = 260;
        expected.argument_registers[0] = 0xffffffffffffffff;
        expected.argument_registers[1] = 0xffffdb44c620;
        expected.argument_registers[2] = 0xa;
        expected.argument_registers[3] = 0x0;
        expected.argument_registers[4] = 0x8;
        expected.argument_registers[5] = 0xaaaaf2a24d90;
        expected.stack_pointer         = 0xffffdb44c560;
        expected.instruction_pointer   = 0xffff8e4ff86c;
    }

    temp_dir test_dir{};
    test_dir.create_file("30739/syscall", line);

    auto root_path = test_dir.get_root();
    auto syscall = pfs::procfs(root_path).get_task(30739).get_syscall();
    REQUIRE(syscall.state == expected.state);
    REQUIRE(syscall.number == expected.number);
    REQUIRE(syscall.argument_registers[0] == expected.argument_registers[0]);
    REQUIRE(syscall.argument_registers[1] == expected.argument_registers[1]);
    REQUIRE(syscall.argument_registers[2] == expected.argument_registers[2]);
    REQUIRE(syscall.argument_registers[3] == expected.argument_registers[3]);
    REQUIRE(syscall.argument_registers[4] == expected.argument_registers[4]);
    REQUIRE(syscall.argument_registers[5] == expected.argument_registers[5]);
    REQUIRE(syscall.stack_pointer == expected.stack_pointer);
    REQUIRE(syscall.instruction_pointer == expected.instruction_pointer);
}
