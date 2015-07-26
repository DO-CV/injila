project (Superpixel)

find_package(DO_Sara COMPONENTS Graphics REQUIRED)

file(GLOB Superpixel_SOURCE_FILES FILES *.hpp *.cpp)
add_library(SuperPixel ${Superpixel_SOURCE_FILES})
target_link_libraries(SuperPixel ERS ${DO_Sara_LIBRARIES})

add_subdirectory(example)
