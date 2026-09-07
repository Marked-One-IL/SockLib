# Status
Support windows only at the moment.<br>

# CMake
Before creating target.
- Make sure you have the directory 'third_party' in your project root.
```cmake
add_subdirectory("third_party/SockLib")
```
After creating target.<br>
```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE SockLib)
```
