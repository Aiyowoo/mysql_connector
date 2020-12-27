//
// Created by m8792 on 2020/12/22.
//

#ifndef MYSQL_CONNECTOR_PREPAREDRESULTSET_H
#define MYSQL_CONNECTOR_PREPAREDRESULTSET_H

#include "ResultMetaData.h"
#include "Bind.h"

#include <mysql/mysql.h>
#include <mysql/decimal.h>

#include <stdexcept>
#include <memory>

namespace db {

/**
 * PreparedStatement的结果集，生命周期要短于PreparedStatement
 *
 * @warning 生命周期和PreparedStatement相同，使用时需要保证PreparedStatement存活
 */
class PreparedResultSet {
public:

    PreparedResultSet(MYSQL_STMT *stmt = nullptr) : stmt_(stmt), currentRowPos_(-1) {
        initMetaData();
    }

    PreparedResultSet(const PreparedResultSet &) = delete;

    PreparedResultSet &operator=(const PreparedResultSet &) = delete;

    PreparedResultSet(PreparedResultSet &&other)
            : stmt_(other.stmt_), colNameIndexMapping_(other.colNameIndexMapping_),
              currentRowPos_(other.currentRowPos_), metaData_(other.metaData_), resultBinds_(other.resultBinds_) {
        PreparedResultSet tmp;
        tmp.swap(other);
    }

    PreparedResultSet &operator=(PreparedResultSet &&other) {
        PreparedResultSet tmp;
        swap(other);
        tmp.swap(other);
    }

    /**
     * 是否有效，是否含有数据
     * @return
     */
    bool isValid() {
        return stmt_ != nullptr;
    }

    const ResultMetaData &getMetaData() const {
        return metaData_;
    }

    /**
     * 获取列的元数据的数量
     * @return
     */
    size_t getFieldCount() {
        return metaData_.getFieldCount();
    }

    /**
     * 移动到下一行
     * @return true-有下一行
     */
    bool next() {
        if (!isValid()) {
            return false;
        }

        int ret = mysql_stmt_fetch(stmt_);
        if (ret != 0) {
            return false;
        }
        ++currentRowPos_;
        return true;
    }

    /**
     * 获取当前行的行号
     * @return
     */
    size_t getCurrentRow() {
        return currentRowPos_;
    }

    /**
     * 回绕到第一行之前
     */
    void rewind() {
        if (!isValid()) {
            throw std::runtime_error("result set is invalid");
        }

        mysql_stmt_data_seek(stmt_, 0);
        currentRowPos_ = -1;
    }

    /**
     * 获取第index列的数据
     * @param index
     * @return
     */
    std::string getString(size_t index) const {
        return resultBinds_.getValue(index).getString();
    }

    /**
     * 获取name列的数据
     * @param name
     * @return
     */
    std::string getString(const std::string &name) const {
        return resultBinds_.getValue(fieldNameToIndex(name)).getString();
    }

    int32_t getInt32(size_t index) const {
        return resultBinds_.getValue(fieldNameToIndex(name)).getInt64();
    }

    int32_t getInt32(const std::string &name) const {
        return getInt32(fieldNameToIndex(name));
    }

    int64_t getInt64(size_t index) const {
        return resultBinds_.getValue(index).getInt64();
    }

    int64_t getInt64(const std::string &name) const {
        return getInt64(fieldNameToIndex(name)).getInt64();
    }

    double getDouble(size_t index) const {
        return resultBinds_.getValue(index).getDouble();
    }

    double getDouble(const std::string &name) const {
        return getDouble(fieldNameToIndex(name)).getDouble();
    }

    Value getValue(size_t index) const {
        return resultBinds_.getValue(index);
    }

    Value getValue(const std::string &name) const {
        return getValue(fieldNameToIndex(name));
    }

    void swap(PreparedResultSet &other) {
        using std::swap;
        swap(stmt_, other.stmt_);
        swap(colNameIndexMapping_, other.colNameIndexMapping_);
        swap(currentRowPos_, other.currentRowPos_);
        swap(metaData_, other.metaData_);
        swap(resultBinds_, other.resultBinds_);
    }

private:
    void initMetaData() {
        if (stmt_ == nullptr) {
            return;
        }

        MYSQL_RES *res = mysql_stmt_result_metadata(stmt_);
        if (res == nullptr) {
            throw std::runtime_error("can't get result set");
        }

        MYSQL_FIELD *fields = mysql_fetch_fields(res);
        if (fields == nullptr) {
            throw std::runtime_error("can't get result field info");
        }
        size_t fieldCount = mysql_num_fields(res);
        metaData_.assign(fields, fieldCount);

        resultBinds_.assign(metaData_);

        if (!mysql_stmt_bind_result(stmt_, resultBinds_.getBinds())) {
            throw std::runtime_error("can't bind results");
        }
    }

    size_t fieldNameToIndex(const std::string &name) const {
        return metaData_.fieldNameToIndex(name);
    }

private:

    MYSQL_STMT *stmt_;

    /**
     * 当前所在行
     */
    int64_t currentRowPos_;

    /**
     * 元数据
     */
    ResultMetaData metaData_;

    Bind resultBinds_;
};

void swap(PreparedResultSet &lhs, PreparedResultSet &rhs) {
    lhs.swap(rhs);
}

}

#endif //MYSQL_CONNECTOR_PREPAREDRESULTSET_H
