drop database if exists mysql_connector_test;

create database mysql_connector_test;

use mysql_connector_test;

create table t_person
(
    id       int primary key auto_increment,
    name     char(64) not null,
    birthday date,
    gender   char(1) -- '0'-男 '1'-女
) engine = innodb
  charset = utf8;

insert into t_person(name, birthday, gender)
values ('aiyowoo', '1996-01-01', '0'),
       ('xixia', '1996-12-18', '1');