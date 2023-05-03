cmake_minimum_required(VERSION 3.25)

project(animatias)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB G_HEADERS
        network/win/*.h
        network/*.h
        voice/*.h
        server/*.h
        third_party/*.h
        *.h)

add_executable(animatias main.cpp ${G_HEADERS} server/server.cpp network/win/winapi_acceptor.cpp network/win/winapi_stream.cpp network/win/wsa.cpp)
include_directories(animaties /)
