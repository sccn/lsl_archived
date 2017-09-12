# Convenience script to build LSL+Apps and package it
# You can either run this script or check an appveyor build output for the
# executed commands.
# 
# Prerequisites (LSL with bundled Boost)
# - CMake 3.5
# - Python 2.7
# - 7zip
# - Windows:
#     - C++ compiler (tested: VS2015)
#     - Ninja (https://ninja-build.org/)
# - Linux
#    - C++ compiler (clang 3.5)
# - OS X
#     - C++ compiler (tested: ?)
# 
# Optional / required for apps:
# - Boost (+path set with -DBOOST_ROOT=path/to/boost)
# - Qt (+path set)
import os
import os.path
import shutil
import subprocess
import sys

presets = {
    'apps': [],
    'build-dir': 'build',
    'dont-build': False,
    'dont-configure': False,
    'dont-run-tests': False,
    'dont-package': False,
    'dry-run': False,
    'interactive': False,
    'clean-builddir': False,
    'generator': None,  # None: let CMake decide
    'cmake_conf': { 'CMAKE_BUILD_TYPE': 'Release'},
}
if os.name == 'nt':
    presets['cmake_conf'].update({'BOOST_ROOT': 'C:/Libraries/boost_1_63_0/',
                             })
elif os.name == 'posix':
    presets['cmake_conf'].update({'LSL_USE_SYSTEM_BOOST': 'On',
                             })

class LSLBuilder:
    def __init__(self, config):
        self.sourcepath = os.path.dirname(os.path.realpath(__file__))
        self.cmake_conf = config['cmake_conf']
        self.cmake_exe = os.environ.get('CMAKE_EXE') or 'cmake' 
        bd = config['build-dir']
        if bd.startswith('/'):
            self.builddir = bd
        else:
            self.builddir = os.path.join(self.sourcepath, bd, 'lsl_' + self.cmake_conf['CMAKE_BUILD_TYPE'], '')
        self.apps = config['apps']
        self.interactive = conf['interactive']
        self.dry_run = config['dry-run']
        self.clean_builddir = config['clean-builddir']
        self.install_prefix = self.builddir + 'lslinstall'
        self.generator = config['generator']
        print('Found source in ' + self.sourcepath)
        print('building in ' + self.builddir)
        self.call_cmake(otherargs=['--version'])

    def mkbuilddir(self, name, remove_if_existing=True):
        dirname = self.builddir + name
        exists = os.path.isdir(dirname)
        if exists and not remove_if_existing:
            return os.path.abspath(dirname)
        if exists:
            print('>>> rm -rf ' + dirname)
            if not self.dry_run:
                shutil.rmtree(dirname)
        if not self.dry_run:
            os.makedirs(dirname)
            print('>>> mkdir -p ' + dirname)
        return os.path.abspath(dirname)

    def call(self, args):
        print('>>> ' + ' '.join(args))
        if not self.dry_run:
            subprocess.check_call(args)

    def chdir(self, newdir):
        print('>>> cd ' + newdir)
        if not self.dry_run:
            os.chdir(newdir)

    def call_cmake(self, generator=None, conf={}, otherargs=[]):
        args = [self.cmake_exe]
        if generator is not None:
            args += ['-G', generator]
        for k, v in conf.items():
            args.append('-D' + k + '=' + v)
        args += otherargs
        self.call(args)

    def configure(self):
        cmakebool = ['Off', 'On']
        self.mkbuilddir('', self.clean_builddir)
        self.chdir(self.builddir)
        self.cmake_conf['CMAKE_INSTALL_PREFIX'] = self.mkbuilddir('lslinstall', self.clean_builddir)
        if self.interactive:
            self.call(['cmake-gui', self.sourcepath])
            return
        #assert (self.install_prefix == self.cmake_conf['CMAKE_INSTALL_PREFIX'])
        for app in self.apps:
            print('Adding ' + app + ' to the to do list')
            self.cmake_conf['LSLAPPS_' + app] = cmakebool[True]

        self.call_cmake(self.generator, self.cmake_conf, [self.sourcepath])  # generate CMake build files

    def build(self):
        if not os.path.exists(os.path.join(self.builddir, 'CMakeCache.txt')) and not self.dry_run:
            raise Exception('No configured build directory found!')
        self.call_cmake(otherargs=['--build', '.', '--target', 'install'])  # build everything and install it

    def run_tests(self):
        if not os.path.exists(os.path.join(self.install_prefix, 'LSL', 'cmake')) and not self.dry_run:
            raise Exception('LSL installation not found in ' + self.install_prefix)
        self.chdir(self.mkbuilddir('lsloot'))
        testconf = {'LSL_ROOT': self.install_prefix + '/LSL/',
            'CMAKE_BUILD_TYPE': self.cmake_conf['CMAKE_BUILD_TYPE']}
        self.call_cmake(conf['generator'], testconf, [self.sourcepath + '/OutOfTreeTest'])
        self.call_cmake(otherargs=['--build', '.', '--target', 'runOutOfTree'])

    def package(self):
        self.call_cmake(otherargs=['--build', self.builddir, '--target', 'LSLPACKAGE'])

def print_conf(conf):
    print('Configuration: ')
    for key, value in conf.items():
        print('\t ' + key.ljust(15) + str(value))
    print('\n')

def print_usage(conf):
    print_conf(conf)
    print(' --generator      set the CMake generator (see cmake -G)')
    print(' --build-dir      build prefix relative to the source or absolute')
    print(' --dry-run        just print the commands, don\'t execute them')
    print(' --interactive    launch the cmake gui to configure the build manually')
    print(' --clean-builddir remove an existing build directory')
    print(' --help           print this message')
    print(' -Dvar=value      set the CMake variable \'var\' to \'value\'')
    print(' --with-App       also build \'App\'')
    print(' --without-App    don\'t build \'App\'')
    print(' --dont-configure don\'t run the configuration step')
    print(' --dont-build     don\'t run the build step')
    print(' --dont-package   don\'t create a distributable archive')
    print(' --dont-run-tests don\'t run tests to see if most things work')
    print('\n')

if __name__ == '__main__':
    conf = presets
    for arg in sys.argv[1:]:
        if arg == '-h' or arg == '--help':
            print_usage(conf)
            sys.exit(0)
        if arg.startswith('-D'):
            [key, val] = arg[2:].split('=')
            conf['cmake_conf'][key] = val
        elif arg.startswith('--with-'):
            conf['apps'].append(arg[7:])
        elif arg.startswith('--without-'):
            conf['apps'].remove(arg[10:])
        elif arg.startswith('--'):
            parts = arg[2:].split('=')
            if len(parts) == 1:
                key = parts[0]
                value = True
            else:
                [key, value] = parts
            if key not in conf:
               print_usage(conf)
               raise Exception('Unknown option \'' + key + '\'')
            conf[key] = value
        else:
            print_usage(conf)
            raise Exception('Invalid command line arg')
    print_conf(conf)
    builder = LSLBuilder(conf)
    if not conf['dont-configure']:
        builder.configure()
    if not conf['dont-build']:
        builder.build()
    if not conf['dont-run-tests']:
        builder.run_tests()
    if not conf['dont-package']:
        builder.package()
