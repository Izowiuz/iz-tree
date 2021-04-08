# cmake minumum version
cmake_minimum_required(VERSION 3.10)

# debug postfix
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if (${uppercase_CMAKE_BUILD_TYPE} MATCHES "^(DEBUG)$")
	set(CMAKE_DEBUG_POSTFIX d)
else()
	set(CMAKE_DEBUG_POSTFIX "")
endif ()

# project name and version
project(IzTree VERSION 1.0 LANGUAGES CXX)

# if CMAKE_INSTALL_PREFIX is set to default value I override it
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	# prefix path for windows platform
	if (${CMAKE_SYSTEM_NAME} MATCHES "^Windows$")
		set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../deploy CACHE PATH "CMAKE_INSTALL_PREFIX path" FORCE)
	endif()
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# project architecture
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(PLATFORM_TARGET "x64")
else()
	set(PLATFORM_TARGET "x86")
endif()

# subdirectories
option(BUILD_LIBRARY "Enable building IzTree library" ON)
option(BUILD_PLUGIN "Enable building IzTree QML plugin" ON)

if(BUILD_LIBRARY)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/library)
endif()
if(BUILD_PLUGIN)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/plugin)
endif()
