# XSQL

一个由 C 语言手写的轻量级 SQL 数据库雏形。

## 项目简介

XSQL 是一个使用纯 C 语言实现的简化 SQL 数据库引擎，
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

##### 建表
```sql
CREATE TABLE product(
    product_id       STRING     comment 'product_id',
    product_name     STRING     comment 'product_name',
    category         STRING     comment 'category',
    brand            STRING     comment 'brand',
    price            STRING     comment 'price',
    stock            STRING     comment 'stock',
    supplier_id      STRING     comment 'supplier_id',
    warehouse_code   STRING     comment 'warehouse_code',
    production_date  STRING     comment 'production_date',
    expiry_date      STRING     comment 'expiry_date'
) COMMENT 'product table';


CREATE TABLE universy(
    id              STRING      comment 'id',
    tuition         STRING      comment 'tuition',
    name            STRING      comment 'name',
    person_num      STRING      comment 'person_number',
    universy_head   STRING      comment 'universy_head'
)COMMENT 'universy';
```

- 建表结构为:CREATE + TABLE + XXX(表名)(...);
- 创建字段格式为 xxx(字段名)  STRING(数据类型)  COMMENT '..'(注释), -- 最后一个字段不需要在末尾加 ','。
- 目前只支持 STRING类型数据存储，尚未支持主键，字段初始化等功能。

## 功能指令

##### 查询
```sql
SELECt * FROM product;

SELECT product_id , category , supplier_id FROM product;

SELECT product_name, category, brand, price, stock FROM product 
where product_id = 'p0001' AND product_name = 'Laptop' AND stock = '150'; 
```

- 支持全字段查询，分字段查询，WHERE条件查询，AND 逻辑查询；


##### 插入
```sql
INSERT product(product_id, product_name, category, brand, price, stock, supplier_id,warehouse_code, production_date, expiry_date) : 
('P0001', 'Laptop', 'Electronics', 'Dell', '5999', '150', 'SUP001', 'WH-A01', '20260101', '20290101');
```
- 关键字INSERT + xxx(表名) + (...[要修改的字段]) + : +(...[修改的对应字段数据]);

##### 表查寻
```sql
SHOW tables;

SHOW table xxx(表名);
```
- 第一行为得到所有表的名称。
- 第二行为得到目标表的所有字段。

##### 更新
```sql
UPDATE product(product_name , category) set ('vivo' , '一加') whrere product_id = 'p0001';
```

##### 删除
```sql
DELETE product where product_id = 'p0001' AND product_name = 'Laptop' OR stock = '100';
```

#### terminal 运行指令
```txt
gcc.exe xsqlg/create.c xsqlg/bplustree.c xsqlg/utils.c xsqlg/delete.c xsqlg/show.c xsqlg/dbt.c main.c log/xlog.c xsqlg/update.c time/xtime.c xsqlg/grm.c list/list.c file/file.c xstr/String.c -o main.exe
```

##### 运行方式
- [1] 打开文件终端，将运行指令，输入终端，回车；输入 .\main.exe , 即可。
- [2] 打开文件文件，点击main.exe(应用程序)。

##### xqsl语句执行方式
- [1] 运行文件终端[.\main.exe]，输入语句。
- [2] 打开主文件，内置createexe.xsql文件 和 exe.xsql文件：createexe.xsql用于建立表的语句，exe.xsql用于执行CRUD指令。
* 执行建表文件语句，运行终端输入：c exe file;
* 执行CRUD文件语句，运行终端输入：exe file;

##### 功能注意
- [1] xsql语句不区分大小写。
