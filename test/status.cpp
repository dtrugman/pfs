#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse status", "[task][status]")
{
    pfs::task_status expected;

    // clang-format off
    std::vector<std::string> content = {
        "Name:   bash",
        "Umask:  0002",
        "State:  S (sleeping)",
        "Tgid:   4481",
        "Ngid:   0",
        "Pid:    4481",
        "PPid:   1322",
        "TracerPid:      0",
        "Uid:    1000\t1000\t1000\t1000",
        "Gid:    1000\t1000\t1000\t1000",
        "FDSize: 256",
        "Groups: 4 24 27",
        "NStgid: 4481\t1",
        "NSpid:  4481\t1",
        "NSpgid: 4481\t1",
        "NSsid:  4481\t1",
        "VmPeak:    23848 kB",
        "VmSize:    23816 kB",
        "VmLck:         0 kB",
        "VmPin:         0 kB",
        "VmHWM:      4488 kB",
        "VmRSS:      4488 kB",
        "RssAnon:            1048 kB",
        "RssFile:            3440 kB",
        "RssShmem:              0 kB",
        "VmData:     1072 kB",
        "VmStk:       132 kB",
        "VmExe:      1040 kB",
        "VmLib:      2484 kB",
        "VmPTE:        92 kB",
        "VmSwap:        0 kB",
        "HugetlbPages:          0 kB",
        "CoreDumping:    0",
        "Threads:        1",
        "SigQ:   1/3697",
        "SigPnd: 0000000000000000",
        "ShdPnd: 0000000000000000",
        "SigBlk: 0000000000010000",
        "SigIgn: 0000000000380004",
        "SigCgt: 000000004b817efb",
        "CapInh: 0000000000000000",
        "CapPrm: 0000000000000000",
        "CapEff: 0000000000000000",
        "CapBnd: 0000003fffffffff",
        "CapAmb: 0000000000000000",
        "NoNewPrivs:     0",
        "Seccomp:        0",
        "Speculation_Store_Bypass:       vulnerable",
        "Cpus_allowed:   1",
        "Cpus_allowed_list:      0",
        "Mems_allowed:   00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001",
        "Mems_allowed_list:      0",
        "voluntary_ctxt_switches:        4731",
        "nonvoluntary_ctxt_switches:     5004" };
    // clang-format on

    std::string file = create_temp_file(content);
    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    task_status_parser parser;

    SECTION("Parse all")
    {
        auto status = parser.parse(file);
        REQUIRE(status.name == "bash");
        REQUIRE(status.umask == 0002);
        REQUIRE(status.state == pfs::task_state::sleeping);
        REQUIRE(status.tgid == 4481);
        REQUIRE(status.ngid == 0);
        REQUIRE(status.pid == 4481);
        REQUIRE(status.ppid == 1322);
        REQUIRE(status.tracer_pid == 0);
        REQUIRE(status.uid.real == 1000);
        REQUIRE(status.uid.effective == 1000);
        REQUIRE(status.uid.saved_set == 1000);
        REQUIRE(status.uid.filesystem == 1000);
        REQUIRE(status.gid.real == 1000);
        REQUIRE(status.gid.effective == 1000);
        REQUIRE(status.gid.saved_set == 1000);
        REQUIRE(status.gid.filesystem == 1000);
        REQUIRE(status.fd_size == 256);
        REQUIRE(status.groups == std::set<uid_t>{4, 24, 27});
        REQUIRE(status.ns_tgid == std::vector<pid_t>{4481, 1});
        REQUIRE(status.ns_pid == std::vector<pid_t>{4481, 1});
        REQUIRE(status.ns_pgid == std::vector<pid_t>{4481, 1});
        REQUIRE(status.ns_sid == std::vector<pid_t>{4481, 1});
        REQUIRE(status.vm_peak == 23848);
        REQUIRE(status.vm_size == 23816);
        REQUIRE(status.vm_lck == 0);
        REQUIRE(status.vm_pin == 0);
        REQUIRE(status.vm_hwm == 4488);
        REQUIRE(status.vm_rss == 4488);
        REQUIRE(status.rss_anon == 1048);
        REQUIRE(status.rss_file == 3440);
        REQUIRE(status.rss_shmem == 0);
        REQUIRE(status.vm_data == 1072);
        REQUIRE(status.vm_stk == 132);
        REQUIRE(status.vm_exe == 1040);
        REQUIRE(status.vm_lib == 2484);
        REQUIRE(status.vm_pte == 92);
        REQUIRE(status.vm_swap == 0);
        REQUIRE(status.huge_tlb_pages == 0);
        REQUIRE(status.core_dumping == false);
        REQUIRE(status.threads == 1);
        REQUIRE(status.sig_q == std::make_pair<size_t, size_t>(1, 3697));
        REQUIRE(status.sig_pnd == pfs::signal_mask(0x0000000000000000));
        REQUIRE(status.shd_pnd == pfs::signal_mask(0x0000000000000000));
        REQUIRE(status.sig_blk == pfs::signal_mask(0x0000000000010000));
        REQUIRE(status.sig_ign == pfs::signal_mask(0x0000000000380004));
        REQUIRE(status.sig_cgt == pfs::signal_mask(0x000000004b817efb));
        REQUIRE(status.cap_inh == pfs::capabilities_mask(0x0000000000000000));
        REQUIRE(status.cap_prm == pfs::capabilities_mask(0x0000000000000000));
        REQUIRE(status.cap_eff == pfs::capabilities_mask(0x0000000000000000));
        REQUIRE(status.cap_bnd == pfs::capabilities_mask(0x0000003fffffffff));
        REQUIRE(status.cap_amb == pfs::capabilities_mask(0x0000000000000000));
        REQUIRE(status.no_new_privs == false);
        REQUIRE(status.seccomp_mode == pfs::task_status::seccomp::disabled);
        REQUIRE(status.voluntary_ctxt_switches == 4731);
        REQUIRE(status.nonvoluntary_ctxt_switches == 5004);
    }

    SECTION("Parse specific keys")
    {
        pfs::task_status::uid_set empty_uids;
        pfs::signal_mask empty_signal_mask;
        pfs::capabilities_mask empty_cap_mask;

        auto status = parser.parse(file, {"Pid"});
        REQUIRE(status.pid == 4481);

        // Expected default values
        REQUIRE(status.name.empty());
        REQUIRE(status.umask == 0);
        REQUIRE(status.state == pfs::task_state::running);
        REQUIRE(status.tgid == pfs::INVALID_PID);
        REQUIRE(status.ngid == pfs::INVALID_PID);
        REQUIRE(status.ppid == pfs::INVALID_PID);
        REQUIRE(status.tracer_pid == pfs::INVALID_PID);
        REQUIRE(status.uid == empty_uids);
        REQUIRE(status.gid == empty_uids);
        REQUIRE(status.fd_size == 0);
        REQUIRE(status.groups.empty());
        REQUIRE(status.ns_tgid.empty());
        REQUIRE(status.ns_pid.empty());
        REQUIRE(status.ns_pgid.empty());
        REQUIRE(status.ns_sid.empty());
        REQUIRE(status.vm_peak == 0);
        REQUIRE(status.vm_size == 0);
        REQUIRE(status.vm_lck == 0);
        REQUIRE(status.vm_pin == 0);
        REQUIRE(status.vm_hwm == 0);
        REQUIRE(status.vm_rss == 0);
        REQUIRE(status.rss_anon == 0);
        REQUIRE(status.rss_file == 0);
        REQUIRE(status.rss_shmem == 0);
        REQUIRE(status.vm_data == 0);
        REQUIRE(status.vm_stk == 0);
        REQUIRE(status.vm_exe == 0);
        REQUIRE(status.vm_lib == 0);
        REQUIRE(status.vm_pte == 0);
        REQUIRE(status.vm_swap == 0);
        REQUIRE(status.huge_tlb_pages == 0);
        REQUIRE(status.core_dumping == false);
        REQUIRE(status.threads == 1);
        REQUIRE(status.sig_q == std::make_pair<size_t, size_t>(0, 0));
        REQUIRE(status.sig_pnd == empty_signal_mask);
        REQUIRE(status.shd_pnd == empty_signal_mask);
        REQUIRE(status.sig_blk == empty_signal_mask);
        REQUIRE(status.sig_ign == empty_signal_mask);
        REQUIRE(status.sig_cgt == empty_signal_mask);
        REQUIRE(status.cap_inh == empty_cap_mask);
        REQUIRE(status.cap_prm == empty_cap_mask);
        REQUIRE(status.cap_eff == empty_cap_mask);
        REQUIRE(status.cap_bnd == empty_cap_mask);
        REQUIRE(status.cap_amb == empty_cap_mask);
        REQUIRE(status.no_new_privs == false);
        REQUIRE(status.seccomp_mode == pfs::task_status::seccomp::disabled);
        REQUIRE(status.voluntary_ctxt_switches == 0);
        REQUIRE(status.nonvoluntary_ctxt_switches == 0);
    }
}
