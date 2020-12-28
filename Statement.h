//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_STATEMENT_H
#define MYSQL_CONNECTOR_STATEMENT_H

#include "ResultSet.h"

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

  Statement& operator=(Statement&&) = default;

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
   * 是否是有效的
   * @return
   */
  bool valid() const;

 private:
  /**
   * mysql链接
   */
  Connection& conn_;
};

}  // namespace db

#endif  // MYSQL_CONNECTOR_STATEMENT_H
