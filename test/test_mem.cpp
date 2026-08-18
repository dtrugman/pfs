#include <fcntl.h>

#include "catch.hpp"
#include <memory>

#include "pfs/procfs.hpp"

static_assert(!std::is_copy_constructible<pfs::mem>::value,
              "mem must not be copy constructible");
static_assert(!std::is_copy_assignable<pfs::mem>::value,
              "mem must not be copy assignable");

TEST_CASE("mem fd has O_CLOEXEC", "[mem][cloexec]")
{
    auto self   = pfs::procfs().get_task();
    auto mem    = self.get_mem();
    auto fds    = self.get_fds();

    int mem_fd = -1;
    for (const auto& fd : fds)
    {
        const auto& num = fd.first;
        const auto& f = fd.second;
        try
        {
            if (f.get_target().find("/mem") != std::string::npos)
            {
                mem_fd = num;
                break;
            }
        }
        catch (const std::system_error&)
        {
            // fd was closed between enumeration and target resolution (e.g.
            // the dirfd used internally by get_fds()); skip it
        }
    }
    REQUIRE(mem_fd != -1);

    int flags = fcntl(mem_fd, F_GETFD);
    REQUIRE(flags != -1);
    REQUIRE(flags & FD_CLOEXEC);
}

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

TEST_CASE("Move mem transfers ownership without double-close", "[mem][move]")
{
    std::vector<uint8_t> secret = {'s', 'e', 'c', 'r', 'e', 't'};

    // Cast first to size_t to avoid sign expansion of the pointer value
    size_t secret_offset = reinterpret_cast<size_t>(secret.data());

    auto self = pfs::procfs().get_task();

    std::unique_ptr<pfs::mem> mem1(new pfs::mem(self.get_mem()));
    pfs::mem mem2 = std::move(*mem1);
    // Destroy the moved-from object immediately. If the move constructor did not
    // clear mem1's fd, this will close the fd still owned by mem2.
    mem1.reset();

    // Use mem2 here and verify it still works.
    auto extracted = mem2.read(static_cast<loff_t>(secret_offset), secret.size());
    REQUIRE(secret == extracted);
}
