#include "asyncConnPool.h"
#include <chrono>
#include <stdexcept>

using namespace std::chrono_literals;

// ==================== ConnectionPool ====================

ConnectionPool::ConnectionPool(const std::string& host, const std::string& user, 
                               const std::string& password, const std::string& database,
                               size_t pool_size)
    : m_host(host), m_user(user), m_password(password), m_database(database)
{
    // 创建连接池
    for (size_t i = 0; i < pool_size; ++i) {
        m_workers.emplace_back(std::make_unique<Worker>(*this, i));
    }
    std::cout << "Connection pool created with " << pool_size << " workers\n";
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

void ConnectionPool::execute(const std::string& query, Callback callback) {
    if (m_shutdown) {
        std::cerr << "Connection pool is shutting down, task rejected\n";
        return;
    }
    
    Task task{query, callback};
    
    // 简单轮询分配策略
    size_t index = m_next_index++;
    if (index >= m_workers.size()) {
        m_next_index = 0;
        index = 0;
    }
    
    m_workers[index]->add_task(std::move(task));
}

void ConnectionPool::start_health_check() {
    if (m_health_check_running) return;
    
    m_health_check_running = true;
    m_health_thread = std::thread(&ConnectionPool::health_check, this);
    std::cout << "Health check started\n";
}

void ConnectionPool::health_check() {
    while (m_health_check_running) {
        std::this_thread::sleep_for(15s);
        
        if (m_shutdown) break;
        
        size_t healthy_count = 0;
        for (auto& worker : m_workers) {
            if (worker->check_connection()) {
                healthy_count++;
            }
        }
        
        std::cout << "Health check: " << healthy_count 
                  << "/" << m_workers.size() << " workers healthy\n";
    }
}

void ConnectionPool::shutdown() {
    if (m_shutdown) return;
    
    m_shutdown = true;
    m_health_check_running = false;
    
    if (m_health_thread.joinable()) {
        m_health_thread.join();
    }
    
    m_workers.clear();
    std::cout << "Connection pool shutdown completed\n";
}

// ==================== Worker ====================

ConnectionPool::Worker::Worker(ConnectionPool& pool, size_t id)
    : m_pool(pool), m_id(id)
{
    m_thread = std::thread(&Worker::run, this);
    std::cout << "Worker " << m_id << " created\n";
}

ConnectionPool::Worker::~Worker() {
    stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
    std::cout << "Worker " << m_id << " destroyed\n";
}

void ConnectionPool::Worker::add_task(Task task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(task));
    }
    m_cond.notify_one();
}

bool ConnectionPool::Worker::check_connection() {
    if (!m_conn) return false;
    
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        stmt->execute("SELECT 1");
        return true;
    } catch (...) {
        try {
            std::cout << "Worker " << m_id << " connection check failed, reconnecting...\n";
            reconnect();
            return true;
        } catch (...) {
            return false;
        }
    }
}

void ConnectionPool::Worker::run() {
    std::cout << "Worker " << m_id << " started\n";
    
    while (m_running) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this] {
                return !m_running || !m_queue.empty();
            });
            
            if (!m_running) break;
            
            task = m_queue.front();
            m_queue.pop();
        }
        
        execute_task(task);
    }
    
    std::cout << "Worker " << m_id << " exiting\n";
}

void ConnectionPool::Worker::execute_task(const Task& task) {
    try {
        // 惰性连接创建
        if (!m_conn) {
            connect();
        }
        
        // 连接健康检查
        if (!m_conn->isValid() || m_conn->isClosed()) {
            reconnect();
        }
        
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        sql::ResultSet* raw_res = stmt->executeQuery(task.query);
        std::shared_ptr<sql::ResultSet> res(raw_res);
        
        if (task.callback) {
            task.callback(res);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Worker " << m_id << " error: " << e.what()
                  << " (Error: " << e.getErrorCode()
                  << ", State: " << e.getSQLState() << ")\n";
        if (task.callback) task.callback(nullptr);
    } catch (const std::exception& e) {
        std::cerr << "Worker " << m_id << " general error: " << e.what() << "\n";
        if (task.callback) task.callback(nullptr);
    }
}

void ConnectionPool::Worker::connect() {
    try {
        sql::Driver* driver = sql::mysql::get_mysql_driver_instance();
        m_conn.reset(driver->connect(m_pool.m_host, m_pool.m_user, m_pool.m_password));
        m_conn->setSchema(m_pool.m_database);
        std::cout << "Worker " << m_id << " connected to database\n";
    } catch (sql::SQLException& e) {
        std::cerr << "Worker " << m_id << " connection failed: " << e.what() 
                  << " (Error: " << e.getErrorCode()
                  << ", State: " << e.getSQLState() << ")\n";
        throw;
    }
}

void ConnectionPool::Worker::reconnect() {
    const int max_retries = 3;
    for (int i = 0; i < max_retries; ++i) {
        try {
            std::cout << "Worker " << m_id << " reconnecting ("
                      << (i+1) << "/" << max_retries << ")...\n";
            connect();
            return;
        } catch (...) {
            if (i == max_retries - 1) throw;
            std::this_thread::sleep_for(1s);
        }
    }
}

void ConnectionPool::Worker::stop() {
    m_running = false;
    m_cond.notify_one();
}
