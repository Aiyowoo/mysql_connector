//
// Created by m8792 on 2021/1/1.
//

#include <gtest/gtest.h>

#include <thread>

#include "ConnectionPool.h"

using namespace db;

TEST(ConnectionPoolTest, Constructor) { ConnectionPool pool(10); }

TEST(ConnectionPoolTest, connect) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(10);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);
}

TEST(ConnectionPoolTest, getConnection) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(10);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    ASSERT_EQ(10, pool->getConnectionCount());
    ConnectionPtr ptr = pool->getConnection();
    ASSERT_TRUE(bool(ptr));
}

TEST(ConnectionPoolTest, resize) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(10);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    ASSERT_EQ(10, pool->getConnectionCount());
    ConnectionPtr ptr = pool->getConnection();
    ASSERT_TRUE(bool(ptr));

    pool->resize(12);
    ASSERT_EQ(12, pool->getConnectionCount());

    pool->resize(9);
    ASSERT_EQ(9, pool->getConnectionCount());
}

TEST(ConnectionPoolTest, waitConnection) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(1);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    ASSERT_EQ(1, pool->getConnectionCount());
    ConnectionPtr ptr = pool->getConnection();
    ASSERT_TRUE(bool(ptr));

    ConnectionPtr conn = pool->getConnection(2000);
    ASSERT_FALSE(bool(conn));
}

TEST(ConnectionPoolTest, revokeConnection) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(1);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    std::thread([pool]() {
        ASSERT_EQ(1, pool->getConnectionCount());
        ConnectionPtr ptr = pool->getConnection();
        ASSERT_TRUE(bool(ptr));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }).detach();

    ConnectionPtr conn = pool->getConnection();
    ASSERT_TRUE(bool(conn));
}

TEST(ConnectionPoolTest, resizeAndWait) {
    ConnectionPoolPtr pool = std::make_shared<ConnectionPool>(1);
    Status s;
    pool->connect("127.0.0.1", 0, "root", "wylj", s);
    ASSERT_TRUE(s);

    ConnectionPtr ptr = pool->getConnection();
    ASSERT_TRUE(bool(ptr));

    std::thread([pool]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        pool->resize(2);
    }).detach();

    ConnectionPtr conn = pool->getConnection();
    ASSERT_TRUE(bool(conn));
}