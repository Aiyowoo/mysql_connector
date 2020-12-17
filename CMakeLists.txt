# cmake_minimum_required(VERSION <specify CMake version here>)
project(mysql_connector)

set(CMAKE_CXX_STANDARD 11)

add_library(mysql_connector library.cpp library.h Status.cpp Status.h Connection.cpp Connection.h DBConfig.h Statement.cpp Statement.h PreparedStatement.cpp PreparedStatement.h)