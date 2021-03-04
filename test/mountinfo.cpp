#include <linux/kdev_t.h>

#include "catch.hpp"

#include "pfs/parsers.hpp"

using namespace pfs::impl::parsers;

TEST_CASE("Parse mountinfo", "[task][mountinfo]")
{
    pfs::mount expected;
    std::string line;

    SECTION("Valid 1")
    {
        line = "27 0 253:0 / / rw,relatime shared:1 - ext4 "
               "/dev/mapper/vagrant--vg-root rw,errors=remount-ro,data=ordered";

        expected.id              = 27;
        expected.parent_id       = 0;
        expected.device          = MKDEV(0x253, 0x00);
        expected.root            = "/";
        expected.point           = "/";
        expected.options         = {"rw", "relatime"};
        expected.optional        = {"shared:1"};
        expected.filesystem_type = "ext4";
        expected.source          = "/dev/mapper/vagrant--vg-root";
        expected.super_options   = {"rw", "errors=remount-ro", "data=ordered"};
    }

    SECTION("Valid 2")
    {
        line = "46 22 0:41 / /proc/sys/fs/binfmt_misc rw,relatime shared:25 - "
               "autofs systemd-1 "
               "rw,fd=25,pgrp=1,timeout=0,minproto=5,maxproto=5,direct,pipe_"
               "ino=13559";

        expected.id              = 46;
        expected.parent_id       = 22;
        expected.device          = MKDEV(0x00, 0x41);
        expected.root            = "/";
        expected.point           = "/proc/sys/fs/binfmt_misc";
        expected.options         = {"rw", "relatime"};
        expected.optional        = {"shared:25"};
        expected.filesystem_type = "autofs";
        expected.source          = "systemd-1";
        expected.super_options   = {"rw",        "fd=25",         "pgrp=1",
                                  "timeout=0", "minproto=5",    "maxproto=5",
                                  "direct",    "pipe_ino=13559"};
    }

    auto mount = parse_mountinfo_line(line);
    REQUIRE(mount.id == expected.id);
    REQUIRE(mount.parent_id == expected.parent_id);
    REQUIRE(mount.device == expected.device);
    REQUIRE(mount.root == expected.root);
    REQUIRE(mount.point == expected.point);
    REQUIRE(mount.options == expected.options);
    REQUIRE(mount.optional == expected.optional);
    REQUIRE(mount.filesystem_type == expected.filesystem_type);
    REQUIRE(mount.source == expected.source);
    REQUIRE(mount.super_options == expected.super_options);
}
