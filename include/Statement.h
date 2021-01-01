//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_STATEMENT_H
#define MYSQL_CONNECTOR_STATEMENT_H

#include <fmt/printf.h>

#include "ResultSet.h"
#include "Status.h"
#include "Util.h"

namespace db {

class Connection;

/**
 * 普通的Statement，能用来执行SQL请求
 */
class Statement {
public:
    Statement(Connection& conn) : conn_(conn) {}

    Statement(const Statement&) = delete;

    Statement& operator=(const Statement&) = delete;

    Statement(Statement&&) = default;

    Statement& operator=(Statement&&) = delete;

    /**
     * 是否是可用的
     * @return
     */
    operator bool() const { return valid(); }

    /**
     * 执行select sql，返回结果集
     * @param sql       select语句
     * @param s
     * @return
     */
    ResultSet executeQuery(const std::string& sql, Status& s);

    /**
     * 执行update/delete语句，返回受到影响的行数
     * @param sql
     * @param s
     * @return
     */
    int executeUpdate(const std::string& sql, Status& s);

    /**
     * 执行sql语句
     * @param sql
     * @param s
     */
    void execute(const std::string& sql, Status& s);

    /**
     * 获取上一个insert语句，生成的自增ID
     * @return
     */
    int64_t getLastInsertId(Status& s);

    /**
     * 获取上一次insert语句，生成的自增ID
     * @return
     * @throws 报错会抛异常 std::runtime_error
     */
    int64_t getLastInsertId();

    /**
     * 获取收到影响的行数
     * @return      返回上次执行update/delete sql语句后，收到影响的行
     * @throws      如果获取失败，会抛出异常
     * @note        可以调用多次，都是最后一次执行的结果
     */
    int64_t getAffectedRowCount();

    /**
     * 获取收到影响的行数
     * @param s     报错信息
     * @return      返回上次执行update/delete sql语句后，收到影响的行
     * @note        出错不会抛出异常
     */
    int64_t getAffectedRowCount(Status& s);

    /**
     * 是否是有效的
     * @return
     */
    bool valid() const;

private:
    void checkValid(Status& s) const;

private:
    /**
     * mysql链接
     */
    Connection& conn_;
};

}  // namespace db

#endif  // MYSQL_CONNECTOR_STATEMENT_H
