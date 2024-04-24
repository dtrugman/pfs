#ifndef PFS_TEST_UTILS_HPP
#define PFS_TEST_UTILS_HPP

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

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

inline std::string build_hex_device_string(dev_t dev_major, dev_t dev_minor)
{
    std::ostringstream out;
    out << std::hex << dev_major << ":" << dev_minor;
    return out.str();
}

inline std::string build_dec_device_string(dev_t dev_major, dev_t dev_minor)
{
    std::ostringstream out;
    out << dev_major << ":" << dev_minor;
    return out.str();
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

class temp_dir
{
public:
    temp_dir()
    {
        char temp_dir[] = "/tmp/pfs_test_XXXXXX";
        char* dirpath   = mkdtemp(temp_dir);
        if (!dirpath)
        {
            throw std::runtime_error("Cannot create temp directory");
        }
        else
        {
            _sandbox_root = std::string{dirpath};
        }
    }
    ~temp_dir() noexcept { cleanup_temp_dir(); }

    const std::string& get_root() const noexcept { return _sandbox_root; }

    void create_file(const std::string& relative_path,
                     const std::string& content) const
    {
        const std::string file_path = _sandbox_root + '/' + relative_path;
        const std::string dir       = file_path.substr(0, file_path.rfind("/"));
        const int result =
            std::system((std::string{"mkdir -p "} + dir).c_str());
        if (result != 0)
        {
            throw std::runtime_error("Cannot create temp directory");
        }
        std::ofstream file(file_path);
        if (!file)
        {
            throw std::runtime_error("Cannot open temp file");
        }

        file << content;
    }

private:
    void cleanup_temp_dir() noexcept
    {
        const int result =
            std::system((std::string{"rm -rf "} + _sandbox_root).c_str());
        if (result != 0)
        {
            perror(std::string{"Failed to clean up temp dir: " + _sandbox_root}
                       .c_str());
        }
    }
    std::string _sandbox_root;
};

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
