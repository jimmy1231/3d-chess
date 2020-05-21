project(render)

cmake_minimum_required(VERSION 3.10)
set(ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Set compiler to compile for c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${ROOT}/glfw glfw)
add_subdirectory(${ROOT}/glad glad)
find_package(OpenGL REQUIRED)

add_executable(render main.cpp 
					  ${ROOT}/src/Scene.cpp
					  ${ROOT}/src/Model.cpp
					  ${ROOT}/src/Orientation.cpp
					  ${ROOT}/src/Light.cpp
					  ${ROOT}/src/Texture.cpp
					  ${ROOT}/src/Data.cpp)
					  
include_directories(AFTER ${ROOT}/include)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/glfw/include)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/glad/include)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/glm)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/json)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC $ENV{HOME}/dev/libs/CImg_latest)

target_link_libraries(render glfw glad OpenGL::GL)
