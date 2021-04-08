get_filename_component(IzTree_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

find_dependency(Qt5 REQUIRED COMPONENTS Core Sql Concurrent)
find_dependency(IzSQLUtilities REQUIRED)

if(NOT TARGET IzTree::IzTree)
    include("${IzTree_CMAKE_DIR}/cmake/IzTree/IzTreeTargets.cmake")
endif()