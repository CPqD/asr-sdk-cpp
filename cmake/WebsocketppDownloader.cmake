# websocketpp.

macro(websocketpp_extract WEBSOCKETPP_ARCHIVE VERSION_STRING WEBSOCKETPP_SOURCE_PATH)

  if(EXISTS "${WEBSOCKETPP_ARCHIVE}")
    set(${WEBSOCKETPP_SOURCE_PATH} "${PROJECT_BINARY_DIR}/websocketpp-${VERSION_STRING}")
    message(STATUS "Extracting Websocketpp sources to '${${WEBSOCKETPP_SOURCE_PATH}}'...")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf "${WEBSOCKETPP_ARCHIVE}"
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
  endif()

endmacro(websocketpp_extract)

macro(websocketpp_download VERSION_STRING WEBSOCKETPP_ARCHIVE)

  set(WEBSOCKETPP_ARCHIVE_URL "https://github.com/zaphoyd/websocketpp/archive/${VERSION_STRING}.zip")
  set(WEBSOCKETPP_DOWNLOAD_PATH "${PROJECT_BINARY_DIR}/websocketpp-${VERSION_STRING}.tar.gz")

  if (NOT EXISTS ${WEBSOCKETPP_DOWNLOAD_PATH})

    message(STATUS "Downloading Websocketpp version ${VERSION_STRING} from '${WEBSOCKETPP_ARCHIVE_URL}'...")
    file(DOWNLOAD "${WEBSOCKETPP_ARCHIVE_URL}" "${WEBSOCKETPP_DOWNLOAD_PATH}" STATUS status)
    list(GET status 0 error_code)

    if(error_code)
      file(REMOVE "${WEBSOCKETPP_DOWNLOAD_PATH}")
      list(GET status 1 error_msg)
      message(FATAL_ERROR
        "Failed to download Websocketpp source archive '${WEBSOCKETPP_ARCHIVE_URL}': ${error_msg}")
    else()
      set(${WEBSOCKETPP_ARCHIVE} "${WEBSOCKETPP_DOWNLOAD_PATH}")
      message(STATUS "Successfully downloaded Websocketpp version ${VERSION_STRING}.")
    endif()

  else()
    set(${WEBSOCKETPP_ARCHIVE} "${WEBSOCKETPP_DOWNLOAD_PATH}")
  endif()

endmacro(websocketpp_download)

macro(websocketpp_download_and_extract VERSION_STRING WEBSOCKETPP_SOURCE_PATH)
  websocketpp_download(${VERSION_STRING} WEBSOCKETPP_ARCHIVE)
  websocketpp_extract(${WEBSOCKETPP_ARCHIVE} ${VERSION_STRING} ${WEBSOCKETPP_SOURCE_PATH})
endmacro(websocketpp_download_and_extract)
