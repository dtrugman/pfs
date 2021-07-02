#ifndef PFS_TEST_UTILS_HPP
#define PFS_TEST_UTILS_HPP

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/kdev_t.h>

#include <fstream>
#include <sstream>

#include "pfs/defer.hpp"

// clang-format off
#if INTPTR_MAX == INT64_MAX
    #define ARCH_64BIT
#elif INTPTR_MAX == INT32_MAX
    #define ARCH_32BIT
#else
    #error Unsupported pointer size or missing size macros
#endif
// clang-format on

template <typename T>
inline T generate_random()
{
    std::random_device rd;
    std::uniform_int_distribution<T> dist;
    return dist(rd);
}

inline std::string build_device_string(dev_t dev_major, dev_t dev_minor)
{
    std::ostringstream out;
    out << std::hex << dev_major << ":" << dev_minor;
    return out.str();
}

inline std::string build_device_string(dev_t dev)
{
    return build_device_string(MAJOR(dev), MINOR(dev));
}

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

template <typename T>
inline std::string join(const T& container)
{
    if (container.empty())
    {
        return "";
    }

    std::ostringstream out;
    for (const auto& v : container)
    {
        out << v << ',';
    }
    auto out_str = out.str();
    out_str.pop_back();
    return out_str;
}

#endif // PFS_TEST_UTILS_HPP
