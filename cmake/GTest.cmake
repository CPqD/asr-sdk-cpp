
# gtest.

include(ExternalProject)

set(URL_MAIN https://github.com/google/googletest/archive)
set(GTEST_VERSION 1.7.0)
set(URL_GTEST_ZIP
    "${URL_MAIN}/release-${GTEST_VERSION}.zip"
    CACHE STRING "Full path or URL of gtest source")

ExternalProject_Add(gtest_ext
        URL ${URL_GTEST_ZIP}
        BINARY_DIR "${CMAKE_BINARY_DIR}/third-party/gtest-build"
        SOURCE_DIR "${CMAKE_BINARY_DIR}/third-party/gtest-src"
        INSTALL_COMMAND ""
        CMAKE_ARGS "${gtest_cmake_args}"
          "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" -DBUILD_SHARED_LIBS=ON
        )
set(GTEST_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third-party/gtest-src/include")
link_directories("${CMAKE_BINARY_DIR}/third-party/gtest-build")

enable_testing()

find_package(Threads)

function(googletest name sources)
#     add_executable(${name} ${sources})
    target_link_libraries(${name} ${ARGN} gtest ${CMAKE_THREAD_LIBS_INIT})
    set_property(TARGET ${name} APPEND PROPERTY INCLUDE_DIRECTORIES
                 ${GTEST_INCLUDE_DIRS}
                 )
    # Working directory: where the dlls are installed.
    add_test(NAME ${name}
             COMMAND ${name})
endfunction()
