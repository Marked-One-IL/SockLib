cmake_minimum_required(VERSION 3.20)
project(SockLib LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
 "src/Deserializer.cpp"
 "src/Exception.cpp"
 "src/Helper.cpp"
 "src/Serializer.cpp"
 "src/Server.cpp"
 "src/Sock.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()