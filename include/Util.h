//
// Created by m8792 on 2020/12/24.
//

#ifndef MYSQL_CONNECTOR_UTIL_H
#define MYSQL_CONNECTOR_UTIL_H

#include <mysql/mysql.h>

#include <string>

#include "Handler.h"

namespace db {

inline std::string getLastError(const ConnectionHandler& handler) {
    if (!handler.valid()) {
        return "";
    }

    return std::to_string(mysql_errno(handler.get())) + ':' +
           mysql_error(handler.get());
}

inline std::string getLastError(MYSQL* mysql) {
    if (mysql == nullptr) {
        return "";
    }

    return std::to_string(mysql_errno(mysql)) + ':' + mysql_error(mysql);
}

inline std::string getLastError(const StatementHandler& handler) {
    if (!handler.valid()) {
        return "";
    }

    return std::to_string(mysql_stmt_errno(handler.get())) + ':' +
           mysql_stmt_error(handler.get());
}

inline std::string getLastError(MYSQL_STMT* stmt) {
    if (stmt == nullptr) {
        return "";
    }

    return std::to_string(mysql_stmt_errno(stmt)) + ':' +
           mysql_stmt_error(stmt);
}

}  // namespace db

#endif  // MYSQL_CONNECTOR_UTIL_H
