#!/bin/bash

set -e
cd build

echo "After pressing Enter, the configuration GUI should appear"
echo "(if not, install CMake and make sure it's on the system path)"
echo ""
echo "In it, set the configuration options you need via the "Add Entry" button"
echo ""
echo "Then click "Configure" and select your compiler."
echo ""
echo "Afterwards, you can also change configuration options and generate the"
echo "build scripts with the "Generate" button"
echo ""
echo "By default, CMake will build release binaries afterwards and put them"
echo "in your CMAKE_INSTALL_PREFIX or build/install"
read
echo ""

cmake-gui ..
cmake --build . --config Release --target install
