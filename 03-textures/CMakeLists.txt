cmake_minimum_required(VERSION 3.3)


project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCE "${SRC_DIR}/main.c")
add_executable(${PROJECT_NAME} "${SRC_DIR}/main.c" "${SRC_DIR}/shader.c")

target_compile_options(${PROJECT_NAME} PUBLIC -g -Wall -Werror -Wextra)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")


# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
add_subdirectory("${GLFW_DIR}")


# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")

target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} glad ${CMAKE_DL_LIBS})


