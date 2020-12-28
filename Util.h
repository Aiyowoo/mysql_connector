//
// Created by m8792 on 2020/12/24.
//

#ifndef MYSQL_CONNECTOR_UTIL_H
#define MYSQL_CONNECTOR_UTIL_H


#include "Handler.h"
#include <mysql/mysql.h>
#include <string>

namespace db {

inline std::string getLastError(const ConnectionHandler &handler) {
    if(!handler.valid()) {
        return "";
    }

    return std::to_string(mysql_errno(handler.get())) + ':' + mysql_error(handler.get());
}

inline std::string getLastError(const StatementHandler &handler) {
    if(!handler.valid()) {
        return "";
    }

    return std::to_string(mysql_stmt_errno(handler.get())) + ':' + mysql_stmt_error(handler.get());
}
}

#endif //MYSQL_CONNECTOR_UTIL_H
