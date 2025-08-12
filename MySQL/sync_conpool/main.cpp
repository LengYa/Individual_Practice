#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "syncConPool.h"

const int Port = 3306;
const int Mysql_Max_Connect = 5;
const int Max_Thread = 10;

void threadWort(int id);

int main()
{
    auto pool = ConnectPool::getInstance();
    mysqlinfo info("tcp://127.0.0.1:3306", "root", "123456", "sql", Port, Mysql_Max_Connect);
    pool->initialize(info);

    pool->printInfo();

    // 创建多个线程，每个线程都从连接池中获取一个连接并执行一些操作
    std::vector<std::thread> threads;
    for(int i = 0; i < Max_Thread; ++i){
        threads.emplace_back(threadWort, i);
    }

    for(auto &t : threads){
        t.join();
    }
    pool->printInfo();
    return 0;
}

void threadWort(int id)
{
    auto pool = ConnectPool::getInstance();
    auto conn = pool->getConnection();

    try{
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        std::shared_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT VERSION()"));

        while(res->next()){
            std::cout << "Thread ID: " << id << ", MySQL Version: " << res->getString(1) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }catch(sql::SQLException &e){
        std::cerr << "SQLException: " << e.what() << std::endl;
    }
}