#include "catch.hpp"

#include <cstdlib>
#include <numeric>

#include "pfs/procfs.hpp"

TEST_CASE("Parse stat", "[procfs][proc_stat]")
{
    static constexpr auto DEVIATION     = 30;
    static constexpr auto CPU_MIN_ENTRIES = 2;

    auto stats = pfs::procfs().get_stat();

    SECTION("Validate CPUs' time")
    {
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
        unsigned long long sum = std::accumulate(
            stats.softirq.per_item.begin(), stats.softirq.per_item.end(), 0);
        REQUIRE(sum == stats.softirq.total);
    }
}
