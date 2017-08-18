include(ExternalProject)
 
set(URL_MAIN https://github.com/chriskohlhoff/asio/archive)
set(ASIO_VERSION 1-10-6)
set(URL_ASIO_ZIP
    "${URL_MAIN}/asio-${ASIO_VERSION}.zip"
    CACHE STRING "Full path or URL of asio source")
 
ExternalProject_Add(asio_ext
        URL ${URL_ASIO_ZIP}
        SOURCE_DIR "${CMAKE_BINARY_DIR}/third-party/asio"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        )
 
set(ASIO_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third-party/asio/asio/include")
