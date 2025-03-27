#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>

using namespace std;

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
