#!/bin/bash
if [ -z "$GITHUB_WORKSPACE" ]; then
    export GITHUB_WORKSPACE="$PWD"
    export GITHUB_REPOSITORY="Fri3dCamp/badge_2024_arduino"
    export GITHUB_EVENT_NAME="release"
    export GITHUB_EVENT_PATH="$GITHUB_WORKSPACE/.github/test-release-published-event.json"
fi

export MODS_DIR="$GITHUB_WORKSPACE/arduino-ide-board-package"

# Clone espressif/arduino-esp32 repo tag 2.0.14 as submodule
UPSTREAM_VERSION=2.0.14
echo "###### Start Downloading arduino-esp32-$UPSTREAM_VERSION.zip from https://github.com/espressif/arduino-esp32/archive/refs/tags/$UPSTREAM_VERSION.tar.gz"
wget -q -O "arduino-esp32-$UPSTREAM_VERSION.zip" "https://github.com/espressif/arduino-esp32/archive/refs/tags/$UPSTREAM_VERSION.tar.gz"
echo "###### extracting arduino-esp32-$UPSTREAM_VERSION.zip"
tar -xzf arduino-esp32-$UPSTREAM_VERSION.zip
export UPSTREAM_DIR="$GITHUB_WORKSPACE/arduino-esp32-$UPSTREAM_VERSION"
echo "###### Done Downloading arduino-esp32-$UPSTREAM_VERSION.zip from https://github.com/espressif/arduino-esp32/archive/refs/tags/$UPSTREAM_VERSION.tar.gz"

export BASE_DIR="$UPSTREAM_DIR"



# Overwrite the files in BASE_DIR with files MODS_DIR
echo "###### Start Copying Fri3d arduino-esp32 mods"
rsync -a --progress "$MODS_DIR/" "$BASE_DIR/"
echo "###### Done Copying Fri3d arduino-esp32 mods"


if [[ ! $GITHUB_EVENT_NAME == "release" ]]; then
    echo "Wrong event '$GITHUB_EVENT_NAME'!"
    exit 1
fi

echo "###### Github event '$GITHUB_EVENT_NAME'!"

EVENT_JSON=`cat $GITHUB_EVENT_PATH`

action=`echo $EVENT_JSON | jq -r '.action'`
if [[ ! $action == "published" ]]; then
    echo "Wrong action '$action'. Exiting now..."
    exit 0
fi

draft=`echo $EVENT_JSON | jq -r '.release.draft'`
if [[ $draft == "true" ]]; then
    echo "It's a draft release. Exiting now..."
    exit 0
fi

RELEASE_PRE=`echo $EVENT_JSON | jq -r '.release.prerelease'`
RELEASE_TAG=`echo $EVENT_JSON | jq -r '.release.tag_name'`
RELEASE_BRANCH=`echo $EVENT_JSON | jq -r '.release.target_commitish'`
RELEASE_ID=`echo $EVENT_JSON | jq -r '.release.id'`
RELEASE_BODY=`echo $EVENT_JSON | jq -r '.release.body'`

OUTPUT_DIR="$BASE_DIR/build"
PACKAGE_NAME="fri3d-esp32-$RELEASE_TAG"
PACKAGE_JSON_MERGE=".github/scripts/merge_packages.py"
PACKAGE_JSON_TEMPLATE="$BASE_DIR/package/package_fri3d-esp32_index.template.json"
PACKAGE_JSON_DEV="package_fri3d-esp32_dev_index.json"
PACKAGE_JSON_REL="package_fri3d-esp32_index.json"

echo "Event: $GITHUB_EVENT_NAME, Repo: $GITHUB_REPOSITORY, Path: $BASE_DIR, Ref: $GITHUB_REF"
echo "Action: $action, Branch: $RELEASE_BRANCH, ID: $RELEASE_ID"
echo "Tag: $RELEASE_TAG, Draft: $draft, Pre-Release: $RELEASE_PRE"

