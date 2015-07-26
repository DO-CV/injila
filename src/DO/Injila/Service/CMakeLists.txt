set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(DO_Sara COMPONENTS Core ImageIO REQUIRED)

include_directories(${Qt5Network_INCLUDE_DIRS})

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

file(GLOB Service_SOURCE_FILES FILES *.cpp *.hpp)
add_library(Service ${Service_SOURCE_FILES})
target_link_libraries(Service
  Qt5::Network Qt5::Widgets
  ${DO_Sara_LIBRARIES}
  FileSystem
)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)


add_subdirectory(test)
