cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(visibility)

add_executable(visibility visibility.cpp)
target_link_libraries(visibility visualization ${GMP_LIBRARIES})

add_executable(minkowski minkowski.cpp)
target_link_libraries(minkowski visualization ${GMP_LIBRARIES})
