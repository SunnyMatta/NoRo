cmake_minimum_required(VERSION 3.16)

project(High-Definition-3DRenderer LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(headers/)

add_executable(High-Definition-3DRenderer src/main.c)

target_link_libraries(High-Definition-3DRenderer PRIVATE GL glfw m GLEW ktx)

include(GNUInstallDirs)
install(TARGETS High-Definition-3DRenderer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
