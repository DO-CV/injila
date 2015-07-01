cmake_minimum_required(VERSION 2.6)

project(ImageAnnotator)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(DO_Sara COMPONENTS ImageIO ImageProcessing REQUIRED)

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


file(GLOB ImageAnnotator_SRC_FILES FILES *.hpp *.cpp)
add_executable(ImageAnnotator ${ImageAnnotator_SRC_FILES})
target_link_libraries(
  ImageAnnotator
  Qt5::Core Qt5::Widgets Qt5::OpenGL
  ${DO_Sara_LIBRARIES}
  SuperPixel)
