//
// Created by m8792 on 2020/12/27.
//

#ifndef MYSQL_CONNECTOR_BIND_H
#define MYSQL_CONNECTOR_BIND_H

#include <fmt/printf.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>

#include "ResultMetaData.h"

namespace db {

/**
 * 入参和结果的 集合
 */
class Bind {
public:
    Bind() : binds_(nullptr), bindCount_(0) {}

    /**
     * 构造 参数Bind数组
     * @param paramCount
     */
    Bind(size_t paramCount) : binds_(nullptr), bindCount_(0) {
        assign(paramCount);
    }

    /**
     * 从ResultMetaData构造 Bind数组
     * @param metaData
     */
    Bind(const ResultMetaData& metaData) : binds_(nullptr), bindCount_(0) {
        assign(metaData);
    }

    ~Bind() { clear(); }

    void assign(size_t paramCount) {
        clear();

        bindCount_ = paramCount;
        if (bindCount_ == 0) {
            binds_ = 0;
            return;
        }

        binds_ = new MYSQL_BIND[bindCount_];
        for (int i = 0; i < bindCount_; ++i) {
            memset(&binds_[i], 0, sizeof(MYSQL_BIND));
        }
    }

    void assign(const ResultMetaData& metaData) {
        clear();

        bindCount_ = metaData.getFieldCount();
        if (bindCount_ == 0) {
            binds_ = nullptr;
            return;
        }

        binds_ = new MYSQL_BIND[bindCount_];
        for (int i = 0; i < bindCount_; ++i) {
            memset(&binds_[i], 0, sizeof(MYSQL_BIND));

            auto p = allocateBuffer(metaData.getOrgFieldType(i), metaData.getFieldMaxLength(i));
            binds_[i].buffer = p.first;
            binds_[i].buffer_length = p.second;

            binds_[i].length = new unsigned long;
            *binds_[i].length = 0;

            binds_[i].is_null = new my_bool;
            *binds_[i].is_null = false;

            binds_[i].buffer_type =
                static_cast<enum_field_types>(metaData.getOrgFieldType(i));
        }
    }

    void clear() {
        if (binds_ == nullptr) {
            return;
        }

        for (int i = 0; i < bindCount_; ++i) {
            clearAllocatedBuffer(&binds_[i]);
        }
        delete binds_;
        binds_ = nullptr;
    }

    MYSQL_BIND* getBinds() const { return binds_; }

    size_t getBindCount() const { return bindCount_; }

    MYSQL_BIND* getBind(size_t index) const {
        if (index >= bindCount_) {
            throw std::out_of_range(
                fmt::sprintf("index %d out of range [0, %d)", 0, bindCount_));
        }
        return &binds_[index];
    }

    /**
     * 绑定第index个参数
     * @param index
     * @param value     参数值
     */
    void setValue(size_t index, int32_t value) {
        checkIndexValid(index);

        char* ptr = new char[8];
        assert(ptr);
        *reinterpret_cast<int64_t*>(ptr) = value;

        clearAllocatedBuffer(&binds_[index]);
        binds_[index].buffer = ptr;
        binds_[index].buffer_length = 0;
        binds_[index].buffer_type = static_cast<enum_field_types>(
            dataTypeToMysqlType(DataType::SIGNED_INTEGER));
        binds_[index].is_null_value = 0;
    }

    void setValue(size_t index, int64_t value) {
        checkIndexValid(index);

        char* ptr = new char[8];
        assert(ptr);
        *reinterpret_cast<int64_t*>(ptr) = value;

        clearAllocatedBuffer(&binds_[index]);
        binds_[index].buffer = ptr;
        binds_[index].buffer_length = 0;
        binds_[index].buffer_type =
            dataTypeToMysqlType(DataType::SIGNED_INTEGER);
        binds_[index].is_null_value = 0;
    }

    void setValue(size_t index, const std::string& value) {
        checkIndexValid(index);

        char* ptr = new char[value.size() + 1];
        assert(ptr);
        strcpy(ptr, value.c_str());

        clearAllocatedBuffer(&binds_[index]);
        binds_[index].buffer = ptr;
        binds_[index].buffer_length = value.size();
        binds_[index].buffer_type = dataTypeToMysqlType(DataType::STRING);
        binds_[index].is_null_value = 0;
    }

