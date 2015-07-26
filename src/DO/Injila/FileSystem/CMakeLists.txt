file(GLOB FileSystem_SOURCE_FILES FILES *.cpp *.hpp)
add_library(FileSystem ${FileSystem_SOURCE_FILES})
target_link_libraries(FileSystem
  ${Boost_LIBRARIES}
)
