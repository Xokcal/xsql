# xsql解析器

## 核心启动器
```c
void XSQL_RUN(TOKENSB *tokensb);
```
## 核心功能
*解析：CREATE ， INSERT ， SELECT 等关键语句*

## 核心语句用法

### CREATE 建表语句
```sql
CREATE TABLE teacher(
    id STRING COMMENT 'id',
    age STRING COMMENT 'age',
    name STRING COMMENT 'name',
    school STRING COMMENT 'school',
    teacher_id STRING COMMENT 'student_id',
    address STRING COMMENT 'address',
    universy STRING COMMENT 'universy'
)COMMENT 'teacher';
```
### INSERT 插入数据语句
```sql
INSERT teacher(id , age ,  address ,  teacher_id , name , school , universy) :
('19' , '21' ,  'island1' , '34' , 'HanShizhong' , 'SeYeplochilsUnivserty', 'TechEcUniversy');

```
### SELECT 查询数据语句
```sql
SELECT * FROM teacher WHERE universy = 'TechEcUniversy';
```

