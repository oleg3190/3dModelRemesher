cmake_minimum_required(VERSION 3.20)
project(qremesh VERSION 2.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optimization flags
if(MSVC)
    add_compile_options(/W4 /WX /O2 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Werror -O3)
endif()

# Dependencies
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(TBB QUIET)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${EIGEN3_INCLUDE_DIR}
)

set(CORE_SOURCES
    src/common/half_edge_mesh.cpp
    src/common/bvh.cpp
    src/remesh_context.cpp
    src/stages/stage0_prep.cpp
    src/stages/stage1_cut.cpp
    src/stages/stage2_size.cpp
    src/stages/stage3_field.cpp
    src/stages/stage4_patch.cpp
    src/stages/stage5_param.cpp
    src/stages/stage6_opt.cpp
    src/qremesh_c_api.cpp
)

# Core Shared Library
add_library(qremesh_core SHARED ${CORE_SOURCES})
target_link_libraries(qremesh_core PRIVATE Eigen3::Eigen)
if(TBB_FOUND)
    target_link_libraries(qremesh_core PRIVATE TBB::tbb)
endif()

# Core Static Library
add_library(qremesh_core_static STATIC ${CORE_SOURCES})
target_link_libraries(qremesh_core_static PRIVATE Eigen3::Eigen)
if(TBB_FOUND)
    target_link_libraries(qremesh_core_static PRIVATE TBB::tbb)
endif()

# Standalone CLI Executable
add_executable(qremesh_cli cli/main.cpp)
target_link_libraries(qremesh_cli PRIVATE qremesh_core Eigen3::Eigen)

# Copy compiled shared library to the Blender addon directory
set(ADDON_WHEELS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/addon/qremesh/wheels")
file(MAKE_DIRECTORY ${ADDON_WHEELS_DIR})

add_custom_command(TARGET qremesh_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:qremesh_core> "${ADDON_WHEELS_DIR}/$<TARGET_FILE_NAME:qremesh_core>"
)
