import os
import platform
import struct
from ctypes import util
import re
from cffi import FFI

def get_lsl_libpath():
    # find the library
    os_name = platform.system()
    bitness = 8 * struct.calcsize("P")
    libname = 'lsl' + str(bitness)
    if os_name in ['Windows', 'Microsoft']:
        libprefix = ''
        libname = 'lib' + libname
        libsuffix = '.dll'
    elif os_name == 'Darwin':
        libprefix = 'lib'
        libsuffix = '.dylib'
    elif os_name == 'Linux':
        libprefix = 'lib'
        libsuffix = '.so'
    else:
        raise RuntimeError("unrecognized operating system:", os_name)
    libpath = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', 'pylsl', libprefix+libname+libsuffix))
    if not os.path.isfile(libpath):
        libpath = util.find_library(libprefix+libname+libsuffix)
    if not libpath:
        raise RuntimeError("library " + libprefix + libname + libsuffix + " was not found - make sure "
                           "that it is on the search path (e.g., in the same "
                           "folder as pylsl.py).")
    return libpath

def get_header_path():
    # Find and read in the include file
    return os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'liblsl', 'include'))

def get_cleaned_header():
    with open(os.path.join(get_header_path(), 'lsl_c.h'), 'r') as myfile:
        header_dat=myfile.read()

    # Parse the include file
    # Cut the #defines and #ifdefs from the beginning and end
    cut_ix = [ss.start() for ss in re.finditer(r"//cut_before", header_dat)][0]
    header_dat = header_dat[cut_ix+13:]
    cut_ix = [ss.start() for ss in re.finditer(r"\n//cut_after", header_dat)][0]
    header_dat = header_dat[:cut_ix]
    # Replace unused define with empty string
    header_dat = re.sub(r"extern LIBLSL_C_API", r"", header_dat)
    # Replace bitwise or operation with its result
    header_dat = re.sub(r"1\|2\|4\|8", r"15", header_dat)
    return header_dat

libpath = get_lsl_libpath()
lib_dir = os.path.dirname(libpath)
lib_name = os.path.splitext(os.path.split(libpath)[1])[0]
if platform.system() not in ['Windows', 'Microsoft']:
    lib_name = lib_name[3:]

    if lib_dir == os.getcwd():
        lib_dir = './'

ffi = FFI()
my_source = """
    #include "lsl_c.h"
    """
# Note, if we set my_source = None, then no compiler is needed and we can still use cffi's ABI out-of-line method.
ffi.cdef(get_cleaned_header())
ffi.set_source("pylsl._pylsl",
               my_source,
               include_dirs=[get_header_path()],
               libraries=[lib_name], library_dirs=[lib_dir])#,
               # runtime_library_dirs=['./'],
               # extra_link_args=['-Wl,-rpath,'+os.path.split(libpath)[1]])

if __name__ == "__main__":
    # if platform.system() == "Darwin":
    #     from subprocess import call
    #     call(['install_name_tool', '-change', '@rpath/' + os.path.split(libpath)[1],
    #           '@loader_path/' + os.path.split(libpath)[1], '_pylsl.cpython-35m-darwin.so'])
    ffi.compile(verbose=True)