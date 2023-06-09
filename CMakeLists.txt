#
# Copyright (C) 2016 necropotame (necropotame@gmail.com)
#
# This file is part of TeeUniverse.
#
# TeeUniverse is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# TeeUniverse is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 2.8.10)
cmake_policy(SET CMP0012 NEW)

project(TeeSolar)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/modules)

########################################################################
# DEPENDENCIES
########################################################################

find_package(SDL2)
find_package(OpenGL)
find_package(Freetype)
if(NOT(WITHOUT_HARFBUZZ))
	find_package(HarfBuzz)
endif()
find_package(ICU COMPONENTS i18n)

########################################################################
# BUILD_TYPE, PLATFORM
########################################################################

if(CMAKE_CONFIGURATION_TYPES) # multiconfig generator
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
else()
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
	endif()
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
	string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()

if(NOT(PLATFORM))
	if(WIN32)
		set(PLATFORM win)
	else()
		add_definitions(-DCONF_FAMILY_UNIX)
		if(APPLE)
			set(PLATFORM osx)
		else()
			set(PLATFORM linux)
		endif()
	endif()
endif()

#Check C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++11 COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG(-std=c++0x COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CXX11_FOUND TRUE)
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  set(CXX11_FOUND TRUE)
endif()

########################################################################
# OPTIONS
########################################################################

if(BUILD_TYPE)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${BUILD_TYPE}/${PLATFORM})
else()
	foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
		string(TOLOWER ${CONFIG_TYPE} CONFIG_TYPE_DIR)
		string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM}/lib)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM}/lib)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR}/build/${CONFIG_TYPE_DIR}/${PLATFORM})
	endforeach(CONFIG_TYPE)
endif()

if(CMAKE_VERSION VERSION_LESS 3)
	foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
		string(TOLOWER ${CONFIG_TYPE} CONFIG_TYPE_DIR)
		set(CONFIG_DIR ${CONFIG_DIR}$<$<CONFIG:${CONFIG_TYPE}>:${CONFIG_TYPE_DIR}>)
	endforeach(CONFIG_TYPE)
	set(DATA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${CONFIG_DIR}/${PLATFORM}/data)
else()
	set(DATA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/$<LOWER_CASE:$<CONFIG>>/${PLATFORM}/data)
endif()

set(TU_EXECNAME_GAME teesolar)
set(TU_EXECNAME_EDITOR teesolar_editor)
set(TU_EXECNAME_SERVER teesolar_srv)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/external")
include_directories("${PROJECT_SOURCE_DIR}/src/external/zlib")
include_directories("${PROJECT_SOURCE_DIR}/src/external/minizip")

if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR})
endif()
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
endif()
if(FREETYPE_FOUND)
	include_directories(${FREETYPE_INCLUDE_DIRS})
endif()
if(HARFBUZZ_FOUND)
	add_definitions(-DHARFBUZZ_ENABLED)
	include_directories(${HARFBUZZ_INCLUDE_DIRS})
endif()
if(ICU_FOUND)
	add_definitions(-DU_USING_ICU_NAMESPACE)
	include_directories(${ICU_INCLUDE_DIRS})
endif()

message(STATUS "******** TeeSolar ********")
if(BUILD_TYPE)
message(STATUS "Build type: ${BUILD_TYPE}")
else()
message(STATUS "Build configuration: ${CMAKE_CONFIGURATION_TYPES}")
endif()
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")

if(CXX11_FOUND)
	message(STATUS " * C++11 support found.")
else()
	message(STATUS " * C++11 support not found. Please use a different C++ compiler.")
endif()

message(STATUS "Install directory: ${CMAKE_INSTALL_PREFIX}")

message(STATUS ${name} "Dependencies:")
function(show_dependency_status name varFound error)
	if(${varFound})
		message(STATUS " * " ${name} " found")
	else()
		message(STATUS " * " ${name} " not found (" ${error} ")")
	endif()
endfunction()

show_dependency_status("ICU" "${ICU_FOUND}" "required by server, client, editor, tools")
show_dependency_status("SDL" "${SDL2_FOUND}" "required by: client, editor")
show_dependency_status("OpenGL" "${OPENGL_FOUND}" "required by: client, editor")
show_dependency_status("FreeType" "${FREETYPE_FOUND}" "required by: client, editor")
show_dependency_status("Harfbuzz" "${HARFBUZZ_FOUND}" "used by: client, editor")

