# cmake minumum version
cmake_minimum_required(VERSION 3.10)

# gnu directories
include(GNUInstallDirs)

# Qt's modules
find_package(Qt5 COMPONENTS Core Sql Concurrent REQUIRED)

# IzSQLUtilities
find_package(IzSQLUtilities CONFIG REQUIRED)

# Qt's moc files
qt5_wrap_cpp(MOC_files
	"include/IzTree/Node.h"
	"include/IzTree/TreeModel.h"
	"include/IzTree/TreeProxyModel.h"
	"include/IzTree/DataProvider.h"
	"include/IzTree/SQLDataProvider.h"
	"include/IzTree/Tree.h"
)

# library definition
add_library(${PROJECT_NAME} SHARED
	"private/TreeProxyModel.cpp"
	"private/DataProvider.cpp"
	"private/Node.cpp"
	"private/SQLDataProvider.cpp"
	"private/Tree.cpp"
	"private/TreeModel.cpp"
	${MOC_files}
)

# public headers
set(PUBLIC_HEADERS
	"include/IzTree/TreeProxyModel.h"
	"include/IzTree/DataProvider.h"
	"include/IzTree/IzTree_Global.h"
	"include/IzTree/SQLDataProvider.h"
	"include/IzTree/Tree.h"
	"include/IzTree/TreeModel.h"
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

# include directories for target
target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private
)

# target compile features
target_compile_features(${PROJECT_NAME}
	PRIVATE
		cxx_std_17
)

# target links
target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Qt5::Core
	PRIVATE
		Qt5::Sql
		Qt5::Concurrent
		IzSQLUtilities::IzSQLUtilities
)

# target compile definitions
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		IZTREE_LIBRARY
		QT_USE_QSTRINGBUILDER
)

# installs
install (TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}-export
	RUNTIME DESTINATION libraries/${PROJECT_NAME}/${CMAKE_INSTALL_BINDIR}/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET}
	LIBRARY DESTINATION libraries/${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET}
	ARCHIVE DESTINATION libraries/${PROJECT_NAME}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_BUILD_TYPE}/${Qt5Core_VERSION}/${CMAKE_CXX_COMPILER_ID}/${PLATFORM_TARGET}
	PUBLIC_HEADER DESTINATION libraries/${PROJECT_NAME}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
install(FILES ${PROJECT_NAME}Config.cmake
	 DESTINATION libraries/${PROJECT_NAME}
)

# export target
install(EXPORT ${PROJECT_NAME}-export
	FILE
		${PROJECT_NAME}Targets.cmake
	NAMESPACE
		IzTree::
	DESTINATION libraries/${PROJECT_NAME}/cmake/${PROJECT_NAME}
)
