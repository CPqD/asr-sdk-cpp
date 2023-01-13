include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#find_package (Threads REQUIRED)
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_SOURCE_DIR}/src/aux/*.cc)
message("OPENSSL_LIBRARIES: " ${OPENSSL_LIBRARIES})
add_executable(cpqd-ws-mock ${SOURCES})
target_link_libraries(cpqd-ws-mock -lssl -lcrypto pthread boost_system dl )

add_dependencies(cpqd-ws-mock openssl_ext)