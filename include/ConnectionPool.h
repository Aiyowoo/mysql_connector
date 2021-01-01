//
// Created by m8792 on 2021/1/1.
//

#ifndef MYSQL_CONNECTOR_CONNECTIONPOOL_H
#define MYSQL_CONNECTOR_CONNECTIONPOOL_H

#include <chrono>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

#include "Connection.h"
#include "Option.h"

namespace db {

class ConnectionPool;

/**
 * 连接池提供给外部调用的链接类
 */
class ConnectionPtr {
    friend class ConnectionPool;

public:
    ConnectionPtr() {}

    ConnectionPtr(std::weak_ptr<ConnectionPool> pool, Connection& connection)
        : pool_(pool), connection_(std::move(connection)) {}

    ConnectionPtr(ConnectionPtr&& other)
        : pool_(std::move(other.pool_)),
          connection_(std::move(other.connection_)) {}

    ConnectionPtr& operator=(ConnectionPtr&& other) {
        pool_ = std::move(other.pool_);
        connection_ = std::move(other.connection_);
    }

    Connection& operator->() { return connection_; }

    operator bool() { return connection_.connected(); }

    /**
     * 释放链接到连接池
     *
     * 还到连接池中，如果没有所属的连接池，就直接关闭
     */
    void release();

    ~ConnectionPtr() { release(); }

private:
    /**
     * 链接所属的链接池
     */
    std::weak_ptr<ConnectionPool> pool_;

    Connection connection_;
};

/**
 * 连接池
 *
 * 管理一组到mysql服务器的链接
 */
class ConnectionPool : std::enable_shared_from_this<ConnectionPool> {
    /**
     * 连接到服务器的配置信息
     */
    struct Config {
        std::string host;
        unsigned short port;
        std::string user;
        std::string password;
        std::string schema;

        Config() : port(0) {}
    };

public:
    /**
     * 创建一个链接数量为connectionCount的连接池
     * @param connectionCount       链接的数量
     */
    ConnectionPool(size_t connectionCount)
        : connectionCount_(connectionCount) {}

    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    ConnectionPool(ConnectionPool&& other) = delete;

    ConnectionPool& operator=(ConnectionPool&& other) = delete;

    /**
     * 改变链接数量
     * @param connectionCount
     */
    void resize(size_t connectionCount) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (connectionCount_ == connectionCount) {
            return;
        }

        if (config_.host.empty()) {
            // 没有配置过，就是没有创建过连接
            connectionCount_ = connectionCount;
            return;
        }

        if (connectionCount < connectionCount_) {
            for (int i = connectionCount;
                 i < connectionCount_ && !connections_.empty(); ++i) {
                Connection& connection = connections_.front();
                connection.close();
                connections_.pop_front();
            }
        } else {
            for (int i = connectionCount_; i < connectionCount; ++i) {
                // 补上缺少的连接
                Status s;
                Connection conn = createConnection(s);
                connections_.emplace_back(std::move(conn));
            }

            // 说明之前是空的， 可能有人在等连接
            if (connections_.size() == connectionCount - connectionCount_) {
                cond_.notify_all();
            }
        }
        connectionCount_ = connectionCount;
    }

    /**
     * 创建connectionCount的连接到数据库
     * @param host          服务器地址
     * @param port          服务器端口 0默认3306
     * @param user          用户名
     * @param password      密码
     * @param schema        数据库
     * @param s             是否成功
     *
     * @warning
     * 只能调用一次，只有在成功连接之后才有用，在最开始都连不上，错误应该直接报出来
     */
    void connect(const std::string& host, unsigned short port,
                 const std::string& user, const std::string& password,
                 const std::string& schema, Status& s) {
        s.clear();

        std::lock_guard<std::mutex> lock(mutex_);

        if (connectInvoked()) {
            s.assign(Status::RUNTIME_ERROR, "connect already invoked");
            return;
        }

        config_.host = host;
        config_.port = port;
        config_.user = user;
        config_.password = password;
        config_.schema = schema;

        for (int i = 0; i < connectionCount_; ++i) {
            Connection connection = createConnection(s);
            if (!s) {
                connections_.clear();
                config_ = Config();
                return;
            }

            connections_.emplace_back(std::move(connection));
        }
    }

    /**
     * 创建connectionCount的连接到数据库
     * @param host          服务器地址
     * @param port          服务器端口 0默认3306
     * @param user          用户名
     * @param password      密码
     * @param s             是否成功
     */
    void connect(const std::string& host, unsigned short port,
                 const std::string& user, const std::string& password,
                 Status& s) {
        connect(host, port, user, password, "", s);
    }

    /**
     * 关闭现在未被使用的所有连接
     *
     * @note 在外面使用的连接会直接释放，而不会返回到连接池
     * @warning 在外面被使用的连接还没有返回回来， 不适合调用第二次
     */
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.clear();
        connectionCount_ = 0;
    }

    /**
     * 获取一个连接
     * @return
     */
    ConnectionPtr getConnection() { return getConnection(-1); }

    /**
     * 获取一个连接
     * @param timeoutMs         超时时间微秒
     * @return
     */
    ConnectionPtr getConnection(int timeoutMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connections_.empty()) {
            if (timeoutMs == 0) {
                return ConnectionPtr();
            }
            if (timeoutMs < 0) {
                cond_.wait(lock, [this] { return !connections_.empty(); });
            } else {
                cond_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                               [this] { return !connections_.empty(); });
            }
        }

        if (connections_.empty()) {
            return ConnectionPtr();
        }

        ConnectionPtr ptr(weak_from_this(), connections_.front());
        connections_.pop_front();

        return ptr;
    }

    size_t getConnectionCount() const {
        // 获取连接的数量
        std::lock_guard<std::mutex> lock(mutex_);
        return connectionCount_;
    }

    /**
     * 回收一个连接
     */
    void revokeConnection(ConnectionPtr& ptr) {
        std::unique_lock<std::mutex> lock(mutex_);
        connections_.emplace_back(std::move(ptr.connection_));
        if (connections_.size() == 1) {
            cond_.notify_one();
        }
    }

private:
    /**
     * 创建一个已经连接到服务器的新连接
     * @return
     */
    Connection createConnection(Status& s) const {
        s.clear();

        Connection connection;

        connection.setOption(option::AutoReconnect(true), s);
        if (!s) {
            return connection;
        }

        connection.setOption(option::ConnectTimeout(3), s);
        if (!s) {
            return connection;
        }

        connection.connect(config_.host, config_.port, config_.user,
                           config_.password, config_.schema, s);
        return connection;
    }

    /**
     * 连接池已经连接到了服务器
     * @return
     */
    bool connectInvoked() const { return !config_.host.empty(); }

private:
    /**
     * 空闲的连接
     */
    std::list<Connection> connections_;

    /**
     * 连接池管理的所有连接数
     */
    size_t connectionCount_;

    /**
     * 服务器配置
     */
    Config config_;

    /**
     * 互斥锁保护内部变量
     */
    mutable std::mutex mutex_;

    /**
     * notify
     */
    std::condition_variable cond_;
};

inline void ConnectionPtr::release() {
    auto ptr = pool_.lock();
    if (ptr) {
        ptr->revokeConnection(*this);
        pool_.reset();
    } else {
        connection_.close();
    }
}

using ConnectionPoolPtr = std::shared_ptr<ConnectionPool>;

}  // namespace db

#endif  // MYSQL_CONNECTOR_CONNECTIONPOOL_H
