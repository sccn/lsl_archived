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
	list(APPEND CMAKE_INSTALL_RPATH "@executable_path/../LSL/lib:@executable_path")
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

	#install(CODE "
	#	include(BundleUtilities)
	#	set(BU_CHMOD_BUNDLE_ITEMS ON)
	#			fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/${target}${CMAKE_EXECUTABLE_SUFFIX}\"
	#			\"\"
	#			\"${CMAKE_INSTALL_PREFIX}/LSL/lib\")" )

	# libraries
	if(WIN32)
		# copy the liblsl dll to the target directory
		install(FILES $<TARGET_FILE:LSL::lsl>
			DESTINATION ${PROJECT_NAME})
	elseif(APPLE)
		# #It should be enough to call fixup_bundle, but this mangles Qt somehow causing crashes on startup.
		# #TODO: Either don't fixup Qt and rely on system Qt, or fix crash.
		#install(CODE "
		#	include(BundleUtilities)
		#	set(BU_CHMOD_BUNDLE_ITEMS ON)
		#	fixup_bundle(${CMAKE_INSTALL_PREFIX}/${target}.app \"\" \"${LSL_ROOT_DIR}/bin\")
		#" COMPONENT Runtime)
		# # macdeployqt somehow also mangles Qt.
		#add_custom_command(TARGET ${target} POST_BUILD
		#	COMMAND macdeployqt
		#	ARGS ${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}/${target}.app
		#) # add -dmg for an image
	endif()

	# do we need to install with Qt5?
	get_target_property(TARGET_LIBRARIES ${target} LINK_LIBRARIES)
	if(";${TARGET_LIBRARIES}" MATCHES ";Qt5::")
		install(CODE "message(STATUS \"Please run windeployqt / macdeployqt manually until we figure out how to make CMake do it\")")
		# TODO: add the executable name to a list we feed to *deployqt later on?

		#set(libdir "${CMAKE_INSTALL_PREFIX}/LSL/lib")
		#set(appbin "${CMAKE_INSTALL_PREFIX}/${target}/${target}${CMAKE_EXECUTABLE_SUFFIX}")
		#install(CODE "(TARGET foo POST_BUILD
		#	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
		#	COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
		#	COMMAND Qt5::windeployqt "--libdir ${libdir}" ${appbin}
		# )
	endif()
endfunction()

