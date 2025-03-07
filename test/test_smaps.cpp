#include "catch.hpp"
#include "test_utils.hpp"

#include "pfs/parsers/mem_map.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse smaps", "[task][smaps]")
{
    // clang-format off
    std::vector<std::string> content = {
        // -- First region header --
        "c0fbeffd0000-c0fbeffe5000 r-xp 00000000 fc:00 1750349                    /usr/lib/systemd/systemd",
        "Size:                 100 kB",
        "KernelPageSize:         5 kB",
        "MMUPageSize:            6 kB",
        "Rss:                  110 kB",
        "Pss:                   55 kB",
        "Pss_Dirty:             10 kB",
        "Shared_Clean:         120 kB",
        "Shared_Dirty:           5 kB",
        "Private_Clean:          15 kB",
        "Private_Dirty:          20 kB",
        "Referenced:           130 kB",
        "Anonymous:             25 kB",
        "KSM:                    2 kB",
        "LazyFree:              30 kB",
        "AnonHugePages:         35 kB",
        "ShmemPmdMapped:        40 kB",
        "FilePmdMapped:         45 kB",
        "Shared_Hugetlb:        50 kB",
        "Private_Hugetlb:       55 kB",
        "Swap:                  60 kB",
        "SwapPss:               65 kB",
        "Locked:                70 kB",
        "THPeligible:            1",
        "VmFlags: rd ex mr mw",
        // -- Second region header --
        "fdef41c40000-fdef41c64000 r-xp 00000000 fc:00 1705464                    /usr/lib/aarch64-linux-gnu/libgpg-error.so.0.34.0",
        "Size:                200 kB",
        "KernelPageSize:        8 kB",
        "MMUPageSize:           9 kB",
        "Rss:                 210 kB",
        "Pss:                  90 kB",
        "Pss_Dirty:            15 kB",
        "Shared_Clean:        220 kB",
        "Shared_Dirty:          10 kB",
        "Private_Clean:         30 kB",
        "Private_Dirty:         40 kB",
        "Referenced:          230 kB",
        "Anonymous:             50 kB",
        "KSM:                    3 kB",
        "LazyFree:              35 kB",
        "AnonHugePages:         40 kB",
        "ShmemPmdMapped:        45 kB",
        "FilePmdMapped:         50 kB",
        "Shared_Hugetlb:        55 kB",
        "Private_Hugetlb:       60 kB",
        "Swap:                  65 kB",
        "SwapPss:               70 kB",
        "Locked:                75 kB",
        "THPeligible:            0",
        "VmFlags: rd ex mr mw me"
    };
    // clang-format on

    std::string file = create_temp_file(content);
    pfs::impl::defer unlink_temp_file([&file] { unlink(file.c_str()); });

    SECTION("Parse all maps")
    {
        auto smaps = parse_mem_map(file);
        REQUIRE(smaps.size() == 2);

        REQUIRE(smaps[0].region.start_address == 0xc0fbeffd0000);
        REQUIRE(smaps[0].region.end_address == 0xc0fbeffe5000);
        REQUIRE(smaps[0].region.pathname == "/usr/lib/systemd/systemd");

        REQUIRE(smaps[0].size == 100);
        REQUIRE(smaps[0].kernel_page_size == 5);
        REQUIRE(smaps[0].mmu_page_size == 6);
        REQUIRE(smaps[0].rss == 110);
        REQUIRE(smaps[0].pss == 55);
        REQUIRE(smaps[0].pss_dirty == 10);
        REQUIRE(smaps[0].shared_clean == 120);
        REQUIRE(smaps[0].shared_dirty == 5);
        REQUIRE(smaps[0].private_clean == 15);
        REQUIRE(smaps[0].private_dirty == 20);
        REQUIRE(smaps[0].referenced == 130);
        REQUIRE(smaps[0].anonymous == 25);
        REQUIRE(smaps[0].ksm == 2);
        REQUIRE(smaps[0].lazy_free == 30);
        REQUIRE(smaps[0].anon_huge_pages == 35);
        REQUIRE(smaps[0].shmem_pmd_mapped == 40);
        REQUIRE(smaps[0].file_pmd_mapped == 45);
        REQUIRE(smaps[0].shared_hugetlb == 50);
        REQUIRE(smaps[0].private_hugetlb == 55);
        REQUIRE(smaps[0].swap == 60);
        REQUIRE(smaps[0].swap_pss == 65);
        REQUIRE(smaps[0].locked == 70);
        REQUIRE(smaps[0].thp_eligible == true);
        REQUIRE(smaps[0].vm_flags == "rd ex mr mw");

        REQUIRE(smaps[1].region.start_address == 0xfdef41c40000);
        REQUIRE(smaps[1].region.end_address == 0xfdef41c64000);
        REQUIRE(smaps[1].region.pathname ==
                "/usr/lib/aarch64-linux-gnu/libgpg-error.so.0.34.0");

        REQUIRE(smaps[1].size == 200);
        REQUIRE(smaps[1].kernel_page_size == 8);
        REQUIRE(smaps[1].mmu_page_size == 9);
        REQUIRE(smaps[1].rss == 210);
        REQUIRE(smaps[1].pss == 90);
        REQUIRE(smaps[1].pss_dirty == 15);
        REQUIRE(smaps[1].shared_clean == 220);
        REQUIRE(smaps[1].shared_dirty == 10);
        REQUIRE(smaps[1].private_clean == 30);
        REQUIRE(smaps[1].private_dirty == 40);
        REQUIRE(smaps[1].referenced == 230);
        REQUIRE(smaps[1].anonymous == 50);
        REQUIRE(smaps[1].ksm == 3);
        REQUIRE(smaps[1].lazy_free == 35);
        REQUIRE(smaps[1].anon_huge_pages == 40);
        REQUIRE(smaps[1].shmem_pmd_mapped == 45);
        REQUIRE(smaps[1].file_pmd_mapped == 50);
        REQUIRE(smaps[1].shared_hugetlb == 55);
        REQUIRE(smaps[1].private_hugetlb == 60);
        REQUIRE(smaps[1].swap == 65);
        REQUIRE(smaps[1].swap_pss == 70);
        REQUIRE(smaps[1].locked == 75);
        REQUIRE(smaps[1].thp_eligible == false);
        REQUIRE(smaps[1].vm_flags == "rd ex mr mw me");
    }
}
