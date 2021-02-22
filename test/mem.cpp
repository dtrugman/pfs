#include "catch.hpp"

#include "pfs/procfs.hpp"

TEST_CASE("Read mem", "[mem][read]")
{
    std::vector<uint8_t> secret = {'s', 'e', 'c', 'r', 'e', 't'};
    off_t secret_offset         = reinterpret_cast<off_t>(&secret[0]);

    auto self      = pfs::procfs().get_task();
    auto mem       = self.get_mem();
    auto extracted = mem.read(secret_offset, secret.size());

    REQUIRE(secret == extracted);
}
