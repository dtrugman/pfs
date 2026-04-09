# Contributing to pfs

## License and Copyright

By submitting a contribution to this project (including pull requests, patches,
and any other material), you agree that:

1. Your contribution is your original work and you have the right to submit it.
2. Your contribution is submitted under the
   [Apache License 2.0](LICENSE) that governs this project, and you grant the
   project author a perpetual, worldwide, non-exclusive, no-charge,
   royalty-free, irrevocable license to use, reproduce, modify, distribute, and
   sublicense your contribution as part of this project.
3. The project author retains complete and sole copyright over the project.

This is consistent with the
[Developer Certificate of Origin (DCO) 1.1](https://developercertificate.org/)
used across many open-source projects.

## Adding a new parser

Every procfs/sysfs file maps to one of two parser patterns. Choose based on the
file's format, implement the parser, wire it in, and add a test.

---

### Pattern 1: `kv_file_parser<T>` — key-value files

Use this when the file has the form `Key: value` (one key per line, order
irrelevant). Examples: `/proc/<pid>/status`, `/proc/stat`.

**How it works:**

`kv_file_parser<T>` (in `include/pfs/parsers/kv_file_parser.hpp`) reads the file
line by line, splits each line on a delimiter, and dispatches the value to a
registered parser function keyed by the field name.

**Steps:**

1. **Add the output type** to `include/pfs/types.hpp` if it doesn't exist.

2. **Create the parser header** in `include/pfs/parsers/`:

    ```cpp
    // include/pfs/parsers/my_file.hpp
    #include "kv_file_parser.hpp"
    #include "pfs/types.hpp"

    namespace pfs { namespace impl { namespace parsers {

    class my_file_parser : public kv_file_parser<my_type>
    {
    public:
        my_file_parser() : kv_file_parser<my_type>(DELIM, PARSERS) {}
    private:
        static const char DELIM;
        static const value_parsers PARSERS;
    };

    }}}
    ```

3. **Create the parser implementation** in `src/parsers/`:

    ```cpp
    // src/parsers/my_file.cpp
    #include "pfs/parsers/my_file.hpp"
    #include "pfs/parsers/number.hpp"

    namespace pfs { namespace impl { namespace parsers {

    const char my_file_parser::DELIM = ':';

    const my_file_parser::value_parsers my_file_parser::PARSERS = {
        {"SomeNumber", [](const std::string& value, my_type& out) {
            to_number(value, out.some_number);
        }},
        {"SomeString", [](const std::string& value, my_type& out) {
            out.some_string = value;
        }},
    };

    }}}
    ```

4. **Wire it in** by adding a getter to the appropriate class (`task`, `net`, or
   `procfs`) that constructs the parser and calls `.parse(path)`.

5. **Add a unit test** in `test/`. Create a temp file with representative
   content and assert the parsed fields. See `test/test_task_status.cpp` for a
   full example.

---

### Pattern 2: `parse_file_lines` — tabular/repeated-row files

Use this when each line is an independent record (a row in a table, one entry
per line). Examples: `/proc/meminfo`, `/proc/net/tcp`, `/proc/<pid>/maps`.

**How it works:**

`parse_file_lines` (in `include/pfs/parsers/lines.hpp`) is a template function
that reads the file line by line, calls a per-line parser function for each, and
inserts the result via an STL inserter. It supports skipping header lines and
applying a post-parse filter.

**Steps:**

1. **Add the output type** to `include/pfs/types.hpp` if it doesn't exist.

2. **Create the parser header** in `include/pfs/parsers/`:

    ```cpp
    // include/pfs/parsers/my_file.hpp
    #include <string>
    #include "pfs/types.hpp"

    namespace pfs { namespace impl { namespace parsers {

    my_type parse_my_file_line(const std::string& line);

    }}}
    ```

3. **Create the parser implementation** in `src/parsers/`:

    ```cpp
    // src/parsers/my_file.cpp
    #include "pfs/parsers/my_file.hpp"
    #include "pfs/parsers/number.hpp"
    #include "pfs/parser_error.hpp"
    #include "pfs/utils.hpp"

    namespace pfs { namespace impl { namespace parsers {

    my_type parse_my_file_line(const std::string& line)
    {
        // split, parse fields, throw parser_error on bad input
        auto tokens = utils::split(line);
        if (tokens.size() < EXPECTED_FIELDS)
        {
            throw parser_error("Corrupted my_file line", line);
        }

        my_type out;
        to_number(tokens[0], out.some_field);
        return out;
    }

    }}}
    ```

4. **Wire it in** by adding a getter that calls `parse_file_lines` with your
   parser function and an appropriate inserter:

    ```cpp
    std::vector<my_type> task::get_my_file() const
    {
        static const std::string MY_FILE("my_file");
        static const size_t HEADER_LINES = 1; // skip if file has a header row
        auto path = _task_root + MY_FILE;

        std::vector<my_type> output;
        parsers::parse_file_lines(path, std::back_inserter(output),
                                  parsers::parse_my_file_line,
                                  /* filter = */ nullptr, HEADER_LINES);
        return output;
    }
    ```

5. **Add a unit test** in `test/`. Create a temp file with representative
   content (including a corrupted-input case) and assert the parsed fields.
   See `test/test_meminfo.cpp` or `test/test_maps.cpp` for examples.

---

### Error handling

- Use `to_number()` (from `include/pfs/parsers/number.hpp`) for all numeric
  conversions. It wraps `utils::stot` and re-throws as `parser_error`.
- Throw `parser_error` for malformed content (wrong field count, unexpected
  value format). Always pass the offending string as the second argument.
- Throw `std::system_error` for I/O failures (file not found, permission
  denied). The file-open paths in `kv_file_parser` and `parse_file_lines` already
  do this.
- Do not swallow errors or return defaults for missing required fields.
