//
// Created by m8792 on 2020/12/24.
//

#ifndef MYSQL_CONNECTOR_OPTION_H
#define MYSQL_CONNECTOR_OPTION_H

#include <mysql/mysql.h>

#include <string>

namespace db {

namespace option {

enum OptionName { AUTO_RECONNECT = 0 };

template <int OptionName> class BoolOption {
 public:
  BoolOption(bool value) : value_(value) {}

  /**
   * 获取选项名
   * @return
   */
  int name() const { return OptionName; }

  /**
   * 获取选项值
   * @return
   */
  void* value() const { return &value_; }

  /**
   * 设置选项值
   * @param value
   */
  void value(bool value) { value_ = value; }

 private:
  my_bool value_;
};

template <int OptionName> class IntegerOption {
 public:
  IntegerOption(int value) : value_(value) {}

  /**
   * 获取选项名
   * @return
   */
  int name() const { return OptionName; }

  /**
   * 获取选项值
   * @return
   */
  void* value() const { return &value_; }

  /**
   * 设置选项值
   * @param value
   */
  void value(int value) { value_ = value; }

 private:
  int32_t value_;
};

template <int OptionName> class StringOption {
 public:
  StringOption(const std::string& value) : value_(value) {}

  /**
   * 获取选项名
   * @return
   */
  int name() const { return OptionName; }

  /**
   * 获取选项值
   * @return
   */
  void* value() const { return value_.c_str(); }

  /**
   * 设置选项值
   * @param value
   */
  void value(const std::string& value) { value_ = value; }

 private:
  std::string value_;
};

}  // namespace option

}  // namespace db

#endif  // MYSQL_CONNECTOR_OPTION_H
