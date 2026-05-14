cmake_minimum_required(VERSION 3.25)
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

if(EXISTS "/etc/os-release")
    file(READ "/etc/os-release" OS_RELEASE_CONTENT)
    if(OS_RELEASE_CONTENT MATCHES "ID_LIKE=.*debian" OR OS_RELEASE_CONTENT MATCHES "ID=debian" OR OS_RELEASE_CONTENT MATCHES "ID=ubuntu")
        set(LINUX_DISTRO "DEBIAN")
    elseif(OS_RELEASE_CONTENT MATCHES "ID_LIKE=.*arch" OR OS_RELEASE_CONTENT MATCHES "ID=arch")
        set(LINUX_DISTRO "ARCH")
    elseif(OS_RELEASE_CONTENT MATCHES "ID_LIKE=.*rhel" OR OS_RELEASE_CONTENT MATCHES "ID=fedora" OR OS_RELEASE_CONTENT MATCHES "ID=rhel")
        set(LINUX_DISTRO "REDHAT")
    else()
        set(LINUX_DISTRO "UNIVERSAL")
    endif()
else()
    set(LINUX_DISTRO "UNIVERSAL")
endif()

message(STATUS "Detected Linux Distro Flavor: ${LINUX_DISTRO}")

if(LINUX_DISTRO STREQUAL "ARCH")
    set(BIN_INSTALL_DIR "usr/bin")
else()
    set(BIN_INSTALL_DIR "usr/local/bin")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})
install(FILES config.json DESTINATION etc/omni_updater)
install(FILES omni_updater-resume.desktop DESTINATION etc/xdg/autostart)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "rocketLover11")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Omni-updater engine for tracking system dependencies.")
set(CPACK_PACKAGE_CONTACT "gideonafisher@icloud.com")

if(LINUX_DISTRO STREQUAL "DEBIAN")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcjson1 (>= 1.7.0)")
    message(STATUS "CPack configured for .deb")
elseif(LINUX_DISTRO STREQUAL "REDHAT")
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_REQUIRES "cjson >= 1.7.0")
    message(STATUS "CPack configured for .rpm")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
