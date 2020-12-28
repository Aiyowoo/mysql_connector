//
// Created by m8792 on 2020/12/17.
//

#include "../include/Status.h"

namespace db {

Status::Status(int code) : code_(code) {}

Status::Status(int code, const std::string& message)
    : code_(code), message_(message) {}

Status::operator bool() const { return code_ == StatusCode::OK; }

void Status::assign(int code, const std::string& message) {
  code_ = code;
  message_ = message;
}

int Status::code() const { return code_; }

const std::string& Status::message() const { return message_; }

void Status::clear() {
  code_ = 0;
  message_.clear();
}

}  // namespace db