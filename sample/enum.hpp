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

#ifndef SAMPLE_ENUM_HPP
#define SAMPLE_ENUM_HPP

#include <string>
#include <vector>

int enum_system(std::vector<std::string>&& args);
int enum_net(std::vector<std::string>&& args);
int enum_tasks(std::vector<std::string>&& args);
int enum_fds(std::vector<std::string>&& args);
int enum_blocks(std::vector<std::string>&& args);

#endif // SAMPLE_ENUM_HPP
