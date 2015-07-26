find_package(DO_Sara COMPONENTS Graphics REQUIRED)

add_executable(superpixel_example example.cpp)
set_target_properties(superpixel_example
  PROPERTIES COMPILE_FLAGS ${DO_DEFINITIONS})
target_link_libraries(superpixel_example
  SuperPixel
  ${DO_Sara_LIBRARIES}
)
