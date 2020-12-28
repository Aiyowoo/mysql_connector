//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_STATUS_H
#define MYSQL_CONNECTOR_STATUS_H

#include <string>

namespace db {

class Status {
public:
    enum StatusCode {
        OK = 0,
        ERROR
    };

public:

    Status(int code);

    Status(int code, const std::string &message);

    Status(const Status &) = default;

    Status &operator=(const Status &) = default;

    Status(Status &&) = default;

    Status &operator=(Status &&) = default;

    ~Status() = default;

    operator bool() const;

    void assign(int code, const std::string &message);

    int code() const;

    const std::string &message() const;

    void clear();

private:
    int code_;
    std::string message_;
};

}

#endif //MYSQL_CONNECTOR_STATUS_H
