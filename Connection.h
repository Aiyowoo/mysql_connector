//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_CONNECTION_H
#define MYSQL_CONNECTOR_CONNECTION_H

#include "DBConfig.h"

#include <mysql/mysql.h>

namespace db {

class Statement;

class PreparedStatement;

/**
 * 到MYSQL服务器 的链接
 */
class Connection {
public:
    /**
     * 选项类型
     */
    enum OptionType {
        AUTO_RECONNECT =  1     // 自动重连
    };

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
     * 链接到数据库服务
     * @param config    配置信息
     * @param s         是否成功链接
     */
    void connect(const DBConfig &config, Status &s);

    /**
     * 关闭链接，并释放资源
     * @param s     是否成功
     */
    void close(Status &s);

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
     * 创建一个Statement
     * @details Statement的生命周期要比Connection的生命周期短
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
     * @param option        选项的类型
     * @param optionValue   选项值
     * @param s
     */
    void setOption(OptionType option, int optionValue, Status &s);

    /**
     * 获取选项的值
     * @param option        选项的类型
     * @param optionValue   选项值
     * @param s
     */
    void getOption(OptionType option, int &optionValue, Status &s);

    /**
     * 设置选项的值
     * @param option        选项的类型
     * @param optionValue   选项的值
     * @param s
     */
    void setOption(OptionType option, bool optionValue, Status &s);

    void getOption(OptionType option, bool &optionValue, Status &s);

    /**
     * 获取事务隔离级别
     * @param s
     * @return  事务隔离级别
     */
    IsolationLevel getIsolationLevel(Status &s);

    /**
     * 设置事务隔离级别
     */
    void setIsolationLevel(IsolationLevel level, Status &s);

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
     * mysql链接句柄
     */
    MYSQL *mysql_;

};

}

#endif //MYSQL_CONNECTOR_CONNECTION_H
