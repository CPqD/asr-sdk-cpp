include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

build_executable(mic mic.cc)
target_link_libraries(mic asr-client)

build_executable(basic basic.cc)
target_link_libraries(basic asr-client)
