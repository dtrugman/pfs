#!/bin/bash

set_version_part() {
    declare -r part="$1"
    declare -r version="$2"

    declare -r define="#define PFS_VER_${part}"

    sed -i "" "s/^$define [[:digit:]]/$define $version/g" "$file"
}

get_version_part() {
    declare -r part="$1"

    grep "#define PFS_VER_${part}" "$file" | cut -d' ' -f3
}

tag_exists() {
    declare -r tag_name="$1"

    git tag --list | grep -q "$tag_name"
}

tag_version() {
    declare -r tag_name="$1"

    git tag "$tag_name" || return 1
    git push origin "$tag_name" || return 1

    return 0
}

tag() {
    declare -r major="$1"
    declare -r minor="$2"
    declare -r patch="$3"

    echo "v${major}.${minor}.${patch}"
}

update_version_file() {
    declare -r major="$1"
    declare -r minor="$2"
    declare -r patch="$3"

    declare -r tag="$(tag "$major" "$minor" "$patch")"

    set_version_part "MAJOR" "$major" || return 1
    set_version_part "MINOR" "$minor" || return 1
    set_version_part "PATCH" "$patch" || return 1

    git add "$file" || return 1
    git commit -m "Automatic version bump to $tag" || return 1

    return 0
}

main() {
    declare -r file="./include/pfs/version.hpp"

    declare major="$(get_version_part "MAJOR")"
    declare minor="$(get_version_part "MINOR")"
    declare patch="$(get_version_part "PATCH")"

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

    declare -r tag="$(tag $major $minor $patch)"
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

    if ! update_version_file "$major" "$minor" "$patch"; then
        echo "Couldn't update version file, please check for leftovers"
        return 1
    fi

    if ! tag_version "$tag"; then
        echo "Couldn't create tag"
        return 1
    fi

    return 0
}

main "$@"
