//
// Created by m8792 on 2020/12/17.
//

#include "Connection.h"
#include "Statement.h"
#include "Status.h"
#include "Util.h"

namespace db {

Connection::Connection(): connected_(false) {
    initializeHandler();
}

Connection::Connection(Connection &&other): conn_(std::move(other.conn_)), connected_(other.connected_) {
    other.connected_ = false;
}

Connection &Connection::operator=(Connection &&other) {
    conn_ = std::move(other.conn_);
    connected_ = other.connected_;
    other.connected_ = false;
}

Connection::~Connection() {
    close();
}

void
Connection::connect(const std::string &host, unsigned short port,
                    const std::string &user, const std::string &password,
                    Status &s) {
    s.clear();

    if(connected()) {
        s.assign(Status::ERROR, "already connected");
        return;
    }

    if(!conn_.valid()) {
        s.assign(Status::ERROR, "connection is invalid");
        return;
    }

    if(nullptr == mysql_real_connect(conn_.get(), host.c_str(), user.c_str(),
                                     password.c_str(), nullptr, port, nullptr, 0)) {
        s.assign(Status::ERROR, getLastError(conn_));
        return;
    }

    setAutoCommit(true, s);
}

void Connection::close() {
    conn_.close();
    connected_ = false;
}

bool Connection::connected() const {
    return connected_;
}

bool Connection::checkConnected() {
    if(!conn_.valid() || !connected()) {
        return false;
    }

    return mysql_ping(conn_.get()) == 0;
}

Statement Connection::createStatement(Status &s) {
    if(!connected()) {
        s.assign(Status::ERROR, "not connected");
        return Statement(*this);
    }

    return Statement(*this);
}

PreparedStatement Connection::prepareStatement(const std::string &sql, Status &s) {
    if(!connected()) {
        s.assign(Status::ERROR, "not connected");
        return PreparedStatement();
    }

    PreparedStatement stmt(mysql_stmt_init(conn_.get()));
    if(!stmt.valid()) {
        s.assign(Status::ERROR, getLastError(conn_.get()));
        return stmt;
    }
    return stmt;
}

void Connection::selectSchema(const std::string &schema, Status &s) {
    s.clear();

    if(!connected()) {
        s.assign(Status::ERROR, "not connected");
        return;
    }

    if(mysql_select_db(conn_.get(), schema.c_str()) != 0) {
        s.assign(Status::ERROR, getLastError(conn_));
        return;
    }
}

void Connection::setAutoCommit(bool autoCommit, Status &s) {
    s.clear();

    if(!connected()) {
        s.assign(Status::ERROR, "not connected");
        return;
    }

    if(!mysql_autocommit(conn_.get(), autoCommit) != 0) {
        s.assign(Status::ERROR, getLastError(conn_));
        return;
    }

    autoCommit_ = autoCommit;
}

bool Connection::getAutoCommit(Status &s) {
    return autoCommit_;
}

void Connection::initializeHandler() {
    conn_.assign(mysql_init(nullptr));
}

}