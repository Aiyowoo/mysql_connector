cmake_minimum_required(VERSION 3.10)
project(mysql_connector)

set(CMAKE_CXX_STANDARD 11)

option(WITH_TEST "generate tests" ON)
option(WITH_DOC "generate documents" ON)

find_package(fmt CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(mysql_connector src/Status.cpp include/Status.h
        src/Connection.cpp include/Connection.h include/DBConfig.h
        src/Statement.cpp include/Statement.h
        src/PreparedStatement.cpp include/PreparedStatement.h
        src/ResultSet.cpp include/ResultSet.h
        src/PreparedResultSet.cpp include/PreparedResultSet.h
        src/ResultMetaData.cpp include/ResultMetaData.h include/Handler.h include/Option.h include/Util.h include/Bind.h test/ConnectionTest.cpp include/ConnectionPool.h)
target_link_libraries(mysql_connector PRIVATE fmt::fmt-header-only)

if (${WITH_TEST})
    add_subdirectory(test)
endif (${WITH_TEST})

if (${WITH_DOC})
    add_subdirectory(doc)
endif (${WITH_DOC})