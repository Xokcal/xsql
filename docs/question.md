##### 问题清单
- 查询（select）此时表为空数据的时候会报错。
``` sql
select * from tableName;  --> tableName 没有数据.
```

- select_exe 执行结束后执行下一条语句会报错（返回索引问题）
``` sql
select * from universy;
select * from user;  --- 报错！
```

- show 在文件执行语句中会重复执行，tables后面接新的show语句，会无限循环。