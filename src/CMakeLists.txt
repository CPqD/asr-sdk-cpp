cmake_minimum_required(VERSION 2.8.11)
include (GenerateExportHeader)
add_definitions(-DASIO_STANDALONE
				-D_WEBSOCKETPP_CPP11_THREAD_
				-D_WEBSOCKETPP_CPP11_RANDOM_DEVICE_
				-D_WEBSOCKETPP_CPP11_INTERNAL_
				-DASIO_HAS_STD_ADDRESSOF
				-DASIO_HAS_STD_TYPE_TRAITS)
if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()
				
if(CMAKE_VERSION VERSION_LESS "3")
  add_definitions(-std=c++11)
else()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories (${ASIO_INCLUDE_DIRS})
include_directories (${JSON11_INCLUDE_DIRS})
include_directories (${OPENSSL_INCLUDE_DIRS})
include_directories (${PORTAUDIO_INCLUDE_DIRS})

file (GLOB SOURCE_FILES "*.cc" "*.cpp")
file (GLOB HEADER_FILES "*.h" "*.hpp")

init_target (asr-client)
build_library (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
GENERATE_EXPORT_HEADER( ${TARGET_NAME})

target_link_libraries(${TARGET_NAME}
                      ${JSON11_LIBRARIES}
                      ${OPENSSL_LIBRARIES}
                      ${PORTAUDIO_LIBRARIES})
target_link_libraries(${TARGET_NAME} dl pthread asound)
link_openssl ()
set_library_version (${TARGET_NAME} ${ASR_CLIENT_VERSION}
                                    ${ASR_CLIENT_MAJOR_VERSION})

add_dependencies(asr-client asio_ext)
add_dependencies(asr-client json11_ext)
add_dependencies(asr-client openssl_ext)
add_dependencies(asr-client portaudio)
