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

#ifndef SAMPLE_MENU_HPP
#define SAMPLE_MENU_HPP

#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class command
{
public:
    using handler = std::function<int(std::vector<std::string>&& args)>;

public:
    command(const std::string& name, const std::string& args,
            const std::string& description, handler fp)
        : _name(name), _args(args), _description(description), _fp(fp)
    {}

    int operator()(std::vector<std::string>&& args)
    {
        return _fp(std::move(args));
    }

    const std::string& name() const { return _name; }
    const std::string& args() const { return _args; }
    const std::string& description() const { return _description; }

    bool operator==(const std::string& name) const { return _name == name; }

private:
    const std::string _name;
    const std::string _args;
    const std::string _description;
    handler _fp;
};

class menu
{
public:
    menu(const std::string& app, const std::string& version,
         const std::vector<command>& commands, int argc, char** argv)
        : _app(app), _version(version), _commands(commands),
          _args(to_args(argc, argv))
    {}

    int run(int bad_usage_retval = -EINVAL)
    {
        auto args = _args;

        auto self = pop_first_arg(args);
        if (args.empty())
        {
            return bad_usage_retval;
        }

        auto name    = pop_first_arg(args);
        auto command = std::find(_commands.begin(), _commands.end(), name);
        if (command == _commands.end())
        {
            return bad_usage_retval;
        }

        return (*command)(std::move(args));
    }

    std::string usage() const
    {
        std::ostringstream out;
        out << _app << ", version " << _version << "\n";
        out << "\n";
        out << "  Usage: " << _args[0] << " <command> [args]...\n";
        out << "\n";

        size_t longest_key = 0;
        for (const auto& command : _commands)
        {
            size_t key  = command.name().length() + 1 + command.args().length();
            longest_key = std::max(longest_key, key);
        }

        for (const auto& command : _commands)
        {
            auto key = command.name() + " " + command.args();

            // clang-format off
            out << "  " << std::left << std::setw(longest_key) << std::setfill(' ') << key
                << "  " << command.description() << "\n";
            // clang-format on
        }

        return out.str();
    }

private:
    static std::string pop_first_arg(std::vector<std::string>& args)
    {
        auto first = args.front();
        args.erase(args.begin());
        return first;
    }

    static std::vector<std::string> to_args(int argc, char** argv)
    {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.emplace_back(argv[i]);
        }
        return args;
    }

private:
    std::string _app;
    std::string _version;
    std::vector<command> _commands;
    std::function<void(const std::string&)> _logger;
    std::vector<std::string> _args;
};

#endif // SAMPLE_MENU_HPP