    Value getValue(size_t index) const {
        checkIndexValid(index);

        MYSQL_BIND* bind = &binds_[index];
        assert(bind);

        if (*(bind->is_null)) {
            return Value();
        }

        switch (mysqlTypeToDataType(bind->buffer_type)) {
        case DataType::SIGNED_INTEGER: {
            int64_t val = 0;
            switch (*bind->length) {
            case 2:
                val = *reinterpret_cast<int16_t*>(bind->buffer);
                break;
            case 4:
                val = *reinterpret_cast<int32_t*>(bind->buffer);
                break;
            case 8:
                val = *reinterpret_cast<int64_t*>(bind->buffer);
                break;
            }
            return Value(val);
        }

        case DataType::UNSIGNED_INTEGER: {
            uint64_t val = 0;
            switch (*bind->length) {
            case 2:
                val = *reinterpret_cast<uint16_t*>(bind->buffer);
                break;
            case 4:
                val = *reinterpret_cast<uint32_t*>(bind->buffer);
                break;
            case 8:
                val = *reinterpret_cast<uint64_t*>(bind->buffer);
                break;
            }
            return Value(val);
        }

        case DataType::STRING: {
            // 如果是Time类型的要做特殊处理
            std::string val;
            switch (bind->buffer_type) {
            case MYSQL_TYPE_DATE: {
                MYSQL_TIME* time = reinterpret_cast<MYSQL_TIME*>(bind->buffer);
                assert(time);
                val = fmt::sprintf("%04d-%02d-%02d", time->year, time->month,
                                   time->day);
                break;
            }

            case MYSQL_TYPE_TIME: {
                MYSQL_TIME* time = reinterpret_cast<MYSQL_TIME*>(bind->buffer);
                assert(time);
                if (time->second_part) {
                    val = fmt::sprintf("%s%02d:%02d:%02d.%06lu",
                                       time->neg ? "-" : "", time->hour,
                                       time->minute, time->second,
                                       time->second_part);
                } else {
                    val = fmt::sprintf("%s%02d:%02d:%02d", time->neg ? "-" : "",
                                       time->hour, time->minute, time->second);
                }
                break;
            }

            case MYSQL_TYPE_TIMESTAMP:
            case MYSQL_TYPE_DATETIME: {
                MYSQL_TIME* time = reinterpret_cast<MYSQL_TIME*>(bind->buffer);
                assert(time);
                if (time->second_part) {
                    val = fmt::sprintf("%04d-%02d-%02d %02d:%02d:%02d.%06lu",
                                       time->year, time->month, time->day,
                                       time->hour, time->minute, time->second,
                                       time->second_part);
                } else {
                    val = fmt::sprintf("%04d-%02d-%02d %02d:%02d:%02d",
                                       time->year, time->month, time->day,
                                       time->hour, time->minute, time->second);
                }
                break;
            }
            default: {
                char* ptr = reinterpret_cast<char*>(bind->buffer);
                val.assign(ptr, ptr + *bind->length);
                break;
            }
            }

            return Value(val);
        }

        case DataType::DOUBLE: {
            double val = 0;
            switch (*bind->length) {
            case 4:
                val = *reinterpret_cast<float*>(bind->buffer);
                break;
            case 8:
                val = *reinterpret_cast<double*>(bind->buffer);
                break;
            }
            return Value(val);
        }

        case DataType::SQLNULL:
        case DataType::UNKNOWN:
            return Value();
        }
    }

private:
    void checkIndexValid(size_t index) const {
        if (index >= bindCount_) {
            throw std::out_of_range("out of range");
        }
    }

    std::pair<char*, size_t> allocateBuffer(int mysqlFieldType, int maxLen) {
        char* buffer = nullptr;
        size_t bufferLen = 0;
        switch (mysqlFieldType) {
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
            buffer = new char[sizeof(MYSQL_TIME)];
            bufferLen = sizeof(MYSQL_TIME);
            break;

        default:
            buffer = new char[maxLen + 8];
            bufferLen = maxLen + 8;
            break;
        }
        return std::make_pair(buffer, bufferLen);
    }

    void clearAllocatedBuffer(MYSQL_BIND* bind) {
        if (bind == nullptr) {
            return;
        }

        if (bind->buffer) {
            char* ptr = reinterpret_cast<char*>(bind->buffer);
            delete[] ptr;
            bind->buffer = nullptr;
        }

        if (bind->is_null) {
            delete bind->is_null;
            bind->is_null = nullptr;
        }

        if (bind->length) {
            delete bind->length;
            bind->length = nullptr;
        }
    }

private:
    MYSQL_BIND* binds_;
    size_t bindCount_;
};

}  // namespace db

#endif  // MYSQL_CONNECTOR_BIND_H
