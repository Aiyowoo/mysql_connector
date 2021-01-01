
# find_package(Catch2 CONFIG REQUIRED)
# target_link_libraries(main PRIVATE Catch2::Catch2)


find_package(GTest CONFIG REQUIRED)
# target_link_libraries(main PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

add_executable(db_test
        ConnectionTest.cpp StatementTest.cpp PreparedStatementTest.cpp ConnectionPoolTest.cpp)
target_link_libraries(db_test PRIVATE GTest::gtest GTest::gtest_main mysql_connector mysqlclient pthread)