if(NOT(ICU_FOUND))
	message(FATAL_ERROR "You must install ICU to compile TeeSolar")
endif()

if(NOT(HARFBUZZ_FOUND) AND NOT(WITHOUT_HARFBUZZ))
	message(STATUS " ! Warning: Without Harfbuzz, the text rendering")
	message(STATUS "            quality is reduced. In particular,")
	message(STATUS "            arabic and farsi can't be displayed")
	message(STATUS "            correctly.")
endif()

message(STATUS ${name} "Target status:")

########################################################################
# DEFINES
########################################################################

if(WIN32)
	add_definitions(-DCONF_FAMILY_WINDOWS)
else()
	add_definitions(-DCONF_FAMILY_UNIX)
	if(APPLE)
		add_definitions(-DCONF_PLATFORM_MACOSX)
	endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-Wall -Wextra -Wno-unused-parameter -Wno-maybe-uninitialized")
endif()

if(MINGW)
	SET(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} --static -static-libgcc")
	SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} --static -static-libgcc -static-libstdc++")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc /GS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4267 /wd4800 /wd4996")
endif()

########################################################################
# SHARED
########################################################################

# Sources
file(GLOB teesolar_shared_src
    "${PROJECT_SOURCE_DIR}/src/external/zlib/*.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/ioapi.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/miniunz.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/minizip.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/zip.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/unzip.c"
    "${PROJECT_SOURCE_DIR}/src/external/minizip/mztools.c"
    "${PROJECT_SOURCE_DIR}/src/external/json-parser/*.c"
    "${PROJECT_SOURCE_DIR}/src/external/ddnet/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/system/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/tl/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/assets/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/components/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/math/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/geometry/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/shared/imageprocessing/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/generated/assets/*.cpp"
)
if(WIN32)
	set(teesolar_shared_src ${teesolar_shared_src} "${PROJECT_SOURCE_DIR}/src/external/minizip/iowin32.c")
endif()

# Build
add_library(teesolar_shared ${teesolar_shared_src})
target_link_libraries (teesolar_shared
	${ICU_LIBRARIES}
)
add_custom_command(TARGET teesolar_shared POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${DATA_OUTPUT_DIRECTORY})
install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/languages" DESTINATION "share/teesolar")
install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/packages" DESTINATION "share/teesolar")

########################################################################
# CLIENT
########################################################################

