# Try to find the labstreaminglayer library and enable
# the imported target LSL::lsl
#
# Use it with
# target_link_libraries(your_target_app PRIVATE LSL::lsl)

if(TARGET lsl)
	add_library(LSL::lsl ALIAS lsl)
	message(STATUS "Found target lsl in current build tree")
else()
	message(STATUS "Trying to find package LSL")
	list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR})
	# If lsl was built with the bundled boost, we have to include it
	find_package(lslboost QUIET)
	find_package(LSL REQUIRED)
endif()

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set runtime path, i.e. where shared libs are searched relative to the exe
if(APPLE)
	list(APPEND CMAKE_INSTALL_RPATH "@executable_path/../LSL/lib")
	list(APPEND CMAKE_INSTALL_RPATH "@executable_path/")
elseif(UNIX)
	list(APPEND CMAKE_INSTALL_RPATH "\$ORIGIN/../LSL/lib:\$ORIGIN")
endif()
# Windows: see .bat install code in installLSLApp()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# Qt5
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Because the ui_mainwindow.h file.
# Enable automatic compilation of .cpp->.moc, xy.ui->ui_xy.h and resource files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORC ON)

# Boost
#SET(Boost_DEBUG OFF) #Switch this and next to ON for help debugging Boost problems.
#SET(Boost_DETAILED_FAILURE_MSG ON)
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()

IF(MSVC)
	# Disable boost auto linking.
	add_definitions(-DBOOST_ALL_NO_LIB )
endif()


# Meta information we might need later when packaging?
set(META_AUTHOR_ORGANIZATION "David Medine -- SCCN")
set(META_AUTHOR_DOMAIN            "https://github.com/sccn/labstreaminglayer/")


# installs additional files (configuration etc.)
function(installLSLAuxFiles target)
	get_target_property(is_bundle ${target} MACOSX_BUNDLE)
	set(destdir ${PROJECT_NAME})
	if(is_bundle AND APPLE)
		set(destdir ${destdir}/${target}.app/Contents/MacOS)
	endif(is_bundle AND APPLE)
	install(FILES ${ARGN}
		DESTINATION ${destdir})
endfunction()


