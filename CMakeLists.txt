# cmake_minimum_required(VERSION <specify CMake version here>)
project(mysql_connector)

set(CMAKE_CXX_STANDARD 11)

add_library(mysql_connector src/Status.cpp include/Status.h
        src/Connection.cpp include/Connection.h include/DBConfig.h
        src/Statement.cpp include/Statement.h
        src/PreparedStatement.cpp include/PreparedStatement.h
        src/ResultSet.cpp include/ResultSet.h
        src/PreparedResultSet.cpp include/PreparedResultSet.h
        src/ResultMetaData.cpp include/ResultMetaData.h include/Handler.h include/Option.h include/Util.h include/Bind.h)