include(ExternalProject)

#set(URL_MAIN http://vmh102.cpqd.com.br:8280/asr_dist/repository)
#set(OPENSSL_VERSION 1.0.2k)
if(UNIX)
set(URL_OPENSSL_ZIP
    "https://github.com/openssl/openssl/archive/OpenSSL_1_0_2-stable.zip"
    CACHE STRING "Full path or URL of openssl source")
endif()

# Not yet supported
#if(MSVC)
#set(URL_OPENSSL_ZIP
#    "${URL_MAIN}/openssl/${OPENSSL_VERSION}/openssl-${OPENSSL_VERSION}-vs2012.zip"
#    CACHE STRING "Full path or URL of openssl source")
#endif()

set(OPENSSL_ROOT "${CMAKE_BINARY_DIR}/third-party/openssl")

if(UNIX)
    set(CONFIGURE_CMD ./config no-shared -fPIC)
    set(BUILD_CMD make)
    set(CRYPTO_LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/openssl/libcrypto.a")
    set(SSL_LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/openssl/libssl.a")
	ExternalProject_Add(openssl_ext
        URL ${URL_OPENSSL_ZIP}
        SOURCE_DIR "${OPENSSL_ROOT}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${CONFIGURE_CMD}
        BUILD_COMMAND ${BUILD_CMD}
        INSTALL_COMMAND ""
        )
endif()

# Not yet supported
#if(MSVC)
#    set(CRYPTO_LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/openssl/lib/libeay32MT.lib ")
#    set(SSL_LIB_LOCATION "${CMAKE_BINARY_DIR}/third-party/openssl/lib/ssleay32MT.lib ")
#	ExternalProject_Add(openssl_ext
#        URL ${URL_OPENSSL_ZIP}
#        SOURCE_DIR "${OPENSSL_ROOT}"
#        CONFIGURE_COMMAND ""
#        BUILD_COMMAND ""
#        INSTALL_COMMAND ""
#        )
#endif()

add_library(ssl STATIC IMPORTED)
set_target_properties(ssl PROPERTIES IMPORTED_LOCATION ${SSL_LIB_LOCATION})
add_library(crypto STATIC IMPORTED)
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION ${CRYPTO_LIB_LOCATION})
 
set(OPENSSL_INCLUDE_DIRS ${OPENSSL_ROOT}/include)
set(OPENSSL_LIB_PATH ${OPENSSL_ROOT})
set(OPENSSL_LIBRARIES crypto ssl)
