cmake_minimum_required(VERSION 2.8)

project(Injila)


set(DO_USE_STATIC_LIBS 1)
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
  third_party
)

link_directories(${Boost_LIBRARY_DIR})


# Third-party libraries.
add_subdirectory(third_party)

# Libraries.
add_subdirectory(kmeans)
add_subdirectory(superpixel)

# Programs.
add_subdirectory(image_annotator)