# Try extracting something like a JSON with a "boards" array/element and "vendor" fields
BOARDS=`echo $RELEASE_BODY | grep -Pzo '(?s){.*}' | jq -r '.boards[]? // .boards? // empty' | xargs echo -n 2>/dev/null`
VENDOR=`echo $RELEASE_BODY | grep -Pzo '(?s){.*}' | jq -r '.vendor? // empty' | xargs echo -n 2>/dev/null`
if ! [ -z "${BOARDS}" ]; then echo "Releasing board(s): $BOARDS" ; fi
if ! [ -z "${VENDOR}" ]; then echo "Setting packager: $VENDOR" ; fi

function get_file_size(){
    local file="$1"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        eval `stat -s "$file"`
        local res="$?"
        echo "$st_size"
        return $res
    else
        stat --printf="%s" "$file"
        return $?
    fi
}

function git_upload_asset(){
    local name=$(basename "$1")
    # local mime=$(file -b --mime-type "$1")
    curl -k -X POST -sH "Authorization: token $GITHUB_TOKEN" -H "Content-Type: application/octet-stream" --data-binary @"$1" "https://uploads.github.com/repos/$GITHUB_REPOSITORY/releases/$RELEASE_ID/assets?name=$name"
}

function git_safe_upload_asset(){
    local file="$1"
    local name=$(basename "$file")
    local size=`get_file_size "$file"`
    local upload_res=`git_upload_asset "$file"`
    if [ $? -ne 0 ]; then
        >&2 echo "ERROR: Failed to upload '$name' ($?)"
        return 1
    fi
    up_size=`echo "$upload_res" | jq -r '.size'`
    if [ $up_size -ne $size ]; then
        >&2 echo "ERROR: Uploaded size does not match! $up_size != $size"
        #git_delete_asset
        return 1
    fi
    echo "$upload_res" | jq -r '.browser_download_url'
    return $?
}



function merge_package_json(){
    local jsonLink=$1
    local jsonOut=$2
    local old_json=$OUTPUT_DIR/oldJson.json
    local merged_json=$OUTPUT_DIR/mergedJson.json

    echo "Downloading previous JSON $jsonLink ..."
    curl -L -o "$old_json" "https://github.com/$GITHUB_REPOSITORY/releases/download/$jsonLink?access_token=$GITHUB_TOKEN" 2>/dev/null
    if [ $? -ne 0 ]; then echo "ERROR: Download Failed! $?"; exit 1; fi

    echo "Creating new JSON ..."
    set +e
    stdbuf -oL python "$PACKAGE_JSON_MERGE" "$jsonOut" "$old_json" > "$merged_json"
    set -e

    set -v
    if [ ! -s $merged_json ]; then
        rm -f "$merged_json"
        echo "Nothing to merge"
    else
        rm -f "$jsonOut"
        mv "$merged_json" "$jsonOut"
        echo "JSON data successfully merged"
    fi
    rm -f "$old_json"
    set +v
}

set -e

##
## PACKAGE ZIP
##

mkdir -p "$OUTPUT_DIR"
PKG_DIR="$OUTPUT_DIR/$PACKAGE_NAME"
PACKAGE_ZIP="$PACKAGE_NAME.zip"

mkdir -p "$PKG_DIR/tools"

# Copy all core files to the package folder
echo "Copying files for packaging ..."
if [ -z "${BOARDS}" ]; then
    # Copy all variants
    cp -f  "$BASE_DIR/boards.txt"                   "$PKG_DIR/"
    cp -Rf "$BASE_DIR/variants"                     "$PKG_DIR/"
