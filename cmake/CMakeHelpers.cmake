
# Initialize target.
macro (init_target NAME)
    set (TARGET_NAME ${NAME})
    message ("** " ${TARGET_NAME})

    # Include our own module path. This makes #include "x.h"
    # work in project subfolders to include the main directory headers.
    include_directories (${CMAKE_CURRENT_SOURCE_DIR})
endmacro ()

# Build executable for executables
macro (build_executable TARGET_NAME)
    set (TARGET_LIB_TYPE "EXECUTABLE")

    add_executable (${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} asr-client)
endmacro ()

# Build library for libraries
macro (build_library TARGET_NAME)
    set (TARGET_LIB_TYPE "LIBRARY")

    add_library (${TARGET_NAME} ${ARGN})
    include_directories (${WEBSOCKETPP_INCLUDE})
    target_link_libraries (${TARGET_NAME} ${WEBSOCKETPP_PLATFORM_LIBS})
endmacro ()

# Finalize target for all types
macro (final_target)
    if ("${TARGET_LIB_TYPE}" STREQUAL "EXECUTABLE")
        install (TARGETS ${TARGET_NAME}
                 RUNTIME DESTINATION "bin"
                 CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
    endif ()

    if ("${TARGET_LIB_TYPE}" STREQUAL "LIBRARY")
        install (TARGETS ${TARGET_NAME}
                 DESTINATION "lib"
                 CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
    endif ()
endmacro ()

macro (set_library_version TARGET_NAME TARGET_VERSION TARGET_SOVERSION)
  set_target_properties(${TARGET_NAME} PROPERTIES VERSION ${TARGET_VERSION}
                                              SOVERSION ${TARGET_SOVERSION})
endmacro ()

macro (link_openssl)
    target_link_libraries (${TARGET_NAME} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
endmacro ()
