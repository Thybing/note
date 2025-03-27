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

        // 创建预处理语句
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO students (name,age) VALUES (?,?)"));
        pstmt->setString(1, "Alice");
        pstmt->setInt(2, 19);
        pstmt->executeUpdate();
        spdlog::info("Inserted Alice into students table.");

        pstmt->setString(1, "Bob");
        pstmt->setInt(2, 21);
        pstmt->executeUpdate();
        spdlog::info("Inserted Bob into students table.");

    } catch (sql::SQLException& e) {
        cerr << "# ERR: " << e.what() << endl;
    };
}
