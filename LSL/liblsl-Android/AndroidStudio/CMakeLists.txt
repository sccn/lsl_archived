# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

file(GLOB_RECURSE SRC
	"../../liblsl/src/*.cpp"
	"../../liblsl/src/*.hpp"
	"../../liblsl/external/lslboost/filesystem/*.cpp"
	"../../liblsl/external/lslboost/filesystem/*.hpp"
	"../../liblsl/external/src/filesystem/*.cpp"
	"../../liblsl/external/src/filesystem/*.hpp"
	"../../liblsl/external/src/thread/src/pthread/once.cpp"
	"../../liblsl/external/src/thread/src/pthread/once_atomic.cpp"
	"../../liblsl/external/src/thread/src/pthread/thread.cpp"
	"../../liblsl/external/lslboost/thread/pthread/once_atomic.hpp"
	"../../liblsl/external/lslboost/thread/pthread/once.hpp"
	"../../liblsl/external/src/thread/src/tss_null.cpp"
	"../../liblsl/external/src/system/src/error_code.cpp"
	"../../liblsl/external/src/system/src/local_free_on_destruction.hpp"
	"../../liblsl/external/src/chrono/src/chrono.cpp"
	"../../liblsl/external/src/serialization/src/*.cpp"
)
add_library( # Sets the name of the library.
             lsl

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
			 ${SRC}
             )

target_include_directories(lsl PUBLIC ../../liblsl/external)

#use CMAKE_STATIC_LINKER_FLAGS for static library. Compiler flags are in build.gradle.
set(CMAKE_SHARED_LINKER_FLAGS "-z muldefs")

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       lsl

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
					   
