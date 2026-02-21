#!/bin/bash

set_version_part() {
    local file="$1"
    local part="$2"
    local version="$3"

    local define="#define PFS_VER_${part}"

    sed -i "" -E "s/^$define [[:digit:]]+/$define $version/g" "$file"
}

get_version_part() {
    local file="$1"
    local part="$2"

    grep "#define PFS_VER_${part}" "$file" | cut -d' ' -f3
}

tag_exists() {
    local tag_name="$1"

    git tag --list | grep -q "$tag_name"
}

tag_version() {
    local tag_name="$1"

    git tag "$tag_name" || return 1
    git push origin "$tag_name" || return 1

    return 0
}

tag() {
    local major="$1"
    local minor="$2"
    local patch="$3"

    echo "v${major}.${minor}.${patch}"
}

update_package_xml() {
    local file="$1"
    local major="$2"
    local minor="$3"
    local patch="$4"

    local old_version="<version>[0-9]*\.[0-9]*\.[0-9]*</version>"
    local new_version="<version>${major}.${minor}.${patch}</version>"

    sed -i "" "s|$old_version|$new_version|g" "$file"
}

update_version_file() {
    local file="$1"
    local major="$2"
    local minor="$3"
    local patch="$4"

    set_version_part "$file" "MAJOR" "$major" || return 1
    set_version_part "$file" "MINOR" "$minor" || return 1
    set_version_part "$file" "PATCH" "$patch" || return 1

    return 0
}

commit_bump() {
    local tag="$1"
    local files="${@:2}"

    git add $files || return 1 # Spreading $files (no double quotes) on purpose
    git commit -m "Automatic version bump to $tag" || return 1

    return 0
}

main() {
    local header_file="./include/pfs/version.hpp"
    local package_xml="./package.xml"

    local major="$(get_version_part "$header_file" "MAJOR")"
    local minor="$(get_version_part "$header_file" "MINOR")"
    local patch="$(get_version_part "$header_file" "PATCH")"

    case "$1" in
        major)
            ((major+=1))
            minor=0
            patch=0
            ;;
        minor)
            ((minor+=1))
            patch=0
            ;;
        patch)
            ((patch+=1))
            ;;
        *)
            echo "Usage: $0 major|minor|patch"
            return 2
            ;;
    esac

    local tag="$(tag $major $minor $patch)"
    read -p "Will create new tag: $tag, do you want to proceed? (y/n) " answer
    if [[ "$answer" != "y" ]]; then
        echo "Aborting..."
        return 1
    fi

    if ! git fetch; then
        echo "Git fetch failed"
        return 1
    fi

    if tag_exists "$tag"; then
        echo "Tag $tag already exists"
        return 1
    fi

    if ! update_version_file "$header_file" "$major" "$minor" "$patch"; then
        echo "Couldn't update $header_file, please check for leftovers"
        return 1
    fi

    if ! update_package_xml "$package_xml" "$major" "$minor" "$patch"; then
        echo "Couldn't update $package_xml, please check for leftovers"
        return 1
    fi

    if ! commit_bump "$tag" "$header_file" "$package_xml"; then
        echo "Couldn't automatically commit"
        return 1
    fi

    if ! tag_version "$tag"; then
        echo "Couldn't create tag"
        return 1
    fi

    return 0
}

main "$@"
