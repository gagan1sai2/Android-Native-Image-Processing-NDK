cmake_minimum_required(VERSION 3.22.1)

project(imageprocessor)

add_library(imageprocessor SHARED
        native-lib.cpp)

find_library(log-lib log)
find_library(android-lib android)
find_library(jnigraphics-lib jnigraphics)

target_link_libraries(imageprocessor
        ${android-lib}
        ${log-lib}
        ${jnigraphics-lib})
