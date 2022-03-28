cmake_minimum_required(VERSION 3.21)
project(mnogochlen)

## If you want to link SFML statically
set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
set(SFML_DIR "SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(mnogochlen main.cpp Node.cpp Node.h Node.cpp Node.h Term.cpp DefineConstants.h WindowManager.cpp WindowManager.h Functions.cpp Functions.h Button.cpp Button.h)
target_link_libraries(mnogochlen sfml-graphics sfml-audio)