else
    # Remove all entries not starting with any board code or "menu." from boards.txt
    cat "$BASE_DIR/boards.txt" | grep "^menu\."         >  "$PKG_DIR/boards.txt"
    for board in ${BOARDS} ; do
        cat "$BASE_DIR/boards.txt" | grep "^${board}\." >> "$PKG_DIR/boards.txt"
    done
    # Copy only relevant variant files
    mkdir "$PKG_DIR/variants/"
    for variant in `cat ${PKG_DIR}/boards.txt | grep "\.variant=" | cut -d= -f2` ; do
        cp -Rf "$BASE_DIR/variants/${variant}"      "$PKG_DIR/variants/"
    done
fi

rm  -rf $BASE_DIR/tools/sdk/esp32c3
rm  -rf $BASE_DIR/tools/sdk/esp32s2
cp -f  "$BASE_DIR/package.json"                     "$PKG_DIR/"
cp -f  "$BASE_DIR/programmers.txt"                  "$PKG_DIR/"
cp -Rf "$BASE_DIR/cores"                            "$PKG_DIR/"
cp -Rf "$BASE_DIR/libraries"                        "$PKG_DIR/"
cp -f  "$BASE_DIR/tools/espota.exe"                 "$PKG_DIR/tools/"
cp -f  "$BASE_DIR/tools/espota.py"                  "$PKG_DIR/tools/"
cp -f  "$BASE_DIR/tools/gen_esp32part.py"           "$PKG_DIR/tools/"
cp -f  "$BASE_DIR/tools/gen_esp32part.exe"          "$PKG_DIR/tools/"
cp -f  "$BASE_DIR/tools/gen_insights_package.py"    "$PKG_DIR/tools/"
cp -f  "$BASE_DIR/tools/gen_insights_package.exe"   "$PKG_DIR/tools/"
cp -Rf "$BASE_DIR/tools/partitions"                 "$PKG_DIR/tools/"
cp -Rf "$BASE_DIR/tools/ide-debug"                  "$PKG_DIR/tools/"
cp -Rf "$BASE_DIR/tools/sdk"                        "$PKG_DIR/tools/"
cp -f $BASE_DIR/tools/platformio-build*.py          "$PKG_DIR/tools/"


# Remove unnecessary files in the package folder
echo "Cleaning up folders ..."
find "$PKG_DIR" -name '*.DS_Store' -exec rm -f {} \;
find "$PKG_DIR" -name '*.git*' -type f -delete

# Replace tools locations in platform.txt
echo "Generating platform.txt..."
cat "$BASE_DIR/platform.txt" | \
sed "s/version=.*/version=$RELEASE_TAG/g" | \
sed 's/tools.xtensa-esp32-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-esp32-elf/tools.xtensa-esp32-elf-gcc.path=\{runtime.tools.xtensa-esp32-elf-gcc.path\}/g' | \
sed 's/tools.xtensa-esp32s2-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-esp32s2-elf/tools.xtensa-esp32s2-elf-gcc.path=\{runtime.tools.xtensa-esp32s2-elf-gcc.path\}/g' | \
sed 's/tools.xtensa-esp32s3-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-esp32s3-elf/tools.xtensa-esp32s3-elf-gcc.path=\{runtime.tools.xtensa-esp32s3-elf-gcc.path\}/g' | \
sed 's/tools.xtensa-esp-elf-gdb.path={runtime.platform.path}\/tools\/xtensa-esp-elf-gdb/tools.xtensa-esp-elf-gdb.path=\{runtime.tools.xtensa-esp-elf-gdb.path\}/g' | \
sed 's/tools.riscv32-esp-elf-gcc.path={runtime.platform.path}\/tools\/riscv32-esp-elf/tools.riscv32-esp-elf-gcc.path=\{runtime.tools.riscv32-esp-elf-gcc.path\}/g' | \
sed 's/tools.riscv32-esp-elf-gdb.path={runtime.platform.path}\/tools\/riscv32-esp-elf-gdb/tools.riscv32-esp-elf-gdb.path=\{runtime.tools.riscv32-esp-elf-gdb.path\}/g' | \
sed 's/tools.esptool_py.path={runtime.platform.path}\/tools\/esptool/tools.esptool_py.path=\{runtime.tools.esptool_py.path\}/g' | \
sed 's/debug.server.openocd.path={runtime.platform.path}\/tools\/openocd-esp32\/bin\/openocd/debug.server.openocd.path=\{runtime.tools.openocd-esp32.path\}\/bin\/openocd/g' | \
sed 's/debug.server.openocd.scripts_dir={runtime.platform.path}\/tools\/openocd-esp32\/share\/openocd\/scripts\//debug.server.openocd.scripts_dir=\{runtime.tools.openocd-esp32.path\}\/share\/openocd\/scripts\//g' | \
sed 's/debug.server.openocd.scripts_dir.windows={runtime.platform.path}\\tools\\openocd-esp32\\share\\openocd\\scripts\\/debug.server.openocd.scripts_dir.windows=\{runtime.tools.openocd-esp32.path\}\\share\\openocd\\scripts\\/g' \
 > "$PKG_DIR/platform.txt"

