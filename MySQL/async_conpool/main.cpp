// main.cpp - 完整测试
#include <iostream>
#include <chrono>
#include <atomic>
#include <string>

#include "asyncConnPool.h"

using namespace std::chrono;

const std::string host = "tcp://127.0.0.1:3306";
const std::string user = "root";
const std::string password = "123456";
const std::string dbname = "tb_test";
const int MAX_POOL_SIZE = 2;

void test_function();

int main() 
{
    try {
        test_function();
       
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void test_function() 
{
     ConnectionPool pool(host, user, password, dbname, MAX_POOL_SIZE);
        pool.start_health_check();
        
        // 第一阶段：简单查询
        pool.execute("SELECT VERSION()", [](auto result) {
            if (result && result->next()) {
                std::cout << "Server version: " << result->getString(1) << "\n";
            }
        });
        
        // 第二阶段：并发测试
        std::atomic<int> completed = 0;
        const int total_queries = 10;
        
        for (int i = 0; i < total_queries; i++) {
            pool.execute(
                "SELECT * FROM v_sanguo1 WHERE 武力 >= 90", 
                [i, &completed](auto result) {
                    std::cout << "Query " << i+1 << " " << (result ? "succeeded" : "failed") << "\n";
                    std::cout << "Query data: \n";
                    while(result->next()){
                        std::cout << "id: " << result->getString(1)  << ", name: " << result->getString(2)
                                  <<", 武力： "<< result->getString(3) <<", 智力: "<<result->getString(4)<<std::endl;
                    }
                    completed++;
                }
            );
        }
        
        // 等待第一阶段完成
        while (completed < total_queries) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // 第三阶段：故障恢复测试
        std::cout << "\nSimulating network failure (stop MySQL service now)...\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        std::cout << "Testing after failure...\n";
        pool.execute("SELECT 1", [](auto result) {
            std::cout << "Recovery test: " 
                      << (result ? "success" : "failure") << "\n";
        });
        
        // 最终等待
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "All tests completed\n";
        
        return 0;
}