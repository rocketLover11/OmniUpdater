cmake_minimum_required(VERSION 3.10)
project(OmniUpdater VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT LINUX)
    message(FATAL_ERROR "This application is only meant for linux at this time.")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED libcjson)

add_executable(${PROJECT_NAME}
    src/main.c
    src/state.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CJSON_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CJSON_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin)