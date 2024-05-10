cmake_minimum_required(VERSION 3.1)
project(testImgui VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

add_compile_options(-march=native -fPIC)

#imgui
set(IMGUI "${PROJECT_SOURCE_DIR}/src/imgui")
list(APPEND INC_DIRS ${IMGUI} "include/glad" "include/GLFW" "include/imgui" "include/KHR" "include/glm" "include")

list(APPEND LINK_LIBS lib)

include_directories( 
	${PROJECT_SOURCE_DIR}/src
	${INC_DIRS}
)

link_directories(LINK_LIBS)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/ImguiwithRobotArm.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/ImguiTest.cpp")

add_executable(testImgui ${SRC})

find_package(OpenGL REQUIRED)

target_link_libraries(testImgui PRIVATE ${OPENGL_LIBRARY} glu32)
target_link_libraries(testImgui PRIVATE glfw3)