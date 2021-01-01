//
// Created by m8792 on 2020/12/31.
//

#include <gtest/gtest.h>

#include "Connection.h"
#include "Option.h"
#include "PreparedStatement.h"

using namespace db;

class ValidPreparedStatement : public testing::Test {
public:
    void SetUp() override {
        Status s;
        connection_.setOption(option::ConnectTimeout(1), s);
        ASSERT_TRUE(s);
        connection_.setOption(option::AutoReconnect(true), s);
        ASSERT_TRUE(s);

        connection_.connect("127.0.0.1", 0, "root", "wylj",
                            "mysql_connector_test", s);
        ASSERT_TRUE(s);
    }
    void TearDown() override {}

    Connection connection_;
};

TEST_F(ValidPreparedStatement, execute) {
    Status s;
    PreparedStatement statement =
        connection_.prepareStatement("select * from t_person where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    statement.bind(1, s);
    ASSERT_TRUE(s) << s.message() << std::endl;

    statement.execute(s);
    ASSERT_TRUE(s);

    PreparedResultSet resultSet = statement.getResultSet(s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(resultSet.valid());
}

TEST_F(ValidPreparedStatement, PreparedResultSet) {
    Status s;
    PreparedStatement statement =
        connection_.prepareStatement("select * from t_person where id >= ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    statement.bind(1, s);
    ASSERT_TRUE(s) << s.message() << std::endl;

    statement.execute(s);
    ASSERT_TRUE(s);

    PreparedResultSet resultSet = statement.getResultSet(s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(resultSet.valid());

    ResultMetaData metaData = resultSet.getMetaData();
    for (int i = 0; i < metaData.getFieldCount(); ++i) {
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << metaData.getFieldName(i);
    }
    std::cout << std::endl;

    while (resultSet.next()) {
        for (int i = 0; i < metaData.getFieldCount(); ++i) {
            if (i != 0) {
                std::cout << ",";
            }
            std::cout << resultSet.getString(i);
        }
        std::cout << std::endl;
    }
}

TEST_F(ValidPreparedStatement, getAffectedRowCount) {
    Status s;
    PreparedStatement statement = connection_.prepareStatement(
        "update t_person set name = ? where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    std::string name = fmt::sprintf("%ld", time(nullptr));
    statement.bind(name, 1, s);
    ASSERT_TRUE(s);

    statement.execute(s);
    ASSERT_TRUE(s);

    int affectedRowCount = statement.getAffectedRowCount();
    EXPECT_EQ(1, affectedRowCount);

    int affectedRowCount2 = statement.getAffectedRowCount();
    EXPECT_EQ(affectedRowCount, affectedRowCount2);
}

TEST_F(ValidPreparedStatement, dateTimeAsParameter) {
    Status s;
    PreparedStatement statement = connection_.prepareStatement(
        "update t_person set birthday = ? where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    statement.bind("2020-11-12", 1, s);
    ASSERT_TRUE(s);

    statement.execute(s);
    ASSERT_TRUE(s);

    PreparedStatement queryStatement = connection_.prepareStatement(
        "select birthday from t_person where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(queryStatement.valid());

    queryStatement.bind(1, s);
    ASSERT_TRUE(s);

    queryStatement.execute(s);
    ASSERT_TRUE(s);

    ASSERT_EQ(-1, queryStatement.getAffectedRowCount());

    PreparedResultSet resultSet = queryStatement.getResultSet(s);
    ASSERT_TRUE(resultSet.next());
    ASSERT_EQ("2020-11-12", resultSet.getString("birthday"));
}

TEST_F(ValidPreparedStatement, forgetInvokeNext) {
    Status s;
    PreparedStatement statement = connection_.prepareStatement(
        "update t_person set birthday = ? where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    statement.bind("2020-11-12", 1, s);
    ASSERT_TRUE(s);

    statement.execute(s);
    ASSERT_TRUE(s);

    PreparedStatement queryStatement = connection_.prepareStatement(
        "select birthday from t_person where id = ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(queryStatement.valid());

    queryStatement.bind(1, s);
    ASSERT_TRUE(s);

    queryStatement.execute(s);
    ASSERT_TRUE(s);

    ASSERT_EQ(-1, queryStatement.getAffectedRowCount());

    PreparedResultSet resultSet = queryStatement.getResultSet(s);
    ASSERT_THROW(resultSet.getString("birthday"), std::runtime_error);
}

TEST_F(ValidPreparedStatement, rewind) {
    Status s;
    PreparedStatement statement =
        connection_.prepareStatement("select * from t_person where id >= ?", s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(statement.valid());

    statement.bind(1, s);
    ASSERT_TRUE(s) << s.message() << std::endl;

    statement.execute(s);
    ASSERT_TRUE(s);

    PreparedResultSet resultSet = statement.getResultSet(s);
    ASSERT_TRUE(s);
    ASSERT_TRUE(resultSet.valid());

    int originalRowCount = 0;
    while (resultSet.next()) {
        ++originalRowCount;
    }

    resultSet.rewind();
    int count = 0;
    while (resultSet.next()) {
        ++count;
    }
    ASSERT_EQ(count, originalRowCount);
}