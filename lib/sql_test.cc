#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>

int main()
{

    sql::Driver *driver;
    driver = get_driver_instance();

    sql::Connection *con;
    con = driver->connect("localhost", "oj_client", "liang");
    con->setSchema("oj");

    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    res = stmt->executeQuery("select * from oj_questions");
    while (res->next())
    {
        // 处理结果集中的每一行
        std::cout << res->getString("number") << std::endl;
    }
    delete res;
    delete stmt;

    delete con;
 
}