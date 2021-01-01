//
// Created by m8792 on 2020/12/29.
//

#include <gtest/gtest.h>

#include "Connection.h"
#include "Option.h"
#include "Statement.h"

using namespace db;

class InvalidStatementTest : public testing::Test {
public:
    void SetUp() override {
        Status s;
        conn_.setOption(db::option::ConnectTimeout(1), s);
        EXPECT_TRUE(s);
        conn_.connect("10.100.2.194", 0, "root", "???", s);
        ASSERT_FALSE(conn_.connected());

        statement_ = new Statement(conn_.createStatement(s));
        ASSERT_NE(statement_, nullptr);

        ASSERT_FALSE(s);
    }

    void TearDown() override {
        if (statement_) {
            delete statement_;
            statement_ = nullptr;
        }
    }

    Connection conn_;

    Statement* statement_;
};

TEST_F(InvalidStatementTest, valid) {
    ASSERT_NE(statement_, nullptr);
    ASSERT_FALSE(statement_->valid());
}

TEST_F(InvalidStatementTest, operatorBool) {
    ASSERT_NE(statement_, nullptr);
    ASSERT_FALSE(bool(*statement_));
}

TEST_F(InvalidStatementTest, executeQuery) {
    ASSERT_NE(statement_, nullptr);
    Status s;
    ResultSet resultSet = statement_->executeQuery("show databases", s);
    ASSERT_FALSE(s);
    ASSERT_FALSE(resultSet.valid());
    ASSERT_FALSE(resultSet.next());
}

TEST_F(InvalidStatementTest, executeUpdate) {
    ASSERT_NE(statement_, nullptr);
    Status s;

    int affectedRowCount = statement_->executeUpdate("show databases", s);
    ASSERT_FALSE(s);
    ASSERT_LE(affectedRowCount, 0);
}

TEST_F(InvalidStatementTest, execute) {
    ASSERT_NE(statement_, nullptr);
    Status s;
    statement_->execute("show databases", s);
    ASSERT_FALSE(s);
}

class ValidStatementTest : public testing::Test {
protected:
    void SetUp() override {
        Status s;
        conn_.setOption(option::AutoReconnect(true), s);
        ASSERT_TRUE(s);
        conn_.setOption(option::ConnectTimeout(1), s);
        ASSERT_TRUE(s);

        conn_.connect("127.0.0.1", 0, "root", "wylj", "mysql_connector_test",
                      s);
        ASSERT_TRUE(s);

        statement_ = new Statement(conn_.createStatement(s));
        ASSERT_TRUE(s);
        ASSERT_TRUE(statement_->valid());
    }
    void TearDown() override {
        if (statement_) {
            delete statement_;
            statement_ = nullptr;
        }
    }

    Connection conn_;

    Statement* statement_;
};

TEST_F(ValidStatementTest, valid) { ASSERT_TRUE(statement_->valid()); }

TEST_F(ValidStatementTest, executeQuery) {
    Status s;
    ResultSet resultSet = statement_->executeQuery("show databases", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(resultSet.valid());
    ResultMetaData metaData = resultSet.getResultMetaData(s);
    ASSERT_TRUE(s && metaData.isValid());
    //    for(int i = 0; i < metaData.getFieldCount(); ++i) {
    //        if(i != 0) {
    //            std::cout << ", ";
    //        }
    //        std::cout << metaData.getFieldName(i);
    //    }
    //    std::cout << std::endl;
    //
    //    while(resultSet.next()) {
    //        for(int i = 0; i < metaData.getFieldCount(); ++i) {
    //            if(i != 0) {
    //                std::cout << ", ";
    //            }
    //            std::cout << resultSet.getString(i);
    //        }
    //        std::cout << std::endl;
    //    }
}

TEST_F(ValidStatementTest, executeUpdate) {
    Status s;
    int affectedRowCount = statement_->executeUpdate(
        "update t_person set name = 'woo' where id = 1", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(affectedRowCount == 1);
}

TEST_F(ValidStatementTest, execute) {
    Status s;
    statement_->execute("update t_person set name = 'nice1' where id = 2", s);

    statement_->execute("update t_person set name = 'nice' where id = 2", s);
    ASSERT_TRUE(s);

    int affectedRowCount = statement_->getAffectedRowCount();
    ASSERT_TRUE(affectedRowCount == 1);

    affectedRowCount = statement_->getAffectedRowCount();
    ASSERT_EQ(1, affectedRowCount);
}

TEST_F(ValidStatementTest, getLastInsertId) {
    Status s;
    statement_->execute(
        "insert into t_person(name, birthday, gender) values('yoyo', "
        "'2020-01-01', '0');",
        s);
    int64_t lastInsertId = statement_->getLastInsertId();
    ASSERT_TRUE(s) << "last Insert Id : " << lastInsertId;
    int64_t lastInsertId2 = statement_->getLastInsertId();
    ASSERT_EQ(lastInsertId2, lastInsertId);
}
