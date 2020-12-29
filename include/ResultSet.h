//
// Created by m8792 on 2020/12/22.
//

#ifndef MYSQL_CONNECTOR_RESULTSET_H
#define MYSQL_CONNECTOR_RESULTSET_H

#include <mysql/mysql.h>

#include <optional>
#include <vector>

#include "Handler.h"
#include "ResultMetaData.h"
#include "Status.h"

namespace db {

/**
 * Statement执行select语句获取的结果集
 */
class ResultSet {
public:
    /**
     * 构造结果集
     * Connection的生命周期要长于ResultSet
     * @param res 如果为空表示是无效的
     */
    explicit ResultSet(MYSQL_RES* res = nullptr) { assign(res); }

    ResultSet(const ResultSet&) = delete;

    ResultSet& operator=(const ResultSet&) = delete;

    ResultSet(ResultSet&& other)
        : res_(std::move(other.res_)),
          currentRow_(other.currentRow_),
          metaData_(other.metaData_) {
        other.res_.assign(nullptr);
        currentRow_ = nullptr;
        metaData_.assign(nullptr, 0);
    }

    ResultSet& operator=(ResultSet&& other) {
        ResultSet tmp;
        this->swap(other);
        other.swap(tmp);
    }

    void swap(ResultSet& other) {
        using std::swap;
        swap(res_, other.res_);
        swap(currentRow_, other.currentRow_);
        swap(metaData_, other.metaData_);
    }

    ~ResultSet() { clear(); }

    void assign(MYSQL_RES* res) {
        clear();

        res_.assign(res);

        if (res_.valid()) {
            // 初始化元数据信息
            int fieldCount = mysql_num_fields(res_.get());
            MYSQL_FIELD* fields = mysql_fetch_fields(res_.get());
            metaData_.assign(fields, fieldCount);
        } else {
            metaData_.assign(nullptr, 0);
        }
    }

    /**
     * 释放资源
     */
    void clear() {
        if (res_.valid()) {
            res_.close();
        }
    }

    /**
     * 获取结果集的元数据信息
     * @return
     */
    const ResultMetaData& getResultMetaData(Status& s) const {
        return metaData_;
    }

    /**
     * 移动到下一行
     * 获取第一行的时候也需要调用next
     * @return 是否有数据
     */
    bool next() {
        if (!valid()) {
            return false;
        }

        currentRow_ = mysql_fetch_row(res_.get());
        return currentRow_ != nullptr;
    }

    /**
     * 回绕到最初的行
     */
    void rewind(Status& s) {
        s.clear();

        if (!valid()) {
            s.assign(Status::ERROR, "not valid");
            return;
        }

        mysql_data_seek(res_.get(), 0);
    }

    /**
     * 是否是有效的， 是否有数据
     * @return
     */
    bool valid() const { return res_.valid(); }

    /**
     * 获取第index列的数据
     * @param index
     * @return
     */
    std::string getString(size_t index) const {
        checkIndexValid(index);

        return currentRow_[index] ? currentRow_[index] : "";
    }

    /**
     * 获取name列的数据
     * @param name
     * @return
     */
    std::string getString(const std::string& name) const {
        return getString(fieldNameToIndex(name));
    }

    int32_t getInt32(size_t index) const {
        checkIndexValid(index);

        return currentRow_[index] ? atoi(currentRow_[index]) : 0;
    }

    int32_t getInt32(const std::string& name) const {
        return getInt32(fieldNameToIndex(name));
    }

    int64_t getInt64(size_t index) const {
        checkIndexValid(index);

        return currentRow_[index] ? atoll(currentRow_[index]) : 0;
    }

    int64_t getInt64(const std::string& name) const {
        return getInt64(fieldNameToIndex(name));
    }

    double getDouble(size_t index) const {
        checkIndexValid(index);

        return currentRow_[index] ? atof(currentRow_[index]) : 0;
    }

    double getDouble(const std::string& name) const {
        return getDouble(fieldNameToIndex(name));
    }

    Value getValue(size_t index) const {
        checkIndexValid(index);

        // 如果是nullptr，当SQLNULL
        if (metaData_.getFieldType(index) == DataType::SQLNULL) {
            return Value();
        }

        switch (metaData_.getFieldType(index)) {
        case DataType::SIGNED_INTEGER:
        case DataType::UNSIGNED_INTEGER:
            return Value(getInt64(index));

        case DataType::DOUBLE:
            return Value(getDouble(index));

        case DataType::STRING:
        case DataType::UNKNOWN:
        default:
            return Value(getString(index));
        }
    }

    Value getValue(const std::string& name) const {
        return getValue(fieldNameToIndex(name));
    }

protected:
    void checkIndexValid(size_t index) const {
        if (index > metaData_.getFieldCount()) {
            throw std::out_of_range("out of range");
        }
    }

    /**
     *
     * @param name
     * @return
     * @throws out_of_range
     */
    size_t fieldNameToIndex(const std::string& name) const {
        return metaData_.fieldNameToIndex(name);
    }

private:
    /**
     * ResultSet所属的Connection
     * conn_的生命周期要长于ResultSet
     */
    ResultSetHandler res_;

    /**
     * 正在访问的当前行
     */
    MYSQL_ROW currentRow_;

    /**
     * 元数据信息
     */
    ResultMetaData metaData_;
};

}  // namespace db

#endif  // MYSQL_CONNECTOR_RESULTSET_H