# adds the specified target to the install list
# when calling make install / ninja install the executable is installed to
# CMAKE_INSTALL_PREFIX/PROJECT_NAME/TARGET_NAME
# e.g. C:/LSL/BrainAmpSeries/BrainAmpSeries.exe
function(installLSLApp target)
	
	install(TARGETS ${target}
		BUNDLE DESTINATION ${PROJECT_NAME}
		RUNTIME DESTINATION ${PROJECT_NAME}
		LIBRARY DESTINATION ${PROJECT_NAME}/lib
	)
	set(appbin "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/${target}${CMAKE_EXECUTABLE_SUFFIX}")
	
	# Copy lsl library for WIN32 or MacOS.
	# On Mac, dylib is only needed for macdeployqt and for non bundles when not using system liblsl.
	# Copy anyway, and fixup_bundle can deal with the dylib already being present.
	if(WIN32 OR APPLE)
		installLSLAuxFiles(${target} $<TARGET_FILE:LSL::lsl>)
	endif()

	# do we need to install with Qt5?
	get_target_property(TARGET_LIBRARIES ${target} LINK_LIBRARIES)
	if(";${TARGET_LIBRARIES}" MATCHES ";Qt5::")
		# TODO: add the executable name to a list we feed to *deployqt later on?
		# https://gitlab.pluribusgames.com/games/apitrace/blob/master/gui/CMakeLists.txt#L121-174
		get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
		execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
		get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
		if(WIN32)
			find_program (QT_DEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
			if (QT_DEPLOYQT_EXECUTABLE)
				file (TO_NATIVE_PATH "${QT_BIN_DIR}" QT_BIN_DIR_NATIVE)
				# It's safer to use `\` separators in the Path, but we need to escape them
				string (REPLACE "\\" "\\\\" QT_BIN_DIR_NATIVE "${QT_BIN_DIR_NATIVE}")
				
				# windeployqt needs VCINSTALLDIR to copy MSVC Runtime files, but it's
				# usually not define with MSBuild builds.
				if ($ENV{VCINSTALLDIR})
					set (VCINSTALLDIR "$ENV{VCINSTALLDIR}")
				elseif (MSVC11)
					set (VCINSTALLDIR "$ENV{VS110COMNTOOLS}../../VC")
				elseif (MSVC12)
					set (VCINSTALLDIR "\$ENV{VS120COMNTOOLS}/../../VC")
				elseif (MSVC14)
					set (VCINSTALLDIR "\$ENV{VS140COMNTOOLS}/../../VC")
				else ()
					message (FATAL_ERROR "Unsupported MSVC version ${MSVC_VERSION}")
				endif ()
				file(TO_CMAKE_PATH ${VCINSTALLDIR} VCINSTALLDIR)
				if(QT_VERSION LESS 5.3.0)
					set(QT_DEPLOYQT_FLAGS --no-translations --no-system-d3d-compiler)
				else()
					set(QT_DEPLOYQT_FLAGS --no-translations --no-system-d3d-compiler --no-opengl-sw --no-compiler-runtime)
				endif()
				install (CODE "
					message (STATUS \"Running Qt Deploy Tool...\")
					if (CMAKE_INSTALL_CONFIG_NAME STREQUAL \"Debug\")
						list (APPEND QT_DEPLOYQT_FLAGS --debug)
					else ()
						list (APPEND QT_DEPLOYQT_FLAGS --release)
					endif ()
					execute_process(COMMAND
						\"${CMAKE_COMMAND}\" -E env
						\"Path=${QT_BIN_DIR_NATIVE};\$ENV{SystemRoot}\\\\System32;\$ENV{SystemRoot}\"
						\"VCINSTALLDIR=${VCINSTALLDIR}\"
						\"${QT_DEPLOYQT_EXECUTABLE}\"
						${QT_DEPLOYQT_FLAGS}
						\"${appbin}\"
					)
				")
			endif(QT_DEPLOYQT_EXECUTABLE)
		
		elseif(APPLE)
			# It should be enough to call fixup_bundle (see below),
			# but this fails to install qt plugins (cocoa).
			# Use macdeployqt instead (but this is bad at grabbing lsl dylib, so we did that above)
			find_program (QT_DEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT_BIN_DIR}")
			if(QT_DEPLOYQT_EXECUTABLE)
				set(QT_DEPLOYQT_FLAGS "-verbose=1")  # Adding -libpath=${CMAKE_INSTALL_PREFIX}/LSL/lib seems to do nothing, maybe deprecated
				install(CODE "
					message(STATUS \"Running Qt Deploy Tool...\")
					#list(APPEND QT_DEPLOYQT_FLAGS -dmg)
					if(CMAKE_INSTALL_CONFIG_NAME STREQUAL \"Debug\")
					    list(APPEND QT_DEPLOYQT_FLAGS -use-debug-libs)
					endif()
					execute_process(COMMAND
						\"${QT_DEPLOYQT_EXECUTABLE}\"
						\"${appbin}.app\"
						${QT_DEPLOYQT_FLAGS}
					)
				")
			endif(QT_DEPLOYQT_EXECUTABLE)
		endif(WIN32)
	elseif(APPLE)
		# fixup_bundle appears to be broken for Qt apps. Use only for non-Qt.
		get_target_property(target_is_bundle ${target} MACOSX_BUNDLE)
		if(target_is_bundle)
			install(CODE "
				include(BundleUtilities)
				set(BU_CHMOD_BUNDLE_ITEMS ON)
				fixup_bundle(
					${appbin}.app
					\"\"
					\"${CMAKE_INSTALL_PREFIX}/LSL/lib\"
				)
				"
				COMPONENT Runtime
			)
		endif(target_is_bundle)
	endif()
endfunction()
