#include "catch.hpp"
#include "test_utils.hpp"

#include <cstdlib>
#include <numeric>

#include "pfs/parsers/proc_stat.hpp"
#include "pfs/procfs.hpp"

using namespace pfs::impl::parsers;

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
            stats.softirq.per_item.begin(), stats.softirq.per_item.end(), 0ULL);
        REQUIRE(sum == stats.softirq.total);
    }
}

TEST_CASE("Parse errors", "[procfs][proc_stat][error]")
{
    std::string field;

    SECTION("ctxt") { field = "ctxt"; }
    SECTION("processes") { field = "processes"; }
    SECTION("procs_running") { field = "procs_running"; }
    SECTION("procs_blocked") { field = "procs_blocked"; }

    std::vector<std::string> content = {field + " not_a_number"};
    std::string file = create_temp_file(content);
    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    REQUIRE_THROWS_WITH(proc_stat_parser().parse(file),
                        Catch::Contains(field));
}
