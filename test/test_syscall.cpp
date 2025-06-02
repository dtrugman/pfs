#include <sstream>

#include "catch.hpp"
#include "test_utils.hpp"
#include "pfs/procfs.hpp"

#include "pfs/parsers/syscall.hpp"
#include "pfs/parser_error.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse corrupted syscall controller", "[procfs][syscall]")
{
    // Missing last token (enabled)

    const std::string content = "232 0x4 0x55a79ea3b180 0x94 0xffffffff 0x0 0x2830 0x7ffc8543e1b0";
    temp_dir test_dir{};

    const std::string root_path{test_dir.get_root()};
    test_dir.create_file("30739/syscall", content);
   // const auto syscall {pfs::procfs(root_path).get_task(30739).get_syscall()};
    
    REQUIRE_THROWS_AS(pfs::procfs(root_path).get_task(30739).get_syscall(), pfs::parser_error);
}

TEST_CASE("Parse syscall controller", "[procfs][syscall]")
{
    const std::string content = "232 0x4 0x55a79ea3b180 0x94 0xffffffff 0x0 0x2830 0x7ffc8543e1b0 0x7f6e19d9768e";

    const size_t sp = 140722544304560; //sp in decimal hex 0x7ffc8543e1b0
    const size_t pc = 140110856812174; //pc in decimal hex 0x7f6e19d9768e
    std::vector<size_t> args = {4, 94178409427328, 148, 4294967295, 0, 10288}; //args in decimale
    temp_dir test_dir{};

    const std::string root_path{test_dir.get_root()};
    test_dir.create_file("30739/syscall", content);
    const auto syscall{pfs::procfs(root_path).get_task(30739).get_syscall()};

    REQUIRE(syscall.number_of_syscall == 232);
    REQUIRE(syscall.arguments == args);
    REQUIRE(syscall.program_counter == pc);
    REQUIRE(syscall.stack_pointer == sp);
}
