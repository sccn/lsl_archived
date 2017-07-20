# Convenience script to build LSL+Apps and package it
# You can either run this script or check an appveyor build output for the
# executed commands.

import os
import os.path
import shutil
import subprocess
import sys

presets = {
	'posix': {
		'buildprefix': '/tmp/lsl',
		'generator': 'Unix Makefiles',
		'Apps': ['Examples', 'LabRecorder', 'XDFBrowser'],
		'exeext': '',
		},
	'nt': {
		'buildprefix': os.path.dirname(os.path.realpath(__file__))+'/build/lsl',
		'generator': 'Ninja',
		'Apps': ['Examples', 'LabRecorder', 'XDFBrowser'],
		'exeext': '.exe',
		'cmake_conf': {
			'BOOST_ROOT': 'C:/Libraries/boost_1_63_0/',
		},
	},
	'all': {
		'system_boost': False,
		'lsl_static': False,
		'variant': 'Release',
		'lsl_sourcepath': os.path.dirname(os.path.realpath(__file__)),
		'cmake_conf': {},
	},
}

def mkbuilddir(conf, name):
	dirname = conf['buildprefix'] + ['32','64'][sys.maxsize>2**32] + '_' + variant + '/' + name
	if os.path.isdir(dirname):
		shutil.rmtree(dirname)
	os.makedirs(dirname)
	print('mkdir -p '+dirname)
	return os.path.abspath(dirname)

def call(args):
	print('>>> ' + ' '.join(args))
	subprocess.check_call(args)

def chdir(newdir):
	print('>>> cd ' + newdir)
	os.chdir(newdir)

def lslbuild(conf):
	cmakebool = ['Off', 'On']
	lsl_sourcedir = os.path.dirname(os.path.realpath(__file__))
	print('Found source in ' + lsl_sourcedir)
	lsl_root = mkbuilddir(conf, 'lsl')
	print('building in ' + lsl_root)
	chdir(lsl_root)

	cmake_conf = conf['cmake_conf']
	cmake_conf['CMAKE_BUILD_TYPE'] = conf['variant']
	conf['install_prefix'] = cmake_conf['CMAKE_INSTALL_PREFIX'] = mkbuilddir(conf, 'lslinstall')
	cmake_conf['LSL_USE_SYSTEM_BOOST'] = cmakebool[conf['system_boost']]
	cmake_conf['LSL_BUILD_STATIC'] = cmakebool[conf['lsl_static']]
	for app in conf['Apps']:
		cmake_conf['LSLAPPS_'+app] = cmakebool[True]
	
	cmake_exe = 'cmake'+conf['exeext']

	args = [cmake_exe, '-G', conf['generator']] + ['-D'+key+'='+value for key, value in cmake_conf.items()] + [conf['lsl_sourcepath']] + sys.argv[1:]
	call(args) # generate CMake build files
	call([cmake_exe, '--build', '.', '--target', 'install']) # build everything and install it
	chdir(mkbuilddir(conf, 'lsloot'))
	call([cmake_exe, '-G', conf['generator'], '-DLSL_ROOT='+cmake_conf['CMAKE_INSTALL_PREFIX']+'/LSL/', lsl_sourcedir+'/OutOfTreeTest'] + sys.argv[1:])
	call([cmake_exe, '--build', '.'])
	return conf

def lslpackage(dir, conf):
	_7z_exe = '7z' + conf['exeext']
	chdir(conf['install_prefix'])
	for dirname in os.listdir('.'):
		if os.path.isdir(dirname):
			call([_7z_exe, 'a', dirname+'.7z',dirname])

conf = presets['all']
conf.update(presets[os.name])
for variant in ['Release']:
	conf['variant'] = variant
	conf = lslbuild(conf)
	#lslpackage('build/lsl64_'+variant, conf)

