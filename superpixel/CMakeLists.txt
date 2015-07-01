cmake_minimum_required(VERSION 2.6)

project (EntropyRateSuperpixel)

find_package(DO_Sara COMPONENTS Graphics REQUIRED)

add_library(SuperPixel ERSInterface.hpp ERSInterface.cpp)
target_link_libraries(SuperPixel ERS ${DO_Sara_LIBRARIES})

add_executable(SuperPixelTest main.cpp)
set_target_properties(SuperPixelTest PROPERTIES COMPILE_FLAGS ${DO_DEFINITIONS})
target_link_libraries(
  SuperPixelTest
  SuperPixel
  ${DO_Sara_LIBRARIES}
)
