cmake_minimum_required (VERSION 2.8)
project(asr-client C CXX)

# Include our cmake macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set (CMAKE_C_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC -pie ")

# Library version
set(ASR_CLIENT_MAJOR_VERSION 0)
set(ASR_CLIENT_MINOR_VERSION 0)
set(ASR_CLIENT_PATCH_VERSION 1)
set(ASR_CLIENT_RC_VERSION "rc1")

if(ASR_CLIENT_RC_VERSION)
   set(ASR_CLIENT_VERSION ${ASR_CLIENT_MAJOR_VERSION}.${ASR_CLIENT_MINOR_VERSION}.${ASR_CLIENT_PATCH_VERSION}-${ASR_CLIENT_RC_VERSION})
else()
   set(ASR_CLIENT_VERSION ${ASR_CLIENT_MAJOR_VERSION}.${ASR_CLIENT_MINOR_VERSION}.${ASR_CLIENT_PATCH_VERSION})
endif()

include(utils)
include(compiler-options)
include(CMakeHelpers)

# including flag -fPIC in all project
set (CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# build components
# ===================================================
CPQD_OPTION(BUILD_TESTS "Build unit tests" OFF )
CPQD_OPTION(BUILD_TOOLS "Build tools" OFF )
CPQD_OPTION(BUILD_EXAMPLES "Build examples" OFF )
CPQD_OPTION(BUILD_SHARED_LIBS "Create shared libraries" ON )
# ===================================================

# Dependencies
find_package(ALSA REQUIRED)

# ----------------------------------------------------------------------------
# Process subdirectories
# ----------------------------------------------------------------------------

# websocketpp
include(WebsocketppDownloader)
set(WEBSOCKETPP_VERSION 0.8.2)
websocketpp_download_and_extract("${WEBSOCKETPP_VERSION}" WEBSOCKETPP_SOURCE_PATH)
set(WEBSOCKETPP_INCLUDE ${WEBSOCKETPP_SOURCE_PATH})
include_directories(${WEBSOCKETPP_INCLUDE})

include(cmake/json11.cmake)
include(cmake/openssl.cmake)
include(cmake/asio.cmake)
include(cmake/portaudio.cmake)

# library
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "include")
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include/aux)
add_subdirectory(src)

# examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (BUILD_WS_MOCK)
  add_subdirectory(ws_mock)
endif()

# unit tests
if(BUILD_TESTS)
  include(cmake/GTest.cmake)
  add_subdirectory(tests/unit)
endif()

# Dependencies for correct compilation order
add_dependencies(openssl_ext asio_ext)

# ----------------------------------------------------------------------------
# Summary:
# ----------------------------------------------------------------------------
status("")
status("General configuration for ASR Client =====================================")
status("Version control:" ${ASR_CLIENT_VERSION})

# ========================== build platform ==========================
status("")
status("Platform:")
status("Host:"             ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR})
if(CMAKE_CROSSCOMPILING)
  status("Target:"         ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR})
endif()
status("CMake:"            ${CMAKE_VERSION})
status("CMake generator:"  ${CMAKE_GENERATOR})
status("CMake build tool:" ${CMAKE_BUILD_TOOL})
if(MSVC)
  status("MSVC:"           ${MSVC_VERSION})
endif()

if(NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
  status("Configuration:"  ${CMAKE_BUILD_TYPE})
endif()

# ========================== Auxiliary libraries ==========================
status("")
status("Websocketpp Version" ${WEBSOCKETPP_VERSION})

# ========================== tools and tests ==========================
status("")
status("  Tests and tools:")
status("    Unit tests:"        BUILD_TESTS       THEN YES ELSE NO)
