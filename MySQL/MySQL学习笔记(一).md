# MySQL学习笔记（一）

简单学习一下MySQL。笔记不会非常全面，大概只是备忘录级别的。

## 常用的关键字(或许可能是关键字的组合？不是很懂。暂时跟着网课看)

**创建资料库**
```SQL
CREATE DATABASE 数据库名 DEFAULT CHARACTER SET 字符编码;
```
e.g.
```SQL
CREATE DATABASE `database_name` DEFAULT CHARACTER SET utf8;
```
命名建议用反引号包裹。防止与关键字冲突

**查看已有的资料库**
```SQL
SHOW DATABASES
```

**用于删除资料库**
```SQL
DROP DATABASE 数据库名称
```
e.g.
```SQL
DROP DATABASE `database_name`;
```

**使用某个资料库**
```SQL
USE 数据库;
```
e.g.
```SQL
USE `database_name`;
```

**创建一个表格**
```SQL
CREATE TABLE
```
e.g.

```SQL
CREATE TABLE `student`(
	`student_id` INT PRIMARY KEY,
	`name` VARCHAR(20),
    `major` VARCHAR(20)
);
-- 也可以如下指定primaryKey
CREATE TABLE `student`(
	`student_id` INT,
	`name` VARCHAR(20),
    `major` VARCHAR(20),
    PRIMARY KEY(`student_id`)
);
```

```SQL
DESCRIBE `tableName`;
-- 查看表格的相关信息
```

```SQL
DROP TABLE `tableName`;
--删除表格
```

```SQL
ALTER TABLE `student` ADD `gpa` DECIMAL(3,2);
--向表格中添加属性
```

```SQL
ALTER TABLE `student` DROP COLUMN `gpa`;
--删除表格中的属性
```

### 增

```SQL
-- 向表格中添加数据，values中参数按照创建时的顺序填入
INSERT INTO `student` VALUES(1,'张三','计算机');

-- 向表格中添加数据，values中参数按照指定顺序
INSERT INTO `student`(`name`,`major`,`student_id`) VALUES('张三','计算机',1);

-- 向表格中添加数据，values中参数默认可以为NULL
INSERT INTO `student` VALUES(2,'李四',NULL);
-- 也可以这样写,`major`即为NULL
INSERT INTO `student`(`name`,`student_id`) VALUES('李四',2);
```

```SQL
-- constraints 限制，约束
-- 例如
CREATE TABLE student(
	`student_id` INT,
	`name` VARCHAR(20) NOT NULL,
    `major` VARCHAR(20) UNIQUE,
    PRIMARY KEY(`student_id`)
);

-- 其中name不可为NULL，major不可重复 

-- 例如
CREATE TABLE student(
	`student_id` INT AUTO_INCREMENT,
	`name` VARCHAR(20) NOT NULL,
    `major` VARCHAR(20) DEFAULT('工科实验班'),
    PRIMARY KEY(`student_id`)
);
-- 其中major的默认值不再是NULL而是DEFAULT指定的缺省值,student_id缺省时会在每一次插入时自动加1。
-- 关于AUTO_INCREMENT,空表插入第一次默认为1。可以用
ALTER TABLE users AUTO_INCREMENT = 100;
-- 来将下一次auto的值设为100。并且这个auto的值，无论如何都不会回退。
-- 如果我主动插入了id n，并且n + 1大于下一次auto的值，那下一次auto将会变成n + 1。
-- 如果小于下一次auto的值，那就不变。并且如果上面这条语句产生了回退，同样不会生效。
-- 换言之这个auto生成的值，一定不会和现有的值重复。（应该是这样吧，如果不对的话后面学到了再修正）
-- 如果用DELETE删除数据，同样也不会回退。用TRUNCATE可以重置整个表，并且auto会从1开始。

```

### 改

```SQL
-- 修改table中的数据 UPDATE，通常会配合WHERE用于筛选
UPDATE table_name
SET column1 = value1, column2 = value2, ...
WHERE condition;
-- e.g.
UPDATE `student`
SET major = "计算机科学"
WHERE major = "计算机";
-- e.g.
UPDATE `student`
SET major = "软件工程"
WHERE `student_id` >= 104;
--  >  <  =  >=  <=  <> 大于 小于 等于 大于等于  小于等于  不等于

```

### 删

```SQL
-- 删除表格中的数据
DELETE FROM table_name WHERE condition;
-- e.g.
DELETE FROM `student`
WHERE `student_id` >= 105 OR `name` = '张三';
-- 常见的逻辑运算符包括 AND、OR、NOT、XOR
-- 优先级 NOT > AND > XOR > OR ，可以添加括号
```

### 查

```SQL
-- SELECT 语句是 SQL 中用于查询数据的核心语句，它可以从数据库中 检索 数据，并支持 筛选、排序、分组、聚合等 多种操作。
SELECT column1, column2, ... FROM table_name WHERE condition;

-- 查询所有数据
SELECT * FROM `table_name`;

-- 可以使用WHERE进行筛选
-- e.g.
SELECT `student_id`, `name` FROM `student`
WHERE `student_id` > 101;

-- 可以使用ORDER BY来进行排序,如果要使用降序则添加DESC。还可以添加两个属性用于排序，第一个属性相同时采用第二个来比较。
-- e.g.
SELECT `student_id`, `name` FROM `student`
WHERE `student_id` > 101
ORDER BY `student_id` DESC;

-- 使用 LIMIT 限制返回行数
-- e.g.
SELECT `student_id`, `name` FROM `student`
ORDER BY `student_id`
LIMIT 3;
-- 只会回传前3笔数据

-- 配合OFFSET进行分页查询
SELECT * FROM users LIMIT 10 OFFSET 20;
-- e.g.
SELECT `student_id`, `name` FROM `student`
ORDER BY `student_id`
LIMIT 3 OFFSET 3;
-- 跳过三个，然后再显示3个

-- 使用 IN 进行多选
-- e.g.
SELECT `student_id`, `name` FROM `student`
WHERE `student_id` IN (100,101,102)
ORDER BY `student_id`
;

-- 使用 DISTINCT 去重
-- e.g.
SELECT  DISTINCT  `major` FROM `student`;
-- 查看表中出现的所有的 major

-- 使用 LIKE 进行模糊匹配
-- 这个还没仔细看，可能是和正则差不多的东西？

```

## 常用的资料类型

* 整形
	- `INT` 4字节
	- `TINYINT` 1字节
	- `BIGINT` 8字节
	后面可以跟着[(m)] [unsigned] [zerofill]

- `DECIMAL(m,n)` 有小数点的数（一共m位，其中n位小数）
- `VARCHAR(n)`  字符串，最大长度为n
- CHAR(n) 定长字符串，长度不够会空格补齐。查询时自动去除空白(可选保留空白)。
- BLOB  即Binary Large Object，可以为图片、视频、档案等等
- DATE  年-月-日 2025-01-21
- TIMESTAMP     "YYYY-MM-DD HH:MM:SS" 时间戳

文本相关
- text 保存变长的大字符串。最多到65535个字符。
- mediumtext 最长2**24-1
- longtext 最长2**32-1

时间相关
- datetime 1000-01-01 00：00：00到9999-12-31 23：59：59
- timesamp 1970-01-01 00：00：00到2037年
 