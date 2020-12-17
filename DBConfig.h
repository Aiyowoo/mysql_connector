//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_DBCONFIG_H
#define MYSQL_CONNECTOR_DBCONFIG_H

namespace db {

struct DBConfig {

    /**
     * mysql服务器地址
     */
    std::string host;

    /**
     * mysql服务器端口号
     */
    unsigned short port;

    /**
     * 用户名
     */
    std::string user;

    /**
     * 密码
     */
    std::string password;

    /**
     * 使用哪个数据库
     */
    std::string schema;

    /**
     * 链接标志
     */
    int connectionFlag;

};

}

#endif //MYSQL_CONNECTOR_DBCONFIG_H