if ! [ -z ${VENDOR} ]; then
    # Append vendor name to platform.txt to create a separate section
    sed -i  "/^name=.*/s/$/ ($VENDOR)/" "$PKG_DIR/platform.txt"
fi

# Add header with version information
echo "Generating core_version.h ..."
ver_define=`echo $RELEASE_TAG | tr "[:lower:].\055" "[:upper:]_"`
ver_hex=`git -C "$BASE_DIR" rev-parse --short=8 HEAD 2>/dev/null`
echo \#define ARDUINO_ESP32_GIT_VER 0x$ver_hex > "$PKG_DIR/cores/esp32/core_version.h"
echo \#define ARDUINO_ESP32_GIT_DESC `git -C "$BASE_DIR" describe --tags 2>/dev/null` >> "$PKG_DIR/cores/esp32/core_version.h"
echo \#define ARDUINO_ESP32_RELEASE_$ver_define >> "$PKG_DIR/cores/esp32/core_version.h"
echo \#define ARDUINO_ESP32_RELEASE \"$ver_define\" >> "$PKG_DIR/cores/esp32/core_version.h"

# Compress package folder
echo "Creating ZIP ..."
pushd "$OUTPUT_DIR" >/dev/null
zip -qr "$PACKAGE_ZIP" "$PACKAGE_NAME"
if [ $? -ne 0 ]; then echo "ERROR: Failed to create $PACKAGE_ZIP ($?)"; exit 1; fi

# Calculate SHA-256
echo "Calculating SHA sum ..."
PACKAGE_PATH="$OUTPUT_DIR/$PACKAGE_ZIP"
PACKAGE_SHA=`shasum -a 256 "$PACKAGE_ZIP" | cut -f 1 -d ' '`
PACKAGE_SIZE=`get_file_size "$PACKAGE_ZIP"`
popd >/dev/null
rm -rf "$PKG_DIR"
echo "'$PACKAGE_ZIP' Created! Size: $PACKAGE_SIZE, SHA-256: $PACKAGE_SHA"
echo

# Upload package to release page
echo "Uploading package to release page ..."
PACKAGE_URL=`git_safe_upload_asset "$PACKAGE_PATH"`
echo "Package Uploaded"
echo "Download URL: $PACKAGE_URL"
echo

##
## PACKAGE JSON
##

# Construct JQ argument with package data
jq_arg=".packages[0].platforms[0].version = \"$RELEASE_TAG\" | \
    .packages[0].platforms[0].url = \"$PACKAGE_URL\" |\
    .packages[0].platforms[0].archiveFileName = \"$PACKAGE_ZIP\" |\
    .packages[0].platforms[0].size = \"$PACKAGE_SIZE\" |\
    .packages[0].platforms[0].checksum = \"SHA-256:$PACKAGE_SHA\""

