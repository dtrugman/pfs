#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/procfs.hpp"

TEST_CASE("Parse task stat", "[task][stat]")
{
    SECTION("Kernel space process")
    {
        const std::string content{
            "30739 (kworker/0:3-cgroup_destroy) I 2 0 0 0 -1 69238880 0 0 0 0 "
            "0 1485 0 0 20 0 1 0 409074 0 0 18446744073709551615 0 0 0 0 0 0 0 "
            "2147483647 0 1 0 0 17 0 0 0 0 0 0 0 0 0 0 0 0 0 0"};
        temp_dir test_dir{};

        const std::string root_path{test_dir.get_root()};
        test_dir.create_file("30739/stat", content);
        const auto stat{pfs::procfs(root_path).get_task(30739).get_stat()};
        REQUIRE(stat.pid == 30739);
        REQUIRE(stat.comm == "kworker/0:3-cgroup_destroy");
        REQUIRE(stat.state == pfs::task_state::idle);
        REQUIRE(stat.ppid == 2);
        REQUIRE(stat.pgrp == 0);
        REQUIRE(stat.session == 0);
        REQUIRE(stat.tty_nr == 0);
        REQUIRE(stat.tgpid == -1);
        REQUIRE(stat.flags == 69238880);
        REQUIRE(stat.minflt == 0);
        REQUIRE(stat.cminflt == 0);
        REQUIRE(stat.majflt == 0);
        REQUIRE(stat.cmajflt == 0);
        REQUIRE(stat.utime == 0);
        REQUIRE(stat.stime == 1485);
        REQUIRE(stat.cutime == 0);
        REQUIRE(stat.cstime == 0);
        REQUIRE(stat.priority == 20);
        REQUIRE(stat.nice == 0);
        REQUIRE(stat.num_threads == 1);
        REQUIRE(stat.itrealvalue == 0);
        REQUIRE(stat.starttime == 409074);
        REQUIRE(stat.vsize == 0);
        REQUIRE(stat.rss == 0);
        REQUIRE(stat.rsslim == 18446744073709551615UL);
        REQUIRE(stat.startcode == 0);
        REQUIRE(stat.endcode == 0);
        REQUIRE(stat.startstack == 0);
        REQUIRE(stat.kstkesp == 0);
        REQUIRE(stat.kstkeip == 0);
        REQUIRE(stat.signal == 0);
        REQUIRE(stat.blocked == 0);
        REQUIRE(stat.sigignore == 2147483647);
        REQUIRE(stat.sigcatch == 0);
        REQUIRE(stat.wchan == 1);
        REQUIRE(stat.nswap == 0);
        REQUIRE(stat.cnswap == 0);
        REQUIRE(stat.exit_signal == 17);
        REQUIRE(stat.processor == 0);
        REQUIRE(stat.rt_priority == 0);
        REQUIRE(stat.policy == 0);
        REQUIRE(stat.delayacct_blkio_ticks == 0);
        REQUIRE(stat.guest_time == 0);
        REQUIRE(stat.cguest_time == 0);
        REQUIRE(stat.start_data == 0);
        REQUIRE(stat.end_data == 0);
        REQUIRE(stat.start_brk == 0);
        REQUIRE(stat.arg_start == 0);
        REQUIRE(stat.arg_end == 0);
        REQUIRE(stat.env_start == 0);
        REQUIRE(stat.env_end == 0);
        REQUIRE(stat.exit_code == 0);
    }

    SECTION("User space process")
    {
        const std::string content{
            "63654 (less) S 3876 63654 3865 34909 63654 4194304 107 0 0 0 0 1 "
            "0 0 15 -5 1 0 671325 4456448 230 18446744073709551615 "
            "367560228864 367560955744 549025709984 0 0 0 0 0 58751527 1 0 0 "
            "17 3 0 0 0 0 0 367561021696 367561032301 367940091904 "
            "549025713116 549025713175 549025713175 549025714154 0"};
        temp_dir test_dir{};

        const std::string root_path{test_dir.get_root()};
        test_dir.create_file("63654/stat", content);
        const auto stat{pfs::procfs(root_path).get_task(63654).get_stat()};
        REQUIRE(stat.pid == 63654);
        REQUIRE(stat.comm == "less");
        REQUIRE(stat.state == pfs::task_state::sleeping);
        REQUIRE(stat.ppid == 3876);
        REQUIRE(stat.pgrp == 63654);
        REQUIRE(stat.session == 3865);
        REQUIRE(stat.tty_nr == 34909);
        REQUIRE(stat.tgpid == 63654);
        REQUIRE(stat.flags == 4194304);
        REQUIRE(stat.minflt == 107);
        REQUIRE(stat.cminflt == 0);
        REQUIRE(stat.majflt == 0);
        REQUIRE(stat.cmajflt == 0);
        REQUIRE(stat.utime == 0);
        REQUIRE(stat.stime == 1);
        REQUIRE(stat.cutime == 0);
        REQUIRE(stat.cstime == 0);
        REQUIRE(stat.priority == 15);
        REQUIRE(stat.nice == -5);
        REQUIRE(stat.num_threads == 1);
        REQUIRE(stat.itrealvalue == 0);
        REQUIRE(stat.starttime == 671325);
        REQUIRE(stat.vsize == 4456448);
        REQUIRE(stat.rss == 230);
        REQUIRE(stat.rsslim == 18446744073709551615UL);
        REQUIRE(stat.startcode == 367560228864);
        REQUIRE(stat.endcode == 367560955744);
        REQUIRE(stat.startstack == 549025709984);
        REQUIRE(stat.kstkesp == 0);
        REQUIRE(stat.kstkeip == 0);
        REQUIRE(stat.signal == 0);
        REQUIRE(stat.blocked == 0);
        REQUIRE(stat.sigignore == 0);
        REQUIRE(stat.sigcatch == 58751527);
        REQUIRE(stat.wchan == 1);
        REQUIRE(stat.nswap == 0);
        REQUIRE(stat.cnswap == 0);
        REQUIRE(stat.exit_signal == 17);
        REQUIRE(stat.processor == 3);
        REQUIRE(stat.rt_priority == 0);
        REQUIRE(stat.policy == 0);
        REQUIRE(stat.delayacct_blkio_ticks == 0);
        REQUIRE(stat.guest_time == 0);
        REQUIRE(stat.cguest_time == 0);
        REQUIRE(stat.start_data == 367561021696);
        REQUIRE(stat.end_data == 367561032301);
        REQUIRE(stat.start_brk == 367940091904);
        REQUIRE(stat.arg_start == 549025713116);
        REQUIRE(stat.arg_end == 549025713175);
        REQUIRE(stat.env_start == 549025713175);
        REQUIRE(stat.env_end == 549025714154);
        REQUIRE(stat.exit_code == 0);
    }
}
