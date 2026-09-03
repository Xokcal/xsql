# XSQL

一个由 C 语言手写的轻量级 SQL 数据库雏形。

## 项目简介

XSQL 是一个使用纯 C 语言实现的简化 SQL 数据库引擎，主要用于学习和验证数据库底层原理。  
目前已经支持建表、插入数据、字段映射、链表存储与基础查询。

## 已实现功能

- CREATE TABLE
- INSERT
- 字段名与存储位置自动映射
- 不同字段顺序插入
- 不同字段数量插入
- DATALINE 链表存储
- 主键查询
- 基础 SQL 解析
- 数据表结构管理

## 示例语法

```text
CREATE TABLE student(
    id STRING COMMENT 'id',
    age STRING COMMENT 'age',
    name STRING COMMENT 'name',
    school STRING COMMENT 'school',
    student_id STRING COMMENT 'student_id',
    address STRING COMMENT 'address'
)COMMENT 'student';

INSERT student(id , school , name) : ('12' , 'SeYeplochilsUnivserty' , 'HadelioManton');

INSERT student(id , school , name) : ('13' , 'MorHaton' , 'GeemMorl');

INSERT student(id , school , name) : ('14' , 'MorHaton2' , 'GeemMorl2');

INSERT student(id , age ,  address ,  student_id , name , school) :
('15' , '24' ,  'island' , '5' , 'GeemMorl3' , 'SeYeplochilsUnivserty');
```
## 运行
gcc main.c file/file.c xstr/String.c xsqlg/grm.c list/list.c -o main.exe
