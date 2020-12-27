//
// Created by m8792 on 2020/12/17.
//

#ifndef MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
#define MYSQL_CONNECTOR_PREPAREDSTATEMENT_H

#include "Handler.h"
#include "PreparedResultSet.h"
#include <mysql/mysql.h>

namespace db {

class PreparedStatement {
public:

    PreparedStatement(MYSQL_STMT *stmt_ = nullptr);

    PreparedStatement() = delete;

    PreparedStatement(const PreparedStatement &) = delete;

    PreparedStatement &operator=(const PreparedStatement &) = delete;

    PreparedStatement(PreparedStatement &&);

    PreparedStatement &operator=(PreparedStatement &&);

    ~PreparedStatement();

    /**
     * 绑定输入参数
     * @tparam Args     输入参数类型，暂时只支持 string, Integer
     * @param args
     * @return
     * @throws 会抛异常
     */
    template<typename ...Args>
    void bind(Args ...args);

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

    /**
     * 获取执行select语句后的ResultSet
     * @return
     */
    PreparedResultSet getResultSet(Status &s);

    /**
     * 是否是有效的
     * @return
     */
    bool valid() const;

private:

    StatementHandler stmt_;
};

}


#endif //MYSQL_CONNECTOR_PREPAREDSTATEMENT_H
