configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/test_config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
foreach(local_file ${SOURCES})
    get_filename_component(local_filename ${local_file} NAME_WE)
    add_executable(${local_filename} ${local_file})
    googletest(${local_filename} ${local_file} asr-client gtest_main)
    add_dependencies(${local_filename} gtest)
endforeach()
