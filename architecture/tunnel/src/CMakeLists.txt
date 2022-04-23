set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
message(${CMAKE_SYSTEM_NAME})
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
list(APPEND SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap/tun_linux.cpp)
else()
list(APPEND SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap/tun_osx.cpp)
add_definitions(-DOSX_SYSTEM)
endif()
add_executable(tunnel ${SRC_LIST})
target_include_directories(tunnel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap)
target_link_libraries(tunnel stdc++ pthread)
