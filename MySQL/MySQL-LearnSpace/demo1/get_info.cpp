
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

        string selectSql = "SELECT * FROM students";

        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(stmt->executeQuery(selectSql));

        spdlog::info("Querying students table...");

        while (res->next()) {
            int id = res->getInt("id");
            string name = res->getString("name");
            int age = res->getInt("age");

            spdlog::info("ID: {}, Name: {}, Age: {}", id, name, age);
        }

    } catch (sql::SQLException& e) {
        cerr << "# ERR: " << e.what() << endl;
    };
}
