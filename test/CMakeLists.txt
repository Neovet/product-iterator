include_directories(${GTEST_INCLUDE_DIRS})

add_executable(iterator_test EXCLUDE_FROM_ALL
  iterator.cpp
)

target_link_libraries(iterator_test ${GTEST_BOTH_LIBRARIES})

GTEST_ADD_TESTS(iterator_test "" iterator.cpp)
