cmake_minimum_required(VERSION 2.8)

project (Ushuaia)

cmake_policy(SET CMP0054 NEW) 
cmake_policy(SET CMP0045 NEW)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if (WIN32)
	set(EXT_DIR D:/3rdParty)
elseif (APPLE)
	set(EXT_DIR ~/Programs/3rdParty)
endif ()
set(BOOST_VER "1_75")
set(BX_INCLUDE_DIR ${EXT_DIR}/bx/include)
set(BIMG_INCLUDE_DIR ${EXT_DIR}/bimg/include)
set(BGFX_INCLUDE_DIR ${EXT_DIR}/bgfx/include)
set(BOOST_INCLUDE_DIR ${EXT_DIR}/boost_${BOOST_VER})
set(XML_INCLUDE_DIR ${EXT_DIR}/pugixml/src)
set(JSON_INCLUDE_DIR ${EXT_DIR}/rapidjson/include)
set(BOOST_LIB_DIR ${BOOST_INCLUDE_DIR}/stage/lib)
set(ENTT_INCLUDE_DIR ${EXT_DIR}/entt/src/entt)
if (WIN32)
	set(BX_COMPAT_DIR ${BX_INCLUDE_DIR}/compat/msvc)
	set(BGFX_LIB_DIR ${BGFX_INCLUDE_DIR}/../.build/win64_vs2017)
elseif (APPLE)
	set(BX_COMPAT_DIR ${BX_INCLUDE_DIR}/compat/osx)
	set(BGFX_LIB_DIR ${BGFX_INCLUDE_DIR}/../.build/osx-x64)
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
	${ENTT_INCLUDE_DIR}
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

#if (CMAKE_BUILD_TYPE MATCHES "")
#	set(CMAKE_BUILD_TYPE "Debug")
#endif ()
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")
	add_definitions(-D_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES "Release")
	MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
else ()
	MESSAGE(STATUS "unknown CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if (APPLE)
	link_libraries(-m64
		"-framework Cocoa"
		"-framework QuartzCore"
		"-framework OpenGL"
		"-weak_framework Metal"
	)
endif ()

add_executable(executable
	src/AppEntry.cpp
	src/Blur.cpp
	src/Camera.cpp
	src/ConfDef.cpp
	src/DrawItem.cpp
	src/Entity.cpp
	src/ForwardRendering.cpp
	src/FrameBuffer.cpp
	src/HDR.cpp
	src/Light.cpp
	src/Material.cpp
	src/Mesh.cpp
	src/Model.cpp
	src/PostProcess.cpp
	src/RenderHelper.cpp
	src/RenderUtil.cpp
	src/Script.cpp
	src/Scene.cpp
	src/Serialize.cpp
	src/Shader.cpp
	src/Shading.cpp
	src/Texture.cpp
	src/VtxDecl.cpp
	src/Workflow.cpp
)

if (WIN32)	# MSVC

	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELWITHDEBINFO
	)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()

	target_compile_features(executable PUBLIC cxx_std_20)

	set_target_properties(executable PROPERTIES VS_USER_PROPS "predef.props")

	set(VC_VER "vc142")

	target_link_libraries(executable
		bx$(Configuration).lib
		bimg$(Configuration).lib
		bimg_decode$(Configuration).lib
		bgfx$(Configuration).lib
		example-glue$(Configuration).lib
		example-common$(Configuration).lib
	)

elseif (APPLE)

	set(CFG ${CMAKE_BUILD_TYPE})

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
