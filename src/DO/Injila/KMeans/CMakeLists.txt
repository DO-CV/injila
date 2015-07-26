find_package(DO_Sara COMPONENTS Core REQUIRED)

file(GLOB KMeans_SOURCE_FILES FILES *.hpp *.cpp)
add_library(KMeans ${KMeans_SOURCE_FILES})
