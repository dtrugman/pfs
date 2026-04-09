#include <unistd.h>

#include "catch.hpp"

#include "pfs/procfs.hpp"

TEST_CASE("get_processes without filter returns all processes", "[procfs][filter]")
{
    auto all     = pfs::procfs().get_processes();
    auto own_pid = getpid();

    // The current process must appear in the full list
    auto it = std::find_if(all.begin(), all.end(),
                           [own_pid](const pfs::task& t) {
                               return t.id() == own_pid;
                           });
    REQUIRE(it != all.end());
}

TEST_CASE("get_processes with drop-all filter returns empty set", "[procfs][filter]")
{
    auto drop_all = [](const pfs::task&) { return pfs::filter::action::drop; };
    auto result   = pfs::procfs().get_processes(drop_all);
    REQUIRE(result.empty());
}

TEST_CASE("get_processes with keep-self filter returns only current process", "[procfs][filter]")
{
    auto own_pid  = getpid();
    auto keep_self = [own_pid](const pfs::task& t) {
        return t.id() == own_pid ? pfs::filter::action::keep
                                 : pfs::filter::action::drop;
    };

    auto result = pfs::procfs().get_processes(keep_self);
    REQUIRE(result.size() == 1);
    REQUIRE(result.begin()->id() == own_pid);
}

TEST_CASE("get_tasks without filter returns all threads", "[task][filter]")
{
    auto self    = pfs::procfs().get_task();
    auto all     = self.get_tasks();
    auto own_tid = getpid(); // main thread tid == pid

    auto it = std::find_if(all.begin(), all.end(),
                           [own_tid](const pfs::task& t) {
                               return t.id() == own_tid;
                           });
    REQUIRE(it != all.end());
}

TEST_CASE("get_tasks with drop-all filter returns empty set", "[task][filter]")
{
    auto drop_all = [](const pfs::task&) { return pfs::filter::action::drop; };
    auto result   = pfs::procfs().get_task().get_tasks(drop_all);
    REQUIRE(result.empty());
}
