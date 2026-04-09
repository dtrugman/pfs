#include <fcntl.h>

#include "catch.hpp"

#include "pfs/procfs.hpp"

TEST_CASE("mem fd has O_CLOEXEC", "[mem][cloexec]")
{
    auto self   = pfs::procfs().get_task();
    auto mem    = self.get_mem();
    auto fds    = self.get_fds();

    int mem_fd = -1;
    for (const auto& [num, f] : fds)
    {
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
