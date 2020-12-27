//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_CONNECTION_H
#define MYSQL_CONNECTOR_CONNECTION_H

#include "DBConfig.h"
#include "Handler.h"
#include "Statement.h"
#include "Status.h"
#include "PreparedStatement.h"
#include "Util.h"
#include <mysql/mysql.h>

namespace db {

/**
 * 到MYSQL服务器 的链接
 */
class Connection {
public:

    /**
     * 事务的隔离级别
     */
    enum IsolationLevel {
        READ_UNCOMMITTED    = 1,    // 读未提交
        READ_COMMITTED      = 2,    // 读已提交
        REPEATABLE_READ     = 3,    // 可重复读
        SERIALIZABLE        = 4     // 串行化
    };

public:
    Connection();

    Connection(const Connection &) = delete;

    Connection &operator=(const Connection &) = delete;

    Connection(Connection &&);

    Connection &operator=(Connection &&);

    ~Connection();

    /**
     * 链接到数据库
     * @param host      mysql服务器ip地址
     * @param port      mysql服务端口号，0的话为默认值
     * @param user      用户名
     * @param password  密码
     * @param s         是否链接成功
     */
    void connect(const std::string &host, unsigned short port,
                 const std::string &user, const std::string &password,
                 Status &s);

    /**
     * 关闭链接，并释放资源
     * @param s     是否成功
     */
    void close();

    /**
     * 当前是否链接成功
     * @return
     */
    bool connected() const;

    /**
     * 检查当前是否链接成功
     * @return true - 链接成功； false - 链接失败
     */
    bool checkConnected();

    /**
     * 创建一个用来执行sql的Statement
     * @param s
     * @return
     */
    Statement createStatement(Status &s);

    /**
     * 创建一个PreparedStatement
     * @param sql       要执行的sql
     * @param s         创建结果
     * @return
     */
    PreparedStatement prepareStatement(const std::string &sql, Status &s);


    /**
     * 切换到schema
     * @param schema    要切换到的schema
     * @param s         结果
     */
    void selectSchema(const std::string &schema, Status &s);

    /**
     * 设置选项值
     * @tparam Option
     * @param option
     * @param s
     */
    template<typename Option>
    void setOption(const Option &option, Status &s) {
        s.clear();
        assert(conn_.valid());

        if(mysql_options(conn_.get(), option.name(), option.value()) != 0) {
            s.assign(Status::ERROR, getLastError(conn_));
            return;
        }
    }

    /**
     * 获取选项值
     * @tparam Option
     * @param option
     * @param s
     */
    template<typename Option>
    void getOption(Option &option, Status &s) {
        s.clear();
        assert(conn_.valid());

        void *arg = nullptr;
        if(mysql_get_option(conn_.get(), option.name(), &arg) != 0) {
            s.assign(Status::ERROR, getLastError(conn_));
            return;
        }

        option.value(arg);
    }

    /**
     * 设置是否自动提交事务
     * @param autoCommit
     * @param s
     */
    void setAutoCommit(bool autoCommit, Status &s);

    /**
     * 获取是否自动提交
     * @param s
     * @return 是否自动提交事务
     */
    bool getAutoCommit(Status &s);

private:
    /**
     * 初始化 handler
     */
    void initializeHandler();

private:
    /**
     * mysql链接句柄
     */
    ConnectionHandler conn_;

    /**
     * 是否已经链接
     */
    bool connected_;

    bool autoCommit_;
};

}

#endif //MYSQL_CONNECTOR_CONNECTION_H
