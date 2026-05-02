#!/usr/bin/env bash

set -e

BUILD_TYPE=${1:-Debug}
ACTION=${2:-build}

BUILD_DIR="build/${BUILD_TYPE}"

case $ACTION in
    clean)
        echo "Cleaning ${BUILD_DIR}..."
        rm -rf "${BUILD_DIR}"
        exit 0
        ;;
    rebuild)
        echo "Rebuilding ${BUILD_DIR}..."
        rm -rf "${BUILD_DIR}"
        ;;
esac

echo "Configuring ${BUILD_TYPE} mode..."
cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "Building..."
cmake --build ${BUILD_DIR} -j $(nproc)

# Run Sandbox
if [ "$ACTION" == "run" ]; then
    echo "Running Sandbox..."
    (cd ${BUILD_DIR}/zkSandbox && ./zkSandbox)
fi
