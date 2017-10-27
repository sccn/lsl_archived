@echo off
REM Create a build directory, launch the CMake GUI for the build configuration,
REM build everything configured and put redistributable binaries in the selected
REM "install" prefix

REM Don't close the window on errors
REM see https://stackoverflow.com/a/33929280
if not defined in_subprocess (cmd /k set in_subprocess=y ^& %0 %*) & exit )

cd build

echo After pressing Enter, the configuration GUI should appear
echo (if not, install CMake and make sure it's on the system path)
echo ---
echo In it, set the configuration options you need via the "Add Entry" button
echo ---
echo Then click "Configure" and select your compiler.
echo ---
echo Afterwards, you can also change configuration options, generate the
echo build scripts with the "Generate" button and optionally launch
echo Visual Studio with the "Open Project" button.
echo ---
echo By default, CMake will build release binaries afterwards and put them
echo in your CMAKE_INSTALL_PREFIX or build/install
pause
echo ---
cmake-gui ..

cmake --build . --config Release --target install
