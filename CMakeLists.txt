cmake_minimum_required(VERSION 2.8)

project (my3D)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(bgfxDir ~/Programs/bgfx)
set(bgfxLibDir ${bgfxDir}/.build/osx64_clang/bin)
set(boostDir ~/Programs/3rdParty/boost_1_66_0)

include_directories(
	${bgfxDir}/../bx/include/compat/osx
	${bgfxDir}/../bx/include
	${bgfxDir}/../bimg/include
	${bgfxDir}/include
	${bgfxDir}/3rdparty
	${boostDir}
)

link_directories(
	${bgfxLibDir}
	${boostDir}/stage/lib
)

add_definitions(
	-D__STDC_LIMIT_MACROS
	-D__STDC_FORMAT_MACROS
	-D__STDC_CONSTANT_MACROS
	-DENTRY_CONFIG_IMPLEMENT_MAIN=1
)

IF (CMAKE_BUILD_TYPE MATCHES "")
	set(CMAKE_BUILD_TYPE "Debug")
ENDIF ()
IF (CMAKE_BUILD_TYPE MATCHES "Debug")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")
	add_definitions(-D_DEBUG)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
ELSE ()
	MESSAGE(STATUS "unknown CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
ENDIF ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

link_libraries(-m64
	"-framework Cocoa"
	"-framework QuartzCore"
	"-framework OpenGL"
	"-weak_framework Metal"
)

add_executable(my3D
	src/appEntry.cpp
	src/camera.cpp
	src/color.cpp
	src/confDef.cpp
	src/drawItem.cpp
	src/entity.cpp
	src/light.cpp
	src/material.cpp
	src/mesh.cpp
	src/model.cpp
	src/my3D.cpp
	src/renderState.cpp
	src/renderUtil.cpp
	src/resData.cpp
	src/shader.cpp
	src/shading.cpp
	src/vtxDecl.cpp
)

set(CFG "Release")

target_link_libraries(my3D
	libexample-common${CFG}.a
	libexample-glue${CFG}.a
	libbgfx${CFG}.a
	libbimg_decode${CFG}.a
	libbimg${CFG}.a
	libbx${CFG}.a
	libboost_system.a
	libboost_filesystem.a
)
