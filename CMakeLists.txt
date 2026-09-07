cmake_minimum_required(VERSION 3.20)
project(SockLib LANGUAGES CXX)

add_library(SockLib STATIC
 "src/Deserializer.cpp"
 "src/Exception.cpp"
 "src/Helper.cpp"
 "src/Serializer.cpp"
 "src/Server.cpp"
 "src/Sock.cpp")

target_include_directories(SockLib PUBLIC "include/")
target_compile_features(SockLib PUBLIC cxx_std_20)
if(WIN32)
    target_link_libraries(SockLib PRIVATE ws2_32)
endif()