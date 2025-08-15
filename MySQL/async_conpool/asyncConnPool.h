#pragma once
#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>

class ConnectionPool {
public:
    using Callback = std::function<void(std::shared_ptr<sql::ResultSet>)>;
    
    struct Task {
        std::string query;
        Callback callback;
    };

    ConnectionPool(const std::string& host, const std::string& user, 
                   const std::string& password, const std::string& database,
                   size_t pool_size = 2);
    ~ConnectionPool();

    void execute(const std::string& query, Callback callback);
    void start_health_check();

private:
    class Worker;   // 前向声明，用于定义内部类Worker
    
    void health_check();
    void shutdown();

    std::string m_host;
    std::string m_user;
    std::string m_password;
    std::string m_database;
    std::atomic<size_t> m_next_index{0};
    std::atomic<bool> m_shutdown{false};
    std::atomic<bool> m_health_check_running{false};
    std::vector<std::unique_ptr<Worker>> m_workers;
    std::thread m_health_thread;
};

class ConnectionPool::Worker {
public:
    Worker(ConnectionPool& pool, size_t id);
    ~Worker();
    
    void add_task(Task task);
    bool check_connection();

private:
    void run();
    void execute_task(const Task& task);
    void connect();
    void reconnect();
    void stop();

    ConnectionPool& m_pool;
    size_t m_id;
    std::unique_ptr<sql::Connection> m_conn;
    std::thread m_thread;
    std::queue<Task> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_running{true};
};
