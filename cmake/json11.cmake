
include(ExternalProject)

set(URL_JSON11_ZIP
    "https://github.com/dropbox/json11/archive/master.zip"
    CACHE STRING "Full path or URL of JSON11 source")

ExternalProject_Add(json11_ext
        URL ${URL_JSON11_ZIP}
        SOURCE_DIR "${CMAKE_BINARY_DIR}/third-party/json11"
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        CMAKE_ARGS "${json11_cmake_args}"
          "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

add_library(libjson11 STATIC IMPORTED)
if (MSVC)
   set(LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/json11/${CMAKE_BUILD_TYPE}/json11.lib")
else ()
    set(LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/json11/libjson11.a")
endif ()
set_target_properties(libjson11 PROPERTIES IMPORTED_LOCATION ${LIB_LOCATION})


set(JSON11_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third-party/json11/")
set(JSON11_LIBRARIES libjson11)
