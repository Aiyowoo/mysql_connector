//
// Created by m8792 on 2020/12/17.
//

#include "Statement.h"

#include "Connection.h"

namespace db {

ResultSet Statement::executeQuery(const std::string& sql, Status& s) {
    s.clear();
    checkValid(s);
    if (!s) {
        return ResultSet();
    }

    if (mysql_real_query(conn_.get(), sql.c_str(), sql.size()) != 0) {
        s.assign(
            Status::RUNTIME_ERROR,
            fmt::sprintf("execute sql failed, %s", getLastError(conn_.get())));
        return ResultSet();
    }

    MYSQL_RES* res = mysql_store_result(conn_.get());
    if (res == nullptr) {
        s.assign(Status::RUNTIME_ERROR,
                 fmt::sprintf("get query result failed, %s",
                              getLastError(conn_.get())));
        return ResultSet();
    }

    return ResultSet(res);
}

int Statement::executeUpdate(const std::string& sql, Status& s) {
    s.clear();

    checkValid(s);
    if (!s) {
        return -1;
    }

    if (mysql_real_query(conn_.get(), sql.c_str(), sql.size()) != 0) {
        s.assign(
            Status::RUNTIME_ERROR,
            fmt::sprintf("execute sql failed, %s", getLastError(conn_.get())));
        return -1;
    }

    return getAffectedRowCount(s);
}

void Statement::execute(const std::string& sql, Status& s) {
    s.clear();

    checkValid(s);
    if (!s) {
        return;
    }

    if (mysql_real_query(conn_.get(), sql.c_str(), sql.size()) != 0) {
        s.assign(
            Status::RUNTIME_ERROR,
            fmt::sprintf("execute sql failed, %s", getLastError(conn_.get())));
        return;
    }
}

int64_t Statement::getLastInsertId(Status& s) {
    s.clear();

    checkValid(s);
    if (!s) {
        return -1;
    }

    return mysql_insert_id(conn_.get());
}

int64_t Statement::getLastInsertId() {
    Status s;
    int64_t lastInsertId = getLastInsertId(s);
    if (!s) {
        throw std::runtime_error(s.message());
    }
    return lastInsertId;
}

int64_t Statement::getAffectedRowCount() {
    Status s;
    int64_t affectedRowCount = getAffectedRowCount(s);
    if (!s) {
        throw std::runtime_error(s.message());
    }
    return affectedRowCount;
}

int64_t Statement::getAffectedRowCount(Status& s) {
    s.clear();

    checkValid(s);
    if (!s) {
        return -1;
    }

    return mysql_affected_rows(conn_.get());
}

bool Statement::valid() const { return conn_.connected(); }

void Statement::checkValid(Status& s) const {
    s.clear();
    if (!valid()) {
        s.assign(Status::RUNTIME_ERROR, "connection is invalid");
    }
}

}  // namespace db
