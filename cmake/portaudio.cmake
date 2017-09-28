
include(ExternalProject)

set(URL_PORTAUDIO_TGZ
    "http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz"
    CACHE STRING "Full path or URL of PORTAUDIO source")

ExternalProject_Add(portaudio_ext
        URL ${URL_PORTAUDIO_TGZ}
        SOURCE_DIR "${CMAKE_BINARY_DIR}/third-party/portaudio"
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        CMAKE_ARGS "${portaudio_cmake_args}"
          "-DCMAKE_BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\" "
          "-DCMAKE_C_FLAGS=\"-fPIC\" "
          "-DCMAKE_CXX_FLAGS=\"-fPIC\" "
          "-DPA_USE_ALSA=1 "
          "-DPA_USE_JACK=0 ")

add_library(portaudio STATIC IMPORTED)
if (MSVC)
   set(LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/portaudio/${CMAKE_BUILD_TYPE}/libportaudio_static.lib")
else ()
    set(LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/portaudio/libportaudio_static.a")
endif ()
set_target_properties(portaudio PROPERTIES IMPORTED_LOCATION ${LIB_LOCATION})

set(PORTAUDIO_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third-party/portaudio/include/")
set(PORTAUDIO_LIBRARIES portaudio)
