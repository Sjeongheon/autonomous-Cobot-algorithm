cmake_minimum_required(VERSION 3.1)
project(robotSim)

set (CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/lvt/cmake/")

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

#option(USE_TMDRIVER "Enable tm-driver." OFF)

option(USE_GUI "Enable gui." OFF)
if(USE_GUI)
add_definitions(-DUSE_GUI)
endif()


#add_compile_options(-march=native -fvisibility=hidden -fPIC)
add_compile_options(-march=native -fPIC)

#imgui
set(IMGUI "${PROJECT_SOURCE_DIR}/imgui")
list(APPEND INC_DIRS ${IMGUI} ${IMGUI}/backends)

#set(IMGUIDIALOG "${PROJECT_SOURCE_DIR}/../../ImGuiFileDialog")
#list(APPEND INC_DIRS ${IMGUIDIALOG} ${IMGUIDIALOG}/stb)

#set(SERIAL "${PROJECT_SOURCE_DIR}/../../serialib")
#list(APPEND INC_DIRS ${SERIAL}/lib)

find_package(OpenCV)
list(APPEND INC_DIRS ${OpenCV_INCLUDE_DIRS})
list(APPEND LINK_LIBS ${OpenCV_LIBS})

find_package(Eigen3)
list(APPEND INC_DIRS ${EIGEN3_INCLUDE_DIR} )

find_package(glfw3 REQUIRED)
list(APPEND LINK_LIBS glfw)

find_package(glm REQUIRED)
#find_package(Open3D REQUIRED)

find_package(assimp REQUIRED)
list(APPEND LINK_LIBS assimp)

find_package(Lua REQUIRED)
list(APPEND LINK_LIBS ${LUA_LIBRARIES})
#if(${LUA_FOUND})
#	message("Found lua verion ${LUA_VERSION_STRING} : ${LUA_LIBRARY}")
#endif()


list(APPEND LINK_LIBS
)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
if(LINUX)
	message("Detected Linux")
	list(APPEND LINK_LIBS GL GLEW udev v4l2 )
endif()

#if($(UNAME_S), Darwin) #APPLE
if(APPLE) #APPLE
	message("Detected OSX")
	#list(APPEND INC_DIRS "-I/usr/local/include -I/opt/local/include -I/opt/homebrew/include -I/usr/local/opt")
	list(APPEND INC_DIRS "-I/usr/local/opt/glfw/include")

	list(APPEND LINK_LIBS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	list(APPEND LINK_LIBS "-L/usr/local/lib" glew)
endif()

include_directories( 
	${CMAKE_SOURCE_DIR}/src
	/usr/include
	${INC_DIRS}
)
 
link_directories(
	"/usr/local/lib"
#	${PCL}/lib
#	${OCCT}/lib
#	${CPPSERIAL}/lib
)
   
####################################################################################################

#add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLEW")
#add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLFW")

file(GLOB IMGUISRC "${IMGUI}/*.cpp")
add_library(imgui SHARED
	${IMGUISRC}
	${IMGUI}/backends/imgui_impl_opengl3.cpp
	${IMGUI}/backends/imgui_impl_glfw.cpp
	${IMGUI}/misc/cpp/imgui_stdlib.cpp
)
TARGET_LINK_LIBRARIES(imgui glfw)

#add_definitions("-DIMGUI_DEFINE_MATH_OPERATORS")

#file(GLOB IMGUIDIALOGSRC "${IMGUIDIALOG}/*.cpp")
#add_library(imguidialog SHARED ${IMGUIDIALOGSRC})
#TARGET_LINK_LIBRARIES(imguidialog imgui)

#MESSAGE("src : ${SERIALPORT}/src/*.cpp")
#file(GLOB SERIALSRC "${SERIAL}/lib/*.cpp")
#add_library(serial SHARED ${SERIALSRC})

##################################################

#add_definitions(-DLUA)
file(GLOB SRCs "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_library(robot SHARED ${SRCs})
TARGET_LINK_LIBRARIES(robot imgui ${LINK_LIBS}) # amvcommon pthread serial)

add_executable(testImgui ${CMAKE_SOURCE_DIR}/apps/testImgui.cpp )
target_link_libraries(testImgui robot)

add_executable(testLua ${CMAKE_SOURCE_DIR}/apps/testLuaScript.cpp )
target_link_libraries(testLua robot)
