# mysql_connector

mysql的C++客户端接口， 针对mysql-c-connector的封装

## Connection
连接到mysql server的连接

## Statement
用来执行sql语句的类

## ResultSet
Statement执行select语句获得的结果

## PreparedStatement
PreparedStatement prepare创建出来的执行语句

## PreparedResultSet
PreparedStatement执行select语句获得的执行结果

## ConnectionPool
连接池，管理一组到服务器的连接

## ConnectionPtr
从连接池获取的连接，对Connection的简单包裹