if(SDL2_FOUND AND OPENGL_FOUND AND FREETYPE_FOUND)

	# Sources
	file(GLOB teesolar_client_src
		"${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
		"${PROJECT_SOURCE_DIR}/src/client/gui/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/client/components/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/client/*.cpp"
	)

	# Libraries
	set(teesolar_client_libraries
		teesolar_shared
		${SDL2_LIBRARY}
		${OPENGL_LIBRARIES}
		${FREETYPE_LIBRARIES}
	)
	
	if(HARFBUZZ_FOUND)
		set(teesolar_client_libraries ${teesolar_client_libraries} ${HARFBUZZ_LIBRARIES})
	endif()

	# Build
	add_library(teesolar_client ${teesolar_client_src})
	target_link_libraries (teesolar_client ${teesolar_client_libraries})
	install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/countryflags" DESTINATION "share/teesolar")
	install(DIRECTORY "${DATA_OUTPUT_DIRECTORY}/fonts" DESTINATION "share/teesolar")
	if(${PLATFORM} STREQUAL "linux")
		install(FILES "datasrc/share/pixmaps/teesolar.png" DESTINATION "share/pixmaps")
	endif()

	########################################################################
	# EDITOR
	########################################################################

	# Sources
	file(GLOB teesolar_editor_src
		"${PROJECT_SOURCE_DIR}/src/editor/gui/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/editor/components/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/editor/*.cpp"
	)

	# Libraries
	set(teesolar_editor_libraries
		teesolar_client
	)

	# Build Editor
	add_executable(${TU_EXECNAME_EDITOR} "${PROJECT_SOURCE_DIR}/src/editor.cpp" ${teesolar_editor_src})
	target_link_libraries (${TU_EXECNAME_EDITOR} ${teesolar_editor_libraries})
	install(TARGETS ${TU_EXECNAME_EDITOR} DESTINATION bin)
	if(${PLATFORM} STREQUAL "linux")
		install(FILES "datasrc/share/mime/packages/teesolar_editor.xml" DESTINATION "share/mime/packages")
		install(FILES "datasrc/share/applications/teesolar_editor.desktop" DESTINATION "share/applications" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	endif()
	message(STATUS " * editor ready for compilation")
	
else()
	#message(STATUS " * client can't be compiled")
	message(STATUS " * editor can't be compiled")
endif()

########################################################################
# SERVER
########################################################################

# Sources
#file(GLOB teesolar_server_src
#    "${PROJECT_SOURCE_DIR}/src/server/*.cpp"
#)

# Libraries
#set(teesolar_server_libraries
#	teesolar_shared
#)

# Build
#add_executable(${TU_EXECNAME_SERVER} "${PROJECT_SOURCE_DIR}/src/server.cpp" ${teesolar_server_src})
#set_target_properties(${TU_EXECNAME_SERVER} PROPERTIES COMPILE_FLAGS "-DKERNEL_TYPE=KERNEL_SERVER")
#target_link_libraries (${TU_EXECNAME_SERVER} ${teesolar_server_libraries})
#install(TARGETS ${TU_EXECNAME_SERVER} DESTINATION bin)
#message(STATUS " * server ready for compilation")
message(STATUS " * server not yet implemented")

########################################################################
# TOOLS
########################################################################

function(set_target_output target dir)
	if(BUILD_TYPE)
		set_target_properties(
			${target}
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${dir}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dir}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dir}"
		)
	else()
		foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
			string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)
			set_target_properties(
				${target}
				PROPERTIES
				ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
				LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
				RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE}}/${dir}"
			)
		endforeach(CONFIG_TYPE)
	endif()
endfunction()

# Sources
file(GLOB teesolar_tools_src
	"${PROJECT_SOURCE_DIR}/src/external/pnglite/*.c"
	"${PROJECT_SOURCE_DIR}/src/client/loading_tools.cpp"
)

# Libraries
set(teesolar_tools_libraries
teesolar_shared
)

# Build
add_library(teesolar_tools ${teesolar_tools_src})
target_link_libraries (teesolar_tools ${teesolar_tools_libraries})

# TUP Info
add_executable(tupinfo ${PROJECT_SOURCE_DIR}/src/tupinfo.cpp)
set_target_output(tupinfo "tupinfo")
target_link_libraries(
	tupinfo
	teesolar_tools
)

# Make Assets
function(make_assets progname)
	add_executable(make_assets_${progname} ${PROJECT_SOURCE_DIR}/src/make_assets_${progname}.cpp)
	set_target_output(make_assets_${progname} "make_assets")
	target_link_libraries(
		make_assets_${progname}
		teesolar_tools
	)
	add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${DATA_OUTPUT_DIRECTORY}/packages")
	add_custom_command(TARGET make_assets_${progname} POST_BUILD COMMAND make_assets_${progname} --data-dir "${CMAKE_SOURCE_DIR}/datasrc" --data-dir "${CMAKE_SOURCE_DIR}/data" --save-dir "${DATA_OUTPUT_DIRECTORY}" --save-dir-noinit)
endfunction()

make_assets(ddnet)
make_assets(gui_editor)
make_assets(infclass)
make_assets(infnext)
make_assets(ninslash)
make_assets(openfng)
make_assets(sport)
make_assets(teeworlds)

message(STATUS " * tools ready for compilation")

########################################################################
# UNIT TEST
########################################################################

enable_testing()

function(make_test testname)
	add_executable(test_${testname} src/unittest/${testname}.cpp)
	set_target_output(test_${testname} "test")
	target_link_libraries(
		test_${testname}
		teesolar_shared
	)
	add_test(NAME test_${testname} COMMAND $<TARGET_FILE:test_${testname}>)
endfunction()

make_test(system_string)
make_test(system_fs)
make_test(shared_kernel)
make_test(localization)
make_test(archivefile)
make_test(assetsmanager)

message(STATUS "******************************")
