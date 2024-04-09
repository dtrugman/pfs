![pfs](./img/pfs.png "Logo")

Production grade, very easy to use, procfs parsing library in C++.
Used in production by S&P 500 tech companies and startups!

**NEW** Basic parsing of sysfs (Additional `sysfs` feature requests are welcome!)

## Build

![Build & Test](https://github.com/dtrugman/pfs/actions/workflows/cmake.yml/badge.svg)

Run `cmake . && make`

### Currently supported CMake configuration flags:

- `CMAKE_BUILD_TYPE=<Debug|Release>`: Standard CMake flags to control build type (DEFAULT: Debug)
- `pfs_BUILD_SHARED_LIBS=<ON|OFF>`: ON to compile a shared library. OFF to compile a static library (DEFAULT: Inherit `BUILD_SHARE_LIBS`, which is `OFF` by default))
- `pfs_BUILD_ASAN=<ON|OFF>`: ON to enable address sanitizer (DEFAULT: `OFF`)
- `pfs_BUILD_SAMPLES=<ON|OFF>`: ON to build the sample programs (DEFAULT: `ON`)
- `pfs_BUILD_TESTS=<ON|OFF>`: ON to build the tests (DEFAULT: `ON`)

You can pass any number of those to the `cmake` command: `cmake -D<CONFIG_FLAG>=<VALUE> .`

**NOTE**: After running `cmake` for the first time, some values are cached in `CMakeCache.txt` and will not change when running `cmake` for a second time with different flags.

### Build using clang

If you prefer using clang, just configure the compiler while running cmake:

`CXX=<clang++> CC=<clang> cmake .`

After that, just use `make` as always.

## Integrate

- Compile as a shared or static library.
- Add the contents of `/lib` into your link directories
- Add the contents of `/include` into your include directories.
That's it, you are good to go.

### Use CMake's [`find_package()`](https://cmake.org/cmake/help/latest/command/find_package.html)

#### Option #1: `make install` (Preferred way)

After building the project, you can install it locally using `make install`.
In your project's CMake file, you can then add the following snippet, and CMake will handle the rest:
```
find_package (pfs REQUIRED)
...
# Somewhere along the file you define your target
add_<library|executable> (<your-target> ...)
...
target_link_libraries (<your-target> pfs)
```

NOTE: CMake generates an `install_manifest.txt` file to track all the created files, this will help you uninstall the library if you need to do so.

#### Option #2: Without `make install`

Build the `pfs` project. No need to call `make install`.
In your project's CMake file, you can then add the following snippet:
```
find_package (pfs REQUIRED)
...
# Somewhere along the file you define your target
add_<library|executable> (<your-target> ...)
...
target_link_libraries (<your-target> -L${pfs_LIBRARY_DIR} ${pfs_LIBRARIES})
target_include_directories (<your-target> [PUBLIC|PRIVATE] ${pfs_INCLUDE_DIRS})
```

## Features

- Parsing system-wide information from files directly under `/procfs`. See `procfs.hpp` for all the supported files.
- Parsing per-task (processes and threads) information from files under `/procfs/[task-id]/`. See `task.hpp` for all the supported files.
- Parsing network information from files under `/procfs/net` (which is an alias to `/procfs/self/net` nowadays)
- **NEW** Parsing of basic disk information from `sysfs/block` (Additional `sysfs` feature requests are welcome!)

## Requirements

- The library requires C++11 or newer
- The library aims to support Linux kernel versions >= 2.6.32.

## Notes

### General notes

- All APIs and function calls might throw `std::bad_alloc` exceptions when allocations of standard containers such as `std::string` fail.
- APIs are thread-safe. There are no internal states/members/caches that might be affected by simultaneous calls.
- Objects do NOT handle data caching. All the APIs are pure getters that always(!) fetch the information from the filesystem.
- The location of the procfs filesystem is configurable. Just create the `procfs` object with the right path for your machine.

### Accessing inexisting tasks

If you call `procfs().get_task(<id>)` and that task doesn't really exist, the constructor will succeed.

Since tasks can die any time, instead of adding extra validation during construction, which might be confusing, the current design assumes the first call after the tasks died will fail.

### Collecting thread information

There are two ways to collect information about a thread:
1. `/proc/<tid>`
1. `/proc/<pid>/task/<tid>`
And the amazing fact is that they MIGHT provide different information.

For example, when accessing using the first path, the `utime` and `stime` values under `stat` represent the amount of time scheduled for the entire process(!), whereas when accessing using the second path, they represent the amount of time scheduled for that thread only.

How does that affect `pfs`?
- When using `procfs().get_task(<id>)` you'll be accessing information using `/proc/<tid>`.
- When using `my_task = procfs().get_task(<pid>)` and then `my_task.get_task(<id>)` OR `my_task.get_tasks()` you'll be accessing information through `/proc/<pid>/task/<tid>`

## Samples

The directory `sample` contains a full blown application that calls all(!) the supported APIs and prints all the information gathered. When compiling the library, the sample applications is compiled as well.

You can find a basic implementations of `netstat` (see `sample/tool_netstat.cpp`) and `lsmod` (see `sample/tool_lsmod.cpp`) that you can easily reuse in your projects.

Anyway, here are some cool (and concise) examples:

**Example 1:** Find all the process that hold an open file descriptor to a specific file:
```
auto file = "/path/to/file";
auto pfs = pfs::procfs();
for (const auto& process : pfs.get_processes())
{
    for (const auto& thread : process.get_tasks())
    {
        for (const auto& fd : thread.get_fds())
        {
            if (fd.second.get_target() == file)
            {
                ... do something ...
            }
        }
    }
}
```
_Note: This is pedantic implementation that takes into account the fact that a threads might not share the file descriptor with the process, see CLONE_FILES in [clone(2)](https://man7.org/linux/man-pages/man2/clone.2.html)_

**Example 2:** Iterate over all the IPv4 TCP sockets currently in listening state (in my current network namespace) and print their local port:
```
auto filter_listening = [](const net_socket& socket){
    return socket.socket_net_state == pfs::net_socket::net_state::listen;
}

for (auto& socket : pfs::procfs().get_net().get_tcp(filter_listening))
{
    std::cout << socket.local_port << std::endl;
}
```

**Example 3:** Find all the memory maps for task 1234 (This can be both a process or a thread) that start with an ELFs header
```
auto task = pfs::procfs().get_task(1234);
auto mem = task.get_mem();
for (auto& map : task.get_maps())
{
    if (!map.perm.can_read)
    {
        continue;
    }

    static const std::vector<uint8_t> ELF_HEADER = { 0x7F, 0x45, 0x4C, 0x46 };
    if (mem.read(map.start_address, ELF_HEADER.size()) == ELF_HEADER)
    {
        ... do something ...
    }
}
```
_(You can either create `pfs::procfs()` every time or once and keep it, the overhead is really small)_

