# 在Cpp程序中调用MySQL接口

**我们直接来看一个[创建表的例子](./MySQL-LearnSpace/demo1/create_table.cpp)**
```cpp
int main() {
    // MySQL 数据库连接信息
    string host = "localhost";     // 或者你的数据库服务器 IP 地址
    string user = "usr";           // MySQL 用户名
    string password = "password";  // MySQL 密码
    string database = "school";    // 数据库名称

    try {
        // 创建 MySQL 驱动实例
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

        // 创建连接
        unique_ptr<sql::Connection> con(driver->connect("tcp://" + host + ":3306", user, password));

        // 选择数据库
        con->setSchema(database);

        // 创建表
        string createTableSQL = R"(CREATE TABLE IF NOT EXISTS students (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            age INT
        ))";

        // 执行创建表的 SQL 语句
        unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->execute(createTableSQL);

        spdlog::info("Table 'students' created successfully.");
    } catch (sql::SQLException& e) {
        cerr << "# ERR: " << e.what() << endl;
    };
}
```

首先设置了数据库的连接信息，然后拿到MySQL的单例驱动实例。
通过这个驱动，创建了一个与MySQL的连接。
设置默认的数据库为指定的school
然后定义了一个字符串变量，包含了创建表的SQL语句。
接下来我们创建了一个Statement对象，可以将它看成一个SQL执行器
可以通过它去执行我们的SQL语句，它通常是短生命周期的，完成任务之后就会被销毁。
而connection对象一般会有更长的生命周期，比如可能放到一个连接池中。

**我们再来看第二个例子，[向刚刚的表中插入数据](./MySQL-LearnSpace/demo1/add_info.cpp)**

和刚刚的代码几乎一模一样。区别就是将创建表的SQL语句换成了插入数据的SQL语句。这里就不展开说了。

**我们再来看第三个例子，[向刚刚的表中插入数据](./MySQL-LearnSpace/demo1/add_info2.cpp)**

为了进行批量类似的SQL操作，我们使用了PreparedStatement

PreparedStatement 允许预先编译带有占位符的 SQL 语句，然后在每次执行时，只需要绑定不同的参数值到这些占位符上。这不仅提高了效率（因为 SQL 语句只需要解析和编译一次），而且还能有效地防止 SQL 注入攻击。

**我们继续看下一个例子，[获取表中的数据](./MySQL-LearnSpace/demo1/get_info.cpp)**

执行查询语句是，我们会使用stmt->executeQuery(sql)方法。它返回一个sql::ResultSet对象，其中包含查询结果
我们获取的查询结果对象，结果集的游标默认指向第一行记录之前的位置。
res->next()可以将游标移动到下一行。如果成功返回true，失败返回false。
所以，我们可以用while(res->next()) {...} 来遍历结果集，在循环中用resultSet::getInt等方法来获取属性
