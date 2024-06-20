#include "catch.hpp"

#include <numeric>

#include "pfs/procfs.hpp"
#include "pfs/defer.hpp"

TEST_CASE("Parse stat", "[procfs][proc_stat]")
{
    static constexpr auto DEVIATION     = 30;
    static constexpr auto CPU_MIN_ENTRIES = 2;

    int fd = open(pfs::procfs::DEFAULT_ROOT.c_str(), O_RDONLY);
    REQUIRE(fd != -1);
    pfs::impl::defer close_fd([fd](){ close(fd); });

    auto stats_fd = pfs::procfs(fd).get_stat();
    auto stats_path = pfs::procfs().get_stat();

    SECTION("Validate CPUs' time")
    {
        auto stats = GENERATE_COPY(stats_fd, stats_path);
        REQUIRE(stats.cpus.per_item.size() + 1 >= CPU_MIN_ENTRIES);

        unsigned long long user_sum = 0;
        for (const pfs::proc_stat::cpu& cpu : stats.cpus.per_item)
        {
            user_sum += cpu.user;
        }
        unsigned long long user_delta =
            std::max(stats.cpus.total.user, user_sum) -
            std::min(stats.cpus.total.user, user_sum);
        REQUIRE(user_delta < DEVIATION);
    }

    SECTION("Validate softirq")
    {
        auto stats = GENERATE_COPY(pfs::procfs(fd).get_stat(), pfs::procfs().get_stat());
        unsigned long long sum = std::accumulate(
            stats.softirq.per_item.begin(), stats.softirq.per_item.end(), 0ULL);
        REQUIRE(sum == stats.softirq.total);
    }
}
