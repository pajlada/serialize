#!/bin/sh

set -e

echo "Build type: ${BUILD_TYPE}"

if [ "$BUILD_TYPE" = "conan" ]; then
    dnf install --assumeyes conan
elif [ "$BUILD_TYPE" = "system" ]; then
    dnf install --assumeyes rapidjson-devel
fi
