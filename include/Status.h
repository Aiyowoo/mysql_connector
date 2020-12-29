//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_STATUS_H
#define MYSQL_CONNECTOR_STATUS_H

#include <string>

namespace db {

/**
 * 报错信息
 */
class Status {
public:
    enum StatusCode { OK = 0, ERROR, RUNTIME_ERROR, UNKNOWN_ERROR };

public:
    Status();

    explicit Status(int code);

    Status(int code, const std::string& message);

    Status(const Status&) = default;

    Status& operator=(const Status&) = default;

    Status(Status&&) = default;

    Status& operator=(Status&&) = default;

    ~Status() = default;

    operator bool() const;

    void assign(int code, const std::string& message);

    int code() const;

    const std::string& message() const;

    void clear();

private:
    /**
     * 返回码
     */
    int code_;

    /**
     * 错误信息
     */
    std::string message_;
};

}  // namespace db

#endif  // MYSQL_CONNECTOR_STATUS_H
