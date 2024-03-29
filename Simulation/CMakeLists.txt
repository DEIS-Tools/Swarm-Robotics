cmake_minimum_required(VERSION 3.7)
include(ExternalProject)

project(SwarmSimulator)

#set(CMAKE_C_COMPILER    "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GetDependencies "Fetch external dependencies from web." ON)
if (GetDependencies)
    ExternalProject_add(json-ext
            URL https://github.com/nlohmann/json/archive/v3.7.3.zip
            URL_HASH SHA512=b47a07de9a071cce645a173d084df5dd31f7669154fc00f6c99e0506474d30e8376acaee1d3c79a50def4f25a36042951bfa4fca9a704687e59c368d05053158
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DJSON_BuildTests=OFF
            )

    ExternalProject_add(argos-ext
            URL https://github.com/ilpincy/argos3/archive/3.0.0-beta56.zip
            URL_HASH SHA512=516864a641cd3222e4260bc3c6dd09939ec9e26a19eb05b874d7ba3dd972ab91d1518d1a735de9265c88d508e3a6177d9978980df05aadcd3602181e19a87ab7
            SOURCE_SUBDIR = ../argos-ext/src
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DARGOS_INSTALL_LDSOCONF=OFF -DARGOS_DOCUMENTATION=OFF
            )

    #ExternalProject_add(model-utilities
    #        URL https://github.com/DEIS-Tools/model-utilities/archive/1.0.1.zip
    #        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    #        )

    include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
    link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
  endif (GetDependencies)


unset(ARGOS_WITH_LUA CACHE)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

# Emit all warning messages
add_definitions(-Wall -Werror)


add_custom_target(build-time-make-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory experiments)

add_subdirectory(controllers)

add_subdirectory(loop_functions)

#add_custom_command(OUTPUT run_a_scene.sh POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ../../experiment/runtime_scripts/run_a_scene.sh
#       ${CMAKE_BINARY_DIR}/external/bin/run_a_scene.sh
#        )



add_custom_target(move_run_script ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ../experiment/runtime_scripts/run_a_scene.sh
        ${CMAKE_BINARY_DIR}/external/bin/run_a_scene.sh)

#add_subdirectory(server)