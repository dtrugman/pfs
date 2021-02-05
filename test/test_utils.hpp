#ifndef PFS_TEST_UTILS_HPP
#define PFS_TEST_UTILS_HPP

#include <stdlib.h>
#include <unistd.h>

#include <fstream>

#include "pfs/defer.hpp"

inline std::string create_temp_file(const std::vector<std::string>& lines)
{
    char temp[] = "/tmp/pfs_ut_XXXXXX";
    int fd      = mkstemp(temp);
    if (fd == -1)
    {
        throw std::runtime_error("Cannot create temp file");
    }
    pfs::impl::defer close_fd([fd] { close(fd); });

    std::ofstream out(temp);
    if (!out)
    {
        throw std::runtime_error("Cannot open temp file");
    }

    for (const auto& line : lines)
    {
        out << line << std::endl;
    }

    return std::string(temp);
}

#endif // PFS_TEST_UTILS_HPP
