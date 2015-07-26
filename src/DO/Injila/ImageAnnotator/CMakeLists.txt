project(ImageAnnotator)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Network OpenGL Widgets REQUIRED)
find_package(DO_Sara COMPONENTS ImageIO ImageProcessing REQUIRED)

include_directories(${Qt5Network_INCLUDE_DIRS})

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


file(GLOB_RECURSE ImageAnnotator_SRC_FILES FILES *.hpp *.cpp)
add_executable(ImageAnnotator ${ImageAnnotator_SRC_FILES})
target_link_libraries(ImageAnnotator
  Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Network
  ${DO_Sara_LIBRARIES}
  FileSystem
  Service
  SuperPixel
)
