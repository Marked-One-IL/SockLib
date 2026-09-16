cmake_minimum_required(VERSION 3.20)
project(SockLib LANGUAGES CXX)

add_executable(Server
 "src/ser.cpp"
 "src/Exception.cpp"
 "src/Helper.cpp"
 "src/Server.cpp"
 "src/Sock.cpp")

add_executable(Client
 "src/cli.cpp"
 "src/Exception.cpp"
 "src/Helper.cpp"
 "src/Server.cpp"
 "src/Sock.cpp")

target_include_directories(Server PUBLIC "include/")
target_compile_features(Server PUBLIC cxx_std_20)
if(WIN32)
    target_link_libraries(Server PRIVATE ws2_32)
endif()

target_include_directories(Client PUBLIC "include/")
target_compile_features(Client PUBLIC cxx_std_20)
if(WIN32)
    target_link_libraries(Client PRIVATE ws2_32)
endif()