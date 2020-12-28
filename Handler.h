//
// Created by m8792 on 2020/12/24.
//

#ifndef MYSQL_CONNECTOR_HANDLER_H
#define MYSQL_CONNECTOR_HANDLER_H

#include <mysql/mysql.h>

#include <utility>

/**
 * 包裹的 各种handler
 */

namespace db {

/**
 * MYSQL* 的包裹类
 */
class ConnectionHandler {
public:

    ConnectionHandler(MYSQL *mysql = nullptr): mysql_(mysql) {}

    ConnectionHandler(const ConnectionHandler&) = delete;

    ConnectionHandler& operator=(const ConnectionHandler&) = delete;

    ConnectionHandler(ConnectionHandler &&other) {
        mysql_ = other.mysql_;
        other.mysql_ = nullptr;
    }

    ConnectionHandler& operator=(ConnectionHandler &&other) {
        ConnectionHandler tmp;
        this->swap(other);
        other.swap(tmp);
    }

    ~ConnectionHandler() {
        close();
    }

    void assign(MYSQL *mysql) {
        ConnectionHandler tmp(mysql);
        this->swap(tmp);
    }

    void close() {
        if(mysql_) {
            mysql_close(mysql_);
            mysql_ = nullptr;
        }
    }

    void swap(ConnectionHandler &other) {
        using std::swap;
        swap(mysql_, other.mysql_);
    }

    MYSQL* get() const {
        return mysql_;
    }

    bool valid() const {
        return mysql_ != nullptr;
    }

private:
    MYSQL *mysql_;
};

inline void swap(ConnectionHandler &lhs, ConnectionHandler &rhs) {
    lhs.swap(rhs);
}

/**
 * MYSQL_STMT* 的包裹类
 */
class StatementHandler {
public:

    StatementHandler(MYSQL_STMT *stmt = nullptr): stmt_(stmt) {}

    StatementHandler(const StatementHandler &) = delete;

    StatementHandler& operator=(const StatementHandler &) = delete;

    StatementHandler(StatementHandler &&other) {
        stmt_ = other.stmt_;
        other.stmt_ = nullptr;
    }

    StatementHandler& operator=(StatementHandler &&other) {
        StatementHandler tmp;
        this->swap(other);
        other.swap(tmp);
    }

    ~StatementHandler() {
        close();
    }

    void close() {
        if(stmt_) {
            mysql_stmt_close(stmt_);
            stmt_ = nullptr;
        }
    }

    void swap(StatementHandler &other) {
        using std::swap;
        swap(stmt_, other.stmt_);
    }

    MYSQL_STMT* get() const {
        return stmt_;
    }

    bool valid() const {
        return stmt_ != nullptr;
    }

private:
    MYSQL_STMT *stmt_;
};

inline void swap(StatementHandler &lhs, StatementHandler &rhs) {
    lhs.swap(rhs);
}

/**
 * MYSQL_RES* 的包裹类
 */
class ResultSetHandler {
public:
    ResultSetHandler(MYSQL_RES *res = nullptr): res_(res) {}

    ResultSetHandler(const ResultSetHandler &) = delete;

    ResultSetHandler& operator=(const ResultSetHandler&) = delete;

    ResultSetHandler(ResultSetHandler &&other) {
        res_ = other.res_;
        other.res_ = nullptr;
    }

    ResultSetHandler& operator=(ResultSetHandler &&other) {
        ResultSetHandler tmp;
        this->swap(other);
        other.swap(tmp);
    }

    ~ResultSetHandler() {
        close();
    }

    void close() {
        if(res_) {
            mysql_free_result(res_);
            res_ = nullptr;
        }
    }

    void swap(ResultSetHandler &other) {
        using std::swap;
        swap(res_, other.res_);
    }

    MYSQL_RES* get() {
        return res_;
    }

    void assign(MYSQL_RES *res) {
        close();
        res_ = res;
    }

    bool valid() const {
        return res_ != nullptr;
    }

private:
    MYSQL_RES *res_;
};

inline void swap(ResultSetHandler &lhs, ResultSetHandler &rhs) {
    lhs.swap(rhs);
}

}


#endif //MYSQL_CONNECTOR_HANDLER_H
