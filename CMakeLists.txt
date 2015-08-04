cmake_minimum_required(VERSION 2.8)

project(Injila)


if (WIN32)
  set(DO_Sara_DIR "C:/Program Files/DO-Sara/include/DO/Sara")
else ()
  set(DO_Sara_DIR /usr/include/DO/Sara)
endif ()


list(APPEND CMAKE_MODULE_PATH ${DO_Sara_DIR}/cmake)

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(DO_Sara REQUIRED)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${CUDA_TOOLKIT_INCLUDE}
  ${DO_Sara_INCLUDE_DIRS}
  src
  third_party
  third_party/json/src
  third_party/gtest/include
)

link_directories(${Boost_LIBRARY_DIR})


# Third-party libraries.
add_subdirectory(third_party)

# Compile libraries and programs.
add_subdirectory(src)
