project(Film_Network)

set(UV_MSG_FRAMING_DIR ./deps/libuv_message_framing)
set(FilmNetworkFiles server.cpp client.cpp write.cpp ${UV_MSG_FRAMING_DIR}/uv_msg_framing.c)

include_directories(${UV_MSG_FRAMING_DIR})

add_library(Film_Network ${FilmNetworkFiles})

include(${CMAKE_CURRENT_SOURCE_DIR}/FindLibUV.cmake)
target_link_libraries(Film_Network uv)
