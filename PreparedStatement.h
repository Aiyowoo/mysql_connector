//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
#define MYSQL_CONNECTOR_PREPAREDSTATEMENT_H

#include "Handler.h"
#include "Bind.h"
#include "Status.h"
#include "PreparedResultSet.h"
#include <utility>
#include <mysql/mysql.h>

namespace db {

/**
 * PreparedStatement类
 *
 * @warning 使用时需要保证Connection存活
 */
class PreparedStatement {
public:

    PreparedStatement(MYSQL_STMT *stmt = nullptr): stmt_(stmt) {}

    PreparedStatement(const PreparedStatement &) = delete;

    PreparedStatement &operator=(const PreparedStatement &) = delete;

    PreparedStatement(PreparedStatement &&other): stmt_(std::move(other.stmt_)) {
    }

    PreparedStatement &operator=(PreparedStatement &&other) {
        PreparedStatement tmp;
        swap(other);
        other.swap(tmp);
    }

    ~PreparedStatement() {
        close();
    }

    void swap(PreparedStatement &other) {
        using std::swap;
        swap(stmt_, other.stmt_);
    }

    /**
     * 绑定输入参数
     * @tparam Args     输入参数类型，暂时只支持 string, Integer
     * @param args 最后一个参数如果是status，则报错信息放在status中，否则抛异常
     * @return
     * @throws 会抛异常
     */
    template<typename ...Args>
    void bind(Args&& ...args) {
        checkValid();

        size_t paramCount = mysql_stmt_param_count(stmt_.get());
        params_.assign(paramCount);

        bindParams(0, std::forward<Args>(args)...);
    }

    /**
     * 执行sql
     * @param s
     */
    void execute(Status &s) {
        s.clear();

        checkValid(s);
        if(!s) {
            return;
        }

        size_t expectedParamCount = mysql_stmt_param_count(stmt_.get());
        if(expectedParamCount != params_.getBindCount()) {
            s.assign(Status::ERROR, "params not bind");
            return;
        }

        if(mysql_stmt_execute(stmt_.get()) != 0) {
            s.assign(Status::ERROR, "statement execute failed");
            return;
        }
    }

    /**
     * 获取execute执行后，影响到的行数
     * @return
     */
    int64_t getAffectedRowsCount() {
        checkValid();
        return mysql_stmt_affected_rows(stmt_.get());
    }

    /**
     * 获取执行select语句后的ResultSet
     * @return
     */
    PreparedResultSet getResultSet(Status &s) {
        s.clear();

        checkValid(s);
        if(!s) {
            return PreparedResultSet();
        }

        return PreparedResultSet(stmt_.get());
    }

    /**
     * 是否是有效的
     * @return
     */
    bool valid() const {
        return stmt_.valid();
    }

    /**
     * 释放资源，并不会关闭Connection
     */
    void close() {
        stmt_.close();
    }

private:
    void checkValid() const {
        if(!valid()) {
            throw std::runtime_error("statement is invalid");
        }
    }

    void checkValid(Status &s) const {
        s.clear();
        if(!valid()) {
            s.assign(Status::ERROR, "statement is invalid");
        }
    }

    template<typename T, typename ...Args>
    void bindParams(int index, T &&val, Args&& ...args) {
        params_.setValue(index, std::forward<T>(val));

        bindParams(index + 1, std::forward<Args>(args)...);
    }

    template<typename T>
    void bindParams(int index, T &&val) {
        params_.setValue(index, std::forward<T>(val));
        if(index + 1 != params_.getBindCount()) {
            throw std::invalid_argument("params count not invalid");
        }

        if(!mysql_stmt_bind_param(stmt_.get(), params_.getBinds())) {
            throw std::runtime_error("failed to bind params");
        }
    }

    template<typename T>
    void bindParams(int index, Status &s) {
        s.clear();

        if(index != params_.getBindCount()) {
            s.assign(Status::ERROR, "params count not invalid");
            return;
        }

        if(!mysql_stmt_bind_param(stmt_.get(), params_.getBinds())) {
            throw std::runtime_error("failed to bind params");
        }
    }

private:

    StatementHandler stmt_;

    Bind params_;
};

}


#endif //MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
