//
// Created by m8792 on 2020/12/27.
//

#ifndef MYSQL_CONNECTOR_BIND_H
#define MYSQL_CONNECTOR_BIND_H

#include "ResultMetaData.h"
#include <mysql/mysql.h>

#include <stdlib.h>

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
    Bind(const ResultMetaData &metaData) : binds_(nullptr), bindCount_(0) {
        assign(metadata);
    }

    ~Bind() {
        clear();
    }

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

    void assign(const ResultMetaData &metaData) {
        clear();

        bindCount_ = metaData.getFieldCount();
        if (bindCount_ == 0) {
            binds_ = nullptr;
            return;
        }

        binds_ = new MYSQL_BIND[bindCount_];
        for (int i = 0; i < bindCount_; ++i) {
            memset(&binds_[i], 0, sizeof(MYSQL_BIND));
            int maxLen = metaData.getFieldMaxLength(i) + 1;
            binds_[i].buffer = new char[maxLen];
            binds_[i].buffer_length = maxLen;

            binds_[i].length = new unsigned long;
            *binds_[i].length = 0;

            binds_[i].is_null = new my_bool;
            *binds_[i].is_null = false;

            binds_[i].buffer_type = metaData.getOrgFieldType(i);
        }
    }

    void clear() {
        if (binds_ == nullptr) {
            return;
        }

        for (int i = 0; i < bindCount_; ++i) {
            if (binds_[i].buffer) {
                clearAllocatedBuffer(&binds_[i]);
            }
        }
        delete binds_;
    }

    MYSQL_BIND *getBinds() const {
        return binds_;
    }

    size_t getBindCount() const {
        return bindCount_;
    }

    MYSQL_BIND *getBind(size_t index) const {
        if (index >= bindCount_) {
            throw std::out_of_range("out of range");
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

        char *ptr = new char[8];
        assert(ptr);
        *reinterpret_cast<int64_t *>(ptr) = value;

        clearAllocatedBuffer(&binds_[index]);
        binds_[index]->buffer = ptr;
        binds_[index].buffer_length = 0;
        binds_[index]->buffer_type = dataTypeToMysqlType(DataType::SIGNED_INTEGER);
        binds_[index].is_null_value = 0;
    }

    void setValue(size_t index, int64_t value) {
        checkIndexValid(index);

        char *ptr = new char[8];
        assert(ptr);
        *reinterpret_cast<int64_t *>(ptr) = value;

        clearAllocatedBuffer(&binds_[index]);
        binds_[index]->buffer = ptr;
        binds_[index].buffer_length = 0;
        binds_[index]->buffer_type = dataTypeToMysqlType(DataType::SIGNED_INTEGER);
        binds_[index].is_null_value = 0;
    }

    void setValue(size_t index, const std::string &value) {
        checkIndexValid(index);

        char *ptr = new char[value.size() + 1];
        assert(ptr);
        strcpy(ptr, value.c_str());

        clearAllocatedBuffer(&binds_[index]);
        binds_[index]->buffer = ptr;
        binds_[index].buffer_length = 0;
        binds_[index]->buffer_type = dataTypeToMysqlType(DataType::STRING);
        binds_[index].is_null_value = 0;
    }

    Value getValue(size_t index) {
        checkIndexValid(index);

        MYSQL_BIND *bind = &binds_[index];
        assert(bind);

        if (*(bind->is_null)) {
            return Value();
        }

        switch (mysqlTypeToDataType(bind->buffer_type)) {
            case DataType::SIGNED_INTEGER: {
                int64_t val = 0;
                switch (bind->length) {
                    case 2:
                        val = *reinterpret_cast<int16_t *>(bind->buffer);
                        break;
                    case 4:
                        val = *reinterpret_cast<int32_t *>(bind->buffer);
                        break;
                    case 8:
                        val = *reinterpret_cast<int64_t *>(bind->buffer);
                        break;
                }
                return Value(DataType::SIGNED_INTEGER, val);
            }

            case DataType::UNSIGNED_INTEGER: {
                uint64_t val = 0;
                switch (bind->length) {
                    case 2:
                        val = *reinterpret_cast<uint16_t *>(bind->buffer);
                        break;
                    case 4:
                        val = *reinterpret_cast<uint32_t *>(bind->buffer);
                        break;
                    case 8:
                        val = *reinterpret_cast<uint64_t *>(bind->buffer);
                        break;
                }
                return Value(DataType::UNSIGNED_INTEGER, val);
            }

            case DataType::STRING: {
                std::string val(bind->buffer,bind->buffer + *bind->length);
                return Value(DataType::STRING, val);
            }

            case DataType::DOUBLE: {
                double val = 0;
                switch (bind->length) {
                    case 4:
                        val = *reinterpret_cast<float *>(bind->buffer);
                        break;
                    case 8:
                        val = *reinterpret_cast<double *>(bind->buffer);
                        break;
                }
                return Value(DataType::DOUBLE, val);
            }

            case DataType::SQLNULL:
            case DataType::UNKNOWN:
                return Value();
        }
    }

private:
    void checkIndexValid(size_t index) const {
        if(index >= bindCount_) {
            throw std::out_of_range("out of range");
        }
    }

    void clearAllocatedBuffer(MYSQL_BIND *bind) {
        if (bind == nullptr) {
            return;
        }

        if (bind->buffer) {
            delete[] bind->buffer;
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

    MYSQL_BIND *binds_;
    size_t bindCount_;
};


}

#endif //MYSQL_CONNECTOR_BIND_H
