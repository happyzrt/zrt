set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
message(${CMAKE_SYSTEM_NAME})
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
list(APPEND SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap/tun_linux.cpp)
else()
list(APPEND SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap/tun_osx.cpp)
add_definitions(-DOSX_SYSTEM)
endif()
if(TUNNEL_ONEFILE MATCHES "yes")
add_executable(tunnel ${CMAKE_CURRENT_SOURCE_DIR}/onefile/tunnel.c)
target_link_libraries(tunnel pthread)
message("onefile")
else()
add_executable(tunnel ${SRC_LIST})
target_include_directories(tunnel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tun_tap)
target_link_libraries(tunnel stdc++ pthread)
message("default")
endif()

set (INSTALL_ROOT "/")
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
set (INSTALL_ROOT "/Users/zrt/")
endif()
message(${INSTALL_ROOT})
install(TARGETS tunnel
RUNTIME DESTINATION ${INSTALL_ROOT}/zrt/architecture/tunnel/bin/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../config/tunnel.service
DESTINATION ${INSTALL_ROOT}/zrt/architecture/tunnel/config/)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../config/tunnel.service
DESTINATION /etc/systemd/system/)
endif()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../script
DESTINATION ${INSTALL_ROOT}/zrt/architecture/tunnel/)
