# cmake minumum version
cmake_minimum_required(VERSION 3.10)

# Qt's modules
find_package(Qt5 COMPONENTS Core Qml Quick REQUIRED)
find_package(Qt5QuickCompiler)


# Qt's moc files
qt5_wrap_cpp(MOC_files
	"include/IzTreeView/QmlPlugin.h"
	"include/IzTreeView/IzTreeView_Enums.h"
	"private/TreeViewController.h"
	"private/TreeVisibilityFilter.h"
	"private/ConnectorsModel.h"
)

# Qt's rcc files
qt5_add_resources(QRC_FILE
	"qml_src.qrc"
)

qtquick_compiler_add_resources(QRC_FILE
	"qml_src.qrc"
)

# library definition
add_library(${PROJECT_NAME}Plugin SHARED
	"private/QmlPlugin.cpp"
	"private/TreeViewController.cpp"
	"private/TreeVisibilityFilter.cpp"
	"private/ConnectorsModel.cpp"
	${QRC_FILE}
	${MOC_files}
)

set_target_properties(${PROJECT_NAME}Plugin PROPERTIES PREFIX "")

# include directories for target
target_include_directories(${PROJECT_NAME}Plugin
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private
)

# target compile features
target_compile_features(${PROJECT_NAME}Plugin
	PRIVATE
		cxx_std_17
)

# target links
target_link_libraries(${PROJECT_NAME}Plugin
	PUBLIC
		Qt5::Core
		Qt5::Qml
		Qt5::Quick
		${PROJECT_NAME}
)

target_include_directories(${PROJECT_NAME}Plugin
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/../library/include
)

# target compile definitions
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		QT_USE_QSTRINGBUILDER
)

# helper variables
set(LIBRARIES_BIN_PATH ${CMAKE_INSTALL_BINDIR}/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET})
set(PLUGINS_BIN_PATH ${CMAKE_INSTALL_PREFIX}/plugins/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET})

# installs
install (TARGETS ${PROJECT_NAME}Plugin
	RUNTIME DESTINATION plugins/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET}/${PROJECT_NAME}View
)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/qmldir"
	DESTINATION "plugins/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET}/IzTreeView"
)

# debug postfix
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if (${uppercase_CMAKE_BUILD_TYPE} MATCHES "^(DEBUG)$")
	set(CMAKE_DEBUG_POSTFIX d)
else()
	set(CMAKE_DEBUG_POSTFIX "")
endif ()

# qmldir generation
include(GenerateQmldirFile.cmake)
generate_qmldir_file(
				"IzTreeView"
				${PROJECT_NAME}
				""
)

#TODO: hack #1 - przekazywanie zmiennych do skryptu instalacyjnego
install (CODE "set(PROJECT_NAME \"${PROJECT_NAME}\")")
install (CODE "set(PROJECT_VERSION \"${PROJECT_VERSION}\")")
install (CODE "set(CMAKE_BUILD_TYPE \"${CMAKE_BUILD_TYPE}\")")
install (CODE "set(Qt5Core_VERSION \"${Qt5Core_VERSION}\")")
install (CODE "set(CMAKE_CXX_COMPILER_ID \"${CMAKE_CXX_COMPILER_ID}\")")
install (CODE "set(PLATFORM_TARGET \"${PLATFORM_TARGET}\")")
install (CODE "set(QT_QMAKE_EXECUTABLE \"${QT_QMAKE_EXECUTABLE}\")")
install (CODE "set(ENV_PATH \"${IzSQLUtilities_DIR}/${LIBRARIES_BIN_PATH};${PLUGINS_BIN_PATH};${CMAKE_INSTALL_PREFIX}/libraries/${PROJECT_NAME}/${LIBRARIES_BIN_PATH}\")")
install (SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/QmlPluginDump.cmake")
