find_package(Qt5 COMPONENTS Test REQUIRED)

add_executable(test_service test_service.cpp)
target_link_libraries(test_service
  ${Qt5Test_LIBRARIES}
  gtest
  Service
)
add_test(injila_test_suite test_service)
