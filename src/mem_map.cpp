/*
 *  Copyright 2020-present Daniel Trugman
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "pfs/parser_error.hpp"
#include "pfs/parsers/maps.hpp"
#include "pfs/parsers/mem_map.hpp"
#include "pfs/types.hpp"
#include "pfs/utils.hpp"

namespace pfs {
namespace impl {
namespace parsers {

namespace {

void parse_size(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.size);
}

void parse_kernel_page_size(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.kernel_page_size);
}

void parse_mmu_page_size(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.mmu_page_size);
}

void parse_rss(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.rss);
}

void parse_pss(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.pss);
}

void parse_pss_dirty(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.pss_dirty);
}

void parse_shared_clean(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.shared_clean);
}

void parse_shared_dirty(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.shared_dirty);
}

void parse_private_clean(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.private_clean);
}

void parse_private_dirty(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.private_dirty);
}

void parse_referenced(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.referenced);
}

void parse_anonymous(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.anonymous);
}

void parse_ksm(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.ksm);
}

void parse_lazy_free(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.lazy_free);
}

void parse_anon_huge_pages(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.anon_huge_pages);
}

void parse_shmem_pmd_mapped(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.shmem_pmd_mapped);
}

void parse_file_pmd_mapped(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.file_pmd_mapped);
}

void parse_shared_hugetlb(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.shared_hugetlb);
}

void parse_private_hugetlb(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.private_hugetlb);
}

void parse_swap(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.swap);
}

void parse_swap_pss(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.swap_pss);
}

void parse_locked(const std::string& value, mem_map& out)
{
    utils::parse_memory_size(value, out.locked);
}

void parse_thp_eligible(const std::string& value, mem_map& out)
{
    int thp_eligible = 0;
    utils::stot(value, thp_eligible);
    out.thp_eligible = thp_eligible != 0;
}

using value_parser =
    std::function<void(const std::string& value, mem_map& out)>;
using value_parsers = std::unordered_map<std::string, value_parser>;

const value_parsers parsers = {{"Size", parse_size},
                               {"KernelPageSize", parse_kernel_page_size},
                               {"MMUPageSize", parse_mmu_page_size},
                               {"Rss", parse_rss},
                               {"Pss", parse_pss},
                               {"Pss_Dirty", parse_pss_dirty},
                               {"Shared_Clean", parse_shared_clean},
                               {"Shared_Dirty", parse_shared_dirty},
                               {"Private_Clean", parse_private_clean},
                               {"Private_Dirty", parse_private_dirty},
                               {"Referenced", parse_referenced},
                               {"Anonymous", parse_anonymous},
                               {"KSM", parse_ksm},
                               {"LazyFree", parse_lazy_free},
                               {"AnonHugePages", parse_anon_huge_pages},
                               {"ShmemPmdMapped", parse_shmem_pmd_mapped},
                               {"FilePmdMapped", parse_file_pmd_mapped},
                               {"Shared_Hugetlb", parse_shared_hugetlb},
                               {"Private_Hugetlb", parse_private_hugetlb},
                               {"Swap", parse_swap},
                               {"SwapPss", parse_swap_pss},
                               {"Locked", parse_locked},
                               {"THPeligible", parse_thp_eligible}};

} // namespace

std::vector<mem_map> parse_mem_map(const std::string& path)
{
    std::ifstream in(path);
    if (!in)
    {
        throw parser_error("Couldn't open file", path);
    }

    std::vector<mem_map> mem_maps;
    std::optional<mem_map> current;
    std::string line;
    while (std::getline(in, line))
    {
        auto tokens = utils::split(line);
        if (tokens.size() >= 5 && tokens[0] != "VmFlags:")
        {
            // Header line
            if (current)
            {
                mem_maps.push_back(std::move(*current));
            }

            current.emplace();
            current->region = parse_maps_line(line);

            continue;
        }

        if (!current)
        {
            throw parser_error("Corrupted block - Missing header", line);
        }

        auto [key, value] = utils::split_once(line, ':');
        if (key.empty())
        {
            throw parser_error("Corrupted line - Missing key", line);
        }

        utils::rtrim(key);
        utils::ltrim(value);
        if (key == "VmFlags")
        {
            // TODO: Split this into tokens and parse them?
            current->vm_flags = value;
            continue;
        }

        auto iter = parsers.find(key);
        if (iter != parsers.end())
        {
            auto& parser = iter->second;
            parser(value, *current);
        }
    }

    if (current)
    {
        mem_maps.push_back(std::move(*current));
    }

    return mem_maps;
}

} // namespace parsers
} // namespace impl
} // namespace pfs
