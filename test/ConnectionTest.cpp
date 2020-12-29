
#include <gtest/gtest.h>

#include "Connection.h"
#include "Option.h"

using namespace db;

TEST(ConnectionTest, constructor) { Connection connection; }

TEST(ConnectionTest, connectToInvalidAddress) {
    Connection connection;
    ASSERT_FALSE(connection.connected());
    Status s;
    connection.setOption(db::option::ConnectTimeout(1), s);
    ASSERT_TRUE(s);

    // connect to invalid address
    connection.connect("10.12.0.2", 0, "root", "wylj", s);
    ASSERT_FALSE(s);

    // TODO: 选项设置后，Connect之后会 清除
    connection.setOption(db::option::ConnectTimeout(1), s);
    ASSERT_TRUE(s);

    // connect twice
    connection.connect("10.12.0.2", 0, "root", "wylj", s);
    ASSERT_FALSE(s);
}

TEST(ConnectionTest, connect) {
    Connection connection;
    ASSERT_FALSE(connection.connected());

    Status s;
    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(connection.connected());
}

TEST(ConnectionTest, setOption) {
    Connection connection;
    // 设置 connect timeout
    Status s;
    connection.setOption(option::ConnectTimeout(1), s);
    ASSERT_TRUE(s);

    // 链接之前设置 断线自动重连
    connection.setOption(option::AutoReconnect(true), s);
    ASSERT_TRUE(s);

    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);
}

TEST(ConnectionTest, setAutoCommit) {
    Connection connection;

    Status s;
    connection.setAutoCommit(false, s);
    ASSERT_FALSE(s);

    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    connection.setAutoCommit(false, s);
    ASSERT_TRUE(s);

    bool autoCommit = connection.getAutoCommit(s);
    ASSERT_TRUE(s);
    ASSERT_FALSE(autoCommit);

    connection.setAutoCommit(true, s);
    ASSERT_TRUE(s);

    autoCommit = connection.getAutoCommit(s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(autoCommit);
}

TEST(ConnectionTest, checkConnected) {
    Connection connection;
    Status s;

    ASSERT_FALSE(connection.connected());
    ASSERT_FALSE(connection.checkConnected());

    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    ASSERT_TRUE(connection.checkConnected());
}

TEST(ConnectionTest, createStatement) {
    Connection connection;
    Status s;

    Statement statement = connection.createStatement(s);
    ASSERT_FALSE(s);
    ASSERT_FALSE(statement.valid());

    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    Statement newStatement = connection.createStatement(s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(newStatement.valid());
}

TEST(ConnectionTest, prepareStatement) {
    Connection connection;

    Status s;

    PreparedStatement preparedStatement =
        connection.prepareStatement("show databases", s);
    ASSERT_FALSE(s);
    ASSERT_FALSE(preparedStatement.valid());

    connection.connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    preparedStatement = connection.prepareStatement("show databases", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(preparedStatement.valid());
}
