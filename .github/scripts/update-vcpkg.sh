#!/usr/bin/env bash
set -euo pipefail

version=${TAG#v}
history="$VCPKG_ROOT/versions/o-/obfuscxx.json"
registered=$(jq --arg version "$version" 'any(.versions[]; .version == $version)' "$history")

if [[ $registered == true ]]; then
    echo "obfuscxx $version is already in the vcpkg registry"
    echo "status=error" >> "$GITHUB_OUTPUT"
    exit 1
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

tag=$(jq -rn --arg tag "$TAG" '$tag | @uri')

curl --fail --location --output "$tmp/source.tar.gz" \
    "https://github.com/$GITHUB_REPOSITORY/archive/$tag.tar.gz"

hash=$(sha512sum "$tmp/source.tar.gz")
hash=${hash%% *}

portfile="$VCPKG_ROOT/ports/obfuscxx/portfile.cmake"
pattern='^([[:blank:]]*SHA512[[:blank:]]+)[[:xdigit:]]{128}([[:space:]]*)$'

sed -Ei "s/[[:space:]]+$//; s/$pattern/\1${hash}\2/" "$portfile"

manifest="$VCPKG_ROOT/ports/obfuscxx/vcpkg.json"

jq --arg version "$version" '.version = $version | del(."port-version")' \
    "$manifest" > "$tmp/vcpkg.json"
mv "$tmp/vcpkg.json" "$manifest"

echo "status=ok" >> "$GITHUB_OUTPUT"
