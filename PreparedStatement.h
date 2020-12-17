//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
#define MYSQL_CONNECTOR_PREPAREDSTATEMENT_H

#include <mysql/mysql.h>

namespace db {

/**
 * PreparedStatement的结果集合
 */
class PreparedResultSet {

};

class PreparedStatement {
public:

    PreparedStatement(MYSQL_STMT *stmt_);
    PreparedStatement() = delete;
    PreparedStatement(const PreparedStatement &) = delete;
    PreparedStatement& operator=(const PreparedStatement &) = delete;
    PreparedStatement(PreparedStatement &&);
    PreparedStatement& operator=(PreparedStatement &&);

    ~PreparedStatement();

    /**
     * 绑定输入参数
     * @tparam Args     输入参数类型，暂时只支持 string, Integer
     * @param args
     * @return
     */
    template<typename ...Args>
    Status bind(Args ...args);

    /**
     * 执行sql
     * @param s
     */
    void execute(Status &s);

    /**
     * 获取execute执行后，影响到的行数
     * @return
     */
    int64_t getAffectedRowsCount();


private:

    MYSQL_STMT *stmt_;
};

}


#endif //MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
