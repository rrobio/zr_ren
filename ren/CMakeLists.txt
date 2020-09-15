cmake_minimum_required(VERSION 3.13)
project(ren VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(libs/glad)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/shader.cpp
  src/object.cpp
  src/resource_manager.cpp
  src/log.cpp

  libs/imgui/imgui.cpp
  libs/imgui/imgui_draw.cpp
  libs/imgui/imgui_impl_glfw.cpp
  libs/imgui/imgui_impl_opengl3.cpp
  libs/imgui/imgui_widgets.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  libs/glm
  libs/stb
  libs/imgui
  libs/nlohmann
)
target_link_libraries(${PROJECT_NAME} glfw glad)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE $ENV{INCLUDE})
