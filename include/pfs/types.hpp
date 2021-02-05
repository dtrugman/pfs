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

#ifndef PFS_TYPES_HPP
#define PFS_TYPES_HPP

#include <array>
#include <string>
#include <vector>

namespace pfs {

struct module
{
    enum class state
    {
        live      = 0,
        loading   = 1,
        unloading = 2,
        last
    };

    std::string name;
    size_t size;
    size_t instances;
    std::vector<std::string> dependencies;
    state current_state;
    size_t offset;
    bool is_out_of_tree;
    bool is_unsigned;

    bool operator<(const module& rhs) const { return name < rhs.name; }
};

struct load_average
{
    double last_1min;
    double last_5min;
    double last_15min;
    int runnable_tasks;
    int total_tasks;
    int last_created_task;
};

struct mount
{
    unsigned id;
    unsigned parent_id;
    dev_t device;
    std::string root;
    std::string point;
    std::vector<std::string> options;
    std::vector<std::string> optional;
    std::string filesystem_type;
    std::string source;
    std::vector<std::string> super_options;

    bool operator<(const mount& rhs) const { return id < rhs.id; }
};

struct zone
{
    size_t node_id;
    std::string name;

    static constexpr size_t CHUNKS_COUNT = 11;
    std::array<size_t, CHUNKS_COUNT> chunks;

    bool operator<(const zone& rhs) const
    {
        return (node_id < rhs.node_id) || (name < rhs.name);
    };
};

} // namespace pfs

#endif // PFS_TYPES_HPP
