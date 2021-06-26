#include "catch.hpp"
#include <iostream>

#include "pfs/procfs.hpp"

TEST_CASE("Read mem", "[mem][read]")
{
    std::vector<uint8_t> secret = {'s', 'e', 'c', 'r', 'e', 't'};

    // Cast first to size_t to avoid sign expansion of the pointer value
    size_t secret_offset = reinterpret_cast<size_t>(secret.data());

    auto self      = pfs::procfs().get_task();
    auto mem       = self.get_mem();
    auto extracted = mem.read(static_cast<loff_t>(secret_offset), secret.size());

    REQUIRE(secret == extracted);
}