# Generate package JSONs
echo "Genarating $PACKAGE_JSON_DEV ..."
cat "$PACKAGE_JSON_TEMPLATE" | jq "$jq_arg" > "$OUTPUT_DIR/$PACKAGE_JSON_DEV"
if [[ "$RELEASE_PRE" == "false" ]]; then
    echo "Genarating $PACKAGE_JSON_REL ..."
    cat "$PACKAGE_JSON_TEMPLATE" | jq "$jq_arg" > "$OUTPUT_DIR/$PACKAGE_JSON_REL"
fi

# Figure out the last release or pre-release
echo "Getting previous releases ..."
releasesJson=`curl -sH "Authorization: token $GITHUB_TOKEN" "https://api.github.com/repos/$GITHUB_REPOSITORY/releases" 2>/dev/null`
if [ $? -ne 0 ]; then echo "ERROR: Get Releases Failed! ($?)"; exit 1; fi

set +e
prev_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .prerelease == false)) | sort_by(.published_at | - fromdateiso8601) | .[0].tag_name")
prev_any_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false)) | sort_by(.published_at | - fromdateiso8601)  | .[0].tag_name")
prev_branch_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .prerelease == false and .target_commitish == \"$RELEASE_BRANCH\")) | sort_by(.published_at | - fromdateiso8601)  | .[0].tag_name")
prev_branch_any_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .target_commitish == \"$RELEASE_BRANCH\")) | sort_by(.published_at | - fromdateiso8601)  | .[0].tag_name")
shopt -s nocasematch
if [[ "$prev_release" == "$RELEASE_TAG" ]]; then
    prev_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .prerelease == false)) | sort_by(.published_at | - fromdateiso8601) | .[1].tag_name")
fi
if [[ "$prev_any_release" == "$RELEASE_TAG" ]]; then
    prev_any_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false)) | sort_by(.published_at | - fromdateiso8601)  | .[1].tag_name")
fi
if [[ "$prev_branch_release" == "$RELEASE_TAG" ]]; then
    prev_branch_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .prerelease == false and .target_commitish == \"$RELEASE_BRANCH\")) | sort_by(.published_at | - fromdateiso8601)  | .[1].tag_name")
fi
if [[ "$prev_branch_any_release" == "$RELEASE_TAG" ]]; then
    prev_branch_any_release=$(echo "$releasesJson" | jq -e -r ". | map(select(.draft == false and .target_commitish == \"$RELEASE_BRANCH\")) | sort_by(.published_at | - fromdateiso8601)  | .[1].tag_name")
fi
shopt -u nocasematch
set -e

echo "Previous Release: $prev_release"
echo "Previous (any)release: $prev_any_release"
echo

# Merge package JSONs with previous releases
if [ ! -z "$prev_any_release" ] && [[ "$prev_any_release" != "null" ]]; then
    echo "Merging with JSON from $prev_any_release ..."
    merge_package_json "$prev_any_release/$PACKAGE_JSON_DEV" "$OUTPUT_DIR/$PACKAGE_JSON_DEV"
fi

if [ "$RELEASE_PRE" == "false" ]; then
    if [ ! -z "$prev_release" ] && [[ "$prev_release" != "null" ]]; then
        echo "Merging with JSON from $prev_release ..."
        merge_package_json "$prev_release/$PACKAGE_JSON_REL" "$OUTPUT_DIR/$PACKAGE_JSON_REL"
    fi
fi

# Upload package JSONs
echo "Uploading $PACKAGE_JSON_DEV ..."
echo "Download URL: "`git_safe_upload_asset "$OUTPUT_DIR/$PACKAGE_JSON_DEV"`
echo
if [ "$RELEASE_PRE" == "false" ]; then
    echo "Uploading $PACKAGE_JSON_REL ..."
    echo "Download URL: "`git_safe_upload_asset "$OUTPUT_DIR/$PACKAGE_JSON_REL"`
    echo
fi

echo "DONE!"
