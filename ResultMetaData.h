//
// Created by m8792 on 2020/12/22.
//

#ifndef MYSQL_CONNECTOR_RESULTMETADATA_H
#define MYSQL_CONNECTOR_RESULTMETADATA_H

#include <mysql/mysql.h>
#include <mysql/mysql_com.h>
#include <stdint.h>
#include <string>
#include <stdexcept>
#include <map>

namespace db {

/**
 * 列的值的类型
 *
 * @note Integer的长度还需要根据field的length确定
 */
class DataType {
public:
    enum {
        UNKNOWN = 0,
        SIGNED_INTEGER,
        UNSIGNED_INTEGER,
        DOUBLE,
        STRING,
        SQLNULL
    };
};

/**
 * 列的值的包裹类型
 *
 * 含有列的值和值类型信息
 * @note 现在仅支持 INT, UINT, DOUBLE, STRING, SQLNULL类型
 */
class Value {
public:

    Value() : valueType_(DataType::SQLNULL) {
        value_.d = 0;
    }

    Value(int32_t value) {
        assign(value);
    }

    Value(uint32_t value) {
        assign(value);
    }

    Value(int64_t value) {
        assign(value);
    }

    Value(uint64_t value) {
        assign(value);
    }

    Value(double value) {
        assign(value);
    }

    Value(const std::string &s) {
        assign(value);
    }

    /**
     * C字符串构造一个Value类型
     * @param str
     */
    Value(const char *str) {
        assign(str);
    }

    void assign(int32_t value) {
        valueType_ = DataType::SIGNED_INTEGER;
        value_.i64 = value;
        s_ = std::to_string(value);
    }

    void assign(uint32_t value) {
        valueType_ = DataType::UNSIGNED_INTEGER;
        value_.u64 = value;
        s_ = std::to_string(value);
    }

    void assign(int64_t value) {
        valueType = DataType::SIGNED_INTEGER;
        value_.i64 = value;
        s_ = std::to_string(value);
    }

    void assign(uint64_t value) {
        valueType_ = DataType::UNSIGNED_INTEGER;
        value_.u64 = value;
        s_ = std::to_string(value);
    }

    void assign(double value) {
        valueType_ = DataType::DOUBLE;
        value_.d = value;
        s_ = std::to_string(value);
    }

    void assign(const std::string &value) {
        valueType_ = DataType::STRING;
        value_.d = 0;
        s_ = value;
    }

    void assign(const char *str) {
        valueType_ = DataType::STRING;
        value_.d = 0;
        s_ = str ? str : "";
    }

    /**
     * 获取 值的类型
     * @return 值的类型
     * @see db::DataType
     */
    int getType() const {
        return valueType_;
    }

    int32_t getInt32() const {
        switch (valueType_) {
            case DataType::SQLNULL:
                return 0;

            case DataType::SIGNED_INTEGER:
                return static_cast<int32_t>(value_.i64);

            case DataType::UNSIGNED_INTEGER:
                return static_cast<int32_t>(value_.u64);

            case DataType::DOUBLE:
                return static_cast<int32_t>(value_.d);

            case DataType::STRING:
                return atoll(s_.c_str());
        }

        assert(false);
        return 0;
    }

    uin32_t getUInt32() const {
        switch (valueType_) {
            case DataType::SQLNULL:
                return 0;

            case DataType::SIGNED_INTEGER:
                return static_cast<uint32_t>(value_.i64);

            case DataType::UNSIGNED_INTEGER:
                return static_cast<uint32_t>(value_.u64);

            case DataType::DOUBLE:
                return static_cast<uint32_t>(value_.d);

            case DataType::STRING:
                return atoll(s_.c_str());
        }

        assert(false);
        return 0;
    }

    int64_t getInt64() const {
        switch (valueType_) {
            case DataType::SQLNULL:
                return 0;

            case DataType::SIGNED_INTEGER:
                return value_.i64;

            case DataType::UNSIGNED_INTEGER:
                return value_.i64;

            case DataType::DOUBLE:
                return static_cast<int64_t>(value_.d);

            case DataType::STRING:
                return atoll(s_.c_str());
        }

        assert(false);
        return 0;
    }

    uint64_t getUInt64() const {
        switch (valueType_) {
            case DataType::SQLNULL:
                return 0;

            case DataType::SIGNED_INTEGER:
                return static_cast<uint64_t>(value_.i64);

            case DataType::UNSIGNED_INTEGER:
                return value_.u64;

            case DataType::DOUBLE:
                return static_cast<uint64_t>(value_.d);

            case DataType::STRING:
                return atoll(s_.c_str());
        }

        assert(false);
        return 0;
    }

