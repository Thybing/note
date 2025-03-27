#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>

using namespace std;

int main() {
    string host = "localhost";
    string user = "usr";
    string password = "password";
    string database = "school";

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        unique_ptr<sql::Connection> con(driver->connect(host, user, password));
        con->setSchema(database);

        string insertSQL = "INSERT INTO students (name, age) VALUES ('John', 20)";

        unique_ptr<sql::Statement> insertStmt(con->createStatement());
        insertStmt->execute(insertSQL);

        spdlog::info("Data inserted successfully: Name='Jone', Age=20");
    } catch (sql::SQLException& e) {
        cerr << "# ERR: " << e.what() << endl;
    }
}