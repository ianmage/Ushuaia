cmake_minimum_required(VERSION 2.8)

project (my3D)

cmake_policy(SET CMP0054 NEW) 
cmake_policy(SET CMP0045 NEW)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if (WIN32)
	set(EXT_DIR D:/HELPER/3rdParty)
elseif (APPLE)
	set(EXT_DIR ~/Programs/3rdParty)
endif ()
set(BX_INCLUDE_DIR ${EXT_DIR}/bx/include)
set(BIMG_INCLUDE_DIR ${EXT_DIR}/bimg/include)
set(BGFX_INCLUDE_DIR ${EXT_DIR}/bgfx/include)
set(BOOST_INCLUDE_DIR ${EXT_DIR}/boost_1_66_0)
set(XML_INCLUDE_DIR ${EXT_DIR}/tinyxml2)
set(JSON_INCLUDE_DIR ${EXT_DIR}/rapidjson/include)
set(BOOST_LIB_DIR ${BOOST_INCLUDE_DIR}/stage/lib)
if (WIN32)
	set(BX_COMPAT_DIR ${BX_INCLUDE_DIR}/compat/msvc)
	set(BGFX_LIB_DIR ${BGFX_INCLUDE_DIR}/../.build/win64_vs2017)
elseif (APPLE)
	set(BX_COMPAT_DIR ${BX_INCLUDE_DIR}/compat/osx)
	set(BGFX_LIB_DIR ${BGFX_INCLUDE_DIR}/../.build/osx64_clang)
endif ()

include_directories(
	${BX_INCLUDE_DIR}
	${BX_COMPAT_DIR}
	${BIMG_INCLUDE_DIR}
	${BGFX_INCLUDE_DIR}
	${BGFX_INCLUDE_DIR}/../3rdparty
	${BOOST_INCLUDE_DIR}
	${XML_INCLUDE_DIR}
	${JSON_INCLUDE_DIR}
)

link_directories(
	${BGFX_LIB_DIR}/bin
	${BOOST_LIB_DIR}
)

add_definitions(
	-D__STDC_LIMIT_MACROS
	-D__STDC_FORMAT_MACROS
	-D__STDC_CONSTANT_MACROS
	-DENTRY_CONFIG_IMPLEMENT_MAIN=1
)
if (WIN32)
add_definitions(-D_WIN32)
elseif (APPLE)
add_definitions(-D_APPLE)
#add_compile_options(-Wmissing-braces)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "")
	set(CMAKE_BUILD_TYPE "Debug")
endif ()
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")
	add_definitions(-D_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES "Release")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
else ()
	MESSAGE(STATUS "unknown CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if (APPLE)
	link_libraries(-m64
		"-framework Cocoa"
		"-framework QuartzCore"
		"-framework OpenGL"
		"-weak_framework Metal"
	)
endif ()

add_executable(executable
	src/appEntry.cpp
	src/blur.cpp
	src/camera.cpp
	src/confDef.cpp
	src/drawItem.cpp
	src/entity.cpp
	src/forwardRendering.cpp
	src/frameBuffer.cpp
	src/hdr.cpp
	src/light.cpp
	src/material.cpp
	src/mesh.cpp
	src/model.cpp
	src/postProcess.cpp
	src/renderHelper.cpp
	src/renderState.cpp
	src/renderTarget.cpp
	src/renderUtil.cpp
	src/script.cpp
	src/scene.cpp
	src/serialize.cpp
	src/shader.cpp
	src/shading.cpp
	src/texture.cpp
	src/vtxDecl.cpp
	src/workFlow.cpp
)

if (WIN32)

	target_link_libraries(executable
		bx.lib
		bimg.lib
		bimg_decode.lib
		bgfx.lib
		example-glue.lib
		example-common.lib
	)

elseif (APPLE)

	set(CFG "Release")

	target_link_libraries(executable
		libbx${CFG}.a
		libbimg${CFG}.a
		libbimg_decode${CFG}.a
		libbgfx${CFG}.a
		#libbimg_encode${CFG}.a
		#libfcpp${CFG}.a
		#libglsl-optimizer${CFG}.a
		#libglslang${CFG}.a
		#libspirv-opt${CFG}.a
		libexample-glue${CFG}.a
		libexample-common${CFG}.a
		libboost_system.a
		libboost_filesystem.a
	)

endif ()
