#include <iostream>

#include "pfs/procfs.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    }
    auto file = std::string(argv[1]);

    auto pfs = pfs::procfs();
    for (const auto& process : pfs.get_processes())
    {
        for (const auto& thread : process.get_tasks())
        {
            for (const auto& fd : thread.get_fds())
            {
                if (fd.second.get_target() == file)
                {
                    std::cout <<  "tid[" << thread.id()
                              << "] fd[" << fd.second.num()
                              << "]" << std::endl;
                }
            }
        }
    }

    return 0;
}
