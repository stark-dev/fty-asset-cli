cmake_minimum_required(VERSION 3.13)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)

##############################################################################################################

project(fty-asset-cli
VERSION 0.1.0
DESCRIPTION "Tool to test new asset interfaces"
)

##############################################################################################################
set(CMAKE_BUILD_TYPE Debug)

etn_target(exe ${PROJECT_NAME}
    SOURCES
        src/fty-asset-cli.cpp

    INCLUDE_DIRS
        include/

    USES
        cxxtools
        fty_common_logging
        fty_common_messagebus
        pthread
        stdc++fs
)

##############################################################################################################