    double getDouble() const {
        switch (valueType_) {
            case DataType::SQLNULL:
                return 0;

            case DataType::SIGNED_INTEGER:
                return static_cast<double>(value_.i64);

            case DataType::UNSIGNED_INTEGER:
                return static_cast<double>(value_.u64);

            case DataType::DOUBLE:
                return value_.d;

            case DataType::STRING:
                return atof(s_.c_str());
        }

        assert(false);
        return 0;
    }

    const std::string &getString() const {
        return s_;
    }

private:
    int valueType_;

    union {
        int64_t i64;
        uint64_t u64;
        double d;
    } value_;

    std::string s_;
};

/**
 * 将mysql的类型转成 DataType
 * @param mysqlFieldType
 * @return
 */
int mysqlTypeToDataType(int mysqlFieldType) {
    switch (mysqlFieldType) {
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG:
            return DataType::SIGNED_INTEGER;

        case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
            return DataType::DOUBLE;

        case MYSQL_TYPE_NULL:
            return sql::DataType::SQLNULL;

        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_TINY_BLOB:// should no appear over the wire
        case MYSQL_TYPE_MEDIUM_BLOB:// should no appear over the wire
        case MYSQL_TYPE_LONG_BLOB:// should no appear over the wire
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_JSON:
        case MYSQL_TYPE_GEOMETRY:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_SET:
            return DataType::STRING;

        default:
            return DataType::UNKNOWN;
    }
}

/**
 * ResultSet的元数据信息
 *
 * @warning 生命周期与ResultSet相同，使用时要保证ResultSet存活
 */
class ResultMetaData {
public:

    ResultMetaData() : fieldS_(nullptr), fieldCount_(0) {}

    ResultMetaData(MYSQL_FIELD *fields, size_t fieldCount) : fields_(fields), fieldCount_(fieldCount) {
        initNameIndex();
    }

    ResultMetaData(const ResultMetaData &) = default;

    ResultMetaData &operator=(const ResultMetaData &) = default;

    ResultMetaData(ResultMetaData &&) = default;

    ResultMetaData &operator=(ResultMetaData &&) = default;

    void assign(MYSQL_FIELD *fields, int fieldCount) {
        fields_ = fields;
        fieldCount_ = fieldCount;
        initNameIndex();
    }

    /**
     * 获取field的数量
     * @return
     */
    size_t getFieldCount() const {
        return fieldCount_;
    }

    /**
     * 获取第index列的名字
     * @param index
     * @return
     */
    std::string getFieldName(size_t index) const {
        if (index >= fieldCount_) {
            // 需要抛异常的时候，还是抛异常
            throw std::out_of_range("out of range");
        }

        return fields_[index].name;
    }

    /**
     * 获取第index个Field的类型
     * @param index
     * @return          类型值 @see db::DataType
     */
    int getFieldType(size_t index) const {
        if (index >= fieldCount_) {
            throw std::out_of_range("out of range");
        }

        return mysqlTypeToDataType(fields_[index].type);
    }

    /**
     * 获取mysql type
     * @param index
     * @return
     */
    int getOrgFieldType(size_t index) const {
        if (index >= fieldCount_) {
            throw std::out_of_range("out of range");
        }

        return fields_[index].type;
    }

    /**
     * 获取第index个列的数据的长度
     * @param index
     * @return
     */
    size_t getFieldLength(size_t index) const {
        if (index >= fieldCount_) {
            throw std::out_of_range("out of range");
        }

        return fields_[index].length;
    }

    /**
     * 获取第index个列的最长的长度，仅在所有的查询结果保存到本地时有效
     */
    size_t getFieldMaxLength(size_t index) const {
        if (index >= fieldCount_) {
            throw std::out_of_range("out of range");
        }

        return fields_[index].max_length;
    }

    /**
     * 是否是有效的
     * @return
     */
    bool isValid() const {
        return fields_ != nullptr;
    }

    size_t fieldNameToIndex(const std::string &name) const {
        auto it = nameToIndex_.find(name);
        if (it == nameToIndex_.end()) {
            throw std::out_of_range("name not found");
        }
        return it->second;
    }

    void swap(ResultMetaData &other) {
        using std::swap;
        swap(fields_, other.fields_);
        swap(fieldCount_, other.fieldCount_);
        swap(nameToIndex_, other.nameToIndex_);
    }

private:
    void initNameIndex() {
        nameToIndex_.clear();
        for (int i = 0; i < fieldCount_; ++i) {
            nameToIndex_[fields_[i].name] = i;
        }
    }

private:
    /**
     * 指向 fields_ 数据的数组
     * @note 生命周期与ResultSet相同，不需要手动释放
     */
    MYSQL_FIELD *fields_;

    /**
     * field数组的长度
     */
    size_t fieldCount_;

    std::map<std::string, size_t> nameToIndex_;
};

void swap(ResultMetaData &lhs, ResultMetaData &rhs) {
    lhs.swap(rhs);
}

}


#endif //MYSQL_CONNECTOR_RESULTMETADATA